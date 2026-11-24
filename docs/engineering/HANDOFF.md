# MaaQQFO handoff

- Branch: `phase/A0-inventory`, fork `327836130/MaaQQFO-Client`, upstream MFAAvalonia tag `v2.16.4` (`edfb87ac7a049a50b7921658cc206265c8b9e4e5`).
- Resume with `git status`, ROADMAP, PROGRESS, BLOCKERS and EVIDENCE_MATRIX. Preserve the three original untracked bootstrap documents.
- A0: historical asset inventory, P3/D1 evidence, P8.3 and C1/C2 checks recorded. A1: locked Windows x64 build, 15 core tests, native Agent handshake, archive checksum and GUI start/exit verified. See STAGE_A0_REVIEW.md and STAGE_A1_REVIEW.md under docs/validation.
- [Draft PR #1](https://github.com/327836130/MaaQQFO-Client/pull/1) targets `qqfo/stable-v2.16.4`. [Windows CI](https://github.com/327836130/MaaQQFO-Client/actions/runs/44521355518) passed on `2f5340f`, with development bundle artifact 14527309277 and test results artifact 14527329931. Check the latest PR run for subsequent changes. Independent review remains pending.
- A2: run `python tools/controller_coordinate_smoke.py bin/AnyCPU/Release/runtimes/win-x64/native` using 64-bit Python. Real 5.14.3 DLL with offline callbacks verifies four screenshot modes and 16 mappings. Next: owned Windows test window, resize/DPI and lifecycle, then connect the production Custom Controller factory. Current fixture contains no game integration.
- Local pinned SDK: `../17.MaaQQFantasy/debug/dotnet-sdk/dotnet.exe`. Local x64 Python: `../17.MaaQQFantasy/.venv/Scripts/python.exe`.

The two queued commits were pushed to PR #1 through the user-provided per-command parameters `-c http.proxy=http://127.0.0.1:7890 -c http.version=HTTP/1.1`. Use these with `git` if direct GitHub HTTPS is unreachable and the proxy is running. Fresh Fork clone build and tests at `8fcf630` passed. Cross-process Agent smoke subsequently passed against the same build. See `docs/validation/a1-fork-clone.md` and check the newest PR run.

A1 additional gates: synthetic resource load and PE architecture checks passed locally. Check latest PR Windows CI jobs and artifact before stage sign-off. Independent review still has no submission.

Independent review dated 2026-11-23 accepted A0 historical inventory only. A1 B1/B2 remediation uses global.json plus tools/record_windows_build.ps1 and needs clean build/CI and independent re-review. A12_NUGET_SECURITY_GATE.md tracks B3. Keep PR #1 draft.

B1/B2 CI passed in run 45190167862. See a1-review-corrections.md and A12_NUGET_SECURITY_GATE.md. A1 remains VERIFY_PENDING. Reproduce A2 with both commands in STAGE_A2_REVIEW.md. Next: production factory/IPC and verified DPI transition. Do not schedule game input until integrated controller is ready for a one-session acceptance plan.

Managed factory smoke added to CI. Continue with a versioned and bounded IPC backend, then register the QQFO factory in MaaProcessor.InitializeController using an explicit controller selection. Test adapter is never selected automatically. Cross-monitor DPI coverage is not available on the current one-monitor setup.

A2 production path uses qqfo_input_session on a Win32 resource profile. Build ManagedControllerSmoke then run tools/owned_window_ipc_smoke.py with SDK executable, built smoke DLL and native directory. 21 core tests passed. Compatible native input host remains A3 work. Keep game-side validation pending P9 authorization.
