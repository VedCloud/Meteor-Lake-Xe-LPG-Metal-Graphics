"""Classify exact26A428 rejection branches from already-exported return PCs.
Never reads a key, key-reference value, header payload or unrelated memory.
"""
from pathlib import Path
import json,datetime
R=Path(__file__).resolve().parent;A=json.loads((R/'ACTIVE-valid.json').read_text());D=Path(A['run'])
report=json.loads((D/'SERVICE-EVENTS.json').read_text());findings=[]
for e in report['events']:
 if e['name']!='CSP-reference-rejected':continue
 slide=int(e['slide'],16);lr=int(e['LR'],16)-slide
 # All three paths end at1844a5b38. Only successful mutex-unlock and memcmp
 # calls change LR within the function before that shared error block.
 if lr==0x1844a5acc:reason='Reference absent from the current CSP session map'
 elif lr==0x1844a5b04:reason='Reference found, but CSSM key-header comparison failed'
 else:reason='Unclassified from LR alone: possible early length rejection or a callee that did not preserve LR'
 findings.append({'event':e,'reason':reason,'basis':'Exact26A428 lookupRefKey disassembly: length branch a5a48; map branch a5ad0 after unlock return a5acc; header branch a5b04 after memcmp; shared throw block a5b38','sourceDisassembly':str(R.parent/'AuxStorageBuild/CSP-LOOKUP-26A428.txt')})
result={'UTC':datetime.datetime.now(datetime.timezone.utc).isoformat(),'run':str(D),'scope':'Exact-build branch classification from signed-code-verified event and canonical return address only; no keys read','findings':findings,'failureReproduced':bool(findings),'graphicalInstallerVerified':False}
(D/'KEY-REFERENCE-CLASSIFICATION.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
