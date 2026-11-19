# Dependency baseline
Upstream: MaaXYZ/MFAAvalonia v2.16.4, edfb87ac7a049a50b7921658cc206265c8b9e4e5.

Observed source dependencies: Maa.Framework 5.10.0 (managed binding), Maa.Framework.Runtimes 5.14.3, Maa.AgentBinary 1.2.0. SDK used: 10.0.402 x64. Package lock files preserve resolved NuGet versions and content hashes.

P9 target 5.16.1 is not installed over this baseline. Same-process native Agent handshake passed on 5.14.3. Custom Agent process compatibility remains pending. Upstream project assembly metadata is 2.14.0, so release mapping must use the commit/tag, not infer tag from assembly version.

Core library build: docs/validation/a1-build.log, exit 0, 23 warnings. Warnings include NuGet vulnerability advisories and existing source warnings; do not treat this as release clearance. Desktop build and native runtime/Agent smoke have separate acceptance gates.
