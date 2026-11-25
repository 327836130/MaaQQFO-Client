# A4 role selection checkpoint

Status: IN_PROGRESS.

The QQFO Win32 resource profile uses the existing device selector to list windows owned by a QQFO process. An unresolved candidate displays its window title, PID and HWND. The production factory checks the selected process/window instance again before creating a controller, and the backend rechecks it before operating. The binding is kept in memory for the current selection.

Run `dotnet test MFAAvalonia.Core.Tests/MFAAvalonia.Core.Tests.csproj -c Release` using SDK 10.0.402. The local run passed all 23 tests. `QqfoWindowSelectionTests` exercises same-title collisions, title changes, reordered candidates, PID/thread/window changes, and restart with a reused PID and HWND.

Live role names, map, stable IDs and multi-window validation remain pending. The shared test pages and profile migration checks are also pending.
