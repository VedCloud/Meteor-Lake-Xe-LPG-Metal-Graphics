#include <mach/kmod.h>
extern "C" {
extern kmod_start_func_t _start;
extern kmod_stop_func_t _stop;
KMOD_EXPLICIT_DECL(com.ved.LatitudeHotkeys,"0.1.1",_start,_stop)
kmod_start_func_t *_realmain=0;
kmod_stop_func_t *_antimain=0;
int _kext_apple_cc=__APPLE_CC__;
}
