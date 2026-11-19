"""Native runtime/Agent handshake smoke; never creates a game controller.
ABI: MaaFramework v5.14.3 MaaBuffer.h and MaaAgent{Client,Server}API.h.
Run with Windows x64 Python. Parent enforces a 30-second process deadline.
"""
import argparse
import ctypes as c
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def smoke(root):
    if c.sizeof(c.c_void_p) != 8:
        raise RuntimeError('Windows x64 Python required')
    with os.add_dll_directory(str(root)):
        fw = c.CDLL(str(root / 'MaaFramework.dll'))
        client = c.CDLL(str(root / 'MaaAgentClient.dll'))
        server = c.CDLL(str(root / 'MaaAgentServer.dll'))
        def bind(lib, name, result, *args):
            fn = getattr(lib, name)
            fn.restype, fn.argtypes = result, list(args)
            return fn
        ptr, boolean = c.c_void_p, c.c_uint8
        version = bind(fw, 'MaaVersion', c.c_char_p)().decode()
        if version != 'v5.14.3' and version != '5.14.3':
            raise RuntimeError(f'Unexpected runtime: {version}')
        make_buffer = bind(fw, 'MaaStringBufferCreate', ptr)
        free_buffer = bind(fw, 'MaaStringBufferDestroy', None, ptr)
        get_string = bind(fw, 'MaaStringBufferGet', c.c_char_p, ptr)
        make_client = bind(client, 'MaaAgentClientCreateV2', ptr, ptr)
        destroy = bind(client, 'MaaAgentClientDestroy', None, ptr)
        identify = bind(client, 'MaaAgentClientIdentifier', boolean, ptr, ptr)
        timeout = bind(client, 'MaaAgentClientSetTimeout', boolean, ptr, c.c_int64)
        connect = bind(client, 'MaaAgentClientConnect', boolean, ptr)
        connected = bind(client, 'MaaAgentClientConnected', boolean, ptr)
        disconnect = bind(client, 'MaaAgentClientDisconnect', boolean, ptr)
        start = bind(server, 'MaaAgentServerStartUp', boolean, c.c_char_p)
        stop = bind(server, 'MaaAgentServerShutDown', None)
        make_resource = bind(fw, 'MaaResourceCreate', ptr)
        free_resource = bind(fw, 'MaaResourceDestroy', None, ptr)
        bind_resource = bind(client, 'MaaAgentClientBindResource', boolean, ptr, ptr)
        resource = make_resource()
        handle, buffer, started = make_client(None), make_buffer(), False
        try:
            assert handle and buffer and resource
            assert bind_resource(handle, resource)
            assert identify(handle, buffer)
            assert timeout(handle, 5000)
            started = bool(start(get_string(buffer)))
            assert started
            assert connect(handle) and connected(handle)
            print(f'OFFLINE_PASS: runtime {version}, native Agent connected', flush=True)
            assert disconnect(handle)
            assert not connected(handle)
        finally:
            if started:
                stop()
            if handle:
                destroy(handle)
            if buffer:
                free_buffer(buffer)
            if resource:
                free_resource(resource)
        print('OFFLINE_PASS: disconnect and cleanup; no game/UI acceptance', flush=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runtime', type=Path)
    parser.add_argument('--child', action='store_true')
    args = parser.parse_args()
    if args.child:
        smoke(args.runtime.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='qqfo-smoke-') as directory:
            result = subprocess.run([sys.executable, str(Path(__file__).resolve()), str(args.runtime.resolve()), '--child'], cwd=directory, timeout=30)
            raise SystemExit(result.returncode)


