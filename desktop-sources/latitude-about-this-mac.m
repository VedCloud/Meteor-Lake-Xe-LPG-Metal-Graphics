#import <Foundation/Foundation.h>
#import <IOKit/IOKitLib.h>
#import <CommonCrypto/CommonDigest.h>
#import <ImageIO/ImageIO.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>


static NSString *const Domain = @"com.apple.SystemProfiler";
static NSString *const Label = @"Dell Latitude 5350 2-in-1 (13-inch, 2024)";
static NSFileManager *fm;
static void require(BOOL ok, NSString *why) {
    if (!ok) @throw [NSException exceptionWithName:@"BrandingError" reason:why userInfo:nil];
}
static NSData *run(NSString *path, NSArray *args, NSData *input) {
    NSTask *t = [NSTask new]; t.executableURL = [NSURL fileURLWithPath:path]; t.arguments = args;
    NSPipe *out = [NSPipe pipe]; t.standardOutput = out;
    NSPipe *in = nil;
    if (input) { in = [NSPipe pipe]; t.standardInput = in; }
    [t launch];
    if (in) { [in.fileHandleForWriting writeData:input]; [in.fileHandleForWriting closeFile]; }
    NSData *data = [out.fileHandleForReading readDataToEndOfFile]; [t waitUntilExit];
    require(t.terminationStatus == 0, [NSString stringWithFormat:@"%@ failed (exit %d)", path, t.terminationStatus]);
    return data;
}
static id plist(NSData *data) {
    id p = [NSPropertyListSerialization propertyListWithData:data options:NSPropertyListMutableContainers format:NULL error:NULL];
    require(p != nil, @"Invalid property list"); return p;
}
static NSData *encoded(id p) {
    NSData *d = [NSPropertyListSerialization dataWithPropertyList:p format:NSPropertyListXMLFormat_v1_0 options:0 error:NULL];
    require(d != nil, @"Cannot encode property list"); return d;
}
static NSString *trim(NSData *d) { return [[[NSString alloc] initWithData:d encoding:NSUTF8StringEncoding] stringByTrimmingCharactersInSet:NSCharacterSet.whitespaceAndNewlineCharacterSet]; }
static NSString *digest(NSData *d) {
    require(d != nil && d.length < UINT32_MAX, @"Missing or oversized file");
    unsigned char bytes[CC_SHA256_DIGEST_LENGTH]; CC_SHA256(d.bytes, (CC_LONG)d.length, bytes);
    NSMutableString *s = [NSMutableString new]; for (int i=0;i<CC_SHA256_DIGEST_LENGTH;i++) [s appendFormat:@"%02x",bytes[i]]; return s;
}
static void save(id p, NSString *path) { require([encoded(p) writeToFile:path atomically:YES], @"Cannot write backup metadata"); }
static NSArray *array(id x) { return [x isKindOfClass:NSArray.class] ? x : (x ? @[x] : @[]); }
static NSString *joined(NSString *a, NSString *b) { return [a stringByAppendingPathComponent:b]; }

