"""Verify historical P8.3 movement and binaries without touching the game."""
import argparse
import hashlib
import json
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument('root', type=Path)
args = parser.parse_args()
rows = [json.loads(line) for line in (args.root / 'logs/player-1876.jsonl').read_text(encoding='utf-8-sig').splitlines() if line.strip()]
for start, end, expected in [(26, 38, 200), (39, 51, -200), (52, 64, 200)]:
    a, b = rows[start], rows[end]
    assert a['pid'] == b['pid'] == 6216
    assert a['player']['valid'] and b['player']['valid']
    delta = (b['player']['x'] - a['player']['x'], b['player']['y'] - a['player']['y'])
    assert delta == (expected, 0), (start, end, delta)
    print(f'historical samples {start}..{end}: delta={delta}')
for record in json.loads((args.root / 'logs/hashes.json').read_text(encoding='utf-8-sig')):
    path = args.root / 'bin' / Path(record['Path']).name
    assert hashlib.sha256(path.read_bytes()).hexdigest().upper() == record['Hash'], path.name
    print(f'historical binary SHA256 matched: {path.name}')
print('OFFLINE_PASS: historical evidence consistency only; no new game test')
