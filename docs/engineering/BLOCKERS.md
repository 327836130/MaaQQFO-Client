# Blockers and pending gates

- Independent review of [draft PR #1](https://github.com/327836130/MaaQQFO-Client/pull/1) remains open. Windows baseline CI and development archive passed on `2f5340f`.
- A2 still needs a production controller adapter and owned test-window checks with resize and DPI changes. The simulated callback fixture covers native scaling only.
- Persistent Automation and first game injection/admin components require explicit authorization under P9 §7.
- Upstream NuGet vulnerability warnings remain in the build logs. Assess before release.
