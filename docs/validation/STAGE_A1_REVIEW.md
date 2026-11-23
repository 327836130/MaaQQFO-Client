# A1 review record

The Windows x64 development baseline uses MFAAvalonia v2.16.4 with the native 5.14.3 runtime that the upstream client declares. SDK 10.0.402 and NuGet package versions are pinned in CI and the lock files. See [dependency baseline](../engineering/DEPENDENCY_BASELINE.md).

- [Clean-source build](a1-clean-result.json) built from a Git archive with no existing bin/obj. It reused the installed SDK and NuGet cache.
- [Core tests](a1-core-tests.log) passed 15 checks. [Native smoke](a1-native-smoke.log) verified the DLL version and same-process Agent connection, disconnect and cleanup.
- [Desktop startup](a1-desktop-startup.md) observed the main window and exit code 0.
- [PR #1](https://github.com/327836130/MaaQQFO-Client/pull/1) tracks review. The [first Windows CI run](https://github.com/327836130/MaaQQFO-Client/actions/runs/44520529533) passed for aa8f079. Assess the latest run before merging.

The CI packaging step creates a framework-dependent development ZIP and SHA256 from a fresh build. The embedded SHA identifies the Git tree actually checked out and built by Actions. The package uses the installed .NET 10 runtime. It is intended for baseline review. A fresh remote clone passed the locked build and tests. A separate-process smoke verified the bundled native Agent protocol. Custom C++ QQFO Agent, game integration, vulnerability remediation and independent review remain open. See [clone evidence](a1-fork-clone.md).

Independent review of b2402a7 found that CI actually selected SDK 11.0.100 and the development ZIP named the Actions test merge SHA without complete PR provenance. Superseding evidence must verify the exact SDK assertion, both PR merge parents and checkout SHA, and matching build/test artifact metadata. See the dependency baseline for reproduction. A1 remains VERIFY_PENDING until independent re-review.
