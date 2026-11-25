# A5 MaaPipelineEditor integration checkpoint

Status: IN_PROGRESS.

Use the Windows amd64 environment ZIP from the [v2.1.3 release](https://github.com/kqcoxn/MaaPipelineEditor/releases/tag/v2.1.3). The ZIP SHA256 is `fe4f5ad334f6ef425353cc9f32576f1ca897788350651683356030d819f74360`. The locally installed MPE Desktop executable reported 2.1.0, so it was excluded from this test.

With the verified ZIP extracted, run `pwsh -File tools/verify_mpe_workspace.ps1 -Bridge <path-to-mpelb.exe>`. The script checks version 2.1.3, creates a synthetic resource workspace, and launches a managed Local Bridge process on loopback. It requests that process's actual bound port because the bridge can choose another port if the requested one is unavailable. The run opened the Pipeline JSON over WebSocket and read the expected node. Logs remain in the generated `artifacts/a5-workspace-*` directory. It exercised file access only.

The release bundle identifies its MaaFramework as 5.16.1. MaaQQFO currently uses 5.14.3. In the tagged source, `CreateWin32Controller` calls `maa.NewWin32Controller`; it does not select the MaaQQFO Custom Controller or its P8.3 session. Establish a shared-controller entry point before testing editor-issued clicks. Keep the framework pin unchanged during this investigation. TemplateMatch, OCR, ROI, custom recognition/action and live coordinate acceptance remain pending. [Tagged controller source](https://github.com/kqcoxn/MaaPipelineEditor/blob/v2.1.3/LocalBridge/internal/mfw/controller_manager.go)
