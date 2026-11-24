using MaaFramework.Binding;
using MaaFramework.Binding.Buffers;
using MaaQQFO.Controller;
using System;
using System.Buffers.Binary;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

internal static class Program
{
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern bool SetDllDirectory(string path);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    private static extern int GetClassName(nint hwnd, StringBuilder className, int size);

    private static void Check(bool condition, string operation)
    {
        if (!condition)
            throw new InvalidOperationException(operation);
    }

    private static void Main(string[] args)
    {
        Check(args.Length is 1 or 2 or 3, "Pass the native runtime directory and optional fixture arguments.");
        var runtime = Path.GetFullPath(args[0]);
        Check(File.Exists(Path.Combine(runtime, "MaaFramework.dll")), "Native runtime missing.");
        Check(SetDllDirectory(runtime), "Cannot set the native runtime search directory.");
        if (args.Length == 3)
        {
            Check(args[1] == "--pipe-fixture", "Unknown fixture option.");
            CheckNativePipeFixture(args[2]);
            return;
        }
        if (args.Length == 2 && OperatingSystem.IsWindows())
        {
            CheckOwnedWindowBridge((nint)long.Parse(args[1]));
            return;
        }
        for (var session = 0; session < 2; ++session)
        {
            var backend = new TestBackend();
            using (var controller = QqfoControllerFactory.Create($"smoke-{session}", backend))
            {
                Check(controller.SetOption(ControllerOption.ScreenshotUseRawSize, true), "Raw screenshot setting failed.");
                Check(controller.LinkStart().Wait() == MaaJobStatus.Succeeded, "Connection failed.");
                Check(controller.Screencap().Wait() == MaaJobStatus.Succeeded, "Capture failed.");
                using var image = new MaaImageBuffer();
                Check(controller.GetCachedImage(image) && image.Width == 2 && image.Height == 2, "Unexpected screenshot dimensions.");
                Check(controller.Click(1, 1).Wait() == MaaJobStatus.Succeeded, "Click failed.");
                Check(backend.Clicked == new ClientPosition(1, 1), "Client coordinates changed.");
            }
            Check(backend.DisposeCount == 1, "Backend release failed.");
        }
        Console.WriteLine("OFFLINE_PASS: managed QQFO factory, native callbacks, raw screenshot, client click and session release/recreate");
    }

    private static void CheckNativePipeFixture(string executable)
    {
        var session = Guid.NewGuid();
        var info = new ProcessStartInfo(Path.GetFullPath(executable)) { UseShellExecute = false, CreateNoWindow = true };
        info.ArgumentList.Add("--pipe-fixture");
        info.ArgumentList.Add(session.ToString("N"));
        info.ArgumentList.Add(Convert.ToHexStringLower(session.ToByteArray()));
        using var host = Process.Start(info) ?? throw new InvalidOperationException("Cannot start native pipe fixture.");
        try
        {
            using var client = new QqfoInputPipe(session, 123, 456);
            Check(client.Connect(), "Native fixture handshake failed.");
            Check(client.Click(new ClientPosition(800, 450)), "Native fixture click acknowledgement failed.");
            Check(host.WaitForExit(5000) && host.ExitCode == 0, "Native fixture did not exit cleanly.");
            Console.WriteLine($"OFFLINE_PASS: managed client -> native {Path.GetFileName(executable)} pipe peer, bind and Click acknowledgement");
        }
        finally
        {
            if (!host.HasExited)
            {
                host.Kill();
                host.WaitForExit();
            }
        }
    }

    [SupportedOSPlatform("windows")]
    private static void CheckOwnedWindowBridge(nint hwnd)
    {
        var className = new StringBuilder(80);
        Check(GetClassName(hwnd, className, className.Capacity) > 0 && className.ToString() == "MaaQQFOControllerSmoke",
            "Expected the owned test window class.");
        var session = Guid.NewGuid();
        using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(10));
        using var pipe = new NamedPipeServerStream(QqfoInputPipe.PipeName(session), PipeDirection.InOut, 1,
            PipeTransmissionMode.Byte, PipeOptions.CurrentUserOnly | PipeOptions.Asynchronous);
        var host = Task.Run(async () =>
        {
            await pipe.WaitForConnectionAsync(deadline.Token);
            var frame = new byte[QqfoInputPipe.FrameSize];
            for (ushort operation = 1; operation <= 2; ++operation)
            {
                await pipe.ReadExactlyAsync(frame, deadline.Token);
                Check(BinaryPrimitives.ReadUInt16LittleEndian(frame.AsSpan(6)) == operation, "Unexpected pipe operation.");
                Check(BinaryPrimitives.ReadUInt64LittleEndian(frame.AsSpan(32)) == (ulong)hwnd, "Wrong window binding.");
                if (operation == 2)
                    Check(BinaryPrimitives.ReadInt32LittleEndian(frame.AsSpan(44)) == 400 &&
                          BinaryPrimitives.ReadInt32LittleEndian(frame.AsSpan(48)) == 300, "Wrong client click coordinates.");
                BinaryPrimitives.WriteInt32LittleEndian(frame.AsSpan(52), 1);
                await pipe.WriteAsync(frame, deadline.Token);
            }
        });
        using var controller = QqfoControllerFactory.Create($"owned-{session:N}",
            new QqfoWindowsBackend(hwnd, session, Win32ScreencapMethods.GDI));
        Check(controller.SetOption(ControllerOption.ScreenshotTargetShortSide, 480), "Scale option failed.");
        Check(controller.LinkStart().Wait() == MaaJobStatus.Succeeded, "Owned-window pipe connection failed.");
        Check(controller.Screencap().Wait() == MaaJobStatus.Succeeded, "Owned-window capture failed.");
        using var picture = new MaaImageBuffer();
        Check(controller.GetCachedImage(picture) && picture.Width == 640 && picture.Height == 480, "Wrong scaled screenshot.");
        Check(controller.Click(320, 240).Wait() == MaaJobStatus.Succeeded, "Owned-window pipe click failed.");
        host.GetAwaiter().GetResult();
        Console.WriteLine("OWNED_WINDOW_PASS: managed production capture + pipe backend, 640x480 image point (320,240) -> client (400,300)");
    }

    private sealed class TestBackend : IQqfoControllerBackend
    {
        // A 2x2 uncompressed 24-bit BMP, used as encoded raw client pixels.
        private static readonly byte[] Frame = Convert.FromHexString(
            "424D46000000000000003600000028000000020000000200000001001800000000001000000000000000000000000000000000000000" +
            "80604080604000008060408060400000");
        public bool Connected { get; private set; }
        public ClientPosition? Clicked { get; private set; }
        public int DisposeCount { get; private set; }
        public bool Connect() => Connected = true;
        public bool TryCapture(out ReadOnlyMemory<byte> clientImage)
        {
            clientImage = Frame;
            return Connected;
        }
        public bool Click(ClientPosition position)
        {
            Clicked = position;
            return Connected;
        }
        public void Dispose()
        {
            ++DisposeCount;
            Connected = false;
        }
    }
}
