#pragma once
#include <stdint.h>
#include <string.h>

typedef struct {
    uint64_t count, bytes, elapsed[5], max_total, started;
    unsigned reports;
} nvmtl_present_cost;

/* Caller-owned statistics: no shared lock or per-frame logging. */
static int nvmtl_present_cost_add(nvmtl_present_cost *cost, const uint64_t stamp[6], uint64_t bytes)
{
    for (unsigned i=0;i<5;i++) if (stamp[i+1]<stamp[i]) return 0;
    if (cost->reports>=8) return 0;
    if (!cost->count) cost->started=stamp[0];
    cost->count++; cost->bytes+=bytes;
    for (unsigned i=0;i<5;i++) cost->elapsed[i]+=stamp[i+1]-stamp[i];
    uint64_t total=stamp[5]-stamp[0];
    if (total>cost->max_total) cost->max_total=total;
    return stamp[5]-cost->started>=2000000000ull;
}

static void nvmtl_present_cost_reset(nvmtl_present_cost *cost)
{
    unsigned reports=cost->reports+1;
    memset(cost,0,sizeof *cost);
    cost->reports=reports;
}
