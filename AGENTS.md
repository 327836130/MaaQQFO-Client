# MaaQQFO Codex Project Instructions

## Authority and hierarchy
- Authoritative project spec: `docs/architecture/MaaQQFO_总体架构设计方案_V1.0_20261118.md`.
- Active implementation brief: `docs/engineering/MaaQQFO_Codex_总开发任务书_P9_20261118.md`.
- Preserve user-approved architecture; change only with explicit user decision or a documented compatibility need.
- On every resume, first inspect `git status`, `docs/engineering/ROADMAP.md`, `PROGRESS.md`, `HANDOFF.md`, `BLOCKERS.md`, and `docs/validation/EVIDENCE_MATRIX.md` if they exist.

## Core architecture
- MaaFramework is the only top-level Pipeline orchestrator.
- Use one C++ QQFO Agent entry with modular Custom Recognition and Custom Action registration; do not impose one agent executable per game role.
- Custom Controller handles input, screenshot and connection; Proto Input from P8.3 is the baseline backend for background left click.
- GameStateReader is read-only and scoped per target `qqfo.exe` process/session; modules in the same session may reuse validated snapshots; no mutable cross-PID state.
- Never conflate `WorldPosition`, `ClientPosition`, `MaaImagePosition`, and `ScreenPosition`; do not apply an extra scale if MaaFramework already performs it.
- Maintain role-first UI, stable identity evidence, PID/HWND fallback, and session invalidation on reconnect.
- Keep MFAAvalonia as trackable upstream fork and use stable, compatible MaaFramework/Agent versions. Preserve historical verified code and evidence.

## Operating pattern
- Execute roadmap phases A0 through A12; deliver real implementation, test logs, CI and PR evidence at each phase, not only planning text.
- Commit small auditable changes; stop and fix failing tests before advancing.
- Do not call an offline simulation a Windows live QQ幻想 PASS.
- When the usage limit or session ends, checkpoint committed progress and exact resume instructions in HANDOFF.md. Don't bypass quotas or purchase credits without permission.
- If a test needs user interaction, prepare a one-click test and ask for the minimum specific action; otherwise continue with independent work.
- Never delete/rename existing remote repositories, force push, modify spending or secrets, or enable persistent local automations without explicit authorization.
- GitHub PR + automated tests + independent review are the normal second-review path. This agent cannot directly invoke the user's existing ChatGPT conversation.

## Completion
- A stage is PASS only when evidence links to actual code, tests, logs, screenshots or live test results.
- A final full release requires a reproducible Windows package, user guide, version/commit mapping, feature matrix, and live controller/route tests.
