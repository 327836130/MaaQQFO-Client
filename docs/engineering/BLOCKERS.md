# Blockers and pending gates
- Remote: connector cannot create a Fork. No gh executable found in PATH or standard locations; next try portable official CLI and inspect existing login without printing credentials.
- Authorization: persistent Automation not enabled; first live injection/admin components need explicit authorization per P9 §7.
- Release risk: upstream NuGet vulnerability warnings retained in build logs; assess before release.
- Not blockers: SDK 10.0.402 and CMake found; P8.3 and C1/C2 artifacts located. Evidence review and baseline startup are outstanding engineering work.

Remote action now requires user CLI login or manual Fork. Portable CLI: .tmp-sdk/gh/bin/gh.exe auth login. Connector authentication does not authenticate CLI. Do not access credential stores or create new credentials without approval.
