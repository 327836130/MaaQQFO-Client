# Evidence matrix
Updated 2026-11-19. All current QQ幻想 live acceptance remains pending.

| Area | Evidence | Result and exact scope |
|---|---|---|
| Legacy inventory | ../migration/legacy-assets.json | PARTIAL; selected extensions, 7067 files; not source migration or functional proof |
| Proto Input inventory | ../migration/p83-assets.json | 43 files hashed, including source/license/logs/binaries |
| Scene inventory | ../migration/scene-assets.json | 43 files hashed; C1/C2 reports and raw snapshots |
| Historical P8.3 | a0-p83-verification.txt; tools/verify_p83_evidence.py | OFFLINE_PASS: three historical position deltas and four binary hashes |
| Historical C2-A | a0-c2a-verification.txt; tools/verify_c2a_evidence.py | OFFLINE_PASS: six snapshots, both passes checked against raw field bytes and Q membership |
| Historical C2-B | a0-c2b-verification.txt; tools/verify_c2b_evidence.py | OFFLINE_PASS: four source hashes, Q slots, raw names/keys/vtables/types/positions/resource key and target presence/absence |
| C1 | scene-assets.json | PARTIAL; raw registry located, detailed verification pending |
| Stable upstream | ../engineering/DEPENDENCY_BASELINE.md | v2.16.4 edfb87ac7a049a50b7921658cc206265c8b9e4e5; Runtime 5.14.3 / Binding 5.10.0 / AgentBinary 1.2.0 |
| Build | a1-build.log; a1-desktop-build.log; a1-locked-build.log | OFFLINE_PASS: core, desktop and locked restore builds; upstream warnings remain |
| Core tests | a1-core-tests.log | OFFLINE_PASS: 15 configuration/screenshot-option tests |
| Native runtime | a1-native-smoke.log; tools/native_smoke.py | OFFLINE_PASS: actual v5.14.3 DLL and resource-bound same-process Agent connect/disconnect/cleanup |
| Clean source build | a1-clean-result.json; a1-clean-run.txt | OFFLINE_PASS: committed 0b1ab07 archive, no old bin/obj; existing SDK/NuGet cache reused; build, 15 tests, native smoke |
| GUI startup | none | VERIFY_PENDING |
| Remote PR/CI/review | none | BLOCKED_USER: existing MaaQQFO forks MaaFramework; correct MFAAvalonia Fork pending |
| Controller / route / game | none | NOT_STARTED; no current live PASS |

The first native smoke failed (missing bound resource); a1-native-smoke-first-failure.log preserves it. Fixed implementation passed; it does not prove C++ custom business modules or cross-process Agent behavior. Tools require Windows/x64 where stated. SDK 10.0.402 and CMake were found outside PATH; the earlier SDK-absent diagnosis is resolved.
