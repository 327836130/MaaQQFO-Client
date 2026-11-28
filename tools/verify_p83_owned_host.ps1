[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$ArchiveBin,
    [Parameter(Mandatory)][string]$Dotnet,
    [string]$Runtime = 'bin/AnyCPU/Release/runtimes/win-x64/native'
)

$ErrorActionPreference = 'Stop'
$archive = (Resolve-Path -LiteralPath $ArchiveBin).Path
$sdk = (Resolve-Path -LiteralPath $Dotnet).Path
$runtimePath = (Resolve-Path -LiteralPath $Runtime).Path
$repo = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$manifest = Get-Content -LiteralPath (Join-Path $repo 'docs/migration/p83-assets.json') -Raw | ConvertFrom-Json
$fixture = Join-Path $repo 'artifacts/a3-native/p83-owned.exe'
if (!(Test-Path -LiteralPath $fixture)) {
    throw 'Build the x86 fixture with tools/verify_native_input_protocol.cmd x86 first.'
}

$run = New-Item -ItemType Directory -Path (Join-Path $repo "artifacts/a3-p83-host-$([guid]::NewGuid().ToString('N'))")
Copy-Item -LiteralPath $fixture -Destination $run.FullName
foreach ($name in 'p82.exe', 'P82Hook.dll', 'EasyHook32.dll') {
    $source = Join-Path $archive $name
    $digest = ($manifest | Where-Object path -eq "bin/$name").sha256
    if ((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $digest) {
        throw "Historical fixture hash mismatch for $name."
    }
    Copy-Item -LiteralPath $source -Destination $run.FullName
}

Push-Location $repo
try {
    $project = 'QQFO/Tests/ManagedControllerSmoke/ManagedControllerSmoke.csproj'
    $hostPath = Join-Path $run.FullName 'p83-owned.exe'
    & $sdk run --project $project -c Release -- $runtimePath --p83-owned-fixture $hostPath 2>&1 |
        Tee-Object -FilePath (Join-Path $run.FullName 'result.log')
    if ($LASTEXITCODE) { throw "P8.3 owned fixture failed. See $($run.FullName)." }
    "Fixture log: $($run.FullName)"
} finally {
    Pop-Location
}
