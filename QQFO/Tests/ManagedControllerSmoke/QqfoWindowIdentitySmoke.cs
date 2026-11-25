using MaaQQFO.Controller;
using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;

[SupportedOSPlatform("windows")]
internal static class QqfoWindowIdentitySmoke
{
    internal static void Verify()
    {
        var directory = Directory.CreateTempSubdirectory("qqfo-owned-window-");
        try
        {
            foreach (var file in Directory.EnumerateFiles(AppContext.BaseDirectory))
                File.Copy(file, Path.Combine(directory.FullName, Path.GetFileName(file)));

            var fixture = Path.Combine(directory.FullName, "qqfo.exe");
            File.Move(Path.Combine(directory.FullName, "ManagedControllerSmoke.exe"), fixture);
            var info = new ProcessStartInfo(fixture)
            {
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardInput = true,
                CreateNoWindow = true,
            };
            info.ArgumentList.Add("--window-identity-fixture");
            using var process = Process.Start(info) ?? throw new InvalidOperationException("Cannot start fixture");
            try
            {
                var line = process.StandardOutput.ReadLineAsync().WaitAsync(TimeSpan.FromSeconds(10)).GetAwaiter().GetResult();
                var handles = line?.Split(' ') ?? [];
                if (handles.Length != 2)
                    throw new InvalidOperationException("Fixture did not return two windows");
                var firstHandle = (nint)long.Parse(handles[0]);
                var secondHandle = (nint)long.Parse(handles[1]);
                var first = QqfoWindowIdentity.Read(firstHandle);
                var second = QqfoWindowIdentity.Read(secondHandle);
                if (first is null || second is null || first == second || first.ProcessId != process.Id ||
                    first.ProcessId != second.ProcessId || first.ThreadId != second.ThreadId ||
                    first.ProcessStartTicks != second.ProcessStartTicks)
                    throw new InvalidOperationException("Owned QQFO fixture identity mismatch");

                process.StandardInput.Close();
                if (!process.WaitForExit(5000) || process.ExitCode != 0)
                    throw new InvalidOperationException("Fixture did not exit cleanly");
                if (QqfoWindowIdentity.Read(firstHandle) is not null || QqfoWindowIdentity.Read(secondHandle) is not null)
                    throw new InvalidOperationException("Exited fixture window still accepted");
                Console.WriteLine("OWNED_WINDOW_PASS: two same-title windows have distinct identities, and both expire on process exit");
            }
            finally
            {
                if (!process.HasExited)
                {
                    process.Kill();
                    process.WaitForExit();
                }
            }
        }
        finally
        {
            foreach (var file in directory.EnumerateFiles())
                file.Delete();
            directory.Delete();
        }
    }

    internal static void RunFixture()
    {
        const string title = "MaaQQFO owned fixture";
        var first = CreateWindowExW(0, "STATIC", title, 0, 0, 0, 100, 100, 0, 0, 0, 0);
        var second = CreateWindowExW(0, "STATIC", title, 0, 0, 0, 100, 100, 0, 0, 0, 0);
        try
        {
            if (first == 0 || second == 0)
                throw new InvalidOperationException("Cannot create owned fixture windows");
            Console.WriteLine($"{first} {second}");
            Console.ReadLine();
        }
        finally
        {
            if (second != 0)
                DestroyWindow(second);
            if (first != 0)
                DestroyWindow(first);
        }
    }

    [DllImport("user32.dll", ExactSpelling = true, CharSet = CharSet.Unicode)]
    private static extern nint CreateWindowExW(uint extendedStyle, string className, string title, uint style,
        int x, int y, int width, int height, nint parent, nint menu, nint instance, nint parameter);

    [DllImport("user32.dll", ExactSpelling = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool DestroyWindow(nint window);
}
