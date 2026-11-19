"""Cross-check all six C2-A historical snapshots against captured raw bytes."""
import argparse
import json
import struct
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('scene_root', type=Path)
args = parser.parse_args()
paths = sorted((args.scene_root / 'snapshots').glob('C2A_*.json'))
assert len(paths) == 6, 'Expected six C2-A snapshots'
for path in paths:
    data = json.loads(path.read_text(encoding='utf-8-sig'))
    assert data['coherent'] and not data['errors']
    assert data['pid'] == 21252 and data['module']['pe_format'] == 'PE32'
    anchors = data['anchors']
    assert anchors == data['end_anchors']
    assert anchors['resource'] == 49
    assert anchors['end'] - anchors['beg'] == 4 * len(anchors['slots'])
    for pass_index, objects in enumerate(data['passes']):
        assert [obj['ptr'] for obj in objects] == anchors['slots']
        for obj in objects:
            for tag, offset, fmt, expected in [
                ('vptr', 0, '<I', (obj['vt'],)),
                ('position', 0x23ec, '<ii', (obj['x'], obj['y'])),
                ('filter', 0x241c, '<I', (obj['filter'],)),
            ]:
                records = [r for r in data['log'] if r['tag'] == tag and r['a'] == obj['ptr'] + offset]
                raw = bytes.fromhex(records[pass_index]['hex'])
                assert struct.unpack(fmt, raw) == expected
            records = [r for r in data['log'] if r['tag'] == 'key_name' and r['a'] == obj['ptr'] + 0x2274]
            raw = bytes.fromhex(records[pass_index]['hex'])
            assert struct.unpack_from('<I', raw)[0] == obj['key']
            assert raw[16:].hex() == obj['name_hex']
    rabbits = [obj for obj in data['passes'][-1] if obj['vt'] == 0x5c3cbc]
    expected_count = 2 if 'after_user_kill' in path.name else 3
    assert len(rabbits) == expected_count
    assert all(bytes.fromhex(obj['name_hex']).split(b'\0')[0].decode('gbk') == '粉粉兔' for obj in rabbits)
    print(f'{path.name}: raw bytes consistent, rabbits={len(rabbits)}')
print('OFFLINE_PASS: historical C2-A consistency; no current live test')
