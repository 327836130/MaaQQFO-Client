"""Archive the Windows baseline from a fresh, unlaunched desktop build directory."""
import argparse
import hashlib
from pathlib import Path
import re
import struct
from zipfile import ZIP_DEFLATED, ZipFile


def pe_machine(path):
    with path.open('rb') as stream:
        dos = stream.read(64)
        if len(dos) != 64 or dos[:2] != b'MZ':
            raise ValueError(f'{path.name}: missing DOS header')
        stream.seek(struct.unpack_from('<I', dos, 0x3c)[0])
        pe = stream.read(6)
        if len(pe) != 6 or pe[:4] != b'PE\0\0':
            raise ValueError(f'{path.name}: missing PE header')
        return struct.unpack_from('<H', pe, 4)[0]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('build', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('commit')
    args = parser.parse_args()
    if not re.fullmatch('[0-9a-f]{40}', args.commit):
        parser.error('a full Git commit SHA is required')
    build, output = args.build.resolve(), args.output.resolve()
    if output == build or build in output.parents:
        parser.error('output must be outside build tree')
    required = ('MFAAvalonia.exe', 'MFAAvalonia.runtimeconfig.json',
                'runtimes/win-x64/native/MaaFramework.dll',
                'runtimes/win-x64/native/MaaAgentClient.dll',
                'runtimes/win-x64/native/MaaAgentServer.dll')
    for name in required:
        path = build / name
        if not path.is_file():
            parser.error(f'missing build output: {name}')
        if path.suffix in {'.exe', '.dll'}:
            machine = pe_machine(path)
            if machine != 0x8664:
                parser.error(f'{name}: expected x64 PE, got machine 0x{machine:04x}')
    print('Desktop executable and three native runtime/Agent DLLs verified as x64 PE')
    output.mkdir(parents=True, exist_ok=True)
    archive = output / f'MaaQQFO-win-x64-dev-{args.commit[:12]}.zip'
    digest_file = archive.with_suffix('.zip.sha256')
    if digest_file.exists():
        parser.error(f'checksum already exists: {digest_file}')
    with ZipFile(archive, 'x', compression=ZIP_DEFLATED) as package:
        for path in sorted(build.rglob('*')):
            if path.is_file():
                package.write(path, path.relative_to(build).as_posix())
        package.writestr('MaaQQFO-build.txt', f'commit={args.commit}\nplatform=win-x64\nconfiguration=Release\n')
    with ZipFile(archive) as package:
        invalid = package.testzip()
        if invalid:
            raise RuntimeError(f'Corrupt archive member: {invalid}')
    with archive.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    with digest_file.open('x', encoding='ascii') as stream:
        stream.write(f'{digest}  {archive.name}\n')
    print(f'{archive.name} SHA256 {digest}')


if __name__ == '__main__':
    main()
