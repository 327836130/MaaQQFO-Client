# A2 owned-window validation

`tools/owned_window_controller_smoke.py` opens a Win32 window owned by the test process. It builds a `MaaCustomControllerCreate` callback fixture over the shipped Win32 controller. The inner controller supplies raw client pixels and receives client-space input. The outer Maa controller applies the configured screenshot-to-client conversion once. The test backend delivers Click by PostMessage only to the fixture's own HWND. The two screen backends are GDI and FramePool.

From the repository root, with x64 Python and a locked Windows desktop build:

```powershell
python tools/owned_window_controller_smoke.py bin/AnyCPU/Release/runtimes/win-x64/native --custom --screencap gdi
python tools/owned_window_controller_smoke.py bin/AnyCPU/Release/runtimes/win-x64/native --custom --screencap framepool
```

| Check | Observed |
|---|---|
| Display environment | Single monitor, 120 DPI (125%), Per-Monitor V2 test thread |
| Capture at 800×600 client | Actual expected center pixel on both backends. Raw 800×600. Long side, short side and expand 640×480. Raw option must be disabled before switching to a scaled mode. |
| Click at 640×480 Maa image size | Four corners and center produced Win32 click release at the corresponding client coordinates. The image center (320,240) arrived at client (400,300). |
| Resize to 720×480 | Original HWND and controller remained in use. Fresh screenshot was 720×480 and the expected corner and center were delivered. |
| Disconnect and replacement | Destroyed controller and window, checked old HWND was invalid, then created and connected a new controller on a new HWND. Raw capture and corner Click passed. |
| DPI and coordinate spaces | Test reads physical client size and per-window DPI. Client-to-screen origin round-trip passed. `WorldPosition` is outside this test. |

See the [GDI log](a2-owned-window-gdi.log), [FramePool log](a2-owned-window-framepool.log) and [existing synthetic mapping log](a2-coordinate-smoke.log). All used Maa native 5.14.3. No second monitor or DPI transition was available. This fixture verifies the owned-window path and callback boundary. The app factory currently instantiates `MaaWin32Controller` in the Win32 branch of `MaaProcessor`; production QQFO controller selection and IPC are still open. Proto Input and game-side acceptance belong to the subsequent integration work. A2 remains IN_PROGRESS.
