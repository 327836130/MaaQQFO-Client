using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Runtime.Versioning;

namespace MaaQQFO.Controller;

internal sealed record QqfoWindowIdentity(nint Handle, uint ProcessId, uint ThreadId, long ProcessStartTicks)
{
    [SupportedOSPlatform("windows")]
    internal static QqfoWindowIdentity? Read(nint handle)
    {
        if (handle == 0)
            return null;
        var threadId = GetWindowThreadProcessId(handle, out var processId);
        if (threadId == 0 || processId == 0)
            return null;

        try
        {
            using var process = Process.GetProcessById(checked((int)processId));
            if (!string.Equals(process.ProcessName, "qqfo", StringComparison.OrdinalIgnoreCase))
                return null;
            var start = process.StartTime.ToUniversalTime().Ticks;
            return !process.HasExited && GetWindowThreadProcessId(handle, out var currentPid) == threadId
                && currentPid == processId
                ? new(handle, processId, threadId, start)
                : null;
        }
        catch (Exception ex) when (ex is ArgumentException or InvalidOperationException or Win32Exception)
        {
            return null;
        }
    }

    [DllImport("user32.dll", ExactSpelling = true)]
    private static extern uint GetWindowThreadProcessId(nint window, out uint processId);
}

internal sealed record QqfoWindowCandidate(QqfoWindowIdentity Identity, string Title)
{
    internal static int FindSelection(IReadOnlyList<QqfoWindowCandidate> candidates, QqfoWindowIdentity? previous)
    {
        if (previous is null)
            return -1;
        for (var i = 0; i < candidates.Count; i++)
        {
            if (candidates[i].Identity == previous)
                return i;
        }
        return -1;
    }

    public override string ToString() => $"未识别 / 待登录 · {Title} · PID {Identity.ProcessId} · HWND 0x{Identity.Handle:X}";
}
