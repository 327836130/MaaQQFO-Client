"""Read-only, streaming asset manifest. No source contents are copied."""
import argparse
import hashlib
import json
import os
from pathlib import Path

SKIP = {'.git', 'node_modules', '.venv', '__pycache__'}

def inventory(root):
    if not root.is_dir():
        raise ValueError(f'Not a directory: {root}')
    rows = []
    def failed(error):
        rows.append({'path': str(error.filename), 'error': str(error)})
    for directory, folders, files in os.walk(root, followlinks=False, onerror=failed):
        folders[:] = sorted(d for d in folders if d not in SKIP and not (Path(directory) / d).is_symlink())
        for name in sorted(files):
            path = Path(directory) / name
            row = {'path': path.relative_to(root).as_posix()}
            if path.is_symlink():
                row['skipped'] = 'symbolic link'
            else:
                try:
                    before = path.stat()
                    digest = hashlib.sha256()
                    with path.open('rb') as stream:
                        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
                            digest.update(chunk)
                    after = path.stat()
                    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
                        raise OSError('file changed during hashing')
                    row.update(bytes=after.st_size, sha256=digest.hexdigest())
                except OSError as error:
                    row['error'] = str(error)
            rows.append(row)
    return sorted(rows, key=lambda row: row['path'])

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('root', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    root, output = args.root.resolve(), args.output.resolve()
    if root == output or root in output.parents:
        parser.error('output must be outside source tree')
    rows = inventory(root)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(rows, ensure_ascii=False, indent=2), encoding='utf-8')
    errors = sum('error' in row for row in rows)
    print(f'{len(rows)} assets; {errors} errors; no source content copied')
    raise SystemExit(bool(errors))

