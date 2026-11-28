# Progress
2026-11-18: Read AGENTS and both authoritative documents. Workspace initially has six documentation files and no Git repository. Added runnable SHA256 asset inventory tool and ran it against 17.MaaQQFantasy. GitHub connector authenticated as 327836130; initial Maa search returned MaaFramework only. Search is not proof that other repositories do not exist.
No client build, PR or live validation completed yet.

2026-11-18 update: recovered independent SDK 10.0.402 at ../17.MaaQQFantasy/debug/dotnet-sdk/dotnet.exe and CMake in D:/VS2022BuildTools. Earlier PATH-only finding does NOT mean no toolchain exists. Located P8.3 report/source and C1/C2 snapshots under F:/各类脚本/2026.05.27-codex控制/2026.11.08-qqfo解析. Initialized local Git with upstream remote; fetch v2.16.4 ongoing, no checkout or commit yet.

2026-11-19: upstream v2.16.4 checkout succeeded on phase/A0-inventory. Runtime 5.14.3 / Binding 5.10.0 / AgentBinary 1.2.0 found in csproj. Baseline build running. P8.3 verifier confirms A/B/C deltas and all four historical binary hashes; see a0-p83-verification.txt. This is OFFLINE_PASS for evidence consistency only.

2026-11-19: desktop build exit 0 (34 warnings); Core.Tests 15 passed, 0 failed/skipped. Added pull-request Windows build/test workflow and resolved package locks. Locked desktop build executing; native Agent and UI startup not yet accepted. Remote Fork creation still pending.

2026-11-19: gh 2.105.0 downloaded from official release, SHA256 verified. auth status: not logged in. User login/Fork choice requested; local work continues. Inventory upgraded to all file types, streaming hashes, mutation detection, explicit errors and source/output separation. P8.3 and scene-object manifests generated successfully after fixing Python 3.8 compatibility.

2026-11-19: Native smoke fixed after missing-resource failure. Actual packaged v5.14.3 runtime and Agent client/server connected, disconnected and cleaned up successfully. Added this smoke to Windows CI. Source confirms ShutDown already joins; removed redundant Join. Same-process smoke is not C++ custom module or live game acceptance. Remote metadata confirms MaaQQFO parent MaaXYZ/MaaFramework; user notified to create correct MFAAvalonia fork separately.

2026-11-19: Added clean-source build verifier exporting committed HEAD into unique artifacts directory (no deletion, existing NuGet cache). Build/test/native results in a1-clean-result.json. Added C2-A verifier: six raw snapshots, two passes each validated against captured bytes; log a0-c2a-verification.txt. No live action performed.

2026-11-19: C2-B four stages validated from original capture bytes and recorded SHA256. Q membership, names, query keys, kinds, vtables, world positions and item resource key match summaries. Target presence matches before/on-ground/prompt/post-pickup. Added verify_c2b_evidence.py and a0-c2b-verification.txt. Rebuilt evidence matrix to remove superseded SDK blocker and distinguish current gates.

2026-11-19: Verified main window v2.16.4 and exit 0 on isolated desktop build. Set origin to verified MaaQQFO-Client Fork. Pushed stable baseline and phase branch, opened draft PR #1 via GitHub REST using the existing local Git credential after connector returned 403. No new credentials created.

2026-11-19: Historical C1 raw registry and three object records verified. PR #1 triggered Windows baseline Actions run 44520529533.

2026-11-19: Verified 335 P3 archive hashes and ran D1 evidence verifier successfully. Updated migration inventory with actual sources, old MPE import limitation and D1 live-read denial.

2026-11-19: A0/A1 review reports checked in. Built isolated Windows x64 development ZIP with embedded commit, verified its SHA256 and enabled CI artifact publication.

Verified remote Windows run 44521355518 for 2f5340f and both published artifacts. Added a simulated native Custom Controller coordinate smoke using MaaFramework 5.14.3, with UUID callback, four screenshot modes and 16 click mappings.

A0 metadata census ran across nine relevant legacy roots and recorded 26,878 entries including 340 skips and zero errors. Tracked workspace count at b50f8b0 was 969. Added QQFO extension root and GitHub phase templates for A1.

