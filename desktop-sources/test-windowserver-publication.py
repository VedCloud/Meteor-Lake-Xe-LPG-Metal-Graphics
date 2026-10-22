from pathlib import Path
import subprocess
r=Path(__file__).resolve().parent.parent;s=(r/'NullMoth/plugin/NVMTLEventSync.m').read_text()
def method(needle):
 a=s.index(needle);b=s.index('{',a);d=1;j=b+1
 while d:d+=(s[j]=='{')-(s[j]=='}');j+=1
 return s[a:j]+'\n'
pre='''#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <objc/message.h>
#include <mach/mach.h>
#include <assert.h>
#include <stdatomic.h>
#include "nvmtl_vk.h"
static int gpu_result,copies,scheduled,drained,serial=1;
@class NVMTLCommandBuffer;
@interface NVMTLTexture:NSObject{@public NVMTLTexture *_parent;BOOL refuse;}- (BOOL)nvmtlSurfaceOut;@end
@implementation NVMTLTexture
- (BOOL)nvmtlSurfaceOut{if(refuse)return NO;copies++;return YES;}
@end
@interface NVMTLSubmissionPart:NSObject{@public nvk_cmdbuf commands;NSSet *resources,*surfaces;NSArray *scratch,*signals;id waitEvent,signalEvent;uint64_t value;unsigned encoders;nvmtl_inflight *inflight;int aheadFailed;BOOL waitFolded;}@end
@implementation NVMTLSubmissionPart @end
typedef NSMutableSet NVMTLResSet;
@interface NVMTLBuffer:NSObject @end
@implementation NVMTLBuffer @end
@interface NVMTLCommandBuffer:NSObject{@public BOOL _executionDone,_scheduleAfterPublish,_wasScheduled,_gpuTimingInvalid,_gpuTimingSeen;NSUInteger _nextPart;NSMutableArray *_parts,*_presentations,*_scratch,*_stTwins,*_scheduledHandlers;id _present,_e78Ring,_resources;NSMutableSet *_surfOverwrites;NSUInteger _e78RingOff;void *_g79BRing[8];NSCondition *_completionCondition;NSError *_cbError;MTLCommandBufferStatus _cbstatus;nvmtl_gpu_interval _gpuTiming;double _legacyKernelStart,_legacyKernelEnd,_gpuStart,_gpuEnd;uint64_t _traceCommitNS;}
- (void)nvmtlDispatchHandlers:(NSArray *)h scheduled:(BOOL)b;
- (void)nvmtlCollectDeviceTiming:(nvk_cmdbuf *)c;
- (void)nvmtlReleaseCapacity;
- (void)nvmtlMarkScheduled;
- (void)nvmtlFinishExecution:(NSError *)e;
- (BOOL)nvmtlAdvancePart;
@end
static NSMutableArray *nvmtl_quarantined_parts;static _Atomic unsigned long long gSkipEmpty;
static int nvmtl_gsub_finish(NVMTLSubmissionPart *p){drained++;p->inflight=NULL;return 0;}
static void nvmtl_surface_finish_writes(NVMTLCommandBuffer *b){}
static void nvmtl_res2_cb_done(NVMTLCommandBuffer *b){}
static BOOL nvmtl_skip_empty_on(void){return NO;}
static NSSet *nvmtl_submission_copies(NSSet *a,BOOL b){return [NSSet set];}
static void nvmtl_buffer_sync_in(NVMTLBuffer *b){}static void nvmtl_buffer_sync_out(NVMTLBuffer *b){}
int nvmtl_vk_submit_wait_at(nvk_cmdbuf *c,const char *who){c->cb=NULL;return gpu_result;}
int nvmtl_vk_cmd_abandon(nvk_cmdbuf *c){return 0;}
int nvmtl_vk_interval_seconds(const nvmtl_gpu_interval *c,double *a,double *b){return -1;}
static void nvmtl_mark_published(NVMTLCommandBuffer *b,NVMTLTexture *t){}
static void nvlog(const char *fmt,...){}
@implementation NVMTLCommandBuffer
- (void)nvmtlDispatchHandlers:(NSArray *)h scheduled:(BOOL)b{if(b)scheduled++;}
- (void)nvmtlCollectDeviceTiming:(nvk_cmdbuf *)c{}
- (void)nvmtlReleaseCapacity{}
'''
post='''@end
static NVMTLCommandBuffer *buffer(BOOL fail){NVMTLCommandBuffer *b=[NVMTLCommandBuffer new];b->_completionCondition=[NSCondition new];b->_parts=[NSMutableArray new];b->_presentations=[NSMutableArray new];b->_scheduledHandlers=[NSMutableArray arrayWithObject:@1];b->_scheduleAfterPublish=YES;
 for(int i=0;i<2;i++){NVMTLSubmissionPart *p=[NVMTLSubmissionPart new];p->commands.cb=(void *)1;p->encoders=1;NVMTLTexture *t=[NVMTLTexture new];t->refuse=fail&&i==1;p->surfaces=[NSSet setWithObject:t];[b->_parts addObject:p];}
 return b;}
int main(void){@autoreleasepool{
 for(int mode=0;mode<3;mode++){copies=scheduled=drained=0;gpu_result=0;NVMTLCommandBuffer *b=buffer(mode==1);[b nvmtlMarkScheduled];assert(!scheduled&&!b->_wasScheduled);assert([b nvmtlAdvancePart]);assert(copies==1&&!scheduled&&!b->_executionDone);if(mode==2)gpu_result=-2;assert([b nvmtlAdvancePart]);assert(b->_executionDone);
 if(!mode){assert(copies==2&&scheduled==1&&b->_cbstatus==MTLCommandBufferStatusCompleted);}
 else{assert(scheduled==0&&b->_cbstatus==MTLCommandBufferStatusError&&b->_cbError);}}
 copies=scheduled=drained=0;gpu_result=0;NVMTLCommandBuffer *b=buffer(NO);NVMTLSubmissionPart *p=b->_parts[0];p->inflight=(nvmtl_inflight *)1;[b nvmtlFinishExecution:nil];assert(drained==1&&scheduled==1);
 puts("PASS actual event scheduling/advance/finish methods: no scheduled callback before all surface parts copy out; GPU/copy failures do not advertise publication; outstanding work drained before scheduling");
}}
'''
f=r/'desktop-build/test-windowserver-publication.m';f.write_text(pre+method('- (void)nvmtlMarkScheduled {')+method('- (void)nvmtlFinishExecution:(NSError *)error {')+method('- (BOOL)nvmtlAdvancePart {')+post);exe=r/'desktop-build/test-windowserver-publication';subprocess.run(['xcrun','clang','-g','-fobjc-arc','-fsanitize=address,undefined','-I',str(r/'NullMoth/plugin'),str(f),'-framework','Foundation','-framework','Metal','-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)
