"""Verify the C1 registry summary against the historical captured bytes."""
import argparse
import json
import struct
from pathlib import Path

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('snapshot', type=Path)
args = parser.parse_args()
data = json.loads(args.snapshot.read_text(encoding='utf-8-sig'))
result = data['result']
reads = {read['tag']: read for read in data['live_reads']}


def raw(tag, size, address=None):
    read = reads[tag]
    value = bytes.fromhex(read['hex'])
    if read['error'] or len(value) < size or (address is not None and read['a'] != address):
        raise ValueError(f'Invalid capture: {tag}')
    return value


def require(condition, field):
    if not condition:
        raise ValueError(f'C1 mismatch: {field}')


registry, begin, end = (int(result[key], 16) if isinstance(result[key], str) else result[key]
                        for key in ('registry', 'array_begin', 'array_end'))
objects = result['objs']
require(result['coherent'] and end - begin == len(objects) * 4, 'bounds and consistency')
require(struct.unpack_from('<I', raw('registry_global', 4))[0] == registry, 'registry pointer')
require(struct.unpack_from('<II', raw('registry_header', 16, registry), 8) == (begin, end), 'registry bounds')
slots = struct.unpack(f'<{len(objects)}I', raw('slots', end - begin, begin))
for index, obj in enumerate(objects):
    address = int(obj['address'], 16)
    require(obj['slot'] == index and slots[index] == address, f'slot {index}')
    for offset, fmt, expected in [(0, '<I', (int(obj['vptr'], 16),)),
                                  (0x2274, '<I', (int(obj['lookup_key'], 16),)),
                                  (0x23ec, '<ii', tuple(obj['raw_signed_pair'])),
                                  (0x241c, '<I', (obj['query_filter_value'],))]:
        tag = f'slot{index}_{offset:x}'
        require(struct.unpack_from(fmt, raw(tag, struct.calcsize(fmt), address + offset)) == expected, tag)
    require(obj['local_role'] == (address == data['anchor']['role']), f'role slot {index}')
name = raw('inline_name_field_2', 9, slots[2] + 0x2284).split(b'\0')[0].decode('gbk')
require(name == '容大胖子', 'NPC name')
print(f'OFFLINE_PASS: {len(objects)} C1 members, raw fields, local role and NPC name match')
