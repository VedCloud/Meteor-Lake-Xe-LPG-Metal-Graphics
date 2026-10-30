# SPDX-License-Identifier: MIT
"""Opt-in lower-overhead kernel diagnostics; preserve instruction-counted mode."""
from pathlib import Path
r=Path(__file__).resolve().parent;p=r/'vmapple-entry.c';s=p.read_text();assert 'FAST_KERNEL_TRACE' not in s
s=s.replace('visits++;last_pc=pc;','''
#ifndef FAST_KERNEL_TRACE
 visits++;
#endif
 last_pc=pc;''')
needle='static bool invalid_memory('
s=s.replace(needle,'''#ifdef FAST_KERNEL_TRACE
/* A block hook schedules a block before execution. The summed slots are an
 * upper-bound diagnostic estimate, NOT retired instructions or the default
 * before-instruction visit count. Avoid both per-instruction tracing and
 * Unicorn's internal count hook during this exploratory long boot. */
static uint64_t next_block_checkpoint=50000000;
static void block_trace(uc_engine*u,uint64_t pc,uint32_t size,void*opaque){
 (void)opaque;last_pc=pc;
 if(!size||(size&3)){bridge_output("FAIL invalid ARM64 translation-block size\\n");uc_emu_stop(u);return;}
 visits+=size/4;
 if(visits>=next_block_checkpoint){char text[170];snprintf(text,sizeof(text),"VMAPPLE block checkpoint: scheduled_slots=%llu pc=0x%llx\\n",(unsigned long long)visits,(unsigned long long)pc);bridge_output(text);next_block_checkpoint=visits+50000000;}
 if(visits>=10000000000ULL)uc_emu_stop(u);
}
#endif
'''+needle)
old='uc_hook hook1,hook2;require(uc_hook_add(uc,&hook1,UC_HOOK_CODE,trace,NULL,1,0),"trace hook");require(uc_hook_add(uc,&hook2,UC_HOOK_MEM_INVALID,invalid_memory,NULL,1,0),"fault hook");'
assert old in s
s=s.replace(old,'''uc_hook hook1,hook2;
#ifdef FAST_KERNEL_TRACE
 uc_hook block_hook;require(uc_hook_add(uc,&block_hook,UC_HOOK_BLOCK,block_trace,NULL,1,0),"block scheduling hook");
 require(uc_hook_add(uc,&hook1,UC_HOOK_CODE,trace,NULL,0xfffffe000a8194ecULL,0xfffffe000a8195a8ULL),"panic trace hook");
 bridge_output("VMAPPLE count metric: scheduled block slots (upper bound), not retired instructions or instruction visits\\n");
#else
 require(uc_hook_add(uc,&hook1,UC_HOOK_CODE,trace,NULL,1,0),"trace hook");
#endif
 require(uc_hook_add(uc,&hook2,UC_HOOK_MEM_INVALID,invalid_memory,NULL,1,0),"fault hook");''')
s=s.replace('uc_err err=uc_emu_start(uc,VMREF_PC,0,0,1000000000);','''
#ifdef FAST_KERNEL_TRACE
 uc_err err=uc_emu_start(uc,VMREF_PC,0,0,0);
#else
 uc_err err=uc_emu_start(uc,VMREF_PC,0,0,1000000000);
#endif
''')
p.write_text(s)
p=r/'link.py';s=p.read_text();s=s.replace("base=[clang,", "if '--fast' in sys.argv:extra+=['-DFAST_KERNEL_TRACE']\nbase=[clang,")
s=s.replace("'mode':'vmapple' if extra else 'diagnostic'", "'mode':'vmapple-block-trace' if '--fast' in sys.argv else 'vmapple' if extra else 'diagnostic'");p.write_text(s)
p=r/'test-sparse-firmware.py';s=p.read_text().replace("'instructionsVisited':visits", "('scheduledInstructionSlotsUpperBound' if build['mode']=='vmapple-block-trace' else 'instructionsVisited'):visits");p.write_text(s)
print('Opt-in block tracing added; rebuild with link.py --vmapple --fast')
