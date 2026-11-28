# A1 desktop startup evidence

On 2026-11-19, launched MFAAvalonia.exe from the isolated Release build of commit 0b1ab07656f10ec3d42bb225bc8b0426ba8120de with no arguments. Windows Process.MainWindowHandle became nonzero (133084, PID 1956), and MainWindowTitle was `MFA 任务管理器 v2.16.4`. CloseMainWindow completed with exit code 0. This verifies a window appeared and closed cleanly. Game control and task execution were outside this check.