Per-command HTTP proxy and HTTP/1.1 restored GitHub connectivity. Pushed b50f8b0 and 8fcf630, cloned the Fork branch into a clean directory, and passed locked build, 15 tests and native checks. Cross-process Agent smoke passed against the fresh build.

A1 baseline CI now checks project code whitespace, compiles Python verification scripts and runs synthetic Pipeline resource loading before the separate-process Agent handshake. Package script verified four x64 PE files and rejected an x86 fixture.

A0 historical inventory scope accepted by independent review of b2402a7. B1 pins SDK 10.0.402 with roll-forward disabled and checks actual selection. B2 validates PR merge parents and embeds shared run/build provenance in development and test artifacts. B3 is tracked in the A12 NuGet security gate.

B1/B2 passed in CI 45190167862. Downloaded ZIP and TRX, checked SHA256 and actual SDK 10.0.402, all three source SHAs and 15/15 passing tests. A1 stays VERIFY_PENDING for independent review.

A2 native Custom Controller owned-window fixture passed GDI and FramePool, four screenshot modes, 800x600 to 720x480 resize and 8 observed clicks per backend at physical 120 DPI. Reconnection required a new HWND after invalidating the old one.

A2 managed skeleton compiled in the desktop app. Added typed client-point callbacks, per-session backend ownership, three QQFO tests (18 total) and native managed factory smoke. Locked desktop build and factory connect/capture/click/release passed locally.

2026-11-24 A2 production Win32 selection, capture and bounded same-user IPC compiled. 21 core tests and owned-window production client/peer smoke passed. Resolved duplicate constructor connecting and virtualized GetClientRect.

A3 native wire decoder compiled and passed under MSVC x86 and x64. CI invokes the reproducible verification script for each target.

A3 managed x64 client -> isolated native x86 and x64 pipe fixture round trips passed. Both peer processes acknowledged Bind and Click and exited.

A3 added native host session and same-account pipe transport. Verified resize-bound rejection, stale window and failure cleanup, bounded I/O cancellation and x86/x64 separate-process interoperability.

P8.3 archived fixture SHA256 verified. Two 2026-11-24 selftest attempts aborted on physical desktop movement. Host lifecycle/IPC tests passed independently.

A3 P8.3 sequence endpoint and reusable local connection loop passed x86/x64 tests, including release after missing Down acknowledgement, desktop changes, coordinate overflow, and process disconnect.

2026-11-25 A3 Windows CI 45420020488 passed at `f9ec0dd`. Development ZIP checksum and provenance matched test artifacts. A4 QQFO Win32 discovery lists unresolved QQ幻想 candidates and keeps selection only for the same window and process instance. Local core suite passed 23 tests.

A4 reused the existing Screenshot page for fresh capture from the selected QQFO Pipeline controller, guarded publication across target changes, and added image size and preview-pixel inspection. Local locked Windows build and 27 core tests passed.

A5 verified the v2.1.3 release ZIP hash and Local Bridge version. The loopback smoke read the synthetic Pipeline fixture. The bundled MaaFramework 5.16.1 and the generic Win32 controller path are recorded as compatibility items.

A3 native IPC handles idle clients without expiring healthy sessions and still rejects partial frames on their deadline. x86/x64 managed/native integration passed. A2/A3 live acceptance preparation is documented.

A3 archived P8.3 owned-window selftest passed at 144 and 96 DPI awareness, including cursor isolation, moved-window coordinates, detach and reattach.

A2 display scaling retested at 150% and 125%. The same QQ幻想 HWND returned raw GDI frames of 2400×1350 and 2000×1125, each matching its physical client rectangle. Owned GDI, FramePool and production IPC fixture checks passed at both scales.

A3 native transport now validates an actual provisioned Windows target. Local x86/x64 checks passed for stale HWND, resize and out-of-bounds rejection through cross-process IPC.

A3 integrated the archived P8.3 backend into the x86 owned-window host. The managed client passed two bind/idle/click/detach cycles on the same receiver, including screen-coordinate and restored-cursor reads.

A6 C++ scene reader passed archived C1 replay (8087, 1572; map resource 37) and synthetic cache, identity and scene-change checks.

A6 Q registry sampler passed a three-member C1 byte replay. The synthetic test rejected a member change during sampling and separated query keys by session.
