"""Validate historical C2-B pickup summaries against hashed capture bytes."""
import argparse
import hashlib
import json
import struct
from pathlib import Path

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('ground_items', type=Path)
a = p.parse_args()
diff = json.loads((a.ground_items / 'ground_item_pickup_diff.json').read_text(encoding='utf-8-sig'))
target = 0x1528e92f
for phase, summary in diff.items():
    if not isinstance(summary, dict) or 'source' not in summary:
        continue
    path = a.ground_items / summary['source']
    content = path.read_bytes()
    assert hashlib.sha256(content).hexdigest() == summary['sha256']
    capture = json.loads(content.decode('utf-8-sig'))
    assert capture['pid'] == summary['pid'] == 21252
    def read(address, size):
        matches = [bytes.fromhex(r['hex'])[address-r['address']:address-r['address']+size]
                   for r in capture['reads'] if r['address'] <= address and address+size <= r['address']+r['n']]
        assert matches, (phase, hex(address), size)
        assert all(value == matches[0] for value in matches), 'Conflicting captured bytes'
        return matches[0]
    def u32(address):
        return struct.unpack('<I', read(address, 4))[0]
    assert u32(capture['base'] + 0x20f700) == summary['q']
    assert u32(summary['q'] + 8) == summary['begin']
    assert u32(summary['q'] + 12) == summary['end']
    assert summary['end'] - summary['begin'] == len(summary['objects']) * 4
    for obj in summary['objects']:
        address = obj['address']
        assert u32(summary['begin'] + obj['slot'] * 4) == address
        assert u32(address) == obj['vtable']
        assert u32(address + 0x2274) == obj['query_key']
        assert u32(address + 0x22a4) == obj['kind']
        assert list(struct.unpack('<ii', read(address + 0x23ec, 8))) == obj['xy']
        assert read(address + 0x2284, len(obj['name'].encode('gbk')) + 1) == obj['name'].encode('gbk') + b'\0'
        if 'catalog_key' in obj:
            assert u32(address + 0x24dc) == obj['catalog_key'] == 4101
    present = any(obj['query_key'] == target for obj in summary['objects'])
    assert present == (phase == 'on_ground')
    print(f'{phase}: hash, membership and raw fields consistent; target present={present}')
print('OFFLINE_PASS: historical pickup evidence; exact removal time/identity permanence unproven')