// Resolve inherited CoreTypes icons rather than guessing a filename for a spoofed model.
static void visit(NSString *type, NSDictionary *types, NSMutableSet *seen, NSMutableSet *icons) {
    if ([seen containsObject:type]) return; [seen addObject:type];
    NSDictionary *entry = types[type]; if (!entry) return;
    NSDictionary *decl = entry[@"decl"];
    NSString *icon = decl[@"UTTypeIcons"][@"UTTypeIconFile"] ?: decl[@"UTTypeIconFile"];
    if (icon) {
        require([icon.lastPathComponent isEqual:icon] && [icon.pathExtension isEqual:@"icns"], @"Unexpected CoreTypes icon path");
        NSString *p = joined(entry[@"resources"],icon);
        require([fm fileExistsAtPath:p], @"Resolved icon does not exist"); [icons addObject:p]; return;
    }
    for (NSString *parent in array(decl[@"UTTypeConformsTo"])) visit(parent,types,seen,icons);
}
static NSArray *resolveIcons(NSString *root, NSString *model) {
    NSMutableDictionary *types = [NSMutableDictionary new];
    NSDirectoryEnumerator *e = [fm enumeratorAtPath:root]; NSString *rel;
    while ((rel=[e nextObject])) {
        if (![rel.lastPathComponent isEqual:@"Info.plist"]) continue;
        NSString *info = joined(root,rel); NSDictionary *p = [NSDictionary dictionaryWithContentsOfFile:info];
        for (NSDictionary *d in p[@"UTExportedTypeDeclarations"]) {
            NSString *ident = d[@"UTTypeIdentifier"]; if (!ident) continue;
            types[ident] = @{@"decl":d, @"resources":joined(info.stringByDeletingLastPathComponent,@"Resources")};
        }
    }
    NSMutableSet *result = [NSMutableSet new]; BOOL matched=NO;
    for (NSString *ident in types) {
        NSDictionary *d = types[ident][@"decl"];
        for (NSString *tag in array(d[@"UTTypeTagSpecification"][@"com.apple.device-model-code"])) {
            if ([tag isEqual:model] || [tag hasPrefix:[model stringByAppendingString:@"@"]]) {
                matched=YES; visit(ident,types,[NSMutableSet new],result);
            }
        }
    }
    require(matched && result.count>0 && result.count<=4, @"Cannot uniquely resolve this model's CoreTypes icons");
    return [result.allObjects sortedArrayUsingSelector:@selector(compare:)];
}
static NSDictionary *readPref(void) {
    CFPreferencesAppSynchronize((__bridge CFStringRef)Domain);
    NSMutableDictionary *p = [NSMutableDictionary new];
    for (NSString *key in @[@"CPU Names",@"ModelNames"]) {
        id value=CFBridgingRelease(CFPreferencesCopyAppValue((__bridge CFStringRef)key,(__bridge CFStringRef)Domain));
        if (value) { require([value isKindOfClass:NSDictionary.class], @"Unexpected model preference format"); p[key]=value; }
    }
    return p;
}
static NSDictionary *changedPrefs(NSDictionary *old, NSString *suffix, NSString *model, NSString *locale, NSArray *languages) {
    NSMutableDictionary *next = [NSMutableDictionary new];
    NSMutableDictionary *cpu = [old[@"CPU Names"] mutableCopy] ?: [NSMutableDictionary new];
    for (NSString *k in cpu.allKeys) if ([k hasPrefix:[suffix stringByAppendingString:@"-"]]) cpu[k]=Label;
    NSString *region = [NSLocale componentsFromLocaleIdentifier:locale][NSLocaleCountryCode] ?: @"GB";
    for (NSString *lang in languages) {
        NSString *l = [[lang componentsSeparatedByString:@"-"] firstObject];
        cpu[[NSString stringWithFormat:@"%@-%@-%@_%@",suffix,l,region,region]]=Label;
    }
    NSMutableDictionary *models = [old[@"ModelNames"] mutableCopy] ?: [NSMutableDictionary new];
    for (NSString *k in models.allKeys) if ([k isEqual:model] || [k hasPrefix:[model stringByAppendingString:@"-"]] || [k hasPrefix:[suffix stringByAppendingString:@"-"]]) models[k]=Label;
    models[model]=Label;
    next[@"CPU Names"]=cpu; next[@"ModelNames"]=models; return next;
}
static NSDictionary *restoredPrefs(NSDictionary *current, NSDictionary *prior) {
    NSMutableDictionary *merged=[current mutableCopy];
    for (NSString *key in @[@"CPU Names",@"ModelNames"]) {
        NSMutableDictionary *values=[current[key] mutableCopy] ?: [NSMutableDictionary new];
        NSDictionary *installed=prior[@"prefsInstalled"][key], *before=prior[@"prefsBefore"][key];
        for (NSString *k in installed) if (![installed[k] isEqual:before[k]] && [values[k] isEqual:installed[k]]) {
            if (before[k]) values[k]=before[k]; else [values removeObjectForKey:k];
        }
        if (values.count || before) merged[key]=values; else [merged removeObjectForKey:key];
    }
    return merged;
}
static void setPrefs(NSDictionary *p) {
    for (NSString *key in @[@"CPU Names",@"ModelNames"]) CFPreferencesSetAppValue((__bridge CFStringRef)key,(__bridge CFPropertyListRef)p[key],(__bridge CFStringRef)Domain);
    require(CFPreferencesAppSynchronize((__bridge CFStringRef)Domain), @"Preference synchronization failed");
    require([readPref() isEqual:p], @"Preference readback failed");
}
static NSDictionary *info(NSString *device) { return plist(run(@"/usr/sbin/diskutil",@[@"info",@"-plist",device],nil)); }
static NSDictionary *systemVolume(NSDictionary *root, NSDictionary *listing, NSDictionary *(^getInfo)(NSString *)) {
    NSString *group=root[@"APFSVolumeGroupID"]; require(group.length>0 && [root[@"APFSSnapshot"] boolValue], @"Expected an APFS system snapshot");
    NSMutableArray *matches=[NSMutableArray new];
    for (NSDictionary *c in listing[@"Containers"]) {
        if (![c[@"ContainerReference"] isEqual:root[@"APFSContainerReference"]]) continue;
        for (NSDictionary *v in c[@"Volumes"]) if ([v[@"Roles"] containsObject:@"System"]) {
            NSDictionary *i=getInfo(v[@"DeviceIdentifier"]);
            if ([i[@"APFSVolumeGroupID"] isEqual:group] && ![i[@"APFSSnapshot"] boolValue]) [matches addObject:i];
        }
    }
    require(matches.count==1, @"Cannot identify exactly one matching APFS System volume"); return matches[0];
}
static NSData *asUser(NSString *self, uid_t uid, NSString *mode, NSData *input) {
    NSString *u=[NSString stringWithFormat:@"%u",uid];
    NSString *directory=joined(@"/private/var/tmp",[@"latitude-prefs-" stringByAppendingString:NSUUID.UUID.UUIDString]);
    require([fm createDirectoryAtPath:directory withIntermediateDirectories:NO attributes:@{NSFilePosixPermissions:@0755} error:NULL],@"Cannot stage per-user helper");
    NSString *helper=joined(directory,@"helper");
    @try {
        require([fm copyItemAtPath:self toPath:helper error:NULL] && chmod(helper.fileSystemRepresentation,0755)==0,@"Cannot stage per-user helper");
        return run(@"/bin/launchctl",@[@"asuser",u,@"/usr/bin/sudo",@"-H",@"-u",[@"#" stringByAppendingString:u],helper,mode],input);
    } @finally { [fm removeItemAtPath:directory error:NULL]; }
}
static void copy(NSString *src, NSString *dst) {
    require([fm createDirectoryAtPath:dst.stringByDeletingLastPathComponent withIntermediateDirectories:YES attributes:nil error:NULL], @"Cannot create backup directory");
    require([fm copyItemAtPath:src toPath:dst error:NULL], [@"Cannot copy " stringByAppendingString:src.lastPathComponent]);
}
static NSString *userCacheDirectory(void) {
    char path[PATH_MAX]; size_t n=confstr(_CS_DARWIN_USER_CACHE_DIR,path,sizeof path);
    require(n>1 && n<=sizeof path,@"Cannot resolve desktop user's cache directory");return @(path);
}
static BOOL quarantineIconDirectory(NSString *path,NSString *backup) {
    struct stat st;
    if(lstat(path.fileSystemRepresentation,&st)!=0){require(errno==ENOENT,@"Cannot inspect icon cache");return NO;}
    require(S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode),@"Icon cache is not a plain directory");
    require(![fm fileExistsAtPath:backup],@"Cache backup already exists");
    require([fm moveItemAtPath:path toPath:backup error:NULL],@"Cannot preserve icon cache");
    NSDictionary *attrs=@{NSFileOwnerAccountID:@(st.st_uid),NSFileGroupOwnerAccountID:@(st.st_gid),NSFilePosixPermissions:@(st.st_mode&07777)};
    if(![fm createDirectoryAtPath:path withIntermediateDirectories:NO attributes:attrs error:NULL]) {
        require([fm moveItemAtPath:backup toPath:path error:NULL],@"Cache recreation and rollback failed; see saved cache");
        require(NO,@"Cannot recreate icon cache; original restored");
    }
    return YES;
}
static void targetGuard(void) {
    require(geteuid()==0, @"Run with sudo on the Dell");
    require([trim(run(@"/usr/bin/uname",@[@"-m"],nil)) isEqual:@"x86_64"], @"Target is the Intel Dell only");
    require([trim(run(@"/usr/bin/sw_vers",@[@"-buildVersion"],nil)) isEqual:@"24H32"], @"Target is Sequoia 24H32 only");
    require([trim(run(@"/usr/sbin/sysctl",@[@"-n",@"hw.model"],nil)) isEqual:@"MacBookPro16,2"], @"Unexpected target SMBIOS");
    NSString *gpu=trim(run(@"/usr/sbin/ioreg",@[@"-r",@"-n",@"GFX0",@"-l",@"-w0"],nil));
    require([gpu containsString:@"\"device-id\" = <457d0000>"], @"Expected Dell Meteor Lake GPU not visible");
}
static void refreshIcons(NSString *self,NSString *bundle) {
    NSString *uidText=NSProcessInfo.processInfo.environment[@"SUDO_UID"];uid_t uid=(uid_t)uidText.intValue;
    require(uid>=501 && getpwuid(uid),@"Cannot identify desktop user for icon refresh");
    NSDictionary *latest=[NSDictionary dictionaryWithContentsOfFile:@"/Library/Application Support/LatitudeBranding/latest.plist"];
    NSString *backup=latest[@"backup"];
    require([backup hasPrefix:@"/Library/Application Support/LatitudeBranding/"] && ![backup.pathComponents containsObject:@".."],@"Missing validated branding backup");
    NSDictionary *saved=[NSDictionary dictionaryWithContentsOfFile:joined(backup,@"backup.plist")];
    require([saved[@"uid"] unsignedIntValue]==uid,@"Branding backup belongs to another user");
    NSString *assetHash=digest([NSData dataWithContentsOfFile:joined(bundle,@"DellLatitude5350.icns")]);
    require([saved[@"installedIconSHA256"] isEqual:assetHash],@"Installed icon differs from this bundle");
    NSString *marker=joined(backup,@"icon-cache-refresh-0.3.14.plist");
    if([fm fileExistsAtPath:marker]){puts("About This Mac icon-cache refresh already prepared; reboot to apply.");return;}
    NSDictionary *context=plist(asUser(self,uid,@"prefs-context",nil));
    NSString *cache=[context[@"cacheDirectory"] stringByStandardizingPath];
    require(([cache hasPrefix:@"/var/folders/"] || [cache hasPrefix:@"/private/var/folders/"]) && [cache.lastPathComponent isEqual:@"C"],@"Unexpected per-user cache path");
    struct stat st;require(stat(cache.fileSystemRepresentation,&st)==0 && S_ISDIR(st.st_mode) && st.st_uid==uid,@"User cache ownership mismatch");
    NSString *store=joined(backup,[@"icon-caches-" stringByAppendingString:NSUUID.UUID.UUIDString]);
    require([fm createDirectoryAtPath:store withIntermediateDirectories:NO attributes:@{NSFilePosixPermissions:@0700} error:NULL],@"Cannot create icon-cache backup");
    NSArray *paths=@[@"/Library/Caches/com.apple.iconservices.store",joined(cache,@"com.apple.iconservices"),joined(cache,@"com.apple.iconservices.store")];
    NSMutableArray *items=[NSMutableArray new];
    for(NSUInteger i=0;i<paths.count;i++) {
        NSString *path=paths[i],*dest=joined(store,[@(i).stringValue stringByAppendingString:@".cache"]);
        BOOL moved=quarantineIconDirectory(path,dest);
        [items addObject:@{@"path":path,@"backup":dest,@"preserved":@(moved)}];
        save(@{@"items":items,@"status":@"prepared-awaiting-reboot"},joined(store,@"manifest.plist"));
    }
    save(@{@"items":items,@"installedIconSHA256":assetHash,@"status":@"prepared-awaiting-reboot"},marker);
    printf("About This Mac icon caches preserved at %s. Reboot refreshes in-memory icons.\n",store.fileSystemRepresentation);
}
static void diagnose(NSString *self, NSString *bundle) {
    NSDictionary *root=info(@"/");
    NSDictionary *latest=[NSDictionary dictionaryWithContentsOfFile:@"/Library/Application Support/LatitudeBranding/latest.plist"];
    NSString *backup=latest[@"backup"];
    NSDictionary *saved=backup ? [NSDictionary dictionaryWithContentsOfFile:joined(backup,@"backup.plist")] : nil;
    NSData *asset=[NSData dataWithContentsOfFile:joined(bundle,@"DellLatitude5350.icns")];
    NSMutableArray *icons=[NSMutableArray new];
    for (NSString *p in resolveIcons(@"/System/Library/CoreServices/CoreTypes.bundle",@"MacBookPro16,2")) {
        NSData *bytes=[NSData dataWithContentsOfFile:p];
        CGImageSourceRef source=CGImageSourceCreateWithData((__bridge CFDataRef)bytes,NULL);
        NSMutableDictionary *item=[@{@"path":p,@"sha256":digest(bytes),@"bytes":@(bytes.length),@"matchesBundledAsset":@(asset && [bytes isEqual:asset]),@"decodableFrames":@(source ? CGImageSourceGetCount(source) : 0)} mutableCopy];
        if (source && CGImageSourceGetCount(source)) {
            CGImageRef image=CGImageSourceCreateImageAtIndex(source,CGImageSourceGetCount(source)-1,NULL);
            if (image) {item[@"decodedWidth"]=@(CGImageGetWidth(image));item[@"decodedHeight"]=@(CGImageGetHeight(image));CGImageRelease(image);}
        }
        if (source) CFRelease(source);
        [icons addObject:item];
    }
    NSMutableDictionary *d=[@{@"activeRootSnapshotUUID":root[@"APFSSnapshotUUID"] ?: @"",@"activeRootSnapshotName":root[@"APFSSnapshotName"] ?: @"",@"activeVolumeGroup":root[@"APFSVolumeGroupID"] ?: @"",@"icons":icons,@"backup":backup ?: @"missing",@"backupStatus":saved[@"status"] ?: @"missing",@"previousSnapshotUUID":saved[@"previousSnapshotUUID"] ?: @"",@"bundledAssetSHA256":asset ? digest(asset) : @"missing",@"imageCacheState":@"Not inferred from files; active icon hashes and decoded contents are evidence only"} mutableCopy];
    if (saved) {
        d[@"iconCacheRefresh"]=[NSDictionary dictionaryWithContentsOfFile:joined(backup,@"icon-cache-refresh-0.3.14.plist")] ?: @{@"status":@"not-yet-prepared"};
        NSDictionary *v=systemVolume(root,plist(run(@"/usr/sbin/diskutil",@[@"apfs",@"list",@"-plist"],nil)),^NSDictionary *(NSString *device){return info(device);});
        d[@"backupMatchesSystemVolume"]=@([saved[@"volumeUUID"] isEqual:v[@"DiskUUID"]]);
        d[@"snapshotChangedSinceInstall"]=@(![saved[@"previousSnapshotUUID"] isEqual:root[@"APFSSnapshotUUID"]]);
        d[@"preferences"]=plist(asUser(self,[saved[@"uid"] unsignedIntValue],@"prefs-read",nil));
    }
    [[NSFileHandle fileHandleWithStandardOutput] writeData:encoded(d)];
}
static void selfTest(void) {
    NSDictionary *old=@{@"CPU Names":@{@"ABCD-en-GB_GB":@"old",@"OTHER-en-US_US":@"keep"},@"ModelNames":@{@"MacBookPro16,2":@"old",@"Other":@"keep"}};
    NSDictionary *n=changedPrefs(old,@"ABCD",@"MacBookPro16,2",@"en_GB",@[@"en-GB"]);
    require([n[@"CPU Names"][@"ABCD-en-GB_GB"] isEqual:Label] && [n[@"CPU Names"][@"OTHER-en-US_US"] isEqual:@"keep"] && [n[@"ModelNames"][@"Other"] isEqual:@"keep"], @"Preference fixture failed");
    NSMutableDictionary *modified=[n mutableCopy];
    NSMutableDictionary *names=[n[@"ModelNames"] mutableCopy]; names[@"LaterUserEntry"]=@"new"; modified[@"ModelNames"]=names;
    NSDictionary *restored=restoredPrefs(modified,@{@"prefsBefore":old,@"prefsInstalled":n});
    require([restored[@"CPU Names"] isEqual:old[@"CPU Names"]] && [restored[@"ModelNames"][@"MacBookPro16,2"] isEqual:@"old"] && [restored[@"ModelNames"][@"LaterUserEntry"] isEqual:@"new"], @"Preference restoration fixture failed");
    NSDictionary *empty=restoredPrefs(changedPrefs(@{},@"ABCD",@"MacBookPro16,2",@"en_GB",@[@"en-GB"]),@{@"prefsBefore":@{},@"prefsInstalled":changedPrefs(@{},@"ABCD",@"MacBookPro16,2",@"en_GB",@[@"en-GB"])});
    require(empty.count==0,@"Absent preference keys must remain absent on restoration");
    NSDictionary *root=@{@"APFSVolumeGroupID":@"g",@"APFSSnapshot":@YES,@"APFSContainerReference":@"disk5"};
    NSDictionary *listing=@{@"Containers":@[@{@"ContainerReference":@"disk5",@"Volumes":@[@{@"Roles":@[@"Data"],@"DeviceIdentifier":@"disk5s1"},@{@"Roles":@[@"System"],@"DeviceIdentifier":@"disk5s7"}]}]};
    NSDictionary *v=systemVolume(root,listing,^NSDictionary *(NSString *s){return @{@"APFSVolumeGroupID":@"g",@"DeviceIdentifier":s,@"APFSSnapshot":@NO};});
    require([v[@"DeviceIdentifier"] isEqual:@"disk5s7"], @"System volume fixture failed");
    BOOL refused=NO; @try {systemVolume(root,@{@"Containers":@[]},^NSDictionary *(NSString *s){(void)s;return @{};});} @catch(NSException *e) {refused=YES;}
    require(refused,@"Ambiguous/missing system volume was accepted");
    NSString *fixture=joined(NSTemporaryDirectory(),[@"latitude-icon-fixture-" stringByAppendingString:NSUUID.UUID.UUIDString]);
    require([fm createDirectoryAtPath:fixture withIntermediateDirectories:NO attributes:nil error:NULL],@"Cannot create cache fixture");
    @try {
        NSString *cache=joined(fixture,@"cache"),*backup=joined(fixture,@"backup");
        require([fm createDirectoryAtPath:cache withIntermediateDirectories:NO attributes:@{NSFilePosixPermissions:@0711} error:NULL],@"Cache fixture mkdir failed");
        require([[@"old icon" dataUsingEncoding:NSUTF8StringEncoding] writeToFile:joined(cache,@"entry") atomically:YES],@"Cache fixture write failed");
        require(quarantineIconDirectory(cache,backup) && [fm fileExistsAtPath:joined(backup,@"entry")] && [fm contentsOfDirectoryAtPath:cache error:NULL].count==0,@"Cache preservation failed");
        struct stat st;require(stat(cache.fileSystemRepresentation,&st)==0 && (st.st_mode&0777)==0711,@"Cache permissions changed");
        require(!quarantineIconDirectory(joined(fixture,@"absent"),joined(fixture,@"unused")),@"Absent cache must be skipped");
        require(symlink(cache.fileSystemRepresentation,joined(fixture,@"link").fileSystemRepresentation)==0,@"Symlink fixture failed");
        BOOL rejected=NO;@try{quarantineIconDirectory(joined(fixture,@"link"),joined(fixture,@"bad"));}@catch(NSException *e){rejected=YES;}
        require(rejected,@"Symlink cache was accepted");
    } @finally {[fm removeItemAtPath:fixture error:NULL];}
    NSArray *icons=resolveIcons(@"/System/Library/CoreServices/CoreTypes.bundle",@"MacBookPro16,2");
    require(icons.count>0,@"Inherited icon resolution failed");
    printf("PASS preferences preserve unrelated entries; APFS volume discovery/refusal; CoreTypes inheritance (%lu icons); reversible cache preservation/symlink refusal. No system writes.\n",(unsigned long)icons.count);
}

