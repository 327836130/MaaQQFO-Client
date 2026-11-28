param(
    [Parameter(Mandatory=$true)][string]$DotNet,
    [Parameter(Mandatory=$true)][string]$Python
)
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$DotNet = (Resolve-Path -LiteralPath $DotNet).Path
$Python = (Resolve-Path -LiteralPath $Python).Path
$revision = (& git -C $repo rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'Cannot resolve HEAD' }
$run = Join-Path $repo ('artifacts/clean-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $run | Out-Null
$archive = Join-Path $run 'source.zip'
& git -C $repo archive --format=zip --output=$archive $revision
if ($LASTEXITCODE -ne 0) { throw 'Archive failed' }
$source = Join-Path $run 'source'
Expand-Archive -LiteralPath $archive -DestinationPath $source
$results = [ordered]@{ commit=$revision; source=$source; cache='Existing user NuGet cache; no pre-existing bin/obj'; checks=@() }
Push-Location $source
try {
    & $DotNet build MFAAvalonia.Desktop/MFAAvalonia.Desktop.csproj -c Release -r win-x64 -p:RestorePackagesWithLockFile=true -p:RestoreLockedMode=true *> (Join-Path $run 'build.log')
    $results.checks += @{ name='locked-desktop-build'; exit=$LASTEXITCODE }
    if ($LASTEXITCODE -ne 0) { throw 'Build failed' }
    & $DotNet test MFAAvalonia.Core.Tests/MFAAvalonia.Core.Tests.csproj -c Release *> (Join-Path $run 'tests.log')
    $results.checks += @{ name='core-tests'; exit=$LASTEXITCODE }
    if ($LASTEXITCODE -ne 0) { throw 'Tests failed' }
    & $Python tools/native_smoke.py bin/AnyCPU/Release/runtimes/win-x64/native *> (Join-Path $run 'native.log')
    $results.checks += @{ name='native-agent-smoke'; exit=$LASTEXITCODE }
    if ($LASTEXITCODE -ne 0) { throw 'Native smoke failed' }
} finally {
    Pop-Location
    $results | ConvertTo-Json -Depth 5 | Set-Content -Encoding utf8 (Join-Path $run 'result.json')
    Write-Output "Evidence: $run"
}
