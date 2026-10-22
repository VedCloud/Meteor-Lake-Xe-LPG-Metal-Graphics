from pathlib import Path
import re, subprocess, tempfile

r = Path(__file__).resolve().parent.parent
s = (r / 'latitude-desktop/Library/NullMoth/load-panel-framebuffer.sh').read_text()
predicates = re.findall(r"/usr/bin/log stream .*?--predicate '([^']+)'", s)
assert len(predicates) == 2 and '--level debug' not in s
source = r'''
#import <Foundation/Foundation.h>
#include <assert.h>
int main(int argc, const char **argv) { @autoreleasepool {
    assert(argc == 3);
    NSPredicate *early = [NSPredicate predicateWithFormat:@"%K == %@", @"process", @"unused"];
    early = [NSPredicate predicateWithFormat:[NSString stringWithUTF8String:argv[1]]];
    NSPredicate *live = [NSPredicate predicateWithFormat:[NSString stringWithUTF8String:argv[2]]];
    NSDictionary *event = @{@"process":@"WindowServer", @"logType":@"default", @"subsystem":@"com.apple.Multitouch", @"composedMessage":@"DHML: clearMotion"};
    assert(![early evaluateWithObject:event] && ![live evaluateWithObject:event]);
    NSMutableDictionary *test = [event mutableCopy];
    test[@"composedMessage"] = @"NullMoth: present-perf0.3.8: wait=10";
    assert([early evaluateWithObject:test] && [live evaluateWithObject:test]);
    test[@"composedMessage"] = @"timed out fence 123";
    test[@"subsystem"] = @"com.apple.coreanimation";
    assert([live evaluateWithObject:test]);
    test = [event mutableCopy]; test[@"logType"] = @"error";
    assert([early evaluateWithObject:test] && [live evaluateWithObject:test]);
    test[@"logType"] = @"fault";
    assert([early evaluateWithObject:test] && [live evaluateWithObject:test]);
    test[@"process"] = @"kernel"; test[@"logType"] = @"default";
    test[@"composedMessage"] = @"drmn0: i915 GPU HANG";
    assert([early evaluateWithObject:test] && [live evaluateWithObject:test]);
    puts("PASS actual NSPredicates: HID chatter excluded, plugin timings, CoreAnimation fences, WindowServer errors/faults and kernel GPU diagnostics preserved");
} return 0; }
'''
with tempfile.TemporaryDirectory() as temp:
    p = Path(temp)
    (p / 'test.m').write_text(source)
    subprocess.run(['xcrun', 'clang', '-Wno-format-security', '-fobjc-arc', '-framework', 'Foundation', str(p / 'test.m'), '-o', str(p / 'test')], check=True)
    subprocess.run([str(p / 'test'), *predicates], check=True)
