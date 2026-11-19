# Migration inventory (A0, ongoing)
Sources are read-only. PARTIAL means located, not newly accepted.

| Asset | Status | Actual source | Remaining check |
|---|---|---|---|
| Controller / IPC | PARTIAL | ../17.MaaQQFantasy/host_support/native_dm32/controller.cpp; common/protocol.h | ABI, licensing, build |
| FramePool / DPI | PARTIAL | ../17.MaaQQFantasy/docs/native-dm-p3-acceptance.md | Preserve original coordinate behavior; inspect implementation |
| Editor / test pages | PARTIAL | ../17.MaaQQFantasy/business/controller_input_tests/docs/mpe-validation.md | Read implementation and backend sharing |
| Proto Input | PARTIAL | F:/各类脚本/2026.05.27-codex控制/2026.11.08-qqfo解析/P8.3 | Report read; raw logs and build pending |
| Player / map / NPC / monster / items | PARTIAL | F:/各类脚本/2026.05.27-codex控制/2026.11.08-qqfo解析/cheatengine-mcp-bridge-main/research/P7_1_Map_Research/scene_objects | C1/C2 snapshots found; field review pending |
| Routes | PARTIAL | F:/各类脚本/2026.05.27-codex控制/P8.0_QQFantasy_NavigationWorkbench | D1 report and resource inspection pending |
| Pipeline / image | PARTIAL | ../17.MaaQQFantasy/resource | Selected file hashes in legacy-assets.json |
| Stable identity | PLAN | Architecture §4 | Query key must not become permanent player ID |

P8.3 report names Proto Input dd29bf8d22349605d0e1950c1c5dc4893d72670e, EasyHook 2.7.7097 x86; describes A/B/C movement and removal sequence 26. DLL residency and abnormal cleanup remain limitations. This is historical evidence, not a current MaaQQFO live PASS.

GitHub connector authenticated as 327836130. Direct MaaQQFO read returned 404 (absent or inaccessible). Stable upstream v2.16.4 resolves to edfb87ac7a049a50b7921658cc206265c8b9e4e5. No existing remote was changed.

2026-11-19 audit: ProtoInput.txt retains MIT notice (Ilyaki 2021); EasyHook.txt retains permission notice (Christoph Husse / Justin Stenning); upstream client LICENSE is GPL v3. Preserve these notices when migrating. No proprietary DM binary is authorized for public redistribution. Legacy IPC protocol.h inspected: packed 237-byte fixed-width packet, version/signature/request/session validation; migration must retain x86/x64 agreement and cancellation semantics.

C2-A six raw snapshots cross-checked using tools/verify_c2a_evidence.py: both read passes agree with captured bytes for vtable, key, name, position and filter; Q membership and map anchors consistent. Three rabbits before versus two after user action. This validates historical evidence consistency only, not precise killed identity or universal monster support. C2-B and C1 raw cross-check remain pending.

C2-B: tools/verify_c2b_evidence.py validates four indexed source hashes, reconstructs Q bounds/slots and checks captured fields for every summarized object. Rabbit-fur query key 0x1528e92f appears only in on_ground stage. Exact disappearance time, stable server ID and other dropped-item classes remain unverified.
