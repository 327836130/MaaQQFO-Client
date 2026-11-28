"""Exercise MaaFramework 5.14.3 coordinate scaling with a simulated controller."""
import argparse
import ctypes as c
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def smoke(runtime):
    ptr, boolean, i32, i64 = c.c_void_p, c.c_uint8, c.c_int32, c.c_int64
    if c.sizeof(ptr) != 8:
        raise RuntimeError('Windows x64 Python required')
    with os.add_dll_directory(str(runtime)):
        fw = c.CDLL(str(runtime / 'MaaFramework.dll'))

        def bind(name, result, *args):
            fn = getattr(fw, name)
            fn.restype, fn.argtypes = result, list(args)
            return fn

        version = bind('MaaVersion', c.c_char_p)()
        if version not in (b'v5.14.3', b'5.14.3'):
            raise RuntimeError(f'Callback layout verified for 5.14.3 only: {version!r}')
        create = bind('MaaCustomControllerCreate', ptr, ptr, ptr)
        destroy = bind('MaaControllerDestroy', None, ptr)
        option = bind('MaaControllerSetOption', boolean, ptr, i32, ptr, c.c_uint64)
        connect = bind('MaaControllerPostConnection', i64, ptr)
        connected = bind('MaaControllerConnected', boolean, ptr)
        wait = bind('MaaControllerWait', i32, ptr, i64)
        capture = bind('MaaControllerPostScreencap', i64, ptr)
        click = bind('MaaControllerPostClick', i64, ptr, i32, i32)
        cached = bind('MaaControllerCachedImage', boolean, ptr, ptr)
        set_uuid = bind('MaaStringBufferSet', boolean, ptr, c.c_char_p)
        make_image = bind('MaaImageBufferCreate', ptr)
        free_image = bind('MaaImageBufferDestroy', None, ptr)
        width = bind('MaaImageBufferWidth', i32, ptr)
        height = bind('MaaImageBufferHeight', i32, ptr)
        set_image = bind('MaaImageBufferSetRawData', boolean, ptr, ptr, i32, i32, i32)

        def require(condition, check):
            if not condition:
                raise RuntimeError(check)

        def case(label, key, value, expected_size, points):
            clicks = []
            pixels = (c.c_uint8 * (1600 * 900 * 3))()
            state = {'connected': False}

            def on_connect(_):
                state['connected'] = True
                return 1

            def on_click(x, y, _):
                clicks.append((x, y))
                return 1

            # Full slot order from v5.14.3 MaaCustomController.h. The exercise
            # only uses connection, feature flags, capture and basic click.
            callbacks = (ptr * 21)()
            handlers = {
                0: c.CFUNCTYPE(boolean, ptr)(on_connect),
                1: c.CFUNCTYPE(boolean, ptr)(lambda _: state['connected']),
                2: c.CFUNCTYPE(boolean, ptr, ptr)(lambda _, buffer: set_uuid(buffer, label.encode('ascii'))),
                3: c.CFUNCTYPE(c.c_uint64, ptr)(lambda _: 0),
                6: c.CFUNCTYPE(boolean, ptr, ptr)(lambda _, image: set_image(image, pixels, 1600, 900, 16)),
                7: c.CFUNCTYPE(boolean, i32, i32, ptr)(on_click),
                19: c.CFUNCTYPE(boolean, ptr)(lambda _: 1),
            }
            for index, callback in handlers.items():
                callbacks[index] = c.cast(callback, ptr)
            handle = create(callbacks, None)
            image = make_image()
            try:
                require(handle and image, f'{label}: allocation failed')
                require(option(handle, key, c.byref(value), c.sizeof(value)), f'{label}: option rejected')
                require(wait(handle, connect(handle)) == 3000 and connected(handle), f'{label}: connect failed')
                require(wait(handle, capture(handle)) == 3000 and cached(handle, image), f'{label}: capture failed')
                require((width(image), height(image)) == expected_size, f'{label}: dimensions {(width(image), height(image))}')
                for point, expected in points:
                    require(wait(handle, click(handle, *point)) == 3000, f'{label}: click failed')
                    require(clicks[-1:] == [expected], f'{label}: {point} -> {clicks[-1:]} expected {expected}')
                print(f'OFFLINE_PASS {label}: screenshot {expected_size}, {len(points)} simulated click mappings', flush=True)
            finally:
                if handle:
                    destroy(handle)
                if image:
                    free_image(image)

        scaled_points = [((0, 0), (0, 0)), ((1279, 0), (1599, 0)), ((0, 719), (0, 899)),
                         ((1279, 719), (1599, 899)), ((640, 360), (800, 450))]
        case('display_long_side', 1, i32(1280), (1280, 720), scaled_points)
        case('display_short_side', 2, i32(720), (1280, 720), scaled_points)
        case('display_raw', 3, boolean(1), (1600, 900), [((1599, 899), (1599, 899))])
        case('display_expand', 8, (i32 * 2)(1000, 720), (1280, 720), scaled_points)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runtime', type=Path)
    parser.add_argument('--child', action='store_true')
    args = parser.parse_args()
    if args.child:
        smoke(args.runtime.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='qqfo-controller-') as directory:
            result = subprocess.run([sys.executable, str(Path(__file__).resolve()), str(args.runtime.resolve()), '--child'], cwd=directory, timeout=30)
            raise SystemExit(result.returncode)
