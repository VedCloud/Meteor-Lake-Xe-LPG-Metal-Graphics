#pragma once
#include <string.h>
/* Preserve only the explicitly enabled test client's bounded controls. */
static inline int nvmtl_probe_controls_enabled(const char *process, const char *diagnostic)
{
    return process && !strcmp(process,"latitude-readback-test") && diagnostic && !strcmp(diagnostic,"1");
}
static inline int nvmtl_probe_control_entry(const char *entry)
{
    static const char *const allowed[]={"NVMTL_DIAGNOSTIC_STDERR=1","NVMTL_COMMAND_POOL_FRESH=1",
        "NVMTL_READBACK_STAGE_FRESH=1","NVMTL_READBACK_TRACE=1","NVMTL_POOL_REUSE=1","NVMTL_ENABLE=1"};
    if (!entry) return 0;
    for (unsigned i=0;i<sizeof allowed/sizeof allowed[0];i++) if (!strcmp(entry,allowed[i])) return 1;
    return 0;
}
