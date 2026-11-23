using MaaFramework.Binding;
using MaaFramework.Binding.Buffers;
using MaaFramework.Binding.Custom;
using System;
using System.Diagnostics;

namespace MaaQQFO.Controller;

internal readonly record struct ClientPosition(int X, int Y);

// One backend instance owns one target session. Captures are encoded raw client pixels.
internal interface IQqfoControllerBackend : IDisposable
{
    bool Connect();
    bool Connected { get; }
    bool TryCapture(out ReadOnlyMemory<byte> clientImage);
    bool Click(ClientPosition position);
}

internal sealed class QqfoControllerCallbacks(string name, IQqfoControllerBackend backend) : IMaaCustomController
{
    private readonly object _gate = new();
    private bool _disposed;

    public string Name { get; set; } = name;

    public bool Connect() => Invoke(backend.Connect);
    public bool Connected() => Invoke(() => backend.Connected);
    public bool RequestUuid(IMaaStringBuffer buffer) => Invoke(() => buffer.TrySetValue(Name));
    public ControllerFeatures GetFeatures() => 0;

    public bool Screencap(IMaaImageBuffer buffer) => Invoke(() => backend.Connected &&
        backend.TryCapture(out var clientImage) && !clientImage.IsEmpty && buffer.TrySetEncodedData(clientImage.Span));

    // MaaFramework has already translated the image point to raw client pixels.
    public bool Click(int x, int y) => Invoke(() => backend.Connected && x >= 0 && y >= 0 &&
        backend.Click(new ClientPosition(x, y)));

    public bool Inactive() => Invoke(() => true);
    public bool StartApp(string intent) => false;
    public bool StopApp(string intent) => false;
    public bool Swipe(int x1, int y1, int x2, int y2, int duration) => false;
    public bool TouchDown(int contact, int x, int y, int pressure) => false;
    public bool TouchMove(int contact, int x, int y, int pressure) => false;
    public bool TouchUp(int contact) => false;
    public bool ClickKey(int keycode) => false;
    public bool InputText(string text) => false;
    public bool KeyDown(int keycode) => false;
    public bool KeyUp(int keycode) => false;
    public bool Scroll(int dx, int dy) => false;
    public bool RelativeMove(int dx, int dy) => false;
    public bool Shell(string command, long timeout, IMaaStringBuffer buffer) => false;
    public bool GetInfo(IMaaStringBuffer buffer) => false;

    public void Dispose()
    {
        lock (_gate)
        {
            if (_disposed)
                return;
            _disposed = true;
            backend.Dispose();
        }
    }

    private bool Invoke(Func<bool> operation)
    {
        lock (_gate)
        {
            if (_disposed)
                return false;
            try
            {
                return operation();
            }
            catch (Exception error)
            {
                // Managed exceptions must not cross MaaFramework's native callback boundary.
                Trace.TraceError("QQFO Controller callback failed: {0}", error.GetType().Name);
                return false;
            }
        }
    }
}

internal static class QqfoControllerFactory
{
    // Ownership of the backend transfers to the managed Controller callback object.
    internal static MaaCustomController Create(string sessionId, IQqfoControllerBackend backend)
    {
        var callbacks = new QqfoControllerCallbacks(sessionId, backend);
        try
        {
            return new MaaCustomController(callbacks);
        }
        catch
        {
            callbacks.Dispose();
            throw;
        }
    }
}
