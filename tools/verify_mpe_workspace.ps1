[CmdletBinding()]
param([Parameter(Mandatory)][string]$Bridge)

$ErrorActionPreference = 'Stop'
$bridgePath = (Resolve-Path -LiteralPath $Bridge).Path
if ((& $bridgePath --version) -ne 'mpelb version 2.1.3' -or $LASTEXITCODE -ne 0) {
    throw 'MPE Local Bridge 2.1.3 is required.'
}

$workspace = New-Item -ItemType Directory -Path (Join-Path $PSScriptRoot "../artifacts/a5-workspace-$([guid]::NewGuid().ToString('N'))") -Force
$resource = New-Item -ItemType Directory -Path (Join-Path $workspace.FullName 'resource/pipeline') -Force
$fixture = Join-Path $resource.FullName 'qqfo-editor-smoke.json'
'{"QQFOEditorSmoke":{"recognition":"DirectHit","action":"DoNothing"}}' |
    Set-Content -LiteralPath $fixture -Encoding utf8
$config = Join-Path $workspace.FullName 'bridge.json'
$listener = [System.Net.Sockets.TcpListener]::new([System.Net.IPAddress]::Loopback, 0)
$listener.Start()
$port = $listener.LocalEndpoint.Port
$listener.Stop()
@{
    server = @{ host = '127.0.0.1'; port = $port }
    log = @{ dir = (Join-Path $workspace.FullName 'logs') }
} | ConvertTo-Json | Set-Content -LiteralPath $config -Encoding utf8

$info = [System.Diagnostics.ProcessStartInfo]::new($bridgePath)
$info.UseShellExecute = $false
$info.CreateNoWindow = $true
$info.RedirectStandardInput = $true
$info.RedirectStandardOutput = $true
$info.RedirectStandardError = $true
$info.WorkingDirectory = $workspace.FullName
foreach ($argument in @('--portable', '--managed', '--config', $config, '--root', (Split-Path $resource.FullName))) {
    $info.ArgumentList.Add($argument)
}
$process = [System.Diagnostics.Process]::Start($info)
$out = $process.StandardOutput.ReadToEndAsync()
$err = $process.StandardError.ReadToEndAsync()
$socket = $null
$deadline = [System.Threading.CancellationTokenSource]::new([TimeSpan]::FromSeconds(15))
try {
    while ($true) {
        $deadline.Token.ThrowIfCancellationRequested()
        if ($process.HasExited) { throw 'Local Bridge exited before accepting a connection.' }
        $log = Get-ChildItem -LiteralPath (Join-Path $workspace.FullName 'logs') -Filter 'lb-*.log' -ErrorAction SilentlyContinue
        $startup = $log | Select-String -Pattern '服务器已启动，监听地址: 127\.0\.0\.1:(\d+)' | Select-Object -Last 1
        if (!$startup) {
            Start-Sleep -Milliseconds 150
            continue
        }
        $listeningPort = [int]$startup.Matches[0].Groups[1].Value
        $uri = [uri]"ws://127.0.0.1:$listeningPort"
        $socket = [System.Net.WebSockets.ClientWebSocket]::new()
        $socket.Options.Proxy = [System.Net.WebProxy]::new()
        try {
            $null = $socket.ConnectAsync($uri, $deadline.Token).GetAwaiter().GetResult()
            break
        }
        catch [System.Net.WebSockets.WebSocketException] {
            $socket.Dispose()
            $socket = $null
            Write-Verbose $_.Exception.Message
            Start-Sleep -Milliseconds 150
        }
    }
    $payload = [Text.Encoding]::UTF8.GetBytes((@{ path = '/etl/open_file'; data = @{ file_path = $fixture } } | ConvertTo-Json -Compress))
    $null = $socket.SendAsync([ArraySegment[byte]]::new($payload), [System.Net.WebSockets.WebSocketMessageType]::Text,
        $true, $deadline.Token).GetAwaiter().GetResult()
    $buf = [byte[]]::new(4096)
    do {
        $message = [IO.MemoryStream]::new()
        try {
            do {
                $frame = $socket.ReceiveAsync([ArraySegment[byte]]::new($buf), $deadline.Token).GetAwaiter().GetResult()
                if ($frame.MessageType -ne 'Text') { throw 'Expected a text message from Local Bridge.' }
                $message.Write($buf, 0, $frame.Count)
            } until ($frame.EndOfMessage)
            $value = [Text.Encoding]::UTF8.GetString($message.ToArray()) | ConvertFrom-Json
            if ($value.path -eq '/error') { throw ($value.data | ConvertTo-Json -Compress) }
        }
        finally { $message.Dispose() }
    } until ($value.path -eq '/lte/file_content')

    if ($value.data.content.QQFOEditorSmoke.action -ne 'DoNothing') { throw 'Unexpected Pipeline file content.' }
    'OFFLINE_PASS: MPE Local Bridge 2.1.3 opened the QQFO Pipeline fixture over loopback.'
}
finally {
    if ($socket) { $socket.Dispose() }
    $deadline.Dispose()
    $process.StandardInput.Close()
    if (!$process.WaitForExit(5000)) {
        $process.Kill()
        $process.WaitForExit()
    }
    $out.GetAwaiter().GetResult() | Set-Content -LiteralPath (Join-Path $workspace.FullName 'stdout.log')
    $err.GetAwaiter().GetResult() | Set-Content -LiteralPath (Join-Path $workspace.FullName 'stderr.log')
    $process.Dispose()
}
