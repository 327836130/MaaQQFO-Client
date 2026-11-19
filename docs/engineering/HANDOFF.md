# MaaQQFO handoff
- Updated: 2026-11-19 +08:00
- Phase: A0 IN_PROGRESS / A1 IN_PROGRESS.
- Branch: phase/A0-inventory; upstream v2.16.4 edfb87ac7a049a50b7921658cc206265c8b9e4e5.
- Resume first: git status, then ROADMAP/PROGRESS/BLOCKERS/EVIDENCE_MATRIX.
- Completed: core/desktop/locked builds, 15 configuration tests, historical P8.3 verification, native v5.14.3 Agent resource binding/connect/disconnect/cleanup smoke. No active build process.
- Next primary task: complete C1/C2 raw snapshot checks and startup verification. Query user-provided corrected MFAAvalonia Fork when available, then publish review branch/PR without modifying existing repositories.
- Remote finding: 327836130/MaaQQFO exists but parent is MaaXYZ/MaaFramework (repository ID 1418945258); not MFAAvalonia. User asked to create a separate MFAAvalonia Fork, suggested MaaQQFO-Client. Do not overwrite or rename wrong-parent fork.
- Validation: native_smoke.py requires x64 Python. Local usable interpreter ../17.MaaQQFantasy/.venv/Scripts/python.exe. SDK ../17.MaaQQFantasy/debug/dotnet-sdk/dotnet.exe.
- First smoke failed due to missing resource binding and timeout; preserved in a1-native-smoke-first-failure.log. Fixed test binds a resource and avoids redundant Join (ShutDown already joins in official source).
- Remaining: clean-room build, GUI startup, independent review, remote CI/PR, complete migration/license audit. Native same-process handshake does not prove custom C++ Agent business behavior or live controller.
- User bootstrap copies at workspace root remain untracked and preserved. No game injection, persistent automation or credential changes authorized.

Latest checkpoint: clean-source build of 0b1ab07 completed; result docs/validation/a1-clean-result.json. C2-A six-snapshot byte validation PASS. Next primary task: C1/C2-B raw verification and GUI startup. Clean build is source-isolated with reused SDK/NuGet cache, not a new machine. Corrected Fork reply remains pending. Scripts tools/verify_clean_build.ps1 and tools/verify_c2a_evidence.py are reproducible entry points.

Latest: C2-B raw verification now PASS (four historical captures), evidence a0-c2b-verification.txt. Next primary task C1 byte-level audit and GUI startup. Clean source build is already complete; do not rerun without changes requiring it. Existing remote unchanged, corrected Fork pending.

Latest checkpoint: origin 327836130/MaaQQFO-Client forks MaaXYZ/MFAAvalonia. Draft PR #1 targets qqfo/stable-v2.16.4 from phase/A0-inventory. GUI startup evidence in docs/validation/a1-desktop-startup.md. Next primary task is check CI and finish C1 verification.

C1 raw verification complete. Resume by checking the latest PR #1 Windows run against current branch HEAD and completing legacy route/editor provenance review. Existing bootstrap files remain untracked.
