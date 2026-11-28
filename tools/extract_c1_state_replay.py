"""Extract the read-only C1 scene fields needed by the C++ replay."""
import argparse
import hashlib
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('snapshot', type=Path)
    parser.add_argument('replay', type=Path)
    args = parser.parse_args()
    raw = args.snapshot.read_bytes()
    manifest_path = Path(__file__).resolve().parents[1] / 'docs/migration/scene-assets.json'
    manifest = json.loads(manifest_path.read_text(encoding='utf-8-sig'))
    expected = next(entry['sha256'] for entry in manifest
                    if entry['path'] == 'snapshots/C1_qw0037_registry.json')
    if hashlib.sha256(raw).hexdigest() != expected:
        raise ValueError('C1 snapshot hash does not match the migration inventory')
    anchor = json.loads(raw.decode('utf-8-sig'))['anchor']
    if not anchor['coherent']:
        raise ValueError('C1 scene sample is incoherent')
    ranges = {
        'manager_roots': (0, 8),
        'manager': (0x12c, 88),
        'grid_header_and_adjacent': (0, 28),
        'role_head': (0, 4),
        'role_position_and_adjacent': (12, 8),
    }
    lines = [f"{anchor['module_base']} {anchor['module_size']} {anchor['player_x']} {anchor['player_y']} 37"]
    for block in anchor['blocks']:
        if block['name'] in ranges:
            offset, size = ranges.pop(block['name'])
            data = bytes.fromhex(block['hex'])[offset:offset + size]
            if len(data) != size:
                raise ValueError(f"Short C1 block: {block['name']}")
            lines.append(f"{block['address'] + offset} {data.hex()}")
    if ranges:
        raise ValueError(f'Missing C1 blocks: {sorted(ranges)}')
    args.replay.write_text('\n'.join(lines) + '\n', encoding='ascii')


if __name__ == '__main__':
    main()
