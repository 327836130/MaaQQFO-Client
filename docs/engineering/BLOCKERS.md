# Blockers and pending gates

- Independent review accepted A0 within its historical scope. B1/B2 corrections in [draft PR #1](https://github.com/327836130/MaaQQFO-Client/pull/1) passed CI 45190167862 and require independent A1 re-review.
- A2 passed owned-window Custom Controller lifecycle, resize and capture/click on GDI and FramePool at 120 DPI. Production client factory/IPC passed with a test peer. Native host integration and cross-monitor DPI transitions remain.
- Persistent Automation and first game injection/admin components require explicit authorization under P9 §7.
- B3 remains open at the [A12 NuGet security gate](A12_NUGET_SECURITY_GATE.md).
- A4 role name, map and stable ID need validated per-session state evidence. Window fallback and shared test entry points can proceed independently.
