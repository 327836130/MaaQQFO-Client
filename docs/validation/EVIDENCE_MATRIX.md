# Evidence matrix
Updated 2026-11-19. All current QQ幻想 live acceptance remains pending.

| Area | Evidence | Result and exact scope |
|---|---|---|
| Metadata census | a0-file-census.md | 26,878 records across nine roots; 340 explicit skip entries; 0 errors |
| Legacy inventory | ../migration/legacy-assets.json | PARTIAL; selected extensions, 7067 files; not source migration or functional proof |
| Proto Input inventory | ../migration/p83-assets.json | 43 files hashed, including source/license/logs/binaries |
| Scene inventory | ../migration/scene-assets.json | 43 files hashed; C1/C2 reports and raw snapshots |
| Historical P8.3 | a0-p83-verification.txt; tools/verify_p83_evidence.py | OFFLINE_PASS: three historical position deltas and four binary hashes |
| Historical C2-A | a0-c2a-verification.txt; tools/verify_c2a_evidence.py | OFFLINE_PASS: six snapshots, both passes checked against raw field bytes and Q membership |
| Historical C2-B | a0-c2b-verification.txt; tools/verify_c2b_evidence.py | OFFLINE_PASS: four source hashes, Q slots, raw names/keys/vtables/types/positions/resource key and target presence/absence |
| Historical C1 | a0-c1-verification.txt; tools/verify_c1_evidence.py | OFFLINE_PASS: three registry members, raw fields, local role and one named NPC |
| Stable upstream | ../engineering/DEPENDENCY_BASELINE.md | v2.16.4 edfb87ac7a049a50b7921658cc206265c8b9e4e5; Runtime 5.14.3 / Binding 5.10.0 / AgentBinary 1.2.0 |
| Build | a1-build.log; a1-desktop-build.log; a1-locked-build.log | OFFLINE_PASS: core, desktop and locked restore builds; upstream warnings remain |
| Core tests | a1-core-tests.log | OFFLINE_PASS: 15 configuration/screenshot-option tests |
| Native runtime | a1-native-smoke.log; tools/native_smoke.py | OFFLINE_PASS: actual v5.14.3 DLL and resource-bound same-process Agent connect/disconnect/cleanup |
| Clean source build | a1-clean-result.json; a1-clean-run.txt | OFFLINE_PASS: committed 0b1ab07 archive, no old bin/obj; existing SDK/NuGet cache reused; build, 15 tests, native smoke |
| Clean Fork clone | a1-fork-clone.md | OFFLINE_PASS: remote clone at 8fcf630, locked build, 15 tests, native Agent and coordinate smokes |
| Native Agent across processes | a1-agent-cross-process.log | OFFLINE_PASS: shipped v5.14.3 client and server DLLs handshake, disconnect and exit in separate processes |
| Resource load and Agent | a1-resource-agent-smoke.log | OFFLINE_PASS: synthetic Pipeline loaded and node found before separate-process Agent handshake |
| Development package architecture | a1-package-architecture.md | Four x64 PE headers validated; non-x64 fixture rejected before archive creation |
| GUI startup | a1-desktop-startup.md | OFFLINE_PASS: main window title v2.16.4, exit code 0 |
| Remote PR/CI/review | https://github.com/327836130/MaaQQFO-Client/pull/1 | Draft opened on verified MFAAvalonia Fork. Windows run 44521355518 passed and uploaded development ZIP and TRX. Independent review pending. |
| A1 independent review corrections | a1-review-corrections.md | SDK 10.0.402 and PR merge provenance verified on run 45190167862, 15/15 tests, A1 independent re-review pending |
| Native coordinate contract | a2-coordinate-smoke.log | OFFLINE_PASS: four screenshot modes and 16 simulated callback mappings on a 1600×900 raw fixture |
| Owned Windows test window | STAGE_A2_REVIEW.md | 120 DPI, GDI and FramePool, all four screenshot modes, resize, HWND replacement and 8 received clicks per backend through native Custom callbacks |
| Managed QQFO factory | a2-managed-controller.log | OFFLINE_PASS: C# adapter on bundled native runtime, two created/released sessions, raw screenshot and typed client click. 18 core tests pass. |
| Controller / route / game | none | No current live PASS |

The first native smoke failed (missing bound resource); a1-native-smoke-first-failure.log preserves it. Fixed implementation passed; the later cross-process smoke verifies the native protocol separately. C++ custom business modules remain unverified. Tools require Windows/x64 where stated. SDK 10.0.402 and CMake were found outside PATH; the earlier SDK-absent diagnosis is resolved.

Independent review of b2402a7 accepted A0 historical inventory and found B1/B2 defects in the prior CI toolchain and provenance. Treat older evidence as historical. The replacement CI must assert SDK 10.0.402 and match the checkout, head and base SHAs. B3 remains tracked in docs/engineering/A12_NUGET_SECURITY_GATE.md.

A2 production client readiness: [owned-window capture and test pipe peer](a2-production-ipc.md) passed at 120 DPI. Locked build and 21 core tests passed. Awaiting native host and cross-DPI evidence.

A3 [native wire verification](a3-input-protocol.md) passed on x86 and x64. Proto Input dispatch and game validation remain pending.

A3 x86/x64 [host lifecycle and Windows IPC](a3-input-protocol.md) passed offline, including invalid client bounds, cleanup and timeout cancellation.

A3 [P8.3 sequence and connection loop](a3-input-protocol.md) passed x86/x64 fixture replay, release and disconnect cases. The concrete historical backend remains pending.

A4 [window selection checkpoint](STAGE_A4_REVIEW.md) covers unresolved candidates and selection invalidation with offline regression tests.

A4 [shared screenshot test](STAGE_A4_REVIEW.md#shared-screenshot-test) compiles into the desktop client. Preview coordinate regressions passed within the 27-test core suite. Live validation remains pending.

A5 [MPE workspace checkpoint](STAGE_A5_REVIEW.md) passed the v2.1.3 loopback Pipeline JSON read. Runtime, recognition and input alignment remain pending.

A3 [input protocol](a3-input-protocol.md) passed idle, partial-frame and disconnect coverage on x86 and x64. [A2/A3 live checklist](A2_A3_LIVE_ACCEPTANCE.md) records pending real-client gates.

A2 [same-monitor scale retest](a2-display-scale-retest.md) verified read-only live raw GDI dimensions and owned-window lifecycle/coordinate checks at 150% and 125%.

A3 [native target checks](a3-input-protocol.md) passed with real owned Windows HWNDs on x86 and x64, including physical bounds and window replacement.

A3 [archived P8.3 backend through the native host](a3-input-protocol.md#archived-backend-through-the-native-host) passed two real owned-window IPC/Click/detach cycles on Windows. Game-mode activation and input remain pending.

A6 [GameStateReader checkpoint](STAGE_A6_REVIEW.md) passed C1 recorded-byte player and map resource decoding and synthetic session/cache tests.
