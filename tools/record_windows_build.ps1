param(
    [Parameter(Mandatory)][string]$OutputPath,
    [string]$DotNet = 'dotnet'
)

$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$output = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutputPath)
Push-Location -LiteralPath $repo
try {
    $expectedSdk = (Get-Content global.json -Raw | ConvertFrom-Json).sdk.version
    $sdk = & $DotNet --version
    if ($LASTEXITCODE -ne 0 -or $sdk -ne $expectedSdk) {
        throw "Expected .NET SDK $expectedSdk, resolved '$sdk'."
    }
    Write-Output "Actual .NET SDK: $sdk"
    $checkout = git rev-parse HEAD
    if ($LASTEXITCODE -ne 0 -or $checkout -notmatch '^[0-9a-f]{40}$') { throw 'Cannot resolve checkout SHA' }
    $kind = 'branch_commit'
    $prHead = $null
    $prBase = $null
    $prNumber = $null
    if ($env:GITHUB_ACTIONS -eq 'true') {
        if ($checkout -ne $env:GITHUB_SHA) { throw 'Checkout does not match GITHUB_SHA' }
        if ($env:GITHUB_EVENT_NAME -eq 'pull_request') {
            $event = Get-Content -LiteralPath $env:GITHUB_EVENT_PATH -Raw | ConvertFrom-Json
            $prHead = $event.pull_request.head.sha
            $prBase = $event.pull_request.base.sha
            $prNumber = $event.number
            $parents = (git show -s --format=%P HEAD) -split ' '
            if ($LASTEXITCODE -ne 0 -or $parents.Count -ne 2 -or
                $parents[0] -ne $prBase -or $parents[1] -ne $prHead) {
                throw 'Checkout is not the merge of the recorded PR base and head'
            }
            $kind = 'pr_test_merge'
        }
    }
    [xml]$project = Get-Content MFAAvalonia/MFAAvalonia.csproj -Raw
    $versions = @{}
    foreach ($dependency in $project.SelectNodes('//PackageReference')) {
        $versions[$dependency.Include] = $dependency.Version
    }
    $record = [ordered]@{
        schema_version = 1
        source_kind = $kind
        checkout_sha = $checkout
        pr_head_sha = $prHead
        pr_base_sha = $prBase
        pull_request = $prNumber
        platform = 'win-x64'
        configuration = 'Release'
        dotnet_sdk = $sdk
        maa_framework_runtime = $versions['Maa.Framework.Runtimes']
        maa_managed_binding = $versions['Maa.Framework']
        maa_agent_binary_package = $versions['Maa.AgentBinary']
        workflow_run_id = $env:GITHUB_RUN_ID
        workflow_run_attempt = $env:GITHUB_RUN_ATTEMPT
    }
    $json = $record | ConvertTo-Json
    New-Item -ItemType Directory -Path (Split-Path -Parent $output) -Force | Out-Null
    Set-Content -LiteralPath $output -Value $json -Encoding utf8
    Write-Output $json
} finally {
    Pop-Location
}
