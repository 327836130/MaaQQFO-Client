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
