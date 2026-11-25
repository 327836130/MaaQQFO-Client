# A4 role selection checkpoint

Status: IN_PROGRESS.

The QQFO Win32 resource profile uses the existing device selector to list windows owned by a QQFO process. An unresolved candidate displays its window title, PID and HWND. The production factory checks the selected process/window instance again before creating a controller, and the backend rechecks it before operating. The binding is kept in memory for the current selection.

Run `dotnet test MFAAvalonia.Core.Tests/MFAAvalonia.Core.Tests.csproj -c Release` using SDK 10.0.402. The local run passed all 23 tests. `QqfoWindowSelectionTests` exercises same-title collisions, title changes, reordered candidates, PID/thread/window changes, and restart with a reused PID and HWND.

Run `dotnet run --project QQFO/Tests/ManagedControllerSmoke/ManagedControllerSmoke.csproj -c Release -- --window-identity` on Windows after building the desktop project. The local smoke validated that two same-title windows owned by a fixture process resolve to distinct window identities and that both stop resolving after the process exits. The workflow runs this same smoke. It uses only windows created by the fixture process.

Live role names, map, stable IDs and multi-window validation remain pending. The shared test pages and profile migration checks are also pending.
