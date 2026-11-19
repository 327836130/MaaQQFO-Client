# Evidence matrix
| Evidence | Result | Scope |
|---|---|---|
| docs/migration/legacy-assets.json | PARTIAL | SHA256 inventory of selected legacy file types, not functional validation |
| 17.MaaQQFantasy/host_support/native_dm32/controller.cpp | PARTIAL | Source exists; review and build pending |
| dotnet --info | BLOCKED_TECH | Runtime exists, SDK absent |
All Windows live acceptance remains pending.

| tools/verify_p83_evidence.py + a0-p83-verification.txt | OFFLINE_PASS | historical A/B/C positions and four binary SHA256 values; not new live acceptance |
| MFAAvalonia/MFAAvalonia.csproj | VERIFIED | official tag runtime 5.14.3, binding 5.10.0, agent binary 1.2.0 |

Desktop compilation: a1-desktop-build.log, exit 0, 34 warnings. Core configuration tests: a1-core-tests.log, 15/15 PASS. Neither proves live game or Agent protocol acceptance. SDK absence entry above is superseded by DEPENDENCY_BASELINE.md.

Native runtime: tools/native_smoke.py + a1-native-smoke.log OFFLINE_PASS for loaded runtime v5.14.3 and same-process resource-bound Agent handshake/cleanup. First failure retained separately. No game controller constructed. Official ABI/source: https://github.com/MaaXYZ/MaaFramework/tree/v5.14.3/include and source/MaaAgentServer/Server/AgentServer.cpp.

C2-A: a0-c2a-verification.txt OFFLINE_PASS (6 snapshots, two passes, raw field bytes/map/membership). Clean source build: a1-clean-result.json documents commit and exit codes; uses existing SDK and NuGet cache, no pre-existing bin/obj.
