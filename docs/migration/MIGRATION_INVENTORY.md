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
