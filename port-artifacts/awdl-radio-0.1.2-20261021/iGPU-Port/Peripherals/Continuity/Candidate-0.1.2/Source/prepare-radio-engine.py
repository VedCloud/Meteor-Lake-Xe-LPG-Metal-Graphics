"""Private OWL overlay: preserve pinned upstream and remove unaligned wire accesses."""
from pathlib import Path
import shutil,subprocess,re,json,hashlib,difflib
r=Path(__file__).resolve().parent.parent;src=r/'research/owl';out=r/'desktop-build/awdl-radio-engine-source'
assert subprocess.check_output(['git','-C',str(src),'rev-parse','HEAD'],text=True).strip()=='0ee3263f944ea721c355f826aa90d636d012a512'
out.mkdir(exist_ok=True)
for name in ['src','radiotap']:shutil.copytree(src/name,out/name,dirs_exist_ok=True,ignore=shutil.ignore_patterns('.git','*.o'))
shutil.copy2(src/'COPYING',out/'COPYING')
p=out/'src/wire.c';s=p.read_text()
for bits in [16,32]:
 for endian in ['le','be']:
  a=f'\t*(uint{bits}_t *) (buf->data + offset) = hto{endian}{bits}(value);';assert s.count(a)==1
  s=s.replace(a,f'\tuint{bits}_t encoded = hto{endian}{bits}(value);\n\tmemcpy(buf->data + offset, &encoded, sizeof(encoded));')
  a=f'\tif (value)\n\t\t*value = {endian}{bits}toh(*(uint{bits}_t *) (buf->data + offset));';assert s.count(a)==1
  s=s.replace(a,f'\tif (value) {{\n\t\tuint{bits}_t encoded;\n\t\tmemcpy(&encoded, buf->data + offset, sizeof(encoded));\n\t\t*value = {endian}{bits}toh(encoded);\n\t}}')
s=re.sub(r'offset < 0 \|\| offset \+ (\d+|ETHER_ADDR_LEN|length) > buf->len',lambda m:f'offset < 0 || '+('length < 0 || ' if m[1]=='length' else '')+f'offset > buf->len || {m[1]} > buf->len - offset',s)
s=s.replace('struct buf *buf = (struct buf *) malloc(sizeof(struct buf));','struct buf *buf = (struct buf *) malloc(sizeof(struct buf));\n\tif (!buf) return NULL;')
s=s.replace('buf->data = (uint8_t *) malloc(len);','buf->data = (uint8_t *) malloc(len);\n\tif (!buf->data && len) { free(buf); return NULL; }')
p.write_text(s)
p=out/'src/tx.c';s=p.read_text();a='\t*(uint16_t *) (tlv->name + len) = htobe16(AWDL_DNS_SHORT_LOCAL);';assert s.count(a)==1;s=s.replace(a,'\tuint16_t dns_suffix = htobe16(AWDL_DNS_SHORT_LOCAL);\n\tmemcpy(tlv->name + len, &dns_suffix, sizeof(dns_suffix));');a='\t*(uint32_t *) end = htole32(crc);';assert s.count(a)==1;s=s.replace(a,'\tuint32_t encoded_crc = htole32(crc);\n\tmemcpy(end, &encoded_crc, sizeof(encoded_crc));');p.write_text(s)
p=out/'src/rx.c';s=p.read_text();a='\t\t\t\t\tif (tsft)\n\t\t\t\t\t\t*tsft = le64toh(*(uint64_t *) iter.this_arg);';assert s.count(a)==1;s=s.replace(a,'\t\t\t\t\tif (tsft) {\n\t\t\t\t\t\tuint64_t encoded_tsft;\n\t\t\t\t\t\tmemcpy(&encoded_tsft, iter.this_arg, sizeof(encoded_tsft));\n\t\t\t\t\t\t*tsft = le64toh(encoded_tsft);\n\t\t\t\t\t}');p.write_text(s)
patch=''
for name in ['src/wire.c','src/tx.c','src/rx.c']:
 patch+=''.join(difflib.unified_diff((src/name).read_text().splitlines(True),(out/name).read_text().splitlines(True),fromfile='a/'+name,tofile='b/'+name))
(out/'radio-wire-alignment.patch').write_text(patch)
(out/'PROVENANCE.json').write_text(json.dumps({'upstream':'https://github.com/seemoo-lab/owl','commit':'0ee3263f944ea721c355f826aa90d636d012a512','privatePatch':'radio-wire-alignment.patch','patchedSourceSHA256':{name:hashlib.sha256((out/name).read_bytes()).hexdigest() for name in ['src/wire.c','src/tx.c','src/rx.c']},'license':'GPL-3.0-or-later; radiotap BSD'},indent=2)+'\n')
print('Prepared private aligned wire overlay; upstream checkout unchanged.')
