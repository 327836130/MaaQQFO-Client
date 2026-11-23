# Dependency baseline
Upstream: MaaXYZ/MFAAvalonia v2.16.4, edfb87ac7a049a50b7921658cc206265c8b9e4e5.

Observed source dependencies: Maa.Framework 5.10.0 (managed binding), Maa.Framework.Runtimes 5.14.3, Maa.AgentBinary 1.2.0. SDK used: 10.0.402 x64. Package lock files preserve resolved NuGet versions and content hashes.

P9 target 5.16.1 is not installed over this baseline. Same-process native Agent handshake passed on 5.14.3. Custom Agent process compatibility remains pending. Upstream project assembly metadata is 2.14.0, so release mapping must use the commit/tag, not infer tag from assembly version.

Core library build: docs/validation/a1-build.log, exit 0, 23 warnings. Warnings include NuGet vulnerability advisories and existing source warnings; do not treat this as release clearance. Desktop build and native runtime/Agent smoke have separate acceptance gates.

B1/B2 correction. The old CI run 45186466211 selected 11.0.100 despite installing 10.0.402. The repository now disables SDK roll-forward in global.json, and tools/record_windows_build.ps1 asserts the actual SDK before building. For pull_request, it compares checkout parents to the event base and head. The pr-merge ZIP names the tested merge SHA and embeds MaaQQFO-build-provenance.json. The TRX artifact carries the same metadata. Manual or local branch builds use a commit label and null PR fields. The A12_NUGET_SECURITY_GATE.md ledger tracks reported advisories.
