#import <Foundation/Foundation.h>
#import <objc/runtime.h>

// A terminal-error command buffer still accepts encoding, then completes with its error.
// Resolve signatures from the real encoder without invoking any of its GPU methods.
@interface NVMTLFailedEncoder : NSProxy {
    id _failedDevice;
    Class _encodingClass;
    NSString *_failedLabel;
}
+ (id)encoderForDevice:(id)device encodingClass:(Class)encodingClass;
@end
@implementation NVMTLFailedEncoder
+ (id)encoderForDevice:(id)device encodingClass:(Class)encodingClass {
    NVMTLFailedEncoder *encoder = [self alloc];
    encoder->_failedDevice = device;
    encoder->_encodingClass = encodingClass;
    return encoder;
}
- (id)device { return _failedDevice; }
- (NSString *)label { return _failedLabel; }
- (void)setLabel:(NSString *)label { _failedLabel = [label copy]; }
- (void)endEncoding {}
- (id)renderCommandEncoder {
    return [NVMTLFailedEncoder encoderForDevice:_failedDevice encodingClass:NSClassFromString(@"NVMTLRenderCommandEncoder")];
}
- (BOOL)respondsToSelector:(SEL)selector {
    return class_getInstanceMethod([NVMTLFailedEncoder class], selector) != NULL ||
           class_getInstanceMethod(_encodingClass, selector) != NULL;
}
- (BOOL)conformsToProtocol:(Protocol *)protocol { return class_conformsToProtocol(_encodingClass, protocol); }
- (NSMethodSignature *)methodSignatureForSelector:(SEL)selector {
    Method method = class_getInstanceMethod(_encodingClass, selector);
    return method ? [NSMethodSignature signatureWithObjCTypes:method_getTypeEncoding(method)] : nil;
}
- (void)forwardInvocation:(NSInvocation *)invocation {
    NSUInteger size = invocation.methodSignature.methodReturnLength;
    if (size) {
        NSMutableData *zero = [NSMutableData dataWithLength:size];
        [invocation setReturnValue:zero.mutableBytes];
    }
}
@end