int main(int argc, const char *argv[]) { @autoreleasepool {
    fm=[NSFileManager defaultManager];
    @try {
        require(argc>=2,@"Expected install, restore, prefs-read, prefs-write or --self-test");
        NSString *mode=@(argv[1]);
        if ([mode isEqual:@"--self-test"]) {selfTest();return 0;}
        if ([mode isEqual:@"prefs-context"]) {require(getuid()!=0,@"Preferences must run as the desktop user");[[NSFileHandle fileHandleWithStandardOutput] writeData:encoded(@{@"prefs":readPref(),@"locale":NSLocale.currentLocale.localeIdentifier,@"languages":NSLocale.preferredLanguages,@"cacheDirectory":userCacheDirectory()})];return 0;}
        if ([mode isEqual:@"prefs-read"]) {require(getuid()!=0,@"Preferences must run as the desktop user");[[NSFileHandle fileHandleWithStandardOutput] writeData:encoded(readPref())];return 0;}
        if ([mode isEqual:@"prefs-write"]) {require(getuid()!=0,@"Preferences must run as the desktop user");setPrefs(plist([[NSFileHandle fileHandleWithStandardInput] readDataToEndOfFile]));return 0;}
        if ([mode isEqual:@"refresh-icons"]) {targetGuard();require(argc==3,@"Expected branding bundle path");refreshIcons([@(argv[0]) stringByStandardizingPath],@(argv[2]));return 0;}
        if ([mode isEqual:@"diagnose"]) {targetGuard();require(argc==3,@"Expected branding bundle path");diagnose([@(argv[0]) stringByStandardizingPath],@(argv[2]));return 0;}
        BOOL restore=[mode isEqual:@"restore"]; require(restore || [mode isEqual:@"install"],@"Unknown operation"); targetGuard();
        require(argc==3,@"Expected bundle directory or backup directory");
        NSString *self=[@(argv[0]) stringByStandardizingPath]; require(self.isAbsolutePath,@"Use an absolute executable path");
        NSString *argument=[@(argv[2]) stringByStandardizingPath]; require(argument.isAbsolutePath,@"Use an absolute path");
        NSDictionary *prior=restore ? [NSDictionary dictionaryWithContentsOfFile:joined(argument,@"backup.plist")] : nil;
        require(!restore || prior!=nil,@"Invalid branding backup");
        NSString *uidText=NSProcessInfo.processInfo.environment[@"SUDO_UID"];
        uid_t uid=restore ? [prior[@"uid"] unsignedIntValue] : (uid_t)uidText.intValue;
        struct passwd *pw=getpwuid(uid); require(uid>=501 && pw && pw->pw_dir,@"Cannot identify the invoking desktop user");
        NSString *bundle=argument;
        NSString *icon=joined(bundle,@"DellLatitude5350.icns");
        if (!restore) {NSData *ic=[NSData dataWithContentsOfFile:icon];require(ic.length>8 && memcmp(ic.bytes,"icns",4)==0,@"Invalid bundled ICNS asset");}
        NSDictionary *root=info(@"/");
        NSDictionary *volume=systemVolume(root,plist(run(@"/usr/sbin/diskutil",@[@"apfs",@"list",@"-plist"],nil)),^NSDictionary *(NSString *d){return info(d);});
        NSString *volumeUUID=volume[@"DiskUUID"]; require(volumeUUID.length>0,@"System volume UUID missing");
        if (restore) require([prior[@"volumeUUID"] isEqual:volumeUUID],@"Backup belongs to a different system volume");
        // Check the authenticated-root policy directly, without changing SIP or NVRAM.
        void *handle=dlopen("/usr/lib/libSystem.B.dylib",RTLD_LAZY);
        int (*csrCheck)(uint32_t)=dlsym(handle,"csr_check");
        require(csrCheck && csrCheck(0x800)==0,@"Authenticated-root is enabled; cosmetic icon left unchanged");
        NSString *latestFile=@"/Library/Application Support/LatitudeBranding/latest.plist";
        NSDictionary *latest=[NSDictionary dictionaryWithContentsOfFile:latestFile];
        NSDictionary *previous=latest ? [NSDictionary dictionaryWithContentsOfFile:joined(latest[@"backup"],@"backup.plist")] : nil;
        if (!restore && [previous[@"uid"] unsignedIntValue]==uid && [previous[@"status"] isEqual:@"installed-awaiting-reboot"] && [previous[@"volumeUUID"] isEqual:volumeUUID] && [previous[@"installedIconSHA256"] isEqual:digest([NSData dataWithContentsOfFile:icon])]) {
            BOOL active=YES;
            for (NSString *rel in previous[@"icons"]) if (![digest([NSData dataWithContentsOfFile:joined(@"/",rel)]) isEqual:previous[@"installedIconSHA256"]]) active=NO;
            if (active && previous[@"icons"]) {
                // Preserve the first pre-branding backup when the graphics installer is run again.
                NSMutableDictionary *current=[plist(asUser(self,uid,@"prefs-read",nil)) mutableCopy];
                for (NSString *key in @[@"CPU Names",@"ModelNames"]) {
                    NSMutableDictionary *values=[current[key] mutableCopy] ?: [NSMutableDictionary new];
                    NSDictionary *installed=previous[@"prefsInstalled"][key], *before=previous[@"prefsBefore"][key];
                    for (NSString *k in installed) if (![installed[k] isEqual:before[k]]) values[k]=installed[k];
                    current[key]=values;
                }
                asUser(self,uid,@"prefs-write",encoded(current));
                printf("About This Mac branding already active. Original backup: %s\n",[latest[@"backup"] fileSystemRepresentation]); return 0;
            }
        }
        NSString *backup=restore ? argument : [@"/Library/Application Support/LatitudeBranding" stringByAppendingPathComponent:[NSString stringWithFormat:@"0.3.11-%@",NSUUID.UUID.UUIDString]];
        require(restore || [fm createDirectoryAtPath:backup withIntermediateDirectories:YES attributes:@{NSFilePosixPermissions:@0700} error:NULL],@"Cannot create branding backup");
        NSString *mount= [@"/private/var/tmp" stringByAppendingPathComponent:[@"latitude-branding-" stringByAppendingString:NSUUID.UUID.UUIDString]];
        BOOL mounted=NO, blessed=NO, prefsChanged=NO; NSMutableArray *written=[NSMutableArray new];
        NSDictionary *context=plist(asUser(self,uid,@"prefs-context",nil));
        NSDictionary *prefsBefore=context[@"prefs"];
        NSMutableDictionary *state=[NSMutableDictionary new]; NSArray *relative;
        require([fm createDirectoryAtPath:mount withIntermediateDirectories:NO attributes:@{NSFilePosixPermissions:@0700} error:NULL],@"Cannot create private mountpoint");
        @try {
            run(@"/sbin/mount",@[@"-t",@"apfs",@"-o",@"nobrowse,rw",volume[@"DeviceNode"],mount],nil); mounted=YES;
            NSDictionary *mountedInfo=info(mount); require([mountedInfo[@"DiskUUID"] isEqual:volumeUUID],@"Mounted system volume mismatch");
            NSString *core=@"System/Library/CoreServices/CoreTypes.bundle";
            require([[[NSDictionary dictionaryWithContentsOfFile:joined(mount,@"System/Library/CoreServices/SystemVersion.plist")] objectForKey:@"ProductBuildVersion"] isEqual:@"24H32"],@"Underlying System volume build differs from booted snapshot");
            if (restore) relative=prior[@"icons"];
            else {
                NSMutableArray *r=[NSMutableArray new];
                for (NSString *p in resolveIcons(joined(mount,core),@"MacBookPro16,2")) [r addObject:[p substringFromIndex:mount.length+1]];
                relative=r;
            }
            require(relative.count>0 && relative.count<=4,@"Invalid icon backup list");
            for (NSString *rel in relative) {
                require([rel hasPrefix:[core stringByAppendingString:@"/"]] && ![rel.pathComponents containsObject:@".."] && [rel.pathExtension isEqual:@"icns"],@"Unsafe backup icon path");
                NSString *dest=joined(mount,rel); struct stat st;
                require(lstat(dest.fileSystemRepresentation,&st)==0 && S_ISREG(st.st_mode),@"Icon is not a regular file");
                if (restore) require([digest([NSData dataWithContentsOfFile:dest]) isEqual:prior[@"installedIconSHA256"]],@"Icon has changed since installation; refusing to overwrite it");
                else {
                    require([[NSData dataWithContentsOfFile:dest] isEqual:[NSData dataWithContentsOfFile:joined(@"/",rel)]],@"Pending System volume changes detected; refusing to create a cosmetic snapshot");
                    copy(dest,joined(backup,rel));
                }
            }
            if (!restore) {
                io_service_t platform=IOServiceGetMatchingService(kIOMainPortDefault,IOServiceMatching("IOPlatformExpertDevice"));
                NSString *serial=platform ? CFBridgingRelease(IORegistryEntryCreateCFProperty(platform,CFSTR("IOPlatformSerialNumber"),kCFAllocatorDefault,0)) : nil;
                if (platform) IOObjectRelease(platform);
                require(serial.length>=4,@"Cannot derive model cache key from platform serial");
                NSString *suffix=[serial substringFromIndex:serial.length-4];
                NSDictionary *next=changedPrefs(prefsBefore,suffix,@"MacBookPro16,2",context[@"locale"],context[@"languages"]);
                state=[@{@"uid":@(uid),@"volumeUUID":volumeUUID,@"build":@"24H32",@"previousSnapshotUUID":root[@"APFSSnapshotUUID"] ?: @"",@"previousSnapshotName":root[@"APFSSnapshotName"] ?: @"",@"icons":relative,@"prefsBefore":prefsBefore,@"prefsInstalled":next,@"installedIconSHA256":digest([NSData dataWithContentsOfFile:icon]),@"status":@"prepared"} mutableCopy];
                save(state,joined(backup,@"backup.plist"));
                copy(icon,joined(backup,@"installed.icns"));
                copy(self,joined(backup,@"latitude-about-this-mac")); chmod(joined(backup,@"latitude-about-this-mac").fileSystemRepresentation,0755);
            }
            for (NSString *rel in relative) {
                NSString *dest=joined(mount,rel); NSData *data=[NSData dataWithContentsOfFile:restore ? joined(backup,rel) : icon];
                require(data!=nil,@"Missing replacement icon");
                // Keep the original inode, ownership, permissions and extended attributes.
                NSFileHandle *h=[NSFileHandle fileHandleForWritingAtPath:dest]; require(h!=nil,@"Cannot write system icon");
                [written addObject:rel]; [h truncateFileAtOffset:0]; [h writeData:data]; [h synchronizeFile]; [h closeFile];
                require([[NSData dataWithContentsOfFile:dest] isEqual:data],@"Icon verification failed");
            }
            NSDictionary *next=restore ? prior[@"prefsBefore"] : state[@"prefsInstalled"];
            if (restore) {
                next=restoredPrefs(prefsBefore,prior);
            }
            prefsChanged=YES; asUser(self,uid,@"prefs-write",encoded(next));
            run(@"/usr/sbin/bless",@[@"--folder",joined(mount,@"System/Library/CoreServices"),@"--bootefi",@"--create-snapshot"],nil); blessed=YES;
            if (!restore) {state[@"status"]=@"installed-awaiting-reboot";save(state,joined(backup,@"backup.plist"));save(@{@"backup":backup},latestFile);chmod(latestFile.fileSystemRepresentation,0600);}
            if (restore) {NSMutableDictionary *done=[prior mutableCopy];done[@"status"]=@"restored-awaiting-reboot";save(done,joined(backup,@"backup.plist"));}
            printf("About This Mac %s prepared.\nBackup: %s\n",restore?"restore":"Dell Latitude 5350 2-in-1 (13-inch, 2024)",backup.fileSystemRepresentation);
            printf("Icon snapshot takes effect after reboot; display/cache formatting must be checked on the Dell.\n");
        } @catch(NSException *error) {
            if (!blessed) {
                BOOL rollbackOK=YES;
                for (NSString *rel in written) @try {
                    NSData *old=[NSData dataWithContentsOfFile:restore ? joined(backup,@"installed.icns") : joined(backup,rel)];
                    require(old!=nil,@"Missing rollback icon");
                    NSFileHandle *h=[NSFileHandle fileHandleForWritingAtPath:joined(mount,rel)];require(h!=nil,@"Cannot roll back icon");[h truncateFileAtOffset:0];[h writeData:old];[h synchronizeFile];[h closeFile];
                } @catch(NSException *e) {rollbackOK=NO;fprintf(stderr,"Icon rollback failed: %s\n",e.reason.UTF8String);}
                if (prefsChanged) @try {asUser(self,uid,@"prefs-write",encoded(prefsBefore));} @catch(NSException *e) {rollbackOK=NO;}
                if (!rollbackOK) fprintf(stderr,"Manual recovery needed; backup: %s\n",backup.fileSystemRepresentation);
            }
            @throw error;
        } @finally {
            if (mounted) @try {run(@"/sbin/umount",@[mount],nil);} @catch(NSException *e) {fprintf(stderr,"Unmount failed: %s\n",mount.fileSystemRepresentation);}
            // Never recursively remove a mountpoint, including after an unmount failure.
            rmdir(mount.fileSystemRepresentation);
        }
        return 0;
    } @catch(NSException *e) {fprintf(stderr,"About This Mac: %s\n",e.reason.UTF8String);return 2;}
}}
