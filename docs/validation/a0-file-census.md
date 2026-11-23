# A0 file census

The metadata scan ran `tools/inventory.py --metadata-only <source-directory> <output-json>` on each root listed below. Manifests are local under `artifacts/a0-inventory-daa8b47896be4e54a96d8982f42141c0`. The tool walks regular files across extensions, records skipped links or caches and fails on read errors. The nine roots contain 26,878 records including 340 skip entries. All scans exited 0.

| Manifest | Source | Records | Skips |
|---|---|---:|---:|
| 0.json | `../1、寻路导航测试` | 4 | 2 |
| 1.json | `../3、路径脚本` | 82 | 7 |
| 2.json | `../17.MaaQQFantasy` | 26,392 | 330 |
| 3.json | `../18.MFAAvalonia` | 269 | 0 |
| 4.json | `../20.MaaDebuggerWorkspace` | 7 | 0 |
| 5.json | `../21.MPE Desktop` | 2 | 0 |
| 6.json | D1 NavigationWorkbench (migration inventory) | 35 | 0 |
| 7.json | P8.3 experiment (migration inventory) | 43 | 0 |
| 8.json | C1/C2 scene_objects (migration inventory) | 44 | 1 |

At baseline commit `b50f8b0`, `git ls-files` listed 969 tracked paths in the new workspace. Full SHA256 manifests for selected evidence remain in `docs/migration`. The metadata scans record file presence and size without reading contents. None of these counts establishes live behavior.
