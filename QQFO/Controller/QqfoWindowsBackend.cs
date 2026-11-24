using MaaFramework.Binding;
using MaaFramework.Binding.Buffers;
using System;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;

namespace MaaQQFO.Controller;

[SupportedOSPlatform("windows")]
internal sealed class QqfoWindowsBackend : IQqfoControllerBackend
{
    private readonly nint _hwnd;
    private readonly uint _pid;
    private readonly MaaWin32Controller _capture;
    private readonly QqfoInputPipe _input;
    private bool _linked;
    private int _width;
    private int _height;

    internal QqfoWindowsBackend(nint hwnd, Guid session, Win32ScreencapMethods captureMethod)
    {
        if (session == Guid.Empty || hwnd == 0 || GetWindowThreadProcessId(hwnd, out _pid) == 0)
            throw new ArgumentException("QQFO requires a live target window and an explicit IPC session.");
        _hwnd = hwnd;
        _capture = new MaaWin32Controller(hwnd, captureMethod, Win32InputMethod.PostMessage,
            Win32InputMethod.PostMessage, LinkOption.None);
        _input = new QqfoInputPipe(session, _pid, unchecked((ulong)hwnd));
    }

    private bool SameWindow => GetWindowThreadProcessId(_hwnd, out var pid) != 0 && pid == _pid;
    public bool Connected => _linked && _input.Connected && SameWindow;

    public bool Connect()
    {
        _linked = false;
        _width = _height = 0;
        _input.Dispose();
        if (!SameWindow || !_capture.SetOption(ControllerOption.ScreenshotUseRawSize, true) ||
            _capture.LinkStart().Wait() != MaaJobStatus.Succeeded || !_input.Connect())
            return false;
        _linked = true;
        return true;
    }

    public bool TryCapture(out ReadOnlyMemory<byte> clientImage)
    {
        clientImage = default;
        _width = _height = 0;
        if (!Connected || _capture.Screencap().Wait() != MaaJobStatus.Succeeded)
            return false;
        using var image = new MaaImageBuffer();
        if (!_capture.GetCachedImage(image) || !SameWindow ||
            !image.TryGetEncodedData(out byte[]? bytes) || bytes is not { Length: > 0 })
            return false;
        _width = image.Width;
        _height = image.Height;
        clientImage = bytes;
        return true;
    }

    public bool Click(ClientPosition position)
    {
        return Connected && TryGetPhysicalClientRect(out var client) && client.Right == _width && client.Bottom == _height &&
            position.X >= 0 && position.Y >= 0 && position.X < _width && position.Y < _height && _input.Click(position);
    }

    private bool TryGetPhysicalClientRect(out Rect client)
    {
        client = default;
        var previous = SetThreadDpiAwarenessContext((nint)(-4)); // Per-Monitor V2
        if (previous == 0)
            return false;
        try
        {
            return GetClientRect(_hwnd, out client);
        }
        finally
        {
            SetThreadDpiAwarenessContext(previous);
        }
    }

    public void Dispose()
    {
        _linked = false;
        _input.Dispose();
        _capture.Dispose();
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct Rect
    {
        internal int Left, Top, Right, Bottom;
    }

    [DllImport("user32.dll", ExactSpelling = true)]
    private static extern uint GetWindowThreadProcessId(nint hwnd, out uint processId);
    [DllImport("user32.dll", ExactSpelling = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool GetClientRect(nint hwnd, out Rect rectangle);

    [DllImport("user32.dll", ExactSpelling = true)]
    private static extern nint SetThreadDpiAwarenessContext(nint value);
}
