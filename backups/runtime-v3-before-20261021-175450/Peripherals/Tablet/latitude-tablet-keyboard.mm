// Session keyboard. No field values, keystroke monitoring or firmware access.
#import <Cocoa/Cocoa.h>
#import <ApplicationServices/ApplicationServices.h>
#import <IOKit/IOKitLib.h>
#include <mach/mach_time.h>
#include <sys/sysctl.h>
#include <unistd.h>
#include <cstring>
#include "LatitudeTabletPolicy.h"
static bool manual=false;
static bool trusted(){return AXIsProcessTrusted();}
static id property(io_service_t service,CFStringRef key){return CFBridgingRelease(IORegistryEntryCreateCFProperty(service,key,kCFAllocatorDefault,0));}
static bool qualifiedTablet(){
 // The measured live-state publisher provides this contract. The passive
 // observer's historical CC/CD candidates intentionally CANNOT activate this UI.
 io_service_t service=IOServiceGetMatchingService(kIOMainPortDefault,IOServiceMatching("LatitudeTabletState"));if(!service)return false;
 id qualified=property(service,CFSTR("LatitudeTabletModeQualified")),mode=property(service,CFSTR("LatitudeTabletModeCurrent")),stamp=property(service,CFSTR("LatitudeTabletModeHeartbeatUptimeNs"));
 bool types=([qualified isKindOfClass:[NSNumber class]] && [mode isKindOfClass:[NSNumber class]] && [stamp isKindOfClass:[NSNumber class]]);
 mach_timebase_info_data_t base;mach_timebase_info(&base);uint64_t now=(uint64_t)((__uint128_t)mach_absolute_time()*base.numer/base.denom);
 uint64_t sampled=types?[stamp unsignedLongLongValue]:0;
 bool valid=types && [qualified boolValue] && [mode boolValue] && sampled<=now && now-sampled<=1500000000ull;
 IOObjectRelease(service);return valid;
}
static bool textFocus(){
 NSRunningApplication*app=NSWorkspace.sharedWorkspace.frontmostApplication;if(!app || app.processIdentifier==getpid())return false;
 AXUIElementRef root=AXUIElementCreateApplication(app.processIdentifier);CFTypeRef element=nullptr,role=nullptr,enabled=nullptr;bool result=false;
 if(!AXUIElementCopyAttributeValue(root,kAXFocusedUIElementAttribute,&element) && element && CFGetTypeID(element)==AXUIElementGetTypeID()){
  auto field=(AXUIElementRef)element;
  if(!AXUIElementCopyAttributeValue(field,kAXRoleAttribute,&role) && role && CFGetTypeID(role)==CFStringGetTypeID()){
   bool text=CFEqual(role,kAXTextFieldRole)||CFEqual(role,kAXTextAreaRole)||CFEqual(role,kAXComboBoxRole);
   Boolean writable=false;
   bool active=AXUIElementCopyAttributeValue(field,kAXEnabledAttribute,&enabled)==kAXErrorSuccess && enabled==kCFBooleanTrue;
   result=text && active && AXUIElementIsAttributeSettable(field,kAXValueAttribute,&writable)==kAXErrorSuccess && writable;
  }
 }
 if(enabled)CFRelease(enabled);if(role)CFRelease(role);if(element)CFRelease(element);CFRelease(root);return result;
}
@interface KeyboardPanel:NSPanel
@end
@implementation KeyboardPanel
-(BOOL)canBecomeKeyWindow{return NO;}
-(BOOL)canBecomeMainWindow{return NO;}
@end
@interface TouchKey:NSButton
@end
@implementation TouchKey
-(BOOL)acceptsFirstMouse:(NSEvent*)event{(void)event;return YES;}
@end
@interface Keyboard:NSObject<NSApplicationDelegate>
@property(strong) KeyboardPanel *panel;
@property(strong) NSTimer *timer;
@property BOOL shifted;
@property BOOL sessionActive;
@end
@implementation Keyboard
-(void)applicationDidFinishLaunching:(NSNotification*)note{
 (void)note;self.sessionActive=YES;
 self.panel=[[KeyboardPanel alloc]initWithContentRect:NSMakeRect(0,0,780,245) styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskNonactivatingPanel backing:NSBackingStoreBuffered defer:NO];
 self.panel.title=@"Latitude Touch Keyboard";self.panel.level=NSFloatingWindowLevel;self.panel.hidesOnDeactivate=NO;
 NSArray*rows=@[@[@"1",@"2",@"3",@"4",@"5",@"6",@"7",@"8",@"9",@"0",@"⌫"],@[@"q",@"w",@"e",@"r",@"t",@"y",@"u",@"i",@"o",@"p"],@[@"a",@"s",@"d",@"f",@"g",@"h",@"j",@"k",@"l",@"Return"],@[@"Shift",@"z",@"x",@"c",@"v",@"b",@"n",@"m",@",",@".",@"?"],@[@"Space",@"←",@"→",@"Close"]];
 for(NSUInteger row=0;row<rows.count;row++){
  NSArray*keys=rows[row];CGFloat width=750.0/keys.count;
  for(NSUInteger column=0;column<keys.count;column++){
   TouchKey*b=[[TouchKey alloc]initWithFrame:NSMakeRect(15+column*width,198-row*47,width-5,42)];b.title=keys[column];b.bezelStyle=NSBezelStyleRounded;b.target=self;b.action=@selector(key:);b.font=[NSFont systemFontOfSize:18];[self.panel.contentView addSubview:b];
  }
 }
 NSNotificationCenter*center=NSWorkspace.sharedWorkspace.notificationCenter;
 [center addObserver:self selector:@selector(inactive:) name:NSWorkspaceSessionDidResignActiveNotification object:nil];
 [center addObserver:self selector:@selector(active:) name:NSWorkspaceSessionDidBecomeActiveNotification object:nil];
 self.timer=[NSTimer scheduledTimerWithTimeInterval:0.5 target:self selector:@selector(update:) userInfo:nil repeats:YES];[self update:nil];
}
-(void)inactive:(NSNotification*)note{(void)note;self.sessionActive=NO;[self.panel orderOut:nil];}
-(void)active:(NSNotification*)note{(void)note;self.sessionActive=YES;}
-(void)update:(NSTimer*)timer{
 (void)timer;bool show=self.sessionActive && trusted() && (manual || (qualifiedTablet() && textFocus()));
 if(!show){[self.panel orderOut:nil];return;}
 if(!self.panel.visible){
  NSScreen*screen=NSScreen.mainScreen;
  for(NSScreen*s in NSScreen.screens)if(CGDisplayIsBuiltin([s.deviceDescription[@"NSScreenNumber"] unsignedIntValue])){screen=s;break;}
  NSRect visible=screen.visibleFrame;[self.panel setFrameOrigin:NSMakePoint(NSMidX(visible)-390,NSMinY(visible)+12)];[self.panel orderFrontRegardless];
 }
}
-(void)key:(NSButton*)sender{
 if(!trusted() || !self.sessionActive)return;
 NSString*label=sender.title;
 if([label isEqual:@"Close"]){[NSApp terminate:nil];return;}
 if(!manual && (!qualifiedTablet() || !textFocus())){[self.panel orderOut:nil];return;}
 if([label isEqual:@"Shift"]){self.shifted=!self.shifted;sender.state=self.shifted?NSControlStateValueOn:NSControlStateValueOff;return;}
 CGKeyCode code=0;NSString*text=nil;
 if([label isEqual:@"⌫"])code=51;else if([label isEqual:@"Return"])code=36;else if([label isEqual:@"←"])code=123;else if([label isEqual:@"→"])code=124;else text=[label isEqual:@"Space"]?@" ":(self.shifted?label.uppercaseString:label);
 CGEventRef down=CGEventCreateKeyboardEvent(nullptr,code,true),up=CGEventCreateKeyboardEvent(nullptr,code,false);
 if(down && up){if(text){UniChar chars[16];NSUInteger n=MIN(text.length,16ul);[text getCharacters:chars range:NSMakeRange(0,n)];CGEventKeyboardSetUnicodeString(down,n,chars);CGEventKeyboardSetUnicodeString(up,n,chars);}CGEventPost(kCGHIDEventTap,down);CGEventPost(kCGHIDEventTap,up);}
 if(down)CFRelease(down);if(up)CFRelease(up);
}
@end
int main(int argc,const char*argv[]){@autoreleasepool{
 int arm=0;size_t length=sizeof arm;sysctlbyname("hw.optional.arm64",&arm,&length,nullptr,0);if(arm){fprintf(stderr,"Run on the Intel Dell.\n");return 2;}
 manual=argc==2 && !strcmp(argv[1],"--manual");
 if(argc==2 && !strcmp(argv[1],"--diagnose")){printf("{\"version\":\"0.2.0\",\"accessibilityTrusted\":%s,\"qualifiedLiveTabletMode\":%s,\"automaticKeyboardNeedsLiveModePublisher\":true}\n",trusted()?"true":"false",qualifiedTablet()?"true":"false");return 0;}
 if(!trusted()){fprintf(stderr,"Enable Latitude Tablet Keyboard in System Settings > Privacy & Security > Accessibility, then reopen it.\n");return 2;}
 NSApplication*app=NSApplication.sharedApplication;app.activationPolicy=NSApplicationActivationPolicyAccessory;Keyboard*delegate=[Keyboard new];app.delegate=delegate;[app run];return 0;
}}
