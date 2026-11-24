"""Run the managed production backend against an owned window and a test pipe peer."""
import argparse
import ctypes as c
from pathlib import Path
import subprocess
import tempfile
import time

from owned_window_controller_smoke import OwnedWindow, bind, require


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('dotnet', type=Path)
    parser.add_argument('smoke_dll', type=Path)
    parser.add_argument('runtime', type=Path)
    args = parser.parse_args()
    user = c.WinDLL('user32', use_last_error=True)
    set_dpi = bind(user, 'SetThreadDpiAwarenessContext', c.c_void_p, c.c_void_p)
    previous = set_dpi(c.c_void_p(-4))
    require(previous, 'Unable to set Per-Monitor V2 DPI awareness')
    try:
        window = OwnedWindow(user)
        try:
            with tempfile.TemporaryDirectory(prefix='qqfo-owned-ipc-') as directory:
                command = [str(path.resolve()) for path in (args.dotnet, args.smoke_dll, args.runtime)]
                with subprocess.Popen([*command, str(window.hwnd)], cwd=directory) as process:
                    deadline = time.monotonic() + 25
                    try:
                        while process.poll() is None:
                            window.pump()
                            require(time.monotonic() < deadline, 'Owned-window IPC smoke timed out')
                            time.sleep(0.01)
                        require(process.returncode == 0, 'Managed controller smoke failed')
                        require(not window.events, 'Test IPC peer unexpectedly delivered real input')
                    finally:
                        if process.poll() is None:
                            process.kill()
        finally:
            window.close()
    finally:
        set_dpi(previous)


if __name__ == '__main__':
    main()
