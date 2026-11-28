# A2 same-monitor display scale retest

2026-11-25. Both rounds used source `d7c9aed04b9f4d8a4df7c8c5f0e32e5f4683a30f` and MaaFramework 5.14.3. The user changed Windows display scaling between rounds. The QQ幻想 process and window handle matched across both captures.

| Check | 150% | 125% |
|---|---|---|
| Per-Monitor V2 owned test window | 144 DPI | 120 DPI |
| QQ幻想 window-reported DPI | 96 DPI | 96 DPI |
| QQ幻想 physical client and raw GDI frame | 2400×1350 | 2000×1125 |
| Owned GDI and FramePool raw/scaled frames | 800×600 / 640×480 | 800×600 / 640×480 |
| Owned Custom Controller | Both backends passed resize, release/reconnect and eight observed clicks per backend | Same |
| Production client with test pipe peer | MaaImage (320, 240) → Client (400, 300) | Same |

The live QQ幻想 capture checked successful raw GDI frame acquisition and exact dimension agreement with the physical client rectangle. It made no input requests and persisted only window metadata and frame dimensions. Capture quality was not visually evaluated. Input in the owned-window fixture and the pipe peer result are separate evidence. These were two fresh controller runs on one monitor. Cross-monitor motion, DPI changes with a controller kept connected and real-game Click remain open.

Local records are under `artifacts/a2-dpi-baseline-20261125-225934` and `artifacts/a2-dpi-retest-20261125-230100`.
