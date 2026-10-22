#include <assert.h>
#include "../NullMoth/plugin/nvmtl_log_failure.h"
int main(void) {
    assert(nvmtl_log_failure("vk: fence wait -> 2 after 5000 ms (budget 5000 ms); disabling backend without retiring in-flight resources"));
    assert(nvmtl_log_failure("vk: queue submit -> -4; disabling backend and retaining uncertain in-flight resources"));
    assert(nvmtl_log_failure("vk: vkAllocateCommandBuffers FAILED -> -2"));
    assert(nvmtl_log_failure("vk: vkAllocateMemory refused the imported host pointer -> -1000072003"));
    assert(!nvmtl_log_failure("vk: queue submit -> 0"));
    assert(!nvmtl_log_failure("default cached coherent memory type 0"));
    assert(nvmtl_log_failure("first-submit: #2 fence result 2 after 5000 ms"));
    assert(nvmtl_log_failure("present-perf0.3.8: readbacks=120 avg_ms wait=2.1"));
    assert(nvmtl_log_failure("present-perf0.3.9: readbacks=120 avg_ms alloc=0.1"));
    assert(nvmtl_log_failure("present-perf0.3.10: readbacks=120 avg_ms copy=2.1"));
    assert(nvmtl_log_failure("present-perf0.3.11: readbacks=120 avg_ms copy=2.1"));
    assert(nvmtl_log_failure("present-perf0.3.12: readbacks=120 avg_ms copy=2.1"));
    assert(!nvmtl_log_failure(NULL));
}
