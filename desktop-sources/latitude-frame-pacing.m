#import <Foundation/Foundation.h>
#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreMedia/CoreMedia.h>
#import <mach/mach_time.h>
#include <assert.h>
#include <math.h>

static double tickMS;
static NSDictionary *summary(NSArray<NSNumber *> *input) {
 if(!input.count)return @{@"count":@0};
 NSArray *sorted=[input sortedArrayUsingSelector:@selector(compare:)];double sum=0;unsigned long over=0;
 for(NSNumber *n in input){sum+=n.doubleValue;if(n.doubleValue>1000.0/60.0+0.5)over++;}
 NSUInteger p95=(NSUInteger)ceil(sorted.count*0.95)-1;
 return @{@"count":@(input.count),@"mean_ms":@(sum/input.count),@"median_ms":sorted[sorted.count/2],@"p95_ms":sorted[p95],@"max_ms":sorted.lastObject,@"intervals_over_60hz_budget_plus_0_5ms":@(over)};
}
static void output(NSDictionary *d) {NSData *data=[NSJSONSerialization dataWithJSONObject:d options:NSJSONWritingPrettyPrinted error:NULL];[[NSFileHandle fileHandleWithStandardOutput] writeData:data];puts("");}
@interface Pacing : NSObject<SCStreamOutput,SCStreamDelegate> {
@public NSMutableArray<NSNumber *> *all,*continuous;uint64_t last;unsigned complete,idle,blank,invalid;BOOL idleSince,recording;NSString *streamError;
}
- (void)event:(NSInteger)status tick:(uint64_t)tick;
@end
@implementation Pacing
- (instancetype)init {if((self=[super init])){all=[NSMutableArray new];continuous=[NSMutableArray new];recording=YES;}return self;}
- (void)event:(NSInteger)status tick:(uint64_t)tick {
 if(!recording)return;
 if(status!=SCFrameStatusComplete){if(status==SCFrameStatusIdle)idle++;else blank++;idleSince=YES;return;}
 if(!tick || (last && tick<=last)){invalid++;return;}complete++;
 if(last && all.count<4096){NSNumber *v=@((tick-last)*tickMS);[all addObject:v];if(!idleSince)[continuous addObject:v];}
 last=tick;idleSince=NO;
}
- (void)stream:(SCStream *)stream didOutputSampleBuffer:(CMSampleBufferRef)buffer ofType:(SCStreamOutputType)type {
 (void)stream;if(type!=SCStreamOutputTypeScreen)return;
 NSArray *array=(__bridge NSArray *)CMSampleBufferGetSampleAttachmentsArray(buffer,false);NSDictionary *a=array.firstObject;
 NSNumber *status=a[SCStreamFrameInfoStatus],*tick=a[SCStreamFrameInfoDisplayTime];
 if(!status){invalid++;return;}[self event:status.integerValue tick:tick.unsignedLongLongValue];
}
- (void)stream:(SCStream *)stream didStopWithError:(NSError *)error {(void)stream;streamError=error.localizedDescription;}
@end
static BOOL waitFor(dispatch_semaphore_t sem,double seconds) {
 uint64_t start=mach_absolute_time();
 while((mach_absolute_time()-start)*tickMS<seconds*1000){
  if(!dispatch_semaphore_wait(sem,DISPATCH_TIME_NOW))return YES;
  [[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]];
 }
 return NO;
}
int main(int argc,const char *argv[]){@autoreleasepool{
 mach_timebase_info_data_t tb;mach_timebase_info(&tb);tickMS=(double)tb.numer/tb.denom/1e6;
 if(argc>1 && !strcmp(argv[1],"--self-test")){
  double saved=tickMS;tickMS=1e-6;Pacing *p=[Pacing new];
  [p event:SCFrameStatusComplete tick:1000000000];[p event:SCFrameStatusComplete tick:1016666667];
  [p event:SCFrameStatusIdle tick:1020000000];[p event:SCFrameStatusComplete tick:1100000000];
  [p event:SCFrameStatusComplete tick:1100000000];[p event:SCFrameStatusComplete tick:1133333333];
  assert(p->complete==4&&p->idle==1&&p->invalid==1&&p->all.count==3&&p->continuous.count==2);
  NSDictionary *d=summary(p->continuous);assert([d[@"intervals_over_60hz_budget_plus_0_5ms"] unsignedIntValue]==1);
  assert(fabs([d[@"mean_ms"] doubleValue]-25.0)<0.001);tickMS=saved;puts("PASS timestamp units, repeated/out-of-order rejection, explicit-idle separation, percentiles and 60Hz-budget comparison. No capture.");return 0;
 }
 NSMutableDictionary *report=[@{@"measurement":@"ScreenCaptureKit WindowServer display timestamps: delivered desktop update pacing",@"total_render_latency_ms":NSNull.null,@"limits":@"Frame intervals are not render-start-to-scanout or input latency. Idle/app-limited updates and capture overhead can affect results. Pixels are discarded without inspection or saving.",@"duration_requested_seconds":@8} mutableCopy];
 if(!CGPreflightScreenCaptureAccess()){report[@"status"]=@"screen_recording_permission_not_granted; skipped without requesting access";output(report);return 0;}
 __block SCShareableContent *content=nil;__block NSError *error=nil;dispatch_semaphore_t sem=dispatch_semaphore_create(0);
 [SCShareableContent getShareableContentExcludingDesktopWindows:NO onScreenWindowsOnly:YES completionHandler:^(SCShareableContent *c,NSError *e){content=c;error=e;dispatch_semaphore_signal(sem);}];
 if(!waitFor(sem,5)||!content||error){report[@"status"]=@"display_discovery_failed_or_timed_out";if(error)report[@"error"]=error.localizedDescription;output(report);return 0;}
 SCDisplay *display=nil;for(SCDisplay *d in content.displays)if(d.displayID==CGMainDisplayID()){display=d;break;}
 if(!display){report[@"status"]=@"main_display_missing";output(report);return 0;}
 CGDisplayModeRef mode=CGDisplayCopyDisplayMode(display.displayID);report[@"reported_refresh_hz"]=@(mode?CGDisplayModeGetRefreshRate(mode):0);if(mode)CGDisplayModeRelease(mode);
 SCContentFilter *filter=[[SCContentFilter alloc] initWithDisplay:display excludingWindows:@[]];
 SCStreamConfiguration *config=[SCStreamConfiguration new];config.width=64;config.height=36;config.minimumFrameInterval=CMTimeMake(1,240);config.queueDepth=3;config.showsCursor=NO;
 Pacing *p=[Pacing new];SCStream *stream=[[SCStream alloc] initWithFilter:filter configuration:config delegate:p];dispatch_queue_t q=dispatch_queue_create("com.latitude.frame-pacing",DISPATCH_QUEUE_SERIAL);
 if(![stream addStreamOutput:p type:SCStreamOutputTypeScreen sampleHandlerQueue:q error:&error]){report[@"status"]=@"stream_output_failed";output(report);return 0;}
 sem=dispatch_semaphore_create(0);[stream startCaptureWithCompletionHandler:^(NSError *e){error=e;dispatch_semaphore_signal(sem);}];
 if(!waitFor(sem,5)||error){report[@"status"]=@"capture_start_failed_or_timed_out";if(error)report[@"error"]=error.localizedDescription;output(report);return 0;}
 fprintf(stderr,"FRAME PACING: move/resize the Weather window continuously for 8 seconds. No images are saved.\n");
 uint64_t start=mach_absolute_time();while((mach_absolute_time()-start)*tickMS<8000)[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]];
 sem=dispatch_semaphore_create(0);[stream stopCaptureWithCompletionHandler:^(NSError *e){error=e;dispatch_semaphore_signal(sem);}];BOOL stopped=waitFor(sem,3);
 dispatch_sync(q,^{p->recording=NO;report[@"status"]=p->complete>1?@"captured":@"insufficient_changed_frames";report[@"complete_events"]=@(p->complete);report[@"idle_events"]=@(p->idle);report[@"blank_or_other_events"]=@(p->blank);report[@"invalid_timestamps"]=@(p->invalid);report[@"all_complete_intervals"]=summary(p->all);report[@"continuous_update_intervals"]=summary(p->continuous);report[@"intervals_ms"]=p->all;});
 report[@"stop_completed"]=@(stopped);if(p->streamError)report[@"stream_error"]=p->streamError;output(report);return 0;
}}
