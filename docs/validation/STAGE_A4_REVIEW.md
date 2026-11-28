# A4 role selection checkpoint

Status: IN_PROGRESS.

The QQFO Win32 resource profile uses the existing device selector to list windows owned by a QQFO process. An unresolved candidate displays its window title, PID and HWND. The production factory checks the selected process/window instance again before creating a controller, and the backend rechecks it before operating. The binding is kept in memory for the current selection.

Run `dotnet test MFAAvalonia.Core.Tests/MFAAvalonia.Core.Tests.csproj -c Release` using SDK 10.0.402. The local run passed all 23 tests. `QqfoWindowSelectionTests` exercises same-title collisions, title changes, reordered candidates, PID/thread/window changes, and restart with a reused PID and HWND.

Run `dotnet run --project QQFO/Tests/ManagedControllerSmoke/ManagedControllerSmoke.csproj -c Release -- --window-identity` on Windows after building the desktop project. The local smoke validated that two same-title windows owned by a fixture process resolve to distinct window identities and that both stop resolving after the process exits. The workflow runs this same smoke. It uses only windows created by the fixture process.

## Shared screenshot test

Connect a QQFO window and leave its Pipeline idle. Open the Screenshot page and choose Screenshot Test. This requests a fresh frame through the current Pipeline controller. A frame from an old controller/target is discarded if the selection changes during capture. The preview reports its image dimensions. Press a point on the preview to see the corresponding MaaImage pixel. Saving exports the tested PNG frame.

SDK 10.0.402 locked Windows desktop build succeeded. All 27 core tests passed, including center, edge and letterbox cases. The interactive page with a live QQ幻想 controller remains unverified.

Live role fields, the remaining shared test pages, multi-window validation and profile migration checks remain pending.
