# A1 clean clone evidence

Cloned `phase/A0-inventory` from the GitHub Fork at `8fcf63068c9115f954ab217d296d3629803ff5ad` into `artifacts/a1-fork-clone-6e6a408df1cf423a845f3855b020e01e/source`. Initial status was clean. SDK 10.0.402 and the existing NuGet cache were used.

- `dotnet build MFAAvalonia.Desktop/MFAAvalonia.Desktop.csproj -c Release -r win-x64 -p:RestorePackagesWithLockFile=true -p:RestoreLockedMode=true` exited 0 with 35 warnings and zero errors.
- `dotnet test MFAAvalonia.Core.Tests/MFAAvalonia.Core.Tests.csproj -c Release` passed 15/15.
- The clone's native Agent and coordinate smoke scripts passed with the built runtime.
- The revised cross-process native smoke also passed when pointed at these same cloned build outputs. See [cross-process log](a1-agent-cross-process.log).
