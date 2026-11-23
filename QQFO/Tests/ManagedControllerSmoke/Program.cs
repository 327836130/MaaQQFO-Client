using MaaFramework.Binding;
using MaaFramework.Binding.Buffers;
using MaaQQFO.Controller;
using System;
using System.IO;
using System.Runtime.InteropServices;

internal static class Program
{
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    private static extern bool SetDllDirectory(string path);

    private static void Check(bool condition, string operation)
    {
        if (!condition)
            throw new InvalidOperationException(operation);
    }

    private static void Main(string[] args)
    {
        Check(args.Length == 1, "Pass the Windows x64 native runtime directory.");
        var runtime = Path.GetFullPath(args[0]);
        Check(File.Exists(Path.Combine(runtime, "MaaFramework.dll")), "Native runtime missing.");
        Check(SetDllDirectory(runtime), "Cannot set the native runtime search directory.");
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
