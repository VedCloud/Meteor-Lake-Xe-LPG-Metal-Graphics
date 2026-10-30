#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <objc/runtime.h>
int main(){@autoreleasepool{for(NSString*n in @[@"CGVirtualDisplayDescriptor",@"CGVirtualDisplay",@"CGVirtualDisplaySettings",@"CGVirtualDisplayMode"]){Class c=NSClassFromString(n);unsigned count=0;Method*m=class_copyMethodList(c,&count);for(unsigned i=0;i<count;i++){const char*s=sel_getName(method_getName(m[i]));if(!strncmp(s,"set",3)||!strncmp(s,"init",4)||!strcmp(s,"displayID")||!strcmp(s,"applySettings:"))printf("%s %s %s\n",n.UTF8String,s,method_getTypeEncoding(m[i]));}free(m);}}return 0;}
