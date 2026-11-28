"""Exercise Maa Win32 lifecycle, screenshot and click on a window we own."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import ctypes as c
from ctypes import wintypes as w
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time

from native_smoke import bind


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


class OwnedWindow:
    def __init__(self, user):
        self.events = []
        self.name = 'MaaQQFOControllerSmoke'
        result = c.c_ssize_t
        callback = c.WINFUNCTYPE(result, w.HWND, w.UINT, w.WPARAM, w.LPARAM)
        default = bind(user, 'DefWindowProcW', result, w.HWND, w.UINT, w.WPARAM, w.LPARAM)

        @callback
        def handle_message(hwnd, message, wp, lp):
            if message == 0x0202:  # WM_LBUTTONUP
                self.events.append((c.c_int16(lp & 0xffff).value, c.c_int16((lp >> 16) & 0xffff).value))
                return 0
            return default(hwnd, message, wp, lp)

        class WindowClass(c.Structure):
            _fields_ = [('style', w.UINT), ('proc', callback), ('class_extra', c.c_int),
                        ('window_extra', c.c_int), ('instance', w.HINSTANCE), ('icon', w.HICON),
                        ('cursor', w.HANDLE), ('background', w.HBRUSH), ('menu', w.LPCWSTR), ('name', w.LPCWSTR)]

        self.proc = handle_message
        gdi = c.WinDLL('gdi32', use_last_error=True)
        kernel = c.WinDLL('kernel32', use_last_error=True)
        self.instance = bind(kernel, 'GetModuleHandleW', w.HINSTANCE, w.LPCWSTR)(None)
        self.brush = bind(gdi, 'CreateSolidBrush', w.HBRUSH, w.DWORD)(0x806040)
        self.delete_brush = bind(gdi, 'DeleteObject', w.BOOL, w.HANDLE)
        self.unregister = bind(user, 'UnregisterClassW', w.BOOL, w.LPCWSTR, w.HINSTANCE)
        self.destroy = bind(user, 'DestroyWindow', w.BOOL, w.HWND)
        self.resize = bind(user, 'SetWindowPos', w.BOOL, w.HWND, w.HWND,
                           c.c_int, c.c_int, c.c_int, c.c_int, w.UINT)
        self.peek = bind(user, 'PeekMessageW', w.BOOL, c.POINTER(w.MSG), w.HWND, w.UINT, w.UINT, w.UINT)
        self.dispatch = bind(user, 'DispatchMessageW', result, c.POINTER(w.MSG))
        register = bind(user, 'RegisterClassW', w.ATOM, c.POINTER(WindowClass))
        self.registered = False
        self.hwnd = None
        try:
            require(self.brush, 'CreateSolidBrush failed')
            window_class = WindowClass(0, self.proc, 0, 0, self.instance, None, None, self.brush, None, self.name)
            self.registered = bool(register(c.byref(window_class)))
            require(self.registered, 'RegisterClassW failed')
            create = bind(user, 'CreateWindowExW', w.HWND, w.DWORD, w.LPCWSTR, w.LPCWSTR, w.DWORD,
                          c.c_int, c.c_int, c.c_int, c.c_int, w.HWND, w.HMENU, w.HINSTANCE, c.c_void_p)
            self.hwnd = create(0x08000000, self.name, 'MaaQQFO owned test window', 0x90000000,
                               40, 40, 800, 600, None, None, self.instance, None)
            require(self.hwnd, 'CreateWindowExW failed')
        except BaseException:
            self.close()
            raise

    def pump(self):
        msg = w.MSG()
        while self.peek(c.byref(msg), None, 0, 0, 1):
            self.dispatch(c.byref(msg))

    def close(self):
        if self.hwnd:
            self.destroy(self.hwnd)
            self.hwnd = None
        if self.registered:
            self.unregister(self.name, self.instance)
            self.registered = False
            self.brush = None  # Windows owns a registered class's background brush.
        if self.brush:
            self.delete_brush(self.brush)
            self.brush = None


def smoke(runtime, screencap, custom):
    ptr, flag, i32, i64, u64 = c.c_void_p, c.c_uint8, c.c_int32, c.c_int64, c.c_uint64
    require(c.sizeof(ptr) == 8, 'Windows x64 Python required')
    user = c.WinDLL('user32', use_last_error=True)
    set_dpi = bind(user, 'SetThreadDpiAwarenessContext', ptr, ptr)
    previous_dpi = set_dpi(ptr(-4))  # PER_MONITOR_AWARE_V2
    require(previous_dpi, 'Cannot set Per-Monitor V2 awareness')
    get_dpi = bind(user, 'GetDpiForWindow', w.UINT, w.HWND)
    is_window = bind(user, 'IsWindow', w.BOOL, w.HWND)
    client_rect = bind(user, 'GetClientRect', w.BOOL, w.HWND, c.POINTER(w.RECT))
    to_screen = bind(user, 'ClientToScreen', w.BOOL, w.HWND, c.POINTER(w.POINT))
    to_client = bind(user, 'ScreenToClient', w.BOOL, w.HWND, c.POINTER(w.POINT))
    with os.add_dll_directory(str(runtime)):
        fw = c.CDLL(str(runtime / 'MaaFramework.dll'))
        require(bind(fw, 'MaaVersion', c.c_char_p)() in (b'v5.14.3', b'5.14.3'), 'Unexpected Maa runtime')
        create = bind(fw, 'MaaWin32ControllerCreate', ptr, ptr, u64, u64, u64)
        create_custom = bind(fw, 'MaaCustomControllerCreate', ptr, ptr, ptr)
        destroy = bind(fw, 'MaaControllerDestroy', None, ptr)
        set_option = bind(fw, 'MaaControllerSetOption', flag, ptr, i32, ptr, u64)
        connect = bind(fw, 'MaaControllerPostConnection', i64, ptr)
        connected = bind(fw, 'MaaControllerConnected', flag, ptr)
        wait = bind(fw, 'MaaControllerWait', i32, ptr, i64)
        capture = bind(fw, 'MaaControllerPostScreencap', i64, ptr)
        click = bind(fw, 'MaaControllerPostClick', i64, ptr, i32, i32)
        cached = bind(fw, 'MaaControllerCachedImage', flag, ptr, ptr)
        make_image = bind(fw, 'MaaImageBufferCreate', ptr)
        free_image = bind(fw, 'MaaImageBufferDestroy', None, ptr)
        image_width = bind(fw, 'MaaImageBufferWidth', i32, ptr)
        image_height = bind(fw, 'MaaImageBufferHeight', i32, ptr)
        channels = bind(fw, 'MaaImageBufferChannels', i32, ptr)
        raw = bind(fw, 'MaaImageBufferGetRawData', ptr, ptr)
        write_raw = bind(fw, 'MaaImageBufferSetRawData', flag, ptr, ptr, i32, i32, i32)
        write_uuid = bind(fw, 'MaaStringBufferSet', flag, ptr, c.c_char_p)
        window = OwnedWindow(user)

        def pump_until(condition, description):
            deadline = time.monotonic() + 8
            while time.monotonic() < deadline:
                window.pump()
                if condition():
                    return
                time.sleep(0.01)
            raise TimeoutError(description)

        def geometry(width, height):
            handle = window.hwnd
            require(window.resize(handle, None, 40, 40, width, height, 0x14), 'SetWindowPos failed')
            window.pump()
            rect = w.RECT()
            require(client_rect(handle, c.byref(rect)), 'GetClientRect failed')
            require((rect.right, rect.bottom) == (width, height), 'Unexpected physical client size')
            origin = w.POINT(0, 0)
            require(to_screen(handle, c.byref(origin)) and to_client(handle, c.byref(origin)), 'Coordinate conversion failed')
            require((origin.x, origin.y) == (0, 0), 'Client/screen origin round trip failed')
            dpi = get_dpi(handle)
            require(dpi > 0, 'GetDpiForWindow failed')
            print(f'Owned window client={width}x{height} DPI={dpi}', flush=True)
            return handle

        def wait_job(controller, job):
            future = worker.submit(wait, controller, job)
            pump_until(future.done, 'Maa controller job timed out')
            require(future.result() == 3000, f'Maa job {job} failed with status {future.result()}')

        def screenshot(controller, image, expected):
            wait_job(controller, capture(controller))
            require(cached(controller, image), 'No cached screenshot')
            width, height = image_width(image), image_height(image)
            require((width, height) == expected, f'Unexpected screenshot {width}x{height}, expected {expected}')
            count, pixels = channels(image), raw(image)
            require(count in (3, 4) and pixels, 'Unexpected screenshot pixel buffer')
            offset = ((height // 2) * width + width // 2) * count
            require(c.string_at(pixels + offset, 3) == bytes((128, 96, 64)), 'Screenshot did not contain the owned window color')

        def expect_click(controller, image_point, client_point):
            events = window.events
            before = len(events)
            wait_job(controller, click(controller, *image_point))
            pump_until(lambda: len(events) > before, 'Owned window did not receive click release')
            require(events[before:] == [client_point], f'Image point {image_point} produced {events[before:]}, expected {client_point}')

        def session(hwnd, *, resize):
            # PostMessage goes only to the HWND owned by this process.
            inner = create(hwnd, {'gdi': 1, 'framepool': 2}[screencap], 4, 4)
            controller = inner
            image = make_image()
            intermediate = None
            try:
                require(controller and image, 'Controller or image allocation failed')
                if custom:
                    intermediate = make_image()
                    require(intermediate, 'Cannot allocate raw screenshot buffer')
                    raw_size = flag(1)
                    require(set_option(inner, 3, c.byref(raw_size), c.sizeof(raw_size)), 'Inner raw option rejected')

                    def capture_client(_, output):
                        if wait(inner, capture(inner)) != 3000 or not cached(inner, intermediate):
                            return 0
                        pixels = raw(intermediate)
                        pixel_type = {3: 16, 4: 24}.get(channels(intermediate))  # CV_8UC3 / CV_8UC4
                        if not pixels or pixel_type is None:
                            return 0
                        return write_raw(output, pixels, image_width(intermediate), image_height(intermediate), pixel_type)

                    callbacks = (ptr * 21)()
                    handlers = {
                        0: c.CFUNCTYPE(flag, ptr)(lambda _: wait(inner, connect(inner)) == 3000),
                        1: c.CFUNCTYPE(flag, ptr)(lambda _: connected(inner)),
                        2: c.CFUNCTYPE(flag, ptr, ptr)(lambda _, output: write_uuid(output, b'qqfo-owned-window')),
                        3: c.CFUNCTYPE(u64, ptr)(lambda _: 0),
                        6: c.CFUNCTYPE(flag, ptr, ptr)(capture_client),
                        7: c.CFUNCTYPE(flag, i32, i32, ptr)(lambda x, y, _: wait(inner, click(inner, x, y)) == 3000),
                        19: c.CFUNCTYPE(flag, ptr)(lambda _: 1),
                    }
                    for index, handler in handlers.items():
                        callbacks[index] = c.cast(handler, ptr)
                    controller = create_custom(callbacks, None)
                    require(controller, 'MaaCustomControllerCreate failed')
                require(not connected(controller), 'New controller was unexpectedly connected')
                wait_job(controller, connect(controller))
                require(connected(controller), 'Controller did not connect')
                if resize:
                    for label, key, value, expected in (
                        ('raw', 3, flag(1), (800, 600)),
                        ('long side', 1, i32(640), (640, 480)),
                        ('expand', 8, (i32 * 2)(640, 420), (640, 480)),
                        ('short side', 2, i32(480), (640, 480)),
                    ):
                        require(set_option(controller, key, c.byref(value), c.sizeof(value)),
                                f'Screenshot {label} option rejected')
                        screenshot(controller, image, expected)
                        if label == 'raw':
                            disabled = flag(0)
                            require(set_option(controller, 3, c.byref(disabled), c.sizeof(disabled)), 'Raw mode disable failed')
                        print(f'{screencap} {label}: screenshot={expected[0]}x{expected[1]}', flush=True)
                    for image_point, client_point in [((0, 0), (0, 0)), ((639, 0), (799, 0)), ((0, 479), (0, 599)), ((639, 479), (799, 599)), ((320, 240), (400, 300))]:
                        expect_click(controller, image_point, client_point)
                    require(geometry(720, 480) == hwnd, 'Resize changed the bound HWND')
                    screenshot(controller, image, (720, 480))
                    expect_click(controller, (719, 479), (719, 479))
                    expect_click(controller, (360, 240), (360, 240))
                else:
                    raw_size = flag(1)
                    require(set_option(controller, 3, c.byref(raw_size), c.sizeof(raw_size)), 'Raw screenshot option rejected')
                    screenshot(controller, image, (720, 480))
                    expect_click(controller, (719, 479), (719, 479))
            finally:
                if controller and controller != inner:
                    destroy(controller)
                if inner:
                    destroy(inner)
                if image:
                    free_image(image)
                if intermediate:
                    free_image(intermediate)

        try:
            with ThreadPoolExecutor(max_workers=1) as worker:
                hwnd = geometry(800, 600)
                session(hwnd, resize=True)
                window.close()
                require(not is_window(hwnd), 'Old window is still registered')
                window = OwnedWindow(user)
                replacement = geometry(720, 480)
                require(replacement != hwnd, 'Expected a new window handle after replacement')
                session(replacement, resize=False)
            print(f'OWNED_WINDOW_PASS {"custom" if custom else "win32"}/{screencap}: create/connect/capture, resize, '
                  'release/destroy/recreate/connect on a new HWND and 8 observed click releases', flush=True)
        finally:
            window.close()
            set_dpi(previous_dpi)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runtime', type=Path)
    parser.add_argument('--screencap', choices=('gdi', 'framepool'), default='gdi')
    parser.add_argument('--custom', action='store_true', help='exercise the custom controller callback boundary')
    parser.add_argument('--child', action='store_true')
    args = parser.parse_args()
    if args.child:
        smoke(args.runtime.resolve(), args.screencap, args.custom)
    else:
        with tempfile.TemporaryDirectory(prefix='qqfo-owned-window-') as directory:
            command = [sys.executable, str(Path(__file__).resolve()), str(args.runtime.resolve()),
                       '--screencap', args.screencap, '--child']
            if args.custom:
                command.append('--custom')
            result = subprocess.run(command, cwd=directory, timeout=45)
            raise SystemExit(result.returncode)
