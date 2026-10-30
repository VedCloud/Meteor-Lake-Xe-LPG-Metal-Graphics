# SPDX-License-Identifier: MIT
"""Correct only the OS-environment metadata of an isolated recovery handoff.

Apple XNU kern_mib.c consumes /chosen/osenvironment. kern_csr.c requires
the exact NUL-terminated property length, so padding a 32-byte placeholder
with a shorter string would not be equivalent. No executable is patched.
"""
from pathlib import Path
import copy, datetime, hashlib, json, mmap, struct, subprocess

BUILD = Path(__file__).resolve().parent
SOURCE = BUILD.parent / 'StartupLoggingBuild/BootContextExperiment'
DEST = SOURCE.resolve().parent / 'BootContextExperiment-RecoveryEnvironment-v9'

def sha(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()

def decode(data, offset=0, depth=0):
    assert depth < 64 and offset + 8 <= len(data), 'invalid node bounds'
    properties, children = struct.unpack_from('<II', data, offset)
    offset += 8
    assert properties < 4096 and children < 4096
    node = {'properties': {}, 'children': []}
    for _ in range(properties):
        assert offset + 36 <= len(data)
        name = data[offset:offset+32].split(b'\0', 1)[0].decode('ascii')
        length = struct.unpack_from('<I', data, offset+32)[0]
        assert not length & 0x80000000, 'unsupported property flags'
        offset += 36
        assert name not in node['properties'] and length <= len(data)-offset
        node['properties'][name] = bytes(data[offset:offset+length])
        offset += (length+3) & ~3
        assert offset <= len(data)
    for _ in range(children):
        child, offset = decode(data, offset, depth+1)
        node['children'].append(child)
    return node, offset

def encode(node):
    result = bytearray(struct.pack('<II', len(node['properties']), len(node['children'])))
    for name, value in node['properties'].items():
        result += name.encode().ljust(32, b'\0')
        result += struct.pack('<I', len(value)) + value + bytes((-len(value)) % 4)
    for item in node['children']:
        result += encode(item)
    return bytes(result)

def child(node, name):
    matches = [n for n in node['children'] if n['properties'].get('name', b'').rstrip(b'\0') == name.encode()]
    assert len(matches) == 1, name
    return matches[0]

def outside_hash(image, spans):
    digest = hashlib.sha256()
    position = 0
    for start, end in sorted(spans) + [(len(image), len(image))]:
        assert position <= start <= end <= len(image)
        while position < start:
            end_chunk = min(start, position + 1024*1024)
            digest.update(image[position:end_chunk])
            position = end_chunk
        position = end
    return digest.hexdigest()

def prepare():
    assert not DEST.exists(), 'refusing to overwrite a prepared profile'
    meta = json.loads((SOURCE/'VMAPPLE-REFERENCE-BOOT.json').read_text())
    original = SOURCE/'vmapple-reference-boot-state.bin'
    assert sha(original) == meta['sha256']
    dt_at = meta['deviceTreePhysical'] - meta['guestBase']
    args_at = meta['X0'] - meta['guestBase']
    with original.open('rb') as stream:
        stream.seek(dt_at)
        old = stream.read(meta['deviceTreeBytes'])
    tree, used = decode(old)
    assert used == len(old) and encode(tree) == old, 'lossless source-tree roundtrip required'
    changed = copy.deepcopy(tree)
    chosen = child(changed, 'chosen')
    previous = chosen['properties']['osenvironment']
    assert len(previous) == 32 and not any(previous), 'expected the verified empty placeholder'
    chosen['properties']['osenvironment'] = b'recoveryos\0'
    memory_map = child(chosen, 'memory-map')['properties']
    memory_map['DeviceTree'] = struct.pack('<QQ', meta['deviceTreePhysical'], len(encode(changed)))
    payload = encode(changed)
    assert struct.unpack('<QQ', memory_map['DeviceTree'])[1] == len(payload)
    assert len(payload) <= meta['deviceTreeCapacityBytes']
    # Reject truncation; require the exact environment length XNU checks.
    verified, used = decode(payload)
    assert used == len(payload) and verified == changed
    assert child(verified, 'chosen')['properties']['osenvironment'] == b'recoveryos\0'
    for invalid in (payload[:7], payload[:-1]):
        try:
            decode(invalid)
        except (AssertionError, struct.error):
            pass
        else:
            raise AssertionError('truncated tree accepted')
    DEST.mkdir()
    target = DEST/original.name
    subprocess.run(['/bin/cp', '-c', str(original), str(target)], check=True)
    spans = [(dt_at, dt_at+meta['deviceTreeCapacityBytes']), (args_at+104, args_at+108)]
    with target.open('r+b') as stream:
        image = mmap.mmap(stream.fileno(), 0)
        before = outside_hash(image, spans)
        image[dt_at:dt_at+meta['deviceTreeCapacityBytes']] = payload + bytes(meta['deviceTreeCapacityBytes']-len(payload))
        struct.pack_into('<I', image, args_at+104, len(payload))
        assert outside_hash(image, spans) == before, 'unrelated initialized memory changed'
        image.flush()
        image.close()
    digest = sha(target)
    updated = {**meta, 'parentCaptureSHA256': meta['sha256'], 'sha256': digest,
               'deviceTreeBytes': len(payload), 'inputRelativePath': 'EngineProbe/vmapple-'+digest+'.bin',
               'OSenvironment': 'recoveryos', 'graphicalInstallerVerified': False}
    (DEST/'VMAPPLE-REFERENCE-BOOT.json').write_text(json.dumps(updated, indent=2)+'\n')
    header = (SOURCE/'vmapple_reference_boot.h').read_text().split('static const uint8_t')[0]
    header += 'static const uint8_t vmref_sha256[32]={' + ','.join(str(v) for v in bytes.fromhex(digest)) + '};\n'
    header += '#define VMREF_INPUT_PATH L' + json.dumps('\\'+updated['inputRelativePath'].replace('/', '\\')) + '\n'
    (DEST/'vmapple_reference_boot.h').write_text(header)
    (DEST/'BOOT-CONTEXT-PREPARATION.json').write_bytes((SOURCE/'BOOT-CONTEXT-PREPARATION.json').read_bytes())
    assert sha(original) == meta['sha256'], 'original handoff modified'
    report = {'UTC': datetime.datetime.now(datetime.timezone.utc).isoformat(),
              'sourceProfile': str(SOURCE.resolve()), 'candidateProfile': str(DEST),
              'sourceSHA256': meta['sha256'], 'candidateSHA256': digest,
              'changedProperty': '/chosen/osenvironment', 'previousBytes': len(previous),
              'value': 'recoveryos', 'propertyBytes': len(b'recoveryos\0'),
              'deviceTreeBytes': len(payload), 'logicalGuestRAMBytes': meta['RAMBytes'],
              'outsideDTAndBootArgsLengthUnchanged': True, 'outsideRangesSHA256': before,
              'sourceUnchanged': True, 'AppleExecutableBytesChanged': False,
              'runtimeVerified': False, 'graphicalInstallerVerified': False}
    (DEST/'PREPARATION.json').write_text(json.dumps(report, indent=2)+'\n')
    assert not (BUILD/'BootContextExperiment').exists()
    (BUILD/'BootContextExperiment').symlink_to(DEST, target_is_directory=True)
    print(json.dumps(report, indent=2))

if __name__ == '__main__':
    prepare()
