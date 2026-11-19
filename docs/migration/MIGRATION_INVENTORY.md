# A0 migration inventory

The [file manifests](../validation/EVIDENCE_MATRIX.md) record source provenance. VERIFIED below applies only to the stated historical evidence.

| Asset | Status | Source and result | Open gate |
|---|---|---|---|
| P1/P3 Controller and IPC | VERIFIED | `17.MaaQQFantasy/host_support/native_dm32/controller.cpp`, `common/protocol.h`, `docs/native-dm-p3-acceptance.md` and `release/p3-controller-plugin-accepted-20261017`. The old package checksum utility verified all 335 entries. | New ABI, session lifecycle and compatible backend require A2/A3 validation. |
| FramePool and DPI | PARTIAL | P3 acceptance report records FramePool capture and DPI. `docs/maadebugger-dpi.md` and `docs/maafw-input-mapping.md` document the old scaling path. | Validate the coordinate contract with the new runtime. |
| Editor and input test pages | PARTIAL | `17.MaaQQFantasy/business/controller_input_tests/docs/mpe-validation.md` records MPE Desktop 2.1.0 with built-in MFW 5.14.3. `business/controller_input_tests` contains the input fixture and documentation. | Historical import, editing, saving and reloading were inconclusive. Check current editor in A5. |
| Proto Input | VERIFIED | `2026.11.08-qqfo解析/P8.3` records historical A/B/C movement and normal removal. Rechecked three deltas and four binary hashes. | Integrated Click, cleanup and multi-window behavior need new tests. |
| Player, map, NPC, monster, item | VERIFIED | `2026.11.08-qqfo解析/cheatengine-mcp-bridge-main/research/P7_1_Map_Research/scene_objects` C1/C2 raw bytes rechecked. Samples cover local role, NPC, one monster type and one item type. | Stable role ID and generic entity coverage remain unverified. |
| D1 navigation workbench | PARTIAL | `2026.05.27-codex控制/P8.0_QQFantasy_NavigationWorkbench` contains C++ reader, CTest results and CE samples. Its `scripts/check_evidence.py` passed against the saved reports. | D1 ordinary-privilege live OpenProcess returned error 5. No live native-read or route PASS. |
| Route calibration | PARTIAL | `1、寻路导航测试/calibrate_A.py` and `calibration_params.json` exist. | Calibration is historical. World/client projection and a compatible reusable route still need validation. |
| Pipeline/image | PARTIAL | `17.MaaQQFantasy/resource/base/pipeline` (e.g. `native_dm_test.json`, `dm_pause_ocr_test.json`) and `resource/base/image`; selected file hashes recorded. | Validate new resource loading and rights to redistribute images before import. |
| Permanent role identity | PLAN | Architecture §4. Current session key and addresses are observational fields. | Verify stable ID across restart and relogin. |

P8.3 pins Proto Input `dd29bf8d22349605d0e1950c1c5dc4893d72670e` and EasyHook 2.7.7097 x86. The source notices are present in `P8.3/licenses`; retain them if migrating code. MFAAvalonia supplies GPL v3 text. Proprietary DM binaries and legacy game image redistribution rights were not established. Old DM IPC uses a 237-byte fixed-width wire packet with signature, version, request and session checks.

GitHub metadata confirms `327836130/MaaQQFO-Client` forks `MaaXYZ/MFAAvalonia`. PR #1 compares against a branch at `v2.16.4`, commit `edfb87ac7a049a50b7921658cc206265c8b9e4e5`. The separate `327836130/MaaQQFO` repository forks `MaaXYZ/MaaFramework`.
