"""Native runtime/Agent handshake smoke; never creates a game controller.
ABI: MaaFramework v5.14.3 MaaBuffer.h and MaaAgent{Client,Server}API.h.
Run with Windows x64 Python. Parent enforces a 30-second process deadline.
"""
import argparse
import ctypes as c
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time


def bind(lib, name, result, *args):
    fn = getattr(lib, name)
    fn.restype, fn.argtypes = result, list(args)
    return fn


def serve(root, identifier):
    with os.add_dll_directory(str(root)):
        server = c.CDLL(str(root / 'MaaAgentServer.dll'))
        start = bind(server, 'MaaAgentServerStartUp', c.c_uint8, c.c_char_p)
        stop = bind(server, 'MaaAgentServerShutDown', None)
        if not start(identifier.encode('utf-8')):
            raise RuntimeError('Agent server startup failed')
        try:
            Path('agent.ready').touch(exist_ok=False)
            sys.stdin.buffer.read(1)
        finally:
            stop()


def smoke(root):
    if c.sizeof(c.c_void_p) != 8:
        raise RuntimeError('Windows x64 Python required')
    with os.add_dll_directory(str(root)):
        fw = c.CDLL(str(root / 'MaaFramework.dll'))
        client = c.CDLL(str(root / 'MaaAgentClient.dll'))
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
        make_resource = bind(fw, 'MaaResourceCreate', ptr)
        free_resource = bind(fw, 'MaaResourceDestroy', None, ptr)
        post_bundle = bind(fw, 'MaaResourcePostBundle', c.c_int64, ptr, c.c_char_p)
        wait_resource = bind(fw, 'MaaResourceWait', c.c_int32, ptr, c.c_int64)
        get_node = bind(fw, 'MaaResourceGetNodeData', boolean, ptr, c.c_char_p, ptr)
        bind_resource = bind(client, 'MaaAgentClientBindResource', boolean, ptr, ptr)
        resource = make_resource()
        handle, buffer = make_client(None), make_buffer()
        server_process = None
        try:
            assert handle and buffer and resource
            pipeline = Path('smoke-bundle/pipeline')
            pipeline.mkdir(parents=True)
            (pipeline / 'baseline.json').write_text(
                json.dumps({'QQFOBaselineSmoke': {'action': 'DoNothing'}}), encoding='utf-8'
            )
            bundle = post_bundle(resource, str(pipeline.parent.resolve()).encode('utf-8'))
            assert wait_resource(resource, bundle) == 3000
            assert get_node(resource, b'QQFOBaselineSmoke', buffer)
            print('OFFLINE_PASS: loaded synthetic Pipeline resource', flush=True)
            assert bind_resource(handle, resource)
            assert identify(handle, buffer)
            assert timeout(handle, 5000)
            identifier = get_string(buffer).decode('utf-8')
            server_process = subprocess.Popen(
                [sys.executable, str(Path(__file__).resolve()), str(root), '--serve', identifier],
                stdin=subprocess.PIPE,
            )
            deadline = time.monotonic() + 5
            while not Path('agent.ready').exists():
                if server_process.poll() is not None or time.monotonic() >= deadline:
                    raise RuntimeError('Agent server did not become ready')
                time.sleep(0.025)
            assert connect(handle) and connected(handle)
            print(f'OFFLINE_PASS: runtime {version}, native Agent connected across processes', flush=True)
            assert disconnect(handle)
            assert not connected(handle)
        finally:
            if handle:
                destroy(handle)
            if buffer:
                free_buffer(buffer)
            if resource:
                free_resource(resource)
            if server_process:
                server_process.stdin.close()
                try:
                    if server_process.wait(timeout=5) != 0:
                        raise RuntimeError('Agent server did not exit cleanly')
                except subprocess.TimeoutExpired:
                    server_process.kill()
                    server_process.wait()
                    raise
        print('OFFLINE_PASS: disconnect and cleanup; no game/UI acceptance', flush=True)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('runtime', type=Path)
    parser.add_argument('--child', action='store_true')
    parser.add_argument('--serve')
    args = parser.parse_args()
    if args.serve is not None:
        serve(args.runtime.resolve(), args.serve)
    elif args.child:
        smoke(args.runtime.resolve())
    else:
        with tempfile.TemporaryDirectory(prefix='qqfo-smoke-') as directory:
            result = subprocess.run([sys.executable, str(Path(__file__).resolve()), str(args.runtime.resolve()), '--child'], cwd=directory, timeout=30)
            raise SystemExit(result.returncode)
