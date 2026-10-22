Date/Time:        2026-10-19 16:56:22.959 +0100
End time:         2026-10-19 16:56:28.481 +0100
OS Version:       Mac OS X 15.8.1 (Build 24H32)
Architecture:     x86_64h
Report Version:   60

Data Source:      Stackshots
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Shared Cache:     E7CA1FD9-6335-372F-8536-8A879AF70D10 slid base address 0x7ff80813e000, slide 0x813e000 (DriverKit)
Reason:           (1 monitored services unresponsive): checkin with service: WindowServer (0 induced crashes) failed to send ping message, WindowServer watchdog request handling code likely hung 80 seconds since last successful checkin, 6 total successful checkins since 16

Command:          WindowServer
Path:             /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/Resources/WindowServer
Codesigning ID:   com.apple.WindowServer
Architecture:     x86_64
PID:              635
Time Since Fork:  71s

Event:            service process watchdog
Duration:         5.52s
Steps:            12

Hardware model:   MacBookPro16,2
Active cpus:      14
Memory size:      16 GB
HW page size:     4096
VM page size:     4096
Boot args:        -v debug=0x100 keepsyms=1 itlwm_he=1 itlwm_mtl_internal_aux=1 amfi_get_out_of_my_way=0x1 amfi=0x80 ipc_control_port_options=0 

Time Awake Since Boot: 185s

Total CPU Time:   0.276s
Models:           UNKNOWN

OS Cryptex File Extents: 1


Process:          WindowServer [635] (suspended) (zombie)
UUID:             1746737B-B232-3669-B691-67604E4C8338
Path:             /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/Resources/WindowServer
Codesigning ID:   com.apple.WindowServer
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        97.13 MB
Time Since Fork:  71s
Num samples:      12 (1-12)
Note:             Suspended for 12 samples
Note:             Terminated (zombie) for 12 samples
Num threads:      1

  Thread 0x1398    Thread name "ws_main_thread"    12 samples (1-12)    priority 63 (base 63)
  <IO tier 0>
  12  <truncated backtrace> 1-12
   *12  ??? (kernel + 522557) [0xffffff800036793d] 1-12
     *12  ??? (kernel + 916716) [0xffffff80003c7cec] 1-12
       *12  ??? (kernel + 6468039) [0xffffff80009131c7] 1-12
         *12  ??? (kernel + 6466919) [0xffffff8000912d67] 1-12
           *12  ??? (kernel + 6327937) [0xffffff80008f0e81] 1-12
             *12  ??? (kernel + 1245366) [0xffffff80004180b6] 1-12
               *12  ??? (kernel + 866775) [0xffffff80003bb9d7] 1-12
                 *12  ??? (kernel + 824330) [0xffffff80003b140a] 1-12
                   *12  ??? (kernel + 2093824) [0xffffff80004e7300] 1-12
                     *12  ??? (kernel + 8704336) [0xffffff8000b35150] 1-12
                       *12  ??? (NMXGPU + 2403642) [0xffffff7f97445d3a] 1-12
                         *12  ??? (NMXGPU + 2401706) [0xffffff7f974455aa] 1-12
                           *12  ??? (NMXGPU + 2318407) [0xffffff7f97431047] 1-12
                             *12  ??? (NMXGPU + 2217896) [0xffffff7f974187a8] 1-12
                               *12  ??? (NMXGPU + 249910) [0xffffff7f97238036] 1-12
                                 *12  ??? (NMXGPU + 248771) [0xffffff7f97237bc3] 1-12
                                   *12  ??? (NMXGPU + 511833) [0xffffff7f97277f59] 1-12
                                     *12  ??? (NMXGPU + 526856) [0xffffff7f9727ba08] 1-12
                                       *12  ??? (NMXGPU + 528586) [0xffffff7f9727c0ca] 1-12
                                         *12  ??? (NMXGPU + 2453786) [0xffffff7f9745211a] 1-12
                                           *12  ??? (NMXGPU + 2519980) [0xffffff7f974623ac] 1-12
                                             *12  ??? (NMXGPU + 1423299) [0xffffff7f973567c3] 1-12
                                               *12  ??? (NMXGPU + 666391) [0xffffff7f9729db17] 1-12
                                                 *12  ??? (NMXGPU + 2542123) [0xffffff7f97467a2b] 1-12
                                                   *12  ??? (NMXGPU + 480029) [0xffffff7f9727031d] 1-12
                                                     *12  ??? (NMXGPU + 1897918) [0xffffff7f973ca5be] 1-12
                                                       *12  ??? (NMXGPU + 1898280) [0xffffff7f973ca728] 1-12
                                                         *12  ??? (NMXGPU + 670846) [0xffffff7f9729ec7e] 1-12
                                                           *12  ??? (NMXGPU + 670689) [0xffffff7f9729ebe1] 1-12
                                                             *12  ??? (NMXGPU + 2283392) [0xffffff7f97428780] 1-12
                                                               *12  ??? (NMXGPU + 2282389) [0xffffff7f97428395] 1-12
                                                                 *12  ??? (NMXGPU + 2387778) [0xffffff7f97441f42] 1-12
                                                                   *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                                                                     *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                                                                       *12  ??? (kernel + 2400279) [0xffffff8000532017] (suspended) 1-12

  Binary Images:
           0x102a37000 -                ???  WindowServer                <1746737B-B232-3669-B691-67604E4C8338>  /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/Resources/WindowServer
   *0xffffff7f971fb000 - 0xffffff7f974e8ec9  com.nullmoth.NMXGPU (1.0.9) <E770B985-14A4-3ADE-A1EE-63204C3A9996>  /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)             <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          AirPlayXPCHelper [160]
UUID:             B0797A06-03D8-3338-BDC5-163956E3AF80
Path:             /usr/libexec/AirPlayXPCHelper
Codesigning ID:   com.apple.AirPlayXPCHelper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2036 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.004s
Num threads:      6
Note:             3 idle work queue threads omitted

  Thread 0x462    Thread name "caulk.messenger.shared:high"    12 samples (1-12)    priority 55 (base 55)
  <IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x476    Thread name "caulk.messenger.shared:17"    12 samples (1-12)    priority 19 (base 19)
  <thread QoS utility (requested utility), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x478    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x103dcb000 -                ???  AirPlayXPCHelper        <B0797A06-03D8-3338-BDC5-163956E3AF80>  /usr/libexec/AirPlayXPCHelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81eec2000 -     0x7ff81eee5e17  caulk                   <684B920A-BD31-31C3-A5AA-B63B6654CDA3>  /System/Library/PrivateFrameworks/caulk.framework/Versions/A/caulk
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          airportd [181]
UUID:             10AE836A-F1DF-335C-89D2-A95A0EB84619
Path:             /usr/libexec/airportd
Codesigning ID:   com.apple.airport.airportd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3948 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
CPU Time:         0.020s
Num threads:      11
Note:             9 idle work queue threads omitted

  Thread 0x43e    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)    cpu time 0.002s
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (airportd + 569882) [0x10c86521a] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x543    Thread name "com.apple.CFSocket.private"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 39422) [0x7ff8137959fe] 1-12
       *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10c7da000 -                ???  airportd                <10AE836A-F1DF-335C-89D2-A95A0EB84619>  /usr/libexec/airportd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          amfid [110]
UUID:             CC3DAF75-FB24-3D83-9094-1B31DCDCA670
Path:             /usr/libexec/amfid
Codesigning ID:   com.apple.amfid
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1056 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3a3    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1027cd000 -                ???  amfid                  <CC3DAF75-FB24-3D83-9094-1B31DCDCA670>  /usr/libexec/amfid
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          analyticsd [167]
UUID:             6AA73497-F4DB-301E-8889-8A733F5A5EF5
Path:             /System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd
Codesigning ID:   com.apple.analyticsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2380 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.006s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x3ce    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (analyticsd + 354546) [0x105b948f2] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x105b3e000 -                ???  analyticsd             <6AA73497-F4DB-301E-8889-8A733F5A5EF5>  /System/Library/PrivateFrameworks/CoreAnalytics.framework/Support/analyticsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          apfsd [198]
UUID:             547DCA3B-D22C-37ED-906B-18BCEDC2D309
Path:             /usr/libexec/apfsd
Codesigning ID:   com.apple.apfsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1184 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4a4    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested utility), timers tier 3 (coalesced), IO tier 1>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (apfsd + 48886) [0x10c272ef6] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10c267000 -                ???  apfsd                  <547DCA3B-D22C-37ED-906B-18BCEDC2D309>  /usr/libexec/apfsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          AppleCredentialManagerDaemon [148]
UUID:             C6CF510E-C8BF-3EA8-8E51-4ED4C4CE0415
Path:             /System/Library/PrivateFrameworks/AppleCredentialManager.framework/AppleCredentialManagerDaemon
Codesigning ID:   com.apple.AppleCredentialManagerDaemon
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1092 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x40d    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x107c53000 -                ???  AppleCredentialManagerDaemon <C6CF510E-C8BF-3EA8-8E51-4ED4C4CE0415>  /System/Library/PrivateFrameworks/AppleCredentialManager.framework/AppleCredentialManagerDaemon
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib            <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib       <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)              <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          appleeventsd [405]
UUID:             7F73879A-9B4F-35CD-8EDA-42265AC0F1A9
Path:             /System/Library/CoreServices/appleeventsd
Codesigning ID:   com.apple.appleeventsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1172 KB
Time Since Fork:  168s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xa2c    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x108603000 -                ???  appleeventsd           <7F73879A-9B4F-35CD-8EDA-42265AC0F1A9>  /System/Library/CoreServices/appleeventsd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          apsd [135]
UUID:             DE561D2C-66F4-386F-B231-FD3968307ECF
Path:             /System/Library/PrivateFrameworks/ApplePushService.framework/apsd
Codesigning ID:   com.apple.apsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3348 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x336    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)    cpu time <0.001s
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (apsd + 998093) [0x10952cacd] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x109439000 -                ???  apsd                   <DE561D2C-66F4-386F-B231-FD3968307ECF>  /System/Library/PrivateFrameworks/ApplePushService.framework/apsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          aslmanager [162]
UUID:             8A8F12C0-E19C-3AC4-ADA9-FC10FAB2D622
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        432 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x362    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 3>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1013ed000 -                ???  ???                    <8A8F12C0-E19C-3AC4-ADA9-FC10FAB2D622>
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          audioclocksyncd [244]
UUID:             146122D6-BFFE-38EA-AB6A-5956B0B3BCE7
Path:             /usr/libexec/audioclocksyncd
Codesigning ID:   com.apple.audioclocksyncd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4344 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
CPU Time:         0.007s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x80c    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x103ddc000 -                ???  audioclocksyncd        <146122D6-BFFE-38EA-AB6A-5956B0B3BCE7>  /usr/libexec/audioclocksyncd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          AudioComponentRegistrar [229]
UUID:             60B3842A-FDEE-3C88-8EF1-8362DB38BAC8
Path:             /System/Library/Frameworks/AudioToolbox.framework/AudioComponentRegistrar
Codesigning ID:   com.apple.AudioComponentRegistrar
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1556 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5b0    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (AudioComponentRegistrar + 2749) [0x100e7babd] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x100e7b000 -                ???  AudioComponentRegistrar <60B3842A-FDEE-3C88-8EF1-8362DB38BAC8>  /System/Library/Frameworks/AudioToolbox.framework/AudioComponentRegistrar
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          authd [204]
UUID:             D3DA4CA1-DF09-3288-9CD0-1ABC9DA60E68
Path:             /System/Library/Frameworks/Security.framework/Versions/A/XPCServices/authd.xpc/Contents/MacOS/authd
Identifier:       com.apple.authd
Version:          1.0 (61439.140.12.707.2)
Build Version:    2
Project Name:     Security_executables_core
Source Version:   61439140012707002
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2096 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      5
Note:             1 idle work queue thread omitted

  Thread 0x4f9    DispatchQueue "com.apple.root.default-qos"(13)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), process unclamped, process received importance donation from loginwindow [640], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 206203) [0x7ff81365457b] 1-12
            12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
              12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                12  ??? (authd + 20492) [0x10041700c] 1-12
                  12  ??? (libbsm.0.dylib + 2455) [0x7ff820ffd997] 1-12
                    12  ??? (libsystem_c.dylib + 199075) [0x7ff81369d9a3] 1-12
                      12  ??? (libsystem_c.dylib + 165269) [0x7ff813695595] 1-12
                        12  ??? (libsystem_c.dylib + 55072) [0x7ff81367a720] 1-12
                          12  ??? (libsystem_kernel.dylib + 9298) [0x7ff81378e452] 1-12
                           *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
                             *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
                               *12  ??? (kernel + 6631249) [0xffffff800093af51] 1-12
                                 *12  ??? (kernel + 6631571) [0xffffff800093b093] 1-12
                                   *12  ??? (kernel + 3182633) [0xffffff80005f1029] 1-12
                                     *12  ??? (kernel + 3187280) [0xffffff80005f2250] 1-12
                                       *12  ??? (kernel + 3304088) [0xffffff800060ea98] 1-12
                                         *12  ??? (kernel + 6009378) [0xffffff80008a3222] 1-12
                                           *12  ??? (kernel + 6476076) [0xffffff800091512c] 1-12
                                             *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
                                               *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                                                 *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                                                   *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0xcd9    DispatchQueue "Connection Dispatch Queue"(501)    12 samples (1-12)    priority 37 (base 37)
  <thread QoS user initiated (requested user interactive), process unclamped, process received importance donation from loginwindow [640], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 205678) [0x7ff81365436e] 1-12
            12  ??? (libdispatch.dylib + 18400) [0x7ff8136267e0] 1-12
              12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                  12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                    12  ??? (authd + 154938) [0x100437d3a] 1-12
                      12  ??? (authd + 23358) [0x100417b3e] 1-12
                        12  ??? (libdispatch.dylib + 59366) [0x7ff8136307e6] 1-12
                          12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                            12  ??? (authd + 23593) [0x100417c29] 1-12
                              12  ??? (authd + 44611) [0x10041ce43] 1-12
                                12  ??? (authd + 32603) [0x100419f5b] 1-12
                                  12  ??? (authd + 53937) [0x10041f2b1] 1-12
                                    12  ??? (authd + 58606) [0x1004204ee] 1-12
                                      12  ??? (authd + 72663) [0x100423bd7] 1-12
                                        12  ??? (libdispatch.dylib + 12486) [0x7ff8136250c6] 1-12
                                          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
                                           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x1310    DispatchQueue "Connection Dispatch Queue"(548)    12 samples (1-12)    priority 37 (base 37)
  <thread QoS user initiated (requested user interactive), process unclamped, process received importance donation from loginwindow [640], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 205678) [0x7ff81365436e] 1-12
            12  ??? (libdispatch.dylib + 18400) [0x7ff8136267e0] 1-12
              12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                  12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                    12  ??? (authd + 154938) [0x100437d3a] 1-12
                      12  ??? (authd + 23358) [0x100417b3e] 1-12
                        12  ??? (libdispatch.dylib + 59366) [0x7ff8136307e6] 1-12
                          12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                            12  ??? (authd + 23593) [0x100417c29] 1-12
                              12  ??? (authd + 44611) [0x10041ce43] 1-12
                                12  ??? (authd + 32603) [0x100419f5b] 1-12
                                  12  ??? (authd + 53937) [0x10041f2b1] 1-12
                                    12  ??? (authd + 58606) [0x1004204ee] 1-12
                                      12  ??? (authd + 72663) [0x100423bd7] 1-12
                                        12  ??? (libdispatch.dylib + 12486) [0x7ff8136250c6] 1-12
                                          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
                                           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x514    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), process unclamped, process received importance donation from loginwindow [640], IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x100412000 -                ???  com.apple.authd 1.0 (61439.140.12.707.2) <D3DA4CA1-DF09-3288-9CD0-1ABC9DA60E68>  /System/Library/Frameworks/Security.framework/Versions/A/XPCServices/authd.xpc/Contents/MacOS/authd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                        <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81366d000 -     0x7ff8136f5a07  libsystem_c.dylib                        <6B1481E5-A9C4-3315-8ADE-BE0EDA06B6EC>  /usr/lib/system/libsystem_c.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib                  <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff820ffd000 -     0x7ff82100c89f  libbsm.0.dylib                           <4A087E19-2A31-3516-A03B-7E17F9317FC4>  /usr/lib/libbsm.0.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          autofsd [143]
UUID:             1D71C908-1683-3C9A-A6DE-F0E050ECDA7D
Path:             /usr/libexec/autofsd
Codesigning ID:   com.apple.autofsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        940 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x894    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1001c7000 -                ???  autofsd                <1D71C908-1683-3C9A-A6DE-F0E050ECDA7D>  /usr/libexec/autofsd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          backgroundtaskmanagementd [234]
UUID:             0A320CAE-4016-3391-BAFA-6405F1340C71
Path:             /System/Library/PrivateFrameworks/BackgroundTaskManagement.framework/Versions/A/Resources/backgroundtaskmanagementd
Codesigning ID:   com.apple.backgroundtaskmanagementd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1632 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x6b9    DispatchQueue "com.apple.root.background-qos"(7)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 206203) [0x7ff81365457b] 1-12
            12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
              12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                12  ??? (backgroundtaskmanagementd + 15995) [0x10367de7b] 1-12
                  12  ??? (libsystem_c.dylib + 104056) [0x7ff813686678] 1-12
                    12  ??? (libsystem_kernel.dylib + 13762) [0x7ff81378f5c2] 1-12
                     *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x6c9    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10367a000 -                ???  backgroundtaskmanagementd <0A320CAE-4016-3391-BAFA-6405F1340C71>  /System/Library/PrivateFrameworks/BackgroundTaskManagement.framework/Versions/A/Resources/backgroundtaskmanagementd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81366d000 -     0x7ff8136f5a07  libsystem_c.dylib         <6B1481E5-A9C4-3315-8ADE-BE0EDA06B6EC>  /usr/lib/system/libsystem_c.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib   <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          backupd [258]
UUID:             74520BCE-DC9D-3155-B644-1FDCAB312660
Path:             /System/Library/CoreServices/TimeMachine/backupd
Codesigning ID:   com.apple.backupd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1356 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x69e    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 3>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (backupd + 966079) [0x10bd8edbf] 1-12
      12  ??? (libswift_Concurrency.dylib + 309935) [0x7ffd228a5aaf] 1-12
        12  ??? (libswift_Concurrency.dylib + 342835) [0x7ffd228adb33] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10bca3000 -                ???  backupd                    <74520BCE-DC9D-3155-B644-1FDCAB312660>  /System/Library/CoreServices/TimeMachine/backupd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                       <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib     <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation             <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ffd2285a000 -     0x7ffd228bfe07  libswift_Concurrency.dylib <3649D4F6-40EE-380C-B347-4066D69D5D29>  /usr/lib/swift/libswift_Concurrency.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)            <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          backupd-helper [179]
UUID:             8AE08760-4EB3-33A8-83EE-C5736506002C
Path:             /System/Library/CoreServices/TimeMachine/backupd-helper
Codesigning ID:   com.apple.backupd-helper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1308 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x43f    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (backupd-helper + 7738) [0x106be1e3a] 1-12
      12  ??? (backupd-helper + 7441) [0x106be1d11] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x106be0000 -                ???  backupd-helper         <8AE08760-4EB3-33A8-83EE-C5736506002C>  /System/Library/CoreServices/TimeMachine/backupd-helper
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          biomed [193]
UUID:             55F373D0-70F0-3F59-8204-594ED138B533
Path:             /System/Library/PrivateFrameworks/BiomeStreams.framework/Support/biomed
Codesigning ID:   com.apple.biomed
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2880 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x775    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10b9d2000 -                ???  biomed                 <55F373D0-70F0-3F59-8204-594ED138B533>  /System/Library/PrivateFrameworks/BiomeStreams.framework/Support/biomed
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          BlueTool [668]
UUID:             F18669FF-A371-3591-BBB5-C298ADC82AE1
Path:             /usr/sbin/BlueTool
Codesigning ID:   com.apple.BlueTool
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        804 KB
Time Since Fork:  17s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x15e5    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 37 (base 37)    cpu time <0.001s
  <thread QoS user initiated (requested user initiated, requested override user initiated), process unclamped, process received importance donation from bluetoothd [667], IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (BlueTool + 161296) [0x10c2c9610] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 504628) [0x7ff8138b8334] 1-12
            12  ??? (CoreFoundation + 755830) [0x7ff8138f5876] 1-12
              12  ??? (libdispatch.dylib + 57055) [0x7ff81362fedf] 1-12
                12  ??? (libdispatch.dylib + 57190) [0x7ff81362ff66] 1-12
                  12  ??? (libdispatch.dylib + 204447) [0x7ff813653e9f] 1-12
                    12  ??? (libdispatch.dylib + 109013) [0x7ff81363c9d5] 1-12
                      12  ??? (libdispatch.dylib + 33171) [0x7ff81362a193] 1-12
                        12  ??? (libdispatch.dylib + 106088) [0x7ff81363be68] 1-12
                          12  ??? (libdispatch.dylib + 96789) [0x7ff813639a15] 1-12
                            12  ??? (libxpc.dylib + 47867) [0x7ff8134cdafb] 1-12
                              12  ??? (libxpc.dylib + 53010) [0x7ff8134cef12] 1-12
                                12  ??? (BlueTool + 164699) [0x10c2ca35b] 1-12
                                  12  ??? (BlueTool + 57318) [0x10c2affe6] 1-12
                                    12  ??? (libsystem_c.dylib + 104056) [0x7ff813686678] 1-12
                                      12  ??? (libsystem_kernel.dylib + 13762) [0x7ff81378f5c2] 1-12
                                       *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x10c2a2000 -                ???  BlueTool               <F18669FF-A371-3591-BBB5-C298ADC82AE1>  /usr/sbin/BlueTool
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff8134c2000 -     0x7ff813508bd5  libxpc.dylib           <A1CBBDA0-6389-3803-91FB-D18379D62FEB>  /usr/lib/system/libxpc.dylib
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81366d000 -     0x7ff8136f5a07  libsystem_c.dylib      <6B1481E5-A9C4-3315-8ADE-BE0EDA06B6EC>  /usr/lib/system/libsystem_c.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          bluetoothd [667]
UUID:             9D4727A2-1FC2-3777-BE26-7BAD0ED65EF0
Path:             /usr/sbin/bluetoothd
Codesigning ID:   com.apple.bluetoothd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3340 KB
Time Since Fork:  17s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      5
Note:             2 idle work queue threads omitted

  Thread 0x15d5    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 37 (base 37)
  <thread QoS user initiated (requested user initiated), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (bluetoothd + 185356) [0x1008f040c] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x15d9    DispatchQueue "com.apple.bluetooth.root"(41)    12 samples (1-12)    priority 37 (base 37)
  <thread QoS user initiated (requested user initiated, workloop servicer override user initiated), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33631) [0x7ff81362a35f] 1-12
              12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                  12  ??? (bluetoothd + 4705959) [0x100d3fea7] 1-12
                    12  ??? (bluetoothd + 4708881) [0x100d40a11] 1-12
                      12  ??? (bluetoothd + 3169562) [0x100bc8d1a] 1-12
                        12  ??? (bluetoothd + 3166735) [0x100bc820f] 1-12
                          12  ??? (bluetoothd + 2369461) [0x100b057b5] 1-12
                            12  ??? (bluetoothd + 2368876) [0x100b0556c] 1-12
                              12  ??? (libxpc.dylib + 45020) [0x7ff8134ccfdc] 1-12
                                12  ??? (libdispatch.dylib + 102705) [0x7ff81363b131] 1-12
                                  12  ??? (libdispatch.dylib + 101696) [0x7ff81363ad40] 1-12
                                    12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                      12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                        12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                         *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1008c3000 -                ???  bluetoothd              <9D4727A2-1FC2-3777-BE26-7BAD0ED65EF0>  /usr/sbin/bluetoothd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff8134c2000 -     0x7ff813508bd5  libxpc.dylib            <A1CBBDA0-6389-3803-91FB-D18379D62FEB>  /usr/lib/system/libxpc.dylib
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          bootinstalld [424]
UUID:             ABDDCC6B-77FA-32BF-90E6-4CFCC93499FD
Path:             /usr/libexec/bootinstalld
Codesigning ID:   com.apple.bootinstalld
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1084 KB
Time Since Fork:  166s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xad3    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (bootinstalld + 26334) [0x103fd26de] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x103fcc000 -                ???  bootinstalld           <ABDDCC6B-77FA-32BF-90E6-4CFCC93499FD>  /usr/libexec/bootinstalld
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          cameracaptured [252]
UUID:             30E48FA8-09FA-3A9A-B371-E6AE718A7EE5
Path:             /usr/libexec/cameracaptured
Codesigning ID:   com.apple.cameracaptured
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1676 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x667    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (cameracaptured + 1870) [0x10b5d274e] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10b5d2000 -                ???  cameracaptured         <30E48FA8-09FA-3A9A-B371-E6AE718A7EE5>  /usr/libexec/cameracaptured
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          captiveagent [411]
UUID:             713D8D3F-AEF1-3A02-B6DC-957896893FF9
Path:             /usr/libexec/captiveagent
Codesigning ID:   com.apple.captiveagent
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1452 KB
Time Since Fork:  166s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xa98    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (captiveagent + 13077) [0x10b6b2315] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10b6af000 -                ???  captiveagent           <713D8D3F-AEF1-3A02-B6DC-957896893FF9>  /usr/libexec/captiveagent
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          cfprefsd [164]
UUID:             DA1DC546-F369-3970-BDEC-4FA745EEB1AD
Path:             /usr/sbin/cfprefsd
Codesigning ID:   com.apple.cfprefsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        1324 KB -> 1396 KB (+72 KB)
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.010s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x3f9    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1036f8000 -                ???  cfprefsd               <DA1DC546-F369-3970-BDEC-4FA745EEB1AD>  /usr/sbin/cfprefsd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          cfprefsd [240]
UUID:             DA1DC546-F369-3970-BDEC-4FA745EEB1AD
Path:             /usr/sbin/cfprefsd
Codesigning ID:   com.apple.cfprefsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        972 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x604    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x103891000 -                ???  cfprefsd               <DA1DC546-F369-3970-BDEC-4FA745EEB1AD>  /usr/sbin/cfprefsd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          cfprefsd [553]
UUID:             DA1DC546-F369-3970-BDEC-4FA745EEB1AD
Path:             /usr/sbin/cfprefsd
Codesigning ID:   com.apple.cfprefsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        1140 KB
Time Since Fork:  117s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xfa7    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1009de000 -                ???  cfprefsd               <DA1DC546-F369-3970-BDEC-4FA745EEB1AD>  /usr/sbin/cfprefsd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          colorsync.displayservices [386]
UUID:             8880E99D-C1C1-34AB-9086-B43EF1055ED1
Path:             /usr/libexec/colorsync.displayservices
Codesigning ID:   com.apple.colorsync
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1344 KB
Time Since Fork:  174s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x9b1    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (colorsync.displayservices + 14508) [0x1084ff8ac] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1084fc000 -                ???  colorsync.displayservices <8880E99D-C1C1-34AB-9086-B43EF1055ED1>  /usr/libexec/colorsync.displayservices
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                      <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation            <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          colorsyncd [392]
UUID:             10F35810-601C-3F1C-B8CD-62240D003D3F
Path:             /usr/libexec/colorsyncd
Codesigning ID:   com.apple.colorsyncd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1004 KB
Time Since Fork:  174s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x9c6    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (colorsyncd + 8494) [0x102c3012e] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x102c2e000 -                ???  colorsyncd             <10F35810-601C-3F1C-B8CD-62240D003D3F>  /usr/libexec/colorsyncd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.AppleUserHIDDrivers.dext [324]
UUID:             E87B51F2-D68F-366F-BD35-BEDB5315B8B9
Path:             /System/Library/DriverExtensions/com.apple.AppleUserHIDDrivers.dext
Codesigning ID:   com.apple.AppleUserHIDDrivers
Shared Cache:     E7CA1FD9-6335-372F-8536-8A879AF70D10 slid base address 0x7ff80813e000, slide 0x813e000 (DriverKit)
Architecture:     x86_64
Footprint:        1468 KB
Time Since Fork:  177s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x89b    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 92265) [0x7ff808c57869] 1-12
    12  ??? (libsystem_kernel.dylib + 23374) [0x7ff808d36b4e] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10f3a2000 -                ???  com.apple.AppleUserHIDDrivers.dext <E87B51F2-D68F-366F-BD35-BEDB5315B8B9>  /System/Library/DriverExtensions/com.apple.AppleUserHIDDrivers.dext
        0x7ff808c41000 -     0x7ff808c90e7f  libdispatch.dylib                  <A03901EA-6F0E-36A9-9F93-15A0641E2641>  /System/DriverKit/usr/lib/system/libdispatch.dylib
        0x7ff808d31000 -     0x7ff808d761df  libsystem_kernel.dylib             <30C21CF2-8D6B-3853-A274-F84A9AA9FBA1>  /System/DriverKit/usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                    <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.audio.DriverHelper [220]
UUID:             C535865A-DCD9-3241-BA87-3EAEB16FA0DF
Path:             /System/Library/Frameworks/CoreAudio.framework/Versions/A/XPCServices/com.apple.audio.DriverHelper.xpc/Contents/MacOS/com.apple.audio.DriverHelper
Identifier:       com.apple.audio.DriverHelper
Version:          1.0 (1)
Build Version:    5
Project Name:     AudioHAL
Source Version:   328704002000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1100 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x575    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10134f000 -                ???  com.apple.audio.DriverHelper 1.0 (1) <C535865A-DCD9-3241-BA87-3EAEB16FA0DF>  /System/Library/Frameworks/CoreAudio.framework/Versions/A/XPCServices/com.apple.audio.DriverHelper.xpc/Contents/MacOS/com.apple.audio.DriverHelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                    <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib               <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                      <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.audio.SandboxHelper [241]
UUID:             8600BDB8-35B8-309B-86DA-302A596F2DD3
Path:             /System/Library/Frameworks/AudioToolbox.framework/XPCServices/com.apple.audio.SandboxHelper.xpc/Contents/MacOS/com.apple.audio.SandboxHelper
Identifier:       com.apple.audio.SandboxHelper
Version:          1.0.1 (1)
Build Version:    2
Project Name:     CoreAudioServices_executables
Source Version:   1456717000000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3692 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x616    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x104772000 -                ???  com.apple.audio.SandboxHelper 1.0.1 (1) <8600BDB8-35B8-309B-86DA-302A596F2DD3>  /System/Library/Frameworks/AudioToolbox.framework/XPCServices/com.apple.audio.SandboxHelper.xpc/Contents/MacOS/com.apple.audio.SandboxHelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.cmio.registerassistantservice [161]
UUID:             64620746-D70D-30A3-A2DB-92935B20A503
Path:             /System/Library/Frameworks/CoreMediaIO.framework/Versions/A/Resources/com.apple.cmio.registerassistantservice
Codesigning ID:   com.apple.cmio.registerassistantservice
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1872 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x359    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (com.apple.cmio.registerassistantservice + 80595) [0x10c570ad3] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10c55d000 -                ???  com.apple.cmio.registerassistantservice <64620746-D70D-30A3-A2DB-92935B20A503>  /System/Library/Frameworks/CoreMediaIO.framework/Versions/A/Resources/com.apple.cmio.registerassistantservice
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.CodeSigningHelper [231]
UUID:             4379F4A1-5FFB-358B-9DFB-965A8AD6D20D
Path:             /System/Library/Frameworks/Security.framework/Versions/A/XPCServices/com.apple.CodeSigningHelper.xpc/Contents/MacOS/com.apple.CodeSigningHelper
Identifier:       com.apple.CodeSigningHelper
Version:          1.0 (1)
Build Version:    2
Project Name:     Security_executables
Source Version:   61439140012707002
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1184 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5e2    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x105cb0000 -                ???  com.apple.CodeSigningHelper 1.0 (1) <4379F4A1-5FFB-358B-9DFB-965A8AD6D20D>  /System/Library/Frameworks/Security.framework/Versions/A/XPCServices/com.apple.CodeSigningHelper.xpc/Contents/MacOS/com.apple.CodeSigningHelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                   <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib              <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                     <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.ColorSyncXPCAgent [388]
UUID:             102E050D-218E-36BA-91C1-190F87B4E559
Path:             /System/Library/Frameworks/ColorSync.framework/Versions/A/XPCServices/com.apple.ColorSyncXPCAgent.xpc/Contents/MacOS/com.apple.ColorSyncXPCAgent
Identifier:       com.apple.ColorSyncXPCAgent
Version:          1.0 (3777.4.1)
Build Version:    2290
Project Name:     ColorSync
Source Version:   3777004001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        808 KB
Time Since Fork:  174s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x9be    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x100620000 -                ???  com.apple.ColorSyncXPCAgent 1.0 (3777.4.1) <102E050D-218E-36BA-91C1-190F87B4E559>  /System/Library/Frameworks/ColorSync.framework/Versions/A/XPCServices/com.apple.ColorSyncXPCAgent.xpc/Contents/MacOS/com.apple.ColorSyncXPCAgent
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                          <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                     <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                            <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.DriverKit-IOUserDockChannelSerial.dext [320]
UUID:             81091778-E4C6-3B16-A11D-40BFE45A3D78
Path:             /System/Library/DriverExtensions/com.apple.DriverKit-IOUserDockChannelSerial.dext
Codesigning ID:   com.apple.DriverKit-IOUserDockChannelSerial
Shared Cache:     E7CA1FD9-6335-372F-8536-8A879AF70D10 slid base address 0x7ff80813e000, slide 0x813e000 (DriverKit)
Architecture:     x86_64
Footprint:        480 KB
Time Since Fork:  177s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x892    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 92265) [0x7ff808c57869] 1-12
    12  ??? (libsystem_kernel.dylib + 23374) [0x7ff808d36b4e] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10bc67000 -                ???  com.apple.DriverKit-IOUserDockChannelSerial.dext <81091778-E4C6-3B16-A11D-40BFE45A3D78>  /System/Library/DriverExtensions/com.apple.DriverKit-IOUserDockChannelSerial.dext
        0x7ff808c41000 -     0x7ff808c90e7f  libdispatch.dylib                                <A03901EA-6F0E-36A9-9F93-15A0641E2641>  /System/DriverKit/usr/lib/system/libdispatch.dylib
        0x7ff808d31000 -     0x7ff808d761df  libsystem_kernel.dylib                           <30C21CF2-8D6B-3853-A274-F84A9AA9FBA1>  /System/DriverKit/usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                  <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.geod [222]
UUID:             EDF4B317-7F41-369C-9CCA-998BA1F4A131
Path:             /System/Library/PrivateFrameworks/GeoServices.framework/Versions/A/XPCServices/com.apple.geod.xpc/Contents/MacOS/com.apple.geod
Identifier:       com.apple.geod
Version:          1.0 (1986.26.4.18.7)
Build Version:    5
Project Name:     GeoServices
Source Version:   1986026004018007
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3120 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x682    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x101605000 -                ???  com.apple.geod 1.0 (1986.26.4.18.7) <EDF4B317-7F41-369C-9CCA-998BA1F4A131>  /System/Library/PrivateFrameworks/GeoServices.framework/Versions/A/XPCServices/com.apple.geod.xpc/Contents/MacOS/com.apple.geod
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                   <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib              <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                     <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.ifdreader [195]
UUID:             DD29B48C-304C-385E-868D-84C78E46552F
Path:             /System/Library/CryptoTokenKit/com.apple.ifdreader.slotd/Contents/MacOS/com.apple.ifdreader
Identifier:       com.apple.ifdreader
Version:          1.0 (1)
Build Version:    2438
Project Name:     CryptoTokenKit_binaries
Source Version:   738100034000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1140 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x49f    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (com.apple.ifdreader + 9230) [0x10f1b740e] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10f1b5000 -                ???  com.apple.ifdreader 1.0 (1) <DD29B48C-304C-385E-868D-84C78E46552F>  /System/Library/CryptoTokenKit/com.apple.ifdreader.slotd/Contents/MacOS/com.apple.ifdreader
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                        <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib      <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation              <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation                  <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)             <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          com.apple.MobileSoftwareUpdate.CleanupPreparePathService (com.apple.MobileSoftwareUpdate.CleanupPreparePathService) [310]
UUID:             8C1B5E68-89A9-3333-83DD-6CEB0337BB21
Path:             /System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/Versions/A/XPCServices/com.apple.MobileSoftwareUpdate.CleanupPreparePathService.xpc/Contents/MacOS/com.apple.MobileSoftwareUpdate.CleanupPreparePathService
Identifier:       com.apple.MobileSoftwareUpdate.CleanupPreparePathService
Version:          1.0 (1)
Build Version:    8
Project Name:     MobileSoftwareUpdate
Source Version:   2171140016702001
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1464 KB
Time Since Fork:  177s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x7d7    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10109b000 -                ???  com.apple.MobileSoftwareUpdate.CleanupPreparePathService 1.0 (1) <8C1B5E68-89A9-3333-83DD-6CEB0337BB21>  /System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/Versions/A/XPCServices/com.apple.MobileSoftwareUpdate.CleanupPreparePathService.xpc/Contents/MacOS/com.apple.MobileSoftwareUpdate.CleanupPreparePathService
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                                <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                           <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                                  <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          configd [105]
UUID:             C798C1A3-A5B6-334C-8D60-3011298F1F0A
Path:             /usr/libexec/configd
Codesigning ID:   com.apple.configd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2524 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.001s
Num threads:      9
Note:             3 idle work queue threads omitted

  Thread 0x31e    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (configd + 8326) [0x10b45d086] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x3be    Thread name "InterfaceNamer thread"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (configd + 80697) [0x10b46eb39] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x408    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (<E431210B-2624-3881-B8E3-678D93F66940> + 40333) [0x10b7a4d8d] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x409    Thread name "com.apple.CFSocket.private"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 39422) [0x7ff8137959fe] 1-12
       *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x446    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (<3887EF77-2AE9-31C7-A424-3B329FBDC7BA> + 24887) [0x10b5fe137] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x455    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (<DAAA36DA-9EE6-310A-AB67-0E33E80B4937> + 19854) [0x10b5a1d8e] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x15a3    DispatchQueue "IPConfigurationAgentQueue"(64)    1 sample (1)    priority 37 (base 37)    cpu time <0.001s
  <thread QoS user initiated (requested user initiated, workloop servicer override user initiated), IO tier 0>
  1  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1
    1  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1
      1  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1
        1  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1
          1  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1
            1  ??? (libdispatch.dylib + 33631) [0x7ff81362a35f] 1
              1  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1
                1  ??? (libsystem_blocks.dylib + 4109) [0x7ff8134bf00d] 1
                  1  ??? (libobjc.A.dylib + 34641) [0x7ff8133e5751] 1
                    1  ??? (libobjc.A.dylib + 177932) [0x7ff81340870c] 1
                      1  ??? (libobjc.A.dylib + 191145) [0x7ff81340baa9] 1
                       *1  ??? (kernel + 522906) [0xffffff8000367a9a] 1
                         *1  ??? (kernel + 2422049) [0xffffff8000537521] 1
                           *1  ??? (kernel + 2464698) [0xffffff8000541bba] 1
                             *1  ??? (kernel + 2425475) [0xffffff8000538283] (running) 1

  Binary Images:
           0x10b45b000 -                ???  configd                 <C798C1A3-A5B6-334C-8D60-3011298F1F0A>  /usr/libexec/configd
           0x10b59d000 -                ???  ???                     <DAAA36DA-9EE6-310A-AB67-0E33E80B4937>
           0x10b5f8000 -                ???  ???                     <3887EF77-2AE9-31C7-A424-3B329FBDC7BA>
           0x10b79b000 -                ???  ???                     <E431210B-2624-3881-B8E3-678D93F66940>
        0x7ff8133dd000 -     0x7ff81342205b  libobjc.A.dylib         <88AA23BF-F45D-3A23-960D-8C093783C3DC>  /usr/lib/libobjc.A.dylib
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff8134be000 -     0x7ff8134c16d8  libsystem_blocks.dylib  <D01A0A23-0FD7-36DC-AC90-32A6EDF52FDD>  /usr/lib/system/libsystem_blocks.dylib
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          containermanagerd [248]
UUID:             6C6DB856-93E9-30AB-8B02-C1D322F76F31
Path:             /usr/libexec/containermanagerd
Codesigning ID:   com.apple.containermanagerd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1428 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x641    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (containermanagerd + 1650) [0x1000eb672] 1-12
      12  ??? (ContainerManagerCommon + 134354) [0x7ffb29da1cd2] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1000eb000 -                ???  containermanagerd      <6C6DB856-93E9-30AB-8B02-C1D322F76F31>  /usr/libexec/containermanagerd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ffb29d81000 -     0x7ffb29ea29b0  ContainerManagerCommon <EBF8D874-3E53-3373-BD6F-634A6644004D>  /System/Library/PrivateFrameworks/ContainerManagerCommon.framework/Versions/A/ContainerManagerCommon
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          containermanagerd_system [170]
UUID:             C805E07B-3F2D-3F1D-8AAD-51035E2CE3C5
Path:             /usr/libexec/containermanagerd_system
Codesigning ID:   com.apple.containermanagerd_system
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        5144 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x402    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (containermanagerd_system + 1650) [0x10a2e4672] 1-12
      12  ??? (ContainerManagerCommon + 134354) [0x7ffb29da1cd2] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10a2e4000 -                ???  containermanagerd_system <C805E07B-3F2D-3F1D-8AAD-51035E2CE3C5>  /usr/libexec/containermanagerd_system
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                     <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation           <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ffb29d81000 -     0x7ffb29ea29b0  ContainerManagerCommon   <EBF8D874-3E53-3373-BD6F-634A6644004D>  /System/Library/PrivateFrameworks/ContainerManagerCommon.framework/Versions/A/ContainerManagerCommon
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          contextstored [178]
UUID:             EAE15059-F098-300A-8F27-351BD88514A3
Path:             /System/Library/PrivateFrameworks/CoreDuetContext.framework/Versions/A/Resources/contextstored
Codesigning ID:   com.apple.contextstored
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2764 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
CPU Time:         0.002s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x436    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (contextstored + 10775) [0x10f660a17] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10f65e000 -                ???  contextstored          <EAE15059-F098-300A-8F27-351BD88514A3>  /System/Library/PrivateFrameworks/CoreDuetContext.framework/Versions/A/Resources/contextstored
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          coreaudiod [189]
UUID:             C229360C-4615-3B32-A919-564491F0EA26
Path:             /usr/sbin/coreaudiod
Codesigning ID:   com.apple.audio.coreaudiod
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3064 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      7
Note:             1 idle work queue thread omitted

  Thread 0x489    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 63 (base 63)
  <IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (coreaudiod + 102512) [0x100cb2070] 1-12
      12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
        12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
          12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
            12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
              12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                 *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x4ab    Thread name "HAL Async Logger"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (CoreAudio + 5287207) [0x7ff816579d27] 1-12
        12  ??? (CoreAudio + 5286880) [0x7ff816579be0] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x4b0    Thread name "caulk.messenger.shared:17"    12 samples (1-12)    priority 19 (base 19)
  <thread QoS utility (requested utility), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x4b1    Thread name "caulk.messenger.shared:high"    12 samples (1-12)    priority 55 (base 55)
  <IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x618    Thread name "caulk.messenger.shared:21"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x61e    Thread name "caulk::deferred_logger"    12 samples (1-12)    priority 37 (base 37)
  <thread QoS user initiated (requested user initiated), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x100c99000 -                ???  coreaudiod              <C229360C-4615-3B32-A919-564491F0EA26>  /usr/sbin/coreaudiod
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff81606f000 -     0x7ff8167ae9c0  CoreAudio               <32DC6076-2E45-316F-9060-C40011959716>  /System/Library/Frameworks/CoreAudio.framework/Versions/A/CoreAudio
        0x7ff81eec2000 -     0x7ff81eee5e17  caulk                   <684B920A-BD31-31C3-A5AA-B63B6654CDA3>  /System/Library/PrivateFrameworks/caulk.framework/Versions/A/caulk
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          coreauthd [447]
UUID:             1DA8415C-BE1A-3530-85EB-58170025BE1F
Path:             /System/Library/Frameworks/LocalAuthentication.framework/Support/coreauthd
Codesigning ID:   com.apple.coreauthd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1920 KB
Time Since Fork:  157s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xb64    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (coreauthd + 45567) [0x10c0f61ff] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10c0eb000 -                ???  coreauthd              <1DA8415C-BE1A-3530-85EB-58170025BE1F>  /System/Library/Frameworks/LocalAuthentication.framework/Support/coreauthd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          corebrightnessd [159]
UUID:             09D88D95-C780-315C-A1B6-BDACA9C4BC0F
Path:             /usr/libexec/corebrightnessd
Codesigning ID:   com.apple.corebrightnessd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1956 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x33d    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (corebrightnessd + 3762) [0x10db6beb2] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x121d    DispatchQueue "BacklightControl - Primary"(29)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default, workloop servicer override default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33631) [0x7ff81362a35f] 1-12
              12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                  12  ??? (CoreBrightness + 744472) [0x7ff823dafc18] 1-12
                    12  ??? (CoreBrightness + 743983) [0x7ff823dafa2f] 1-12
                      12  ??? (CoreBrightness + 743757) [0x7ff823daf94d] 1-12
                        12  ??? (SkyLight + 483876) [0x7ff8194b3224] 1-12
                          12  ??? (SkyLight + 57603) [0x7ff81944b103] 1-12
                            12  ??? (SkyLight + 12230) [0x7ff81943ffc6] 1-12
                              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                   *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile waiting for launchd [1] send port name "com.apple.windowserver.active" in domain 1 after 2 hops) 1-12

  Binary Images:
           0x10db6b000 -                ???  corebrightnessd         <09D88D95-C780-315C-A1B6-BDACA9C4BC0F>  /usr/libexec/corebrightnessd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff81943d000 -     0x7ff81990bf2f  SkyLight                <14C81C57-7629-32F2-9E8C-AF32EF962700>  /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/SkyLight
        0x7ff823cfa000 -     0x7ff823e1a6b2  CoreBrightness          <D4A25A42-8CB7-3DA4-8184-B8400D5A3752>  /System/Library/PrivateFrameworks/CoreBrightness.framework/Versions/A/CoreBrightness
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          coreduetd [128]
UUID:             D22A1924-2C54-303E-B3E4-95FA8DBAF268
Path:             /usr/libexec/coreduetd
Codesigning ID:   com.apple.coreduetd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3408 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x342    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (coreduetd + 72294) [0x100adaa66] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x100ac9000 -                ???  coreduetd              <D22A1924-2C54-303E-B3E4-95FA8DBAF268>  /usr/libexec/coreduetd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          coreservicesd [172]
UUID:             08CEEFA5-B816-30F4-AF57-66AAFB95F940
Path:             /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Support/coreservicesd
Codesigning ID:   com.apple.coreservicesd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2112 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      4
Note:             1 idle work queue thread omitted

  Thread 0x410    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (CarbonCore + 520479) [0x7ff816ec011f] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x41c    DispatchQueue "fmodWatchProducer"(47)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default, workloop servicer override default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33631) [0x7ff81362a35f] 1-12
              12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                  12  ??? (libsystem_kernel.dylib + 5554) [0x7ff81378d5b2] 1-12
                   *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
                     *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
                       *12  ??? (kernel + 6632007) [0xffffff800093b247] 1-12
                         *12  ??? (kernel + 6631571) [0xffffff800093b093] 1-12
                           *12  ??? (kernel + 3270947) [0xffffff8000606923] 1-12
                             *12  ??? (kernel + 6476190) [0xffffff800091519e] 1-12
                               *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                                 *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                                   *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x41d    DispatchQueue "fmodWatchProducer"(48)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default, workloop servicer override default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33631) [0x7ff81362a35f] 1-12
              12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                  12  ??? (CarbonCore + 299396) [0x7ff816e8a184] 1-12
                    12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
                     *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Binary Images:
           0x104110000 -                ???  coreservicesd             <08CEEFA5-B816-30F4-AF57-66AAFB95F940>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/Support/coreservicesd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                      <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib   <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation            <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff816e41000 -     0x7ff8171292f1  CarbonCore                <CD54B1F0-2CF9-34BB-8247-6365AAC7791C>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/CarbonCore
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1) <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread


Process:          corespeechd_system [116]
UUID:             FFA8AD13-1544-3223-87C9-7F58861607CF
Path:             /System/Library/PrivateFrameworks/CoreSpeech.framework/corespeechd_system
Codesigning ID:   com.apple.corespeechd_system
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1536 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3cb    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (corespeechd_system + 28774) [0x1097e8066] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1097e1000 -                ???  corespeechd_system     <FFA8AD13-1544-3223-87C9-7F58861607CF>  /System/Library/PrivateFrameworks/CoreSpeech.framework/corespeechd_system
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          coresymbolicationd [410]
UUID:             EAB98C5C-6043-3328-B1B5-6E9C22D6662D
Path:             /System/Library/PrivateFrameworks/CoreSymbolication.framework/coresymbolicationd
Codesigning ID:   com.apple.coresymbolicationd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        912 KB
Time Since Fork:  168s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xa7a    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10ffa9000 -                ???  coresymbolicationd     <EAB98C5C-6043-3328-B1B5-6E9C22D6662D>  /System/Library/PrivateFrameworks/CoreSymbolication.framework/coresymbolicationd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          countryd [397]
UUID:             8DA2CD49-1402-31A3-B60A-8B3B9AA84EF3
Path:             /usr/libexec/countryd
Codesigning ID:   com.apple.countryd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1376 KB
Time Since Fork:  172s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x9f5    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x108bfc000 -                ???  countryd               <8DA2CD49-1402-31A3-B60A-8B3B9AA84EF3>  /usr/libexec/countryd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          cryptexd [209]
UUID:             0DA6E8B3-53CB-30AE-89E3-6F8E74BB8EEF
Path:             /usr/libexec/cryptexd
Codesigning ID:   com.apple.security.cryptexd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1316 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4f4    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x107be4000 -                ???  cryptexd               <0DA6E8B3-53CB-30AE-89E3-6F8E74BB8EEF>  /usr/libexec/cryptexd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          csnameddatad [187]
UUID:             0CB79B53-0621-3257-9095-078B9B918C91
Path:             /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/XPCServices/csnameddatad.xpc/Contents/MacOS/csnameddatad
Identifier:       com.apple.carboncore.csnameddata
Version:          1.0 (1)
Build Version:    1287
Project Name:     CarbonCore
Source Version:   1371000000000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        936 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x51e    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x107fa0000 -                ???  com.apple.carboncore.csnameddata 1.0 (1) <0CB79B53-0621-3257-9095-078B9B918C91>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/XPCServices/csnameddatad.xpc/Contents/MacOS/csnameddatad
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                        <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          csnameddatad [560]
UUID:             0CB79B53-0621-3257-9095-078B9B918C91
Path:             /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/XPCServices/csnameddatad.xpc/Contents/MacOS/csnameddatad
Identifier:       com.apple.carboncore.csnameddata
Version:          1.0 (1)
Build Version:    1287
Project Name:     CarbonCore
Source Version:   1371000000000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        772 KB
Time Since Fork:  117s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xfd1    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x108071000 -                ???  com.apple.carboncore.csnameddata 1.0 (1) <0CB79B53-0621-3257-9095-078B9B918C91>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/CarbonCore.framework/Versions/A/XPCServices/csnameddatad.xpc/Contents/MacOS/csnameddatad
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                        <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          ctkahp (Smartcard Agent) [449]
UUID:             64580311-C8C7-3EC7-ACD0-979C0C56D25C
Path:             /System/Library/Frameworks/CryptoTokenKit.framework/ctkahp.bundle/Contents/MacOS/ctkahp
Identifier:       com.apple.ctkahp
Version:          1.0 (1)
Build Version:    2438
Project Name:     CryptoTokenKit_binaries
Source Version:   738100034000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1288 KB
Time Since Fork:  156s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xb93    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (ctkahp + 8530) [0x109e45152] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x109e43000 -                ???  com.apple.ctkahp 1.0 (1) <64580311-C8C7-3EC7-ACD0-979C0C56D25C>  /System/Library/Frameworks/CryptoTokenKit.framework/ctkahp.bundle/Contents/MacOS/ctkahp
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                     <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation           <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation               <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          ctkd [453]
UUID:             094ADF2A-6C5F-3F08-B002-6FF08BC1D287
Path:             /System/Library/Frameworks/CryptoTokenKit.framework/ctkd
Codesigning ID:   com.apple.ctkd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1004 KB
Time Since Fork:  155s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xbaa    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x101811000 -                ???  ctkd                   <094ADF2A-6C5F-3F08-B002-6FF08BC1D287>  /System/Library/Frameworks/CryptoTokenKit.framework/ctkd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          dasd [145]
UUID:             74A45F85-E121-3745-97CD-4896BE45143D
Path:             /usr/libexec/dasd
Codesigning ID:   com.apple.dasd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3184 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      4
Note:             2 idle work queue threads omitted

  Thread 0x347    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (dasd + 1021737) [0x10aa79729] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10a980000 -                ???  dasd                   <74A45F85-E121-3745-97CD-4896BE45143D>  /usr/libexec/dasd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          diagnosticd [353]
UUID:             63483E19-7DE9-358A-9F57-2CA9259813CB
Path:             /usr/libexec/diagnosticd
Codesigning ID:   com.apple.diagnosticd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1296 KB
Time Since Fork:  176s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x901    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1095bd000 -                ???  diagnosticd            <63483E19-7DE9-358A-9F57-2CA9259813CB>  /usr/libexec/diagnosticd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          diagnosticservicesd [440]
UUID:             BF60DABA-6515-3448-A714-A82BE9AFD08F
Path:             /System/Library/CoreServices/diagnosticservicesd
Codesigning ID:   com.apple.diagnosticservicesd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4344 KB
Time Since Fork:  161s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xb24    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x102199000 -                ???  diagnosticservicesd    <BF60DABA-6515-3448-A714-A82BE9AFD08F>  /System/Library/CoreServices/diagnosticservicesd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          dirhelper [150]
UUID:             3412C022-6A6D-3F73-B03F-510A59CF7619
Path:             /usr/libexec/dirhelper
Codesigning ID:   com.apple.dirhelper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        572 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x378    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1009c9000 -                ???  dirhelper              <3412C022-6A6D-3F73-B03F-510A59CF7619>  /usr/libexec/dirhelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          diskarbitrationd [125]
UUID:             DD768E83-E93A-32A7-8523-3429A3457A3E
Path:             /usr/libexec/diskarbitrationd
Codesigning ID:   com.apple.diskarbitrationd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1600 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3ac    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x109c9a000 -                ???  diskarbitrationd       <DD768E83-E93A-32A7-8523-3429A3457A3E>  /usr/libexec/diskarbitrationd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          DiskManagementSubscriber [658]
UUID:             8BA2BB55-74C8-3AF0-86E7-E41961DCF1E2
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/DiskManagementSubscriber.xpc/Contents/MacOS/DiskManagementSubscriber
Identifier:       com.apple.remotemanagement.DiskManagementSubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3788 KB
Time Since Fork:  51s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14c8    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1019b3000 -                ???  com.apple.remotemanagement.DiskManagementSubscriber (2.0) <8BA2BB55-74C8-3AF0-86E7-E41961DCF1E2>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/DiskManagementSubscriber.xpc/Contents/MacOS/DiskManagementSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          displaypolicyd [144]
UUID:             57AD1829-EC96-3A83-A03C-B9C6B7B830BD
Path:             /usr/libexec/displaypolicyd
Codesigning ID:   com.apple.displaypolicyd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1164 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x335    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested utility), timers tier 3 (coalesced), IO tier 1>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (displaypolicyd + 8280) [0x10007f058] 1-12
      12  ??? (displaypolicyd + 7271) [0x10007ec67] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x3fe    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (displaypolicyd + 105788) [0x100096d3c] 1-12
        12  ??? (libsystem_kernel.dylib + 30938) [0x7ff8137938da] 1-12
         *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
           *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
             *12  ??? (kernel + 6906599) [0xffffff800097e2e7] 1-12
               *12  ??? (kernel + 6476076) [0xffffff800091512c] 1-12
                 *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
                   *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                     *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                       *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Binary Images:
           0x10007d000 -                ???  displaypolicyd          <57AD1829-EC96-3A83-A03C-B9C6B7B830BD>  /usr/libexec/displaypolicyd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [147]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        772 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3c9    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1025dd6c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1025d9000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [208]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        692 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x51b    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x10e6a36c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10e69f000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [210]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        692 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x51c    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1091056c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x109101000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [223]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        692 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5bc    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1033c06c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1033bc000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [224]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        692 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5b9    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1041046c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x104100000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [279]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        676 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x6fa    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x10df1a6c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10df16000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [291]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        684 KB
Time Since Fork:  178s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x756    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1076e76c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1076e3000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [319]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        692 KB
Time Since Fork:  177s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x7dc    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x10fe946c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10fe90000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [362]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        676 KB
Time Since Fork:  176s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x94d    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1036d06c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1036cc000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [543]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        668 KB
Time Since Fork:  122s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xf46    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1091646c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x109160000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [546]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        660 KB
Time Since Fork:  118s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xf6a    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x1037b16c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1037ad000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          distnoted [651]
UUID:             82C4BB86-3554-343F-BF48-05F8A52355A2
Path:             /usr/sbin/distnoted
Codesigning ID:   com.apple.distnoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64h
Footprint:        684 KB
Time Since Fork:  52s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x1498    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (distnoted + 18120) [0x10225e6c8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10225a000 -                ???  distnoted              <82C4BB86-3554-343F-BF48-05F8A52355A2>  /usr/sbin/distnoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          eligibilityd [359]
UUID:             3C853069-EC43-3361-9621-0426D32A26D3
Path:             /usr/libexec/eligibilityd
Codesigning ID:   com.apple.eligibilityd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2368 KB
Time Since Fork:  176s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x945    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x108c3a000 -                ???  eligibilityd           <3C853069-EC43-3361-9621-0426D32A26D3>  /usr/libexec/eligibilityd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          endpointsecurityd [106]
UUID:             6979E6C3-8A99-3727-8423-953A74AF6ABB
Path:             /usr/libexec/endpointsecurityd
Codesigning ID:   com.apple.endpointsecurityd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        960 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x343    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (endpointsecurityd + 7183) [0x10e3fbc0f] 1-12
      12  ??? (endpointsecurityd + 15433) [0x10e3fdc49] 1-12
        12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
          12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10e3fa000 -                ???  endpointsecurityd      <6979E6C3-8A99-3727-8423-953A74AF6ABB>  /usr/libexec/endpointsecurityd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          findmybeaconingd [363]
UUID:             E14910CC-7BB3-3317-AE51-FD3C8A8CCAAF
Path:             /usr/libexec/findmybeaconingd
Codesigning ID:   com.apple.findmy.findmybeaconingd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2380 KB
Time Since Fork:  176s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x946    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (findmybeaconingd + 8479) [0x101f8f11f] 1-12
      12  ??? (libswift_Concurrency.dylib + 309935) [0x7ffd228a5aaf] 1-12
        12  ??? (libswift_Concurrency.dylib + 342835) [0x7ffd228adb33] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x101f8d000 -                ???  findmybeaconingd           <E14910CC-7BB3-3317-AE51-FD3C8A8CCAAF>  /usr/libexec/findmybeaconingd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                       <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib     <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation             <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ffd2285a000 -     0x7ffd228bfe07  libswift_Concurrency.dylib <3649D4F6-40EE-380C-B347-4066D69D5D29>  /usr/lib/swift/libswift_Concurrency.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)            <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          findmydeviced [183]
UUID:             4AF5795E-3608-3CF0-8F54-1FB3292AF721
Path:             /usr/libexec/findmydeviced
Codesigning ID:   com.apple.icloud.findmydeviced
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2408 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x457    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (findmydeviced + 106920) [0x10fc581a8] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10fc3e000 -                ???  findmydeviced          <4AF5795E-3608-3CF0-8F54-1FB3292AF721>  /usr/libexec/findmydeviced
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          FindMyMacd [199]
UUID:             CF2E6C24-3028-3F86-B423-B77FDBE47557
Path:             /System/Library/PrivateFrameworks/FindMyMac.framework/Versions/A/Resources/FindMyMacd
Codesigning ID:   com.apple.FindMyMacd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1008 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4ae    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested utility), timers tier 3 (coalesced), IO tier 1>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (FindMyMacd + 19707) [0x1058bdcfb] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1058b9000 -                ???  FindMyMacd             <CF2E6C24-3028-3F86-B423-B77FDBE47557>  /System/Library/PrivateFrameworks/FindMyMac.framework/Versions/A/Resources/FindMyMacd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          firmwaresyncd [119]
UUID:             C619CA93-D1E6-341C-B63B-D3F5947DDCAE
Path:             /usr/libexec/firmwaresyncd
Codesigning ID:   com.apple.firmwaresyncd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        904 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x34f    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (firmwaresyncd + 2222) [0x10031f8ae] 1-12
      12  ??? (libsystem_c.dylib + 104056) [0x7ff813686678] 1-12
        12  ??? (libsystem_kernel.dylib + 13762) [0x7ff81378f5c2] 1-12
         *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x10031f000 -                ???  firmwaresyncd          <C619CA93-D1E6-341C-B63B-D3F5947DDCAE>  /usr/libexec/firmwaresyncd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81366d000 -     0x7ff8136f5a07  libsystem_c.dylib      <6B1481E5-A9C4-3315-8ADE-BE0EDA06B6EC>  /usr/lib/system/libsystem_c.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          fseventsd [98]
UUID:             248CE240-F20A-3652-AFC3-1ED32602D251
Path:             /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/FSEvents.framework/Versions/A/Support/fseventsd
Codesigning ID:   com.apple.fseventsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2504 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.001s
Num threads:      6
Note:             1 idle work queue thread omitted

  Thread 0x35b    Thread name "com.apple.fseventsd.notify"    12 samples (1-12)    priority 49 (base 49)    cpu time 0.001s
  <IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (fseventsd + 66383) [0x10a4f034f] 1-12
        12  ??? (fseventsd + 64150) [0x10a4efa96] 1-12
          12  ??? (fseventsd + 65825) [0x10a4f0121] 1-12
            12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
             *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x35c    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x389    Thread name "com.apple.fseventsd.volume"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0 and passive>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (fseventsd + 15561) [0x10a4e3cc9] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x38a    Thread name "com.apple.fseventsd.dev.fsevents"    12 samples (1-12)    priority 50 (base 50)    cpu time <0.001s
  <IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 5554) [0x7ff81378d5b2] 1-12
       *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
         *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
           *12  ??? (kernel + 6632007) [0xffffff800093b247] 1-12
             *12  ??? (kernel + 6631571) [0xffffff800093b093] 1-12
               *12  ??? (kernel + 3270947) [0xffffff8000606923] 1-12
                 *12  ??? (kernel + 6476190) [0xffffff800091519e] 1-12
                   *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                     *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                       *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x391    Thread name "com.apple.fseventsd.disklogger.98"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0 and passive>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (fseventsd + 46291) [0x10a4eb4d3] 1-12
        12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
         *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Binary Images:
           0x10a4e0000 -                ???  fseventsd                 <248CE240-F20A-3652-AFC3-1ED32602D251>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/FSEvents.framework/Versions/A/Support/fseventsd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib   <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation            <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1) <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread


Process:          fskit_helper [232]
UUID:             BDA81F8B-749D-351A-B56E-B5BF96FD6858
Path:             /usr/libexec/fskit_helper
Codesigning ID:   com.apple.fskit_helper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1012 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5c5    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10646f000 -                ???  fskit_helper           <BDA81F8B-749D-351A-B56E-B5BF96FD6858>  /usr/libexec/fskit_helper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          hidd [360]
UUID:             ED74F725-F42C-3ADC-B765-F25A1FB3C8C4
Path:             /usr/libexec/hidd
Codesigning ID:   com.apple.hidd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1096 KB
Time Since Fork:  176s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x93f    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (hidd + 2631) [0x104e86a47] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x104e86000 -                ???  hidd                   <ED74F725-F42C-3ADC-B765-F25A1FB3C8C4>  /usr/libexec/hidd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          iconservicesd [123]
UUID:             4D3301C7-9568-3D90-8D4F-8358A024B767
Path:             /System/Library/CoreServices/iconservicesd
Codesigning ID:   com.apple.iconservicesd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1220 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x77b    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x106829000 -                ???  iconservicesd          <4D3301C7-9568-3D90-8D4F-8358A024B767>  /System/Library/CoreServices/iconservicesd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          imklaunchagent [641]
UUID:             00AC8081-7240-3A17-AFB5-FC79882CF993
Path:             /System/Library/Frameworks/InputMethodKit.framework/Versions/A/Resources/imklaunchagent
Codesigning ID:   com.apple.imklaunchagent
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2424 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x141d    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 46 (base 46)
  <thread QoS user interactive (requested user interactive), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (imklaunchagent + 32605) [0x10d123f5d] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10d11c000 -                ???  imklaunchagent         <00AC8081-7240-3A17-AFB5-FC79882CF993>  /System/Library/Frameworks/InputMethodKit.framework/Versions/A/Resources/imklaunchagent
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          InteractiveLegacyProfilesSubscriber (InteractiveLegacyProfilesSubscriber) [652]
UUID:             0530D354-D5CF-3F3A-A54C-EE881FF06DAC
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/InteractiveLegacyProfilesSubscriber.xpc/Contents/MacOS/InteractiveLegacyProfilesSubscriber
Identifier:       com.apple.remotemanagement.InteractiveLegacyProfilesSubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3740 KB
Time Since Fork:  52s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14a2    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10bcb9000 -                ???  com.apple.remotemanagement.InteractiveLegacyProfilesSubscriber (2.0) <0530D354-D5CF-3F3A-A54C-EE881FF06DAC>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/InteractiveLegacyProfilesSubscriber.xpc/Contents/MacOS/InteractiveLegacyProfilesSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                                    <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                               <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                                      <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          KernelEventAgent [153]
UUID:             91B4BEA2-7B85-325C-A247-265A9B135443
Path:             /usr/sbin/KernelEventAgent
Codesigning ID:   com.apple.KernelEventAgent
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        908 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x382    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10120e000 -                ???  KernelEventAgent       <91B4BEA2-7B85-325C-A247-265A9B135443>  /usr/sbin/KernelEventAgent
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          kernelmanager_helper [321]
UUID:             FE593CB9-5709-3B49-BB2F-A827622F0646
Path:             /usr/libexec/kernelmanager_helper
Codesigning ID:   com.apple.kernelmanager_helper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        8548 KB
Time Since Fork:  177s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x8c9    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10157a000 -                ???  kernelmanager_helper   <FE593CB9-5709-3B49-BB2F-A827622F0646>  /usr/libexec/kernelmanager_helper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          kernelmanagerd [124]
UUID:             F6D98646-83CE-36EA-BAB5-B04FB6CAD8B5
Path:             /usr/libexec/kernelmanagerd
Codesigning ID:   com.apple.kernelmanagerd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        9716 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x815    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1057d4000 -                ???  kernelmanagerd         <F6D98646-83CE-36EA-BAB5-B04FB6CAD8B5>  /usr/libexec/kernelmanagerd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          keybagd [114]
UUID:             5C0B120A-DA42-3485-BE6C-DE2BABE3C132
Path:             /usr/libexec/keybagd
Codesigning ID:   com.apple.keybagd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1016 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x324    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (keybagd + 45388) [0x108cee14c] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x108ce3000 -                ???  keybagd                <5C0B120A-DA42-3485-BE6C-DE2BABE3C132>  /usr/libexec/keybagd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          launchd [1]
UUID:             551C6EA4-14C6-3C95-B138-CED066115DC8
Path:             /sbin/launchd
Codesigning ID:   com.apple.xpc.launchd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Parent:           kernel_task [0]
Footprint:        16.24 MB
Time Since Fork:  184s
Num samples:      12 (1-12)
CPU Time:         0.001s
Num threads:      4
Note:             3 idle work queue threads omitted

  Thread 0x26a    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10a80d000 -                ???  launchd                <551C6EA4-14C6-3C95-B138-CED066115DC8>  /sbin/launchd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          launchservicesd [136]
UUID:             A4E56372-D0F4-3C77-A178-E241C23F9837
Path:             /System/Library/CoreServices/launchservicesd
Codesigning ID:   com.apple.launchservicesd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1784 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3f0    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10a36c000 -                ???  launchservicesd        <A4E56372-D0F4-3C77-A178-E241C23F9837>  /System/Library/CoreServices/launchservicesd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          LegacyProfilesSubscriber [656]
UUID:             59917FF3-BB36-354E-8B93-FD2CD2617296
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/LegacyProfilesSubscriber.xpc/Contents/MacOS/LegacyProfilesSubscriber
Identifier:       com.apple.remotemanagement.LegacyProfilesSubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3680 KB
Time Since Fork:  52s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14bb    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10afd9000 -                ???  com.apple.remotemanagement.LegacyProfilesSubscriber (2.0) <59917FF3-BB36-354E-8B93-FD2CD2617296>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/LegacyProfilesSubscriber.xpc/Contents/MacOS/LegacyProfilesSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          locationd [141]
UUID:             4D68526E-7590-34B3-9815-E75CF597EFA1
Path:             /usr/libexec/locationd
Codesigning ID:   com.apple.locationd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4264 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.014s
Num threads:      5
Note:             3 idle work queue threads omitted

  Thread 0x3c8    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)    cpu time <0.001s
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (locationd + 6750700) [0x10620e1ec] 1-12
      12  ??? (locationd + 6738045) [0x10620b07d] 1-12
        12  ??? (locationd + 6734445) [0x10620a26d] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x569    Thread name "com.apple.CoreMotion.MotionThread"    12 samples (1-12)    priority 4 (base 4)    cpu time <0.001s
  <thread QoS background, thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (locationd + 1645489) [0x105d2fbb1] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x105b9e000 -                ???  locationd               <4D68526E-7590-34B3-9815-E75CF597EFA1>  /usr/libexec/locationd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          logd [93]
UUID:             A6DE6637-5EE4-3C05-9449-B3B0592D0BB4
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3244 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      4
Note:             3 idle work queue threads omitted

  Thread 0x372    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x105fe7000 -                ???  ???                    <A6DE6637-5EE4-3C05-9449-B3B0592D0BB4>
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          logd_helper [369]
UUID:             B6221631-C4CD-378C-827E-ED8711BC0B36
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        772 KB
Time Since Fork:  175s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x98b    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10a9ed000 -                ???  ???                    <B6221631-C4CD-378C-827E-ED8711BC0B36>
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          logind [151]
UUID:             A79D9EFA-F363-3A6D-9DC3-04EC8E2F26E5
Path:             /System/Library/CoreServices/logind
Codesigning ID:   com.apple.logind
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1364 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x338    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (logind + 3810) [0x10a540ee2] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10a540000 -                ???  logind                 <A79D9EFA-F363-3A6D-9DC3-04EC8E2F26E5>  /System/Library/CoreServices/logind
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          loginwindow [640]
UUID:             E1A3CABA-02EC-3049-9792-1EC013BF2800
Path:             /System/Library/CoreServices/loginwindow.app/Contents/MacOS/loginwindow
Identifier:       com.apple.loginwindow
Version:          9.0 (3037.6.3.101)
Build Version:    2
Project Name:     loginwindow
Source Version:   3037006003101000
Is First Party:   Yes
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        5764 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x13fa    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 47 (base 47)
  <thread QoS user interactive (requested user interactive), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (loginwindow + 25727) [0x10602647f] 1-12
      12  ??? (AppKit + 200361) [0x7ff81730eea9] 1-12
        12  ??? (AppKit + 11328535) [0x7ff817dabc17] 1-12
          12  ??? (AppKit + 261569) [0x7ff81731ddc1] 1-12
            12  ??? (HIToolbox + 2310554) [0x7ff81f53519a] 1-12
              12  ??? (HIToolbox + 700311) [0x7ff81f3abf97] 1-12
                12  ??? (HIToolbox + 688340) [0x7ff81f3a90d4] 1-12
                  12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                    12  ??? (CoreFoundation + 504628) [0x7ff8138b8334] 1-12
                      12  ??? (CoreFoundation + 755830) [0x7ff8138f5876] 1-12
                        12  ??? (libdispatch.dylib + 57055) [0x7ff81362fedf] 1-12
                          12  ??? (libdispatch.dylib + 57190) [0x7ff81362ff66] 1-12
                            12  ??? (libdispatch.dylib + 204447) [0x7ff813653e9f] 1-12
                              12  ??? (libdispatch.dylib + 83962) [0x7ff8136367fa] 1-12
                                12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                                  12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                    12  ??? (loginwindow + 199599) [0x106050baf] 1-12
                                      12  ??? (loginwindow + 50131) [0x10602c3d3] 1-12
                                        12  ??? (libdispatch.dylib + 14573) [0x7ff8136258ed] 1-12
                                          12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                            12  ??? (loginwindow + 428190) [0x10608889e] 1-12
                                              12  ??? (loginwindow + 50240) [0x10602c440] 1-12
                                                12  ??? (SkyLight + 3705480) [0x7ff8197c5a88] 1-12
                                                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                                       *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile waiting for WindowServer [635] after 2 hops) 1-12

  Thread 0x1442    Thread name "_normal_auth_at_loginwindow"    12 samples (1-12)    priority 47 (base 47)
  <thread QoS user interactive (requested user interactive), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 347481) [0x7ff8149f6d59] 1-12
        12  ??? (loginwindow + 43164) [0x10602a89c] 1-12
          12  ??? (loginwindow + 45739) [0x10602b2ab] 1-12
            12  ??? (Security + 1802021) [0x7ff816966f25] 1-12
              12  ??? (Security + 1802925) [0x7ff8169672ad] 1-12
                12  ??? (libxpc.dylib + 45020) [0x7ff8134ccfdc] 1-12
                  12  ??? (libdispatch.dylib + 102705) [0x7ff81363b131] 1-12
                    12  ??? (libdispatch.dylib + 101696) [0x7ff81363ad40] 1-12
                      12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                        12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                          12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                           *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x106020000 -                ???  com.apple.loginwindow 9.0 (3037.6.3.101) <E1A3CABA-02EC-3049-9792-1EC013BF2800>  /System/Library/CoreServices/loginwindow.app/Contents/MacOS/loginwindow
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                     <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff8134c2000 -     0x7ff813508bd5  libxpc.dylib                             <A1CBBDA0-6389-3803-91FB-D18379D62FEB>  /usr/lib/system/libxpc.dylib
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                        <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib                  <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                           <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation                               <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
        0x7ff8167af000 -     0x7ff816b8c62d  Security                                 <FC8E92AA-DF82-3224-B0A8-71548346DCD0>  /System/Library/Frameworks/Security.framework/Versions/A/Security
        0x7ff8172de000 -     0x7ff81882ec13  AppKit                                   <BDAACBEF-8C1C-3716-9E23-FB955BFFEA2A>  /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
        0x7ff81943d000 -     0x7ff81990bf2f  SkyLight                                 <14C81C57-7629-32F2-9E8C-AF32EF962700>  /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/SkyLight
        0x7ff81f301000 -     0x7ff81f5e26ed  HIToolbox                                <23B7558D-4EF9-3BA5-99FC-7C09357E5E5B>  /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/HIToolbox
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          lsd [174]
UUID:             1C58156E-3A0E-3204-99CD-8E9637D4F1E5
Path:             /usr/libexec/lsd
Codesigning ID:   com.apple.lsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2236 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x413    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (LaunchServices + 1996083) [0x7ff813eda533] 1-12
      12  ??? (LaunchServices + 1996171) [0x7ff813eda58b] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10cce4000 -                ???  lsd                    <1C58156E-3A0E-3204-99CD-8E9637D4F1E5>  /usr/libexec/lsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff813cf3000 -     0x7ff813feb2d4  LaunchServices         <B6B9504B-E1D3-33A7-AAF2-F929A0E87DE5>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/LaunchServices
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          lsd [559]
UUID:             1C58156E-3A0E-3204-99CD-8E9637D4F1E5
Path:             /usr/libexec/lsd
Codesigning ID:   com.apple.lsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2036 KB
Time Since Fork:  117s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xfc0    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (LaunchServices + 1996083) [0x7ff813eda533] 1-12
      12  ??? (LaunchServices + 1996171) [0x7ff813eda58b] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x100045000 -                ???  lsd                    <1C58156E-3A0E-3204-99CD-8E9637D4F1E5>  /usr/libexec/lsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff813cf3000 -     0x7ff813feb2d4  LaunchServices         <B6B9504B-E1D3-33A7-AAF2-F929A0E87DE5>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/LaunchServices
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          ManagedConfigurationFilesSubscriber (ManagedConfigurationFilesSubscriber) [661]
UUID:             71374FDE-6FD2-31B7-BA84-FBC4F1D20239
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/ManagedConfigurationFilesSubscriber.xpc/Contents/MacOS/ManagedConfigurationFilesSubscriber
Identifier:       com.apple.remotemanagement.ManagedConfigurationFilesSubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3968 KB
Time Since Fork:  51s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14d9    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10e961000 -                ???  com.apple.remotemanagement.ManagedConfigurationFilesSubscriber (2.0) <71374FDE-6FD2-31B7-BA84-FBC4F1D20239>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/ManagedConfigurationFilesSubscriber.xpc/Contents/MacOS/ManagedConfigurationFilesSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                                    <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                               <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                                      <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          ManagementTestSubscriber [660]
UUID:             64B979B3-B25C-3291-B376-C2D107D6E8EB
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/ManagementTestSubscriber.xpc/Contents/MacOS/ManagementTestSubscriber
Identifier:       com.apple.remotemanagement.ManagementTestSubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3884 KB
Time Since Fork:  51s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14d2    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10846f000 -                ???  com.apple.remotemanagement.ManagementTestSubscriber (2.0) <64B979B3-B25C-3291-B376-C2D107D6E8EB>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/ManagementTestSubscriber.xpc/Contents/MacOS/ManagementTestSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mDNSResponder [191]
UUID:             CA28DB20-64FA-34CC-88B4-EE77123879D4
Path:             /usr/sbin/mDNSResponder
Codesigning ID:   com.apple.mDNSResponder
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2160 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x48b    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (mDNSResponder + 13048) [0x1035fd2f8] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x54d    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 22562) [0x7ff813791822] 1-12
       *12  ??? (kernel + 6240976) [0xffffff80008dbad0] 1-12

  Binary Images:
           0x1035fa000 -                ???  mDNSResponder           <CA28DB20-64FA-34CC-88B4-EE77123879D4>  /usr/sbin/mDNSResponder
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mDNSResponderHelper [206]
UUID:             82450BA9-7A75-384F-8ECA-E5A228635773
Path:             /usr/sbin/mDNSResponderHelper
Codesigning ID:   com.apple.mDNSResponderHelper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1480 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x4fc    12 samples (1-12)    priority 31 (base 31)    cpu time <0.001s
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (mDNSResponderHelper + 20336) [0x10ba52f70] 1-12
        12  ??? (libsystem_c.dylib + 104056) [0x7ff813686678] 1-12
          12  ??? (libsystem_kernel.dylib + 13762) [0x7ff81378f5c2] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x512    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10ba4e000 -                ???  mDNSResponderHelper     <82450BA9-7A75-384F-8ECA-E5A228635773>  /usr/sbin/mDNSResponderHelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81366d000 -     0x7ff8136f5a07  libsystem_c.dylib       <6B1481E5-A9C4-3315-8ADE-BE0EDA06B6EC>  /usr/lib/system/libsystem_c.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mds [122]
UUID:             F13019BE-6203-3F7C-85F1-18C97CC6E626
Path:             /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/Metadata.framework/Versions/A/Support/mds
Codesigning ID:   com.apple.mds
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        14.75 MB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.003s
Num threads:      6
Note:             3 idle work queue threads omitted

  Thread 0x32b    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)    cpu time <0.001s
  <thread QoS default (requested default), IO tier 0 and passive>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (mds + 51110) [0x10b9307a6] 1-12
      12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
        12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
          12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
            12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
              12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                 *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x41e    12 samples (1-12)    priority 50 (base 50)    cpu time <0.001s
  <IO tier 0 and passive>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 5554) [0x7ff81378d5b2] 1-12
       *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
         *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
           *12  ??? (kernel + 6632007) [0xffffff800093b247] 1-12
             *12  ??? (kernel + 6631571) [0xffffff800093b093] 1-12
               *12  ??? (kernel + 3270947) [0xffffff8000606923] 1-12
                 *12  ??? (kernel + 6476190) [0xffffff800091519e] 1-12
                   *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                     *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                       *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x736    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0 and passive>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (SpotlightIndex + 15384) [0x7ff82fadcc18] 1-12
        12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
          12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
            12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
             *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10b924000 -                ???  mds                     <F13019BE-6203-3F7C-85F1-18C97CC6E626>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/Metadata.framework/Versions/A/Support/mds
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff82fad9000 -     0x7ff83004126b  SpotlightIndex          <CEBB7BA9-C145-3E22-B8D3-19DFCDDF59A8>  /System/Library/PrivateFrameworks/SpotlightIndex.framework/Versions/A/SpotlightIndex
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mds_stores [287]
UUID:             01F2FB8F-5CEC-3FD7-A6F8-4C6585C25451
Path:             /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/Metadata.framework/Versions/A/Support/mds_stores
Codesigning ID:   com.apple.mds_stores
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        8572 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
CPU Time:         0.001s
Num threads:      4
Note:             2 idle work queue threads omitted

  Thread 0x715    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)    cpu time <0.001s
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (mds_stores + 6476) [0x10674894c] 1-12
      12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
        12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
          12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
            12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
              12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                 *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x71d    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (SpotlightIndex + 15384) [0x7ff82fadcc18] 1-12
        12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
          12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
            12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
             *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x106747000 -                ???  mds_stores              <01F2FB8F-5CEC-3FD7-A6F8-4C6585C25451>  /System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/Metadata.framework/Versions/A/Support/mds_stores
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff82fad9000 -     0x7ff83004126b  SpotlightIndex          <CEBB7BA9-C145-3E22-B8D3-19DFCDDF59A8>  /System/Library/PrivateFrameworks/SpotlightIndex.framework/Versions/A/SpotlightIndex
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mdworker [571]
UUID:             90840ACA-706A-3E2D-8C30-B5AA455304B9
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1476 KB
Time Since Fork:  116s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x101a    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background, thread darwinbg, process darwinbg, IO tier 3>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (<90840ACA-706A-3E2D-8C30-B5AA455304B9> + 38882) [0x104f117e2] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x104f08000 -                ???  ???                    <90840ACA-706A-3E2D-8C30-B5AA455304B9>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mediaremoted [99]
UUID:             928E0E78-646E-354F-AD64-AC8C40C7FC6A
Path:             /System/Library/PrivateFrameworks/MediaRemote.framework/Support/mediaremoted
Codesigning ID:   com.apple.mediaremoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2608 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      4
Note:             1 idle work queue thread omitted

  Thread 0x341    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (mediaremoted + 560134) [0x10293dc06] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x560    Thread name "caulk.messenger.shared:17"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested utility), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x561    Thread name "caulk.messenger.shared:high"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background, thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x1028b5000 -                ???  mediaremoted            <928E0E78-646E-354F-AD64-AC8C40C7FC6A>  /System/Library/PrivateFrameworks/MediaRemote.framework/Support/mediaremoted
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
        0x7ff81eec2000 -     0x7ff81eee5e17  caulk                   <684B920A-BD31-31C3-A5AA-B63B6654CDA3>  /System/Library/PrivateFrameworks/caulk.framework/Versions/A/caulk
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          mobileassetd [197]
UUID:             A26D3F5A-A9B6-38A2-87DA-7F3159EC44BA
Path:             /usr/libexec/mobileassetd
Codesigning ID:   com.apple.mobileassetd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3764 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4a2    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (mobileassetd + 8223) [0x1006c501f] 1-12
      12  ??? (mobileassetd + 26966) [0x1006c9956] 1-12
        12  ??? (mobileassetd + 23544) [0x1006c8bf8] 1-12
          12  ??? (<99FF91A1-24C7-30C3-897A-DE4C8AECA4F2> + 2146328) [0x10a54a018] 1-12
            12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
              12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                  12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                    12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                      12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                        12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                         *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1006c3000 -                ???  mobileassetd           <A26D3F5A-A9B6-38A2-87DA-7F3159EC44BA>  /usr/libexec/mobileassetd
           0x10a33e000 -                ???  ???                    <99FF91A1-24C7-30C3-897A-DE4C8AECA4F2>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          modelcatalogd [545]
UUID:             72833BD4-8489-3112-AD1B-B8382B538D2D
Path:             /System/Library/PrivateFrameworks/ModelCatalogRuntime.framework/Versions/A/modelcatalogd
Codesigning ID:   com.apple.modelcatalogd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1696 KB
Time Since Fork:  118s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xf70    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x108879000 -                ???  modelcatalogd          <72833BD4-8489-3112-AD1B-B8382B538D2D>  /System/Library/PrivateFrameworks/ModelCatalogRuntime.framework/Versions/A/modelcatalogd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          nehelper [184]
UUID:             47399936-5D10-36CD-BC53-4F0EC0D7A62A
Path:             /usr/libexec/nehelper
Codesigning ID:   com.apple.nehelper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2288 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x465    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (nehelper + 176104) [0x10803ffe8] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x108015000 -                ???  nehelper               <47399936-5D10-36CD-BC53-4F0EC0D7A62A>  /usr/libexec/nehelper
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          nesessionmanager [142]
UUID:             A9A4A8BF-7663-383B-9056-FE74C09CC05A
Path:             /usr/libexec/nesessionmanager
Codesigning ID:   com.apple.nesessionmanager
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2024 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x34c    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (nesessionmanager + 556047) [0x10f73bc0f] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10f6b4000 -                ???  nesessionmanager       <A9A4A8BF-7663-383B-9056-FE74C09CC05A>  /usr/libexec/nesessionmanager
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          netbiosd [414]
UUID:             2D64D7E5-C168-3A0E-A803-A3F99D078B44
Path:             /usr/sbin/netbiosd
Codesigning ID:   com.apple.netbiosd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1068 KB
Time Since Fork:  166s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xacf    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, timers tier 3 (coalesced), IO tier 3>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10597b000 -                ???  netbiosd               <2D64D7E5-C168-3A0E-A803-A3F99D078B44>  /usr/sbin/netbiosd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          notifyd [157]
UUID:             D312B2AC-E786-3A50-8DA1-5D2FC4E9B301
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1012 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.001s
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x358    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10bb1b000 -                ???  ???                    <D312B2AC-E786-3A50-8DA1-5D2FC4E9B301>
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          nsurlsessiond [201]
UUID:             F3ACC75A-F314-3FDB-881B-3D5FF046A2FF
Path:             /usr/libexec/nsurlsessiond
Codesigning ID:   com.apple.nsurlsessiond
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2224 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x4b2    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (nsurlsessiond + 13487) [0x105a6e4af] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x105a6b000 -                ???  nsurlsessiond          <F3ACC75A-F314-3FDB-881B-3D5FF046A2FF>  /usr/libexec/nsurlsessiond
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          opendirectoryd [133]
UUID:             A43A1E9F-B1B9-3357-ADF8-F21BA6A5DE5E
Path:             /usr/libexec/opendirectoryd
Codesigning ID:   com.apple.opendirectoryd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3980 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      4
Note:             1 idle work queue thread omitted

  Thread 0x3ba    DispatchQueue "com.apple.opendirectoryd.module.SystemCache.kauth_workq"(1068)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 205678) [0x7ff81365436e] 1-12
            12  ??? (libdispatch.dylib + 18400) [0x7ff8136267e0] 1-12
              12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                  12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                    12  ??? (libsystem_kernel.dylib + 45994) [0x7ff8137973aa] 1-12
                     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x3b7    DispatchQueue "com.apple.root.default-qos"(13)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 206203) [0x7ff81365457b] 1-12
            12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
              12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                12  ??? (<43E262BD-79C5-3D1C-B459-271243E923DB> + 162710) [0x106edcb96] 1-12
                  12  ??? (libsystem_kernel.dylib + 29014) [0x7ff813793156] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile UNKNOWN waiting for receive port name "com.apple.system.DirectoryService.libinfo_v1" in domain 1, mach_msg receive on port name "com.apple.system.DirectoryService.libinfo_v1" in domain 1 name 0x7d07) 1-12

  Thread 0x3e3    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x106c22000 -                ???  opendirectoryd          <A43A1E9F-B1B9-3357-ADF8-F21BA6A5DE5E>  /usr/libexec/opendirectoryd
           0x106eb5000 -                ???  ???                     <43E262BD-79C5-3D1C-B459-271243E923DB>
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          osanalyticshelper [445]
UUID:             2001CDB4-95C3-397E-B871-EE104618889D
Path:             /System/Library/CoreServices/osanalyticshelper
Codesigning ID:   com.apple.osanalyticshelper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1552 KB
Time Since Fork:  161s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xb31    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (osanalyticshelper + 64377) [0x106988b79] 1-12
      12  ??? (osanalyticshelper + 88876) [0x10698eb2c] 1-12
        12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
          12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x106979000 -                ???  osanalyticshelper      <2001CDB4-95C3-397E-B871-EE104618889D>  /System/Library/CoreServices/osanalyticshelper
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          PasscodeSettingsSubscriber [657]
UUID:             C928DC74-D9F9-3411-AD21-39F75B60E4EE
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/PasscodeSettingsSubscriber.xpc/Contents/MacOS/PasscodeSettingsSubscriber
Identifier:       com.apple.remotemanagement.PasscodeSettingsSubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3712 KB
Time Since Fork:  51s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14c1    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x109a23000 -                ???  com.apple.remotemanagement.PasscodeSettingsSubscriber (2.0) <C928DC74-D9F9-3411-AD21-39F75B60E4EE>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/PasscodeSettingsSubscriber.xpc/Contents/MacOS/PasscodeSettingsSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                           <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                      <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                             <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          PerfPowerServices [149]
UUID:             1ADAF064-5AA1-3479-BD17-0D88F1FE1FFD
Path:             /usr/libexec/PerfPowerServices
Codesigning ID:   com.apple.PerfPowerServices
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4896 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.001s
Num threads:      5
Note:             2 idle work queue threads omitted

  Thread 0x351    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (PerfPowerServices + 2384) [0x10f89d950] 1-12
      12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0xa4a    Thread name "caulk.messenger.shared:17"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested utility), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0xa4c    Thread name "caulk.messenger.shared:high"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background, thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x10f89d000 -                ???  PerfPowerServices       <1ADAF064-5AA1-3479-BD17-0D88F1FE1FFD>  /usr/libexec/PerfPowerServices
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
        0x7ff81eec2000 -     0x7ff81eee5e17  caulk                   <684B920A-BD31-31C3-A5AA-B63B6654CDA3>  /System/Library/PrivateFrameworks/caulk.framework/Versions/A/caulk
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          PerfPowerTelemetryClientRegistrationService (PerfPowerTelemetryClientRegistrationService) [255]
UUID:             DC9A6B8D-FA87-3E7F-8918-D9DDCD21852F
Path:             /System/Library/PrivateFrameworks/PowerLog.framework/Versions/A/XPCServices/PerfPowerTelemetryClientRegistrationService.xpc/Contents/MacOS/PerfPowerTelemetryClientRegistrationService
Identifier:       com.apple.PerfPowerTelemetryClientRegistrationService
Version:          1.0 (1)
Build Version:    361
Project Name:     PerfPowerServices_Client_Framework
Source Version:   2423140012000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1840 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x76a    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10a024000 -                ???  com.apple.PerfPowerTelemetryClientRegistrationService 1.0 (1) <DC9A6B8D-FA87-3E7F-8918-D9DDCD21852F>  /System/Library/PrivateFrameworks/PowerLog.framework/Versions/A/XPCServices/PerfPowerTelemetryClientRegistrationService.xpc/Contents/MacOS/PerfPowerTelemetryClientRegistrationService
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                             <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                        <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                               <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          pkd [645]
UUID:             66887A7C-3092-383F-A059-6E7B69E1D388
Path:             /usr/libexec/pkd
Codesigning ID:   com.apple.pluginkit.pkd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1536 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x142f    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (pkd + 1765) [0x1050176e5] 1-12
      12  ??? (<FCB4D710-6FE6-39B4-A43C-9FB5DE58DE08> + 80405) [0x1050d6a15] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x105017000 -                ???  pkd                    <66887A7C-3092-383F-A059-6E7B69E1D388>  /usr/libexec/pkd
           0x1050c3000 -                ???  ???                    <FCB4D710-6FE6-39B4-A43C-9FB5DE58DE08>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          PlugInLibraryService [264]
UUID:             4079EF59-FFB0-34CC-B141-89A8B0B6F399
Path:             /System/Library/Frameworks/NetFS.framework/Versions/A/XPCServices/PlugInLibraryService.xpc/Contents/MacOS/PlugInLibraryService
Identifier:       com.apple.filesystems.netfs.PlugInLibraryService
Version:          1.0 (1)
Build Version:    4
Project Name:     NetFSFramework
Source Version:   61000000702001
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        936 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x6b7    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x106ed4000 -                ???  com.apple.filesystems.netfs.PlugInLibraryService 1.0 (1) <4079EF59-FFB0-34CC-B141-89A8B0B6F399>  /System/Library/Frameworks/NetFS.framework/Versions/A/XPCServices/PlugInLibraryService.xpc/Contents/MacOS/PlugInLibraryService
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                        <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                   <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                          <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          powerd [107]
UUID:             04EC9DA8-C6BC-35E5-B7C7-EADF8792FA39
Path:             /System/Library/CoreServices/powerd.bundle/powerd
Codesigning ID:   com.apple.powerd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1976 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.004s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x444    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x105899000 -                ???  powerd                 <04EC9DA8-C6BC-35E5-B7C7-EADF8792FA39>  /System/Library/CoreServices/powerd.bundle/powerd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          powerexperienced [175]
UUID:             7B1B0FA8-70F3-3476-BF96-41B9300DB170
Path:             /usr/libexec/powerexperienced
Codesigning ID:   com.apple.powerexperienced
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1424 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x449    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1033cd000 -                ???  powerexperienced       <7B1B0FA8-70F3-3476-BF96-41B9300DB170>  /usr/libexec/powerexperienced
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          PowerUIAgent [180]
UUID:             0B6EBA06-A2FB-3BCD-8568-2FBA94AEFADB
Path:             /usr/libexec/PowerUIAgent
Codesigning ID:   com.apple.PowerUIAgent
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2272 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x43b    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (PowerUIAgent + 2591) [0x10f1a1a1f] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10f1a1000 -                ???  PowerUIAgent           <0B6EBA06-A2FB-3BCD-8568-2FBA94AEFADB>  /usr/libexec/PowerUIAgent
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          remoted [112]
UUID:             63F91743-749B-31C2-B750-FDBD1DAFC1FA
Path:             /usr/libexec/remoted
Codesigning ID:   com.apple.remoted
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1232 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x384    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x108aff000 -                ???  remoted                <63F91743-749B-31C2-B750-FDBD1DAFC1FA>  /usr/libexec/remoted
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          remotemanagementd [649]
UUID:             983D6114-D994-3D1F-9CAA-29F0B5C8E547
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/remotemanagementd
Codesigning ID:   com.apple.remotemanagementd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2124 KB
Time Since Fork:  52s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x1493    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10f135000 -                ???  remotemanagementd      <983D6114-D994-3D1F-9CAA-29F0B5C8E547>  /System/Library/PrivateFrameworks/RemoteManagement.framework/remotemanagementd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          ReportCrash [408]
UUID:             0CBC1175-1DF2-334D-98F6-A37272CDE66F
Path:             /System/Library/CoreServices/ReportCrash
Codesigning ID:   com.apple.CrashReporter
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4996 KB
Time Since Fork:  168s
Num samples:      12 (1-12)
Num threads:      5
Note:             1 idle work queue thread omitted

  Thread 0xa63    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested utility), timers tier 3 (coalesced), IO tier 1>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (ReportCrash + 152594) [0x1004a2412] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0xa73    Thread name "ExceptionSafeThread"    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (CoreSymbolication + 946954) [0x7ff82c26330a] 1-12
        12  ??? (CoreSymbolication + 947817) [0x7ff82c263669] 1-12
          12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
           *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0xb18    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested utility), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DebugSymbols + 44575) [0x7ff82c166e1f] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0xe5b    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (ReportCrash + 142589) [0x10049fcfd] 1-12
        12  ??? (libsystem_kernel.dylib + 39998) [0x7ff813795c3e] 1-12
          12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
            12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
             *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile UNKNOWN waiting for receive port name "com.apple.ReportCrash.DirectoryService" in domain 1, mach_msg receive on port name "com.apple.ReportCrash.DirectoryService" in domain 1 name 0xe03) 1-12

  Binary Images:
           0x10047d000 -                ???  ReportCrash               <0CBC1175-1DF2-334D-98F6-A37272CDE66F>  /System/Library/CoreServices/ReportCrash
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                      <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib   <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation            <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation                <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
        0x7ff82c15c000 -     0x7ff82c17b159  DebugSymbols              <09C6AE94-BC4A-3F06-AF4B-550F2924602A>  /System/Library/PrivateFrameworks/DebugSymbols.framework/Versions/A/DebugSymbols
        0x7ff82c17c000 -     0x7ff82c28de4f  CoreSymbolication         <2A4D7536-E23D-39E5-AD0B-37A60F9DF809>  /System/Library/PrivateFrameworks/CoreSymbolication.framework/Versions/A/CoreSymbolication
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1) <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread


Process:          revisiond [152]
UUID:             D2F325F0-B4A0-3F2D-B2F4-6328A65D3B4D
Path:             /System/Library/PrivateFrameworks/GenerationalStorage.framework/Versions/A/Support/revisiond
Codesigning ID:   com.apple.revisiond
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1892 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x350    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (revisiond + 81777) [0x1003b6f71] 1-12
      12  ??? (revisiond + 80650) [0x1003b6b0a] 1-12
        12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
          12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x767    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 5554) [0x7ff81378d5b2] 1-12
       *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
         *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
           *12  ??? (kernel + 6632007) [0xffffff800093b247] 1-12
             *12  ??? (kernel + 6631571) [0xffffff800093b093] 1-12
               *12  ??? (kernel + 3182633) [0xffffff80005f1029] 1-12
                 *12  ??? (kernel + 3187280) [0xffffff80005f2250] 1-12
                   *12  ??? (kernel + 3304088) [0xffffff800060ea98] 1-12
                     *12  ??? (AppleFSCompressionTypeDataless + 6462) [0xffffff800190a93e] 1-12
                       *12  ??? (kernel + 6476076) [0xffffff800091512c] 1-12
                         *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
                           *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                             *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                               *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Binary Images:
           0x1003a3000 -                ???  revisiond                                                             <D2F325F0-B4A0-3F2D-B2F4-6328A65D3B4D>  /System/Library/PrivateFrameworks/GenerationalStorage.framework/Versions/A/Support/revisiond
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                                                  <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                                <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib                                               <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                                                        <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation                                                            <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                                       <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff8001909000 - 0xffffff800190affe  com.apple.AppleFSCompression.AppleFSCompressionTypeDataless (1.0.0d1) <3482FB42-25C6-3FB9-846E-A202733E43BB>  /System/Library/Extensions/AppleFSCompressionTypeDataless.kext/Contents/MacOS/AppleFSCompressionTypeDataless


Process:          rpcsvchost [288]
UUID:             A3BE138D-332F-3D72-931E-54EEC3105395
Path:             /usr/libexec/rpcsvchost
Codesigning ID:   com.apple.rpcsvchost
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1188 KB
Time Since Fork:  178s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      16
Note:             1 idle work queue thread omitted

  Thread 0x735    12 samples (1-12)    priority 20 (base 20)    cpu time <0.001s
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 196251) [0x7ffa184c4e9b] 1-12
          12  ??? (DCERPC + 10290) [0x7ffa18497832] 1-12
            12  ??? (DCERPC + 10194) [0x7ffa184977d2] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x747    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (rpcsvchost + 9463) [0x10be894f7] 1-12
        12  ??? (DCERPC + 254573) [0x7ffa184d326d] 1-12
          12  ??? (DCERPC + 10290) [0x7ffa18497832] 1-12
            12  ??? (DCERPC + 10194) [0x7ffa184977d2] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x748    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x749    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 259961) [0x7ffa184d4779] 1-12
          12  ??? (libsystem_kernel.dylib + 39422) [0x7ff8137959fe] 1-12
           *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x74a    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x74b    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x74c    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x74d    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x74e    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x74f    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x750    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x751    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x752    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x753    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 236628) [0x7ffa184cec54] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x754    12 samples (1-12)    priority 20 (base 20)
  <thread QoS utility (requested default), timers tier 3 (coalesced), IO tier 1>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (DCERPC + 3598) [0x7ffa18495e0e] 1-12
        12  ??? (DCERPC + 336302) [0x7ffa184e71ae] 1-12
          12  ??? (DCERPC + 3167) [0x7ffa18495c5f] 1-12
            12  ??? (DCERPC + 3072) [0x7ffa18495c00] 1-12
              12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
               *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Binary Images:
           0x10be87000 -                ???  rpcsvchost                <A3BE138D-332F-3D72-931E-54EEC3105395>  /usr/libexec/rpcsvchost
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib         <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib   <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ffa18495000 -     0x7ffa18502587  DCERPC                    <8315D3EA-4888-3307-9B8E-1309CF2FAC14>  /System/Library/PrivateFrameworks/DCERPC.framework/Versions/A/DCERPC
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1) <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread


Process:          runningboardd [171]
UUID:             E341AA04-62ED-368A-94BB-1443F81E1B34
Path:             /usr/libexec/runningboardd
Codesigning ID:   com.apple.runningboardd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2716 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x406    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (runningboardd + 2007) [0x1038607d7] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x103860000 -                ???  runningboardd          <E341AA04-62ED-368A-94BB-1443F81E1B34>  /usr/libexec/runningboardd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          sandboxd [158]
UUID:             260951BA-C4EA-304C-A9F1-2035D8A2A2E0
Path:             /usr/libexec/sandboxd
Codesigning ID:   com.apple.sandboxd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1480 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x37d    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10adfc000 -                ???  sandboxd               <260951BA-C4EA-304C-A9F1-2035D8A2A2E0>  /usr/libexec/sandboxd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          ScreenSharingSubscriber [653]
UUID:             398AF0BC-5975-39E5-8929-8BAC167B2E7F
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/ScreenSharingSubscriber.xpc/Contents/MacOS/ScreenSharingSubscriber
Identifier:       com.apple.remotemanagement.ScreenSharingSubscriber
Version:          1.0 (1)
Build Version:    60
Project Name:     ScreenSharing
Source Version:   714006002000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4148 KB
Time Since Fork:  52s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14a8    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10228d000 -                ???  com.apple.remotemanagement.ScreenSharingSubscriber 1.0 (1) <398AF0BC-5975-39E5-8929-8BAC167B2E7F>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/ScreenSharingSubscriber.xpc/Contents/MacOS/ScreenSharingSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                          <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                     <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                            <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          searchpartyd [267]
UUID:             2A50EB75-E921-3E12-9306-A0ACF00D410C
Path:             /usr/libexec/searchpartyd
Codesigning ID:   com.apple.icloud.searchpartyd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3552 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x6c1    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (searchpartyd + 15325583) [0x110e7598f] 1-12
      12  ??? (libswift_Concurrency.dylib + 309935) [0x7ffd228a5aaf] 1-12
        12  ??? (libswift_Concurrency.dylib + 342835) [0x7ffd228adb33] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10ffd8000 -                ???  searchpartyd               <2A50EB75-E921-3E12-9306-A0ACF00D410C>  /usr/libexec/searchpartyd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                       <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib     <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation             <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ffd2285a000 -     0x7ffd228bfe07  libswift_Concurrency.dylib <3649D4F6-40EE-380C-B347-4066D69D5D29>  /usr/lib/swift/libswift_Concurrency.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)            <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          secd [561]
UUID:             CAA612EA-9629-3365-9440-120826CD7E71
Path:             /usr/libexec/secd
Codesigning ID:   com.apple.secd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3452 KB
Time Since Fork:  117s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xfdb    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (secd + 16448) [0x1082c1040] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1082bd000 -                ???  secd                   <CAA612EA-9629-3365-9440-120826CD7E71>  /usr/libexec/secd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          secinitd [228]
UUID:             BFF3CB01-9C0F-3C13-A97C-00AA60743FDF
Path:             /usr/libexec/secinitd
Codesigning ID:   com.apple.secinitd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1832 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5d6    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x100477000 -                ???  secinitd               <BFF3CB01-9C0F-3C13-A97C-00AA60743FDF>  /usr/libexec/secinitd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          secinitd [233]
UUID:             BFF3CB01-9C0F-3C13-A97C-00AA60743FDF
Path:             /usr/libexec/secinitd
Codesigning ID:   com.apple.secinitd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1668 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5f2    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1021d8000 -                ???  secinitd               <BFF3CB01-9C0F-3C13-A97C-00AA60743FDF>  /usr/libexec/secinitd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          SecurityAgent [632]
UUID:             63BA54DA-5A4E-3127-8567-84735DFFB20F
Path:             /System/Library/Frameworks/Security.framework/Versions/A/MachServices/SecurityAgent.bundle/Contents/MacOS/SecurityAgent
Identifier:       com.apple.SecurityAgent
Version:          9.0 (55579.140.3.0.1)
Build Version:    90
Project Name:     SecurityAgent
Source Version:   55579140003000001
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        51.90 MB
Time Since Fork:  79s
Num samples:      12 (1-12)
CPU Time:         0.012s
Num threads:      11
Note:             3 idle work queue threads omitted

  Thread 0x1359    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 47 (base 47)
  <process frontmost, thread QoS user interactive (requested user interactive), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (SecurityAgent + 128792) [0x107a30718] 1-12
      12  ??? (AppKit + 16389) [0x7ff8172e2005] 1-12
        12  ??? (AppKit + 200361) [0x7ff81730eea9] 1-12
          12  ??? (AppKit + 11328535) [0x7ff817dabc17] 1-12
            12  ??? (AppKit + 261569) [0x7ff81731ddc1] 1-12
              12  ??? (HIToolbox + 2310554) [0x7ff81f53519a] 1-12
                12  ??? (HIToolbox + 700311) [0x7ff81f3abf97] 1-12
                  12  ??? (HIToolbox + 688340) [0x7ff81f3a90d4] 1-12
                    12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                      12  ??? (CoreFoundation + 504628) [0x7ff8138b8334] 1-12
                        12  ??? (CoreFoundation + 755830) [0x7ff8138f5876] 1-12
                          12  ??? (libdispatch.dylib + 57055) [0x7ff81362fedf] 1-12
                            12  ??? (libdispatch.dylib + 57190) [0x7ff81362ff66] 1-12
                              12  ??? (libdispatch.dylib + 204688) [0x7ff813653f90] 1-12
                                12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                  12  ??? (libdispatch.dylib + 59570) [0x7ff8136308b2] 1-12
                                    12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                      12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 115311) [0x10aee326f] 1-12
                                        12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 101650) [0x10aedfd12] 1-12
                                          12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 66015) [0x10aed71df] 1-12
                                            12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 66575) [0x10aed740f] 1-12
                                              12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 59063) [0x10aed56b7] 1-12
                                                12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 14059) [0x10aeca6eb] 1-12
                                                  12  ??? (AppKit + 1394633) [0x7ff8174327c9] 1-12
                                                    12  ??? (AppKit + 2110770) [0x7ff8174e1532] 1-12
                                                      12  ??? (AppKit + 2111075) [0x7ff8174e1663] 1-12
                                                        12  ??? (AppKit + 184853) [0x7ff81730b215] 1-12
                                                          12  ??? (AppKit + 2117141) [0x7ff8174e2e15] 1-12
                                                            12  ??? (AppKit + 10913183) [0x7ff817d4659f] 1-12
                                                              12  ??? (AppKit + 2117194) [0x7ff8174e2e4a] 1-12
                                                                12  ??? (AppKit + 2117569) [0x7ff8174e2fc1] 1-12
                                                                  12  ??? (AppKit + 3318057) [0x7ff817608129] 1-12
                                                                    12  ??? (AppKit + 2121803) [0x7ff8174e404b] 1-12
                                                                      12  ??? (AppKit + 1791803) [0x7ff81749373b] 1-12
                                                                        12  ??? (AppKit + 2135589) [0x7ff8174e7625] 1-12
                                                                          12  ??? (TextInputUIMacHelper + 16196) [0x7ffd132c8f44] 1-12
                                                                            12  ??? (libdispatch.dylib + 14573) [0x7ff8136258ed] 1-12
                                                                              12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                                                                12  ??? (TextInputUIMacHelper + 16215) [0x7ffd132c8f57] 1-12
                                                                                  12  ??? (TextInputUIMacHelper + 16595) [0x7ffd132c90d3] 1-12
                                                                                    12  ??? (TextInputUIMacHelper + 13543) [0x7ffd132c84e7] 1-12
                                                                                      12  ??? (AppKit + 341854) [0x7ff81733175e] 1-12
                                                                                        12  ??? (AppKit + 343076) [0x7ff817331c24] 1-12
                                                                                          12  ??? (AppKit + 345244) [0x7ff81733249c] 1-12
                                                                                            12  ??? (AppKit + 15588374) [0x7ff8181bbc16] 1-12
                                                                                              12  ??? (AppKit + 15619122) [0x7ff8181c3432] 1-12
                                                                                                12  ??? (AppKit + 15619333) [0x7ff8181c3505] 1-12
                                                                                                  12  ??? (QuartzCore + 2384280) [0x7ff81cd0b198] 1-12
                                                                                                    12  ??? (QuartzCore + 2384554) [0x7ff81cd0b2aa] 1-12
                                                                                                      12  ??? (QuartzCore + 533277) [0x7ff81cb4731d] 1-12
                                                                                                        12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                                                                                          12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                                                                                            12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                                                                                             *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x1457    DispatchQueue "com.apple.scenekit.renderingQueue.LUIAVTView0x7ff5b0626020"(428)    12 samples (1-12)    priority 47 (base 47)
  <process frontmost, thread QoS user interactive (requested user interactive, workloop servicer override user interactive), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33171) [0x7ff81362a193] 1-12
              12  ??? (libdispatch.dylib + 83962) [0x7ff8136367fa] 1-12
                12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                  12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                    12  ??? (SceneKit + 5366030) [0x7ff91ee2510e] 1-12
                      12  ??? (SceneKit + 1935521) [0x7ff91eadf8a1] 1-12
                        12  ??? (SceneKit + 2390276) [0x7ff91eb4e904] 1-12
                          12  ??? (AvatarKit + 155443) [0x7ffb26d26f33] 1-12
                            12  ??? (SceneKit + 2391990) [0x7ff91eb4efb6] 1-12
                              12  ??? (SceneKit + 2472942) [0x7ff91eb62bee] 1-12
                                12  ??? (SceneKit + 2471671) [0x7ff91eb626f7] 1-12
                                  12  ??? (SceneKit + 2470190) [0x7ff91eb6212e] 1-12
                                    12  ??? (SceneKit + 2469786) [0x7ff91eb61f9a] 1-12
                                      12  ??? (SceneKit + 4114719) [0x7ff91ecf391f] 1-12
                                        12  ??? (SceneKit + 5174163) [0x7ff91edf6393] 1-12
                                          12  ??? (QuartzCore + 79940) [0x7ff81cad8844] 1-12
                                            12  ??? (QuartzCore + 2489760) [0x7ff81cd24da0] 1-12
                                              12  ??? (QuartzCore + 2505470) [0x7ff81cd28afe] 1-12
                                                12  ??? (QuartzCore + 291665) [0x7ff81cb0c351] 1-12
                                                  12  ??? (QuartzCore + 291787) [0x7ff81cb0c3cb] 1-12
                                                    12  ??? (QuartzCore + 954707) [0x7ff81cbae153] 1-12
                                                      12  ??? (QuartzCore + 292018) [0x7ff81cb0c4b2] 1-12
                                                        12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                                          12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                                            12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                                             *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x1362    DispatchQueue "loginwindow:login queue"(259)    12 samples (1-12)    priority 37 (base 37)
  <process frontmost, thread QoS user initiated (requested user initiated, workloop servicer override user initiated), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33631) [0x7ff81362a35f] 1-12
              12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                  12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 114894) [0x10aee30ce] 1-12
                    12  ??? (libdispatch.dylib + 59022) [0x7ff81363068e] 1-12
                      12  ??? (libdispatch.dylib + 60002) [0x7ff813630a62] 1-12
                        12  ??? (libdispatch.dylib + 11724) [0x7ff813624dcc] 1-12
                          12  ??? (libsystem_kernel.dylib + 8982) [0x7ff81378e316] 1-12
                           *12  ??? (kernel + 6667936) [0xffffff8000943ea0] 1-12

  Thread 0x13f0    DispatchQueue "request_queue"(31)    12 samples (1-12)    priority 37 (base 37)
  <process frontmost, thread QoS user initiated (requested user initiated, workloop servicer override user initiated), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33171) [0x7ff81362a193] 1-12
              12  ??? (libdispatch.dylib + 109013) [0x7ff81363c9d5] 1-12
                12  ??? (libdispatch.dylib + 33171) [0x7ff81362a193] 1-12
                  12  ??? (libdispatch.dylib + 106088) [0x7ff81363be68] 1-12
                    12  ??? (libdispatch.dylib + 96789) [0x7ff813639a15] 1-12
                      12  ??? (libxpc.dylib + 47867) [0x7ff8134cdafb] 1-12
                        12  ??? (libxpc.dylib + 53010) [0x7ff8134cef12] 1-12
                          12  ??? (SecurityAgent + 10622) [0x107a1397e] 1-12
                            12  ??? (SecurityAgent + 18532) [0x107a15864] 1-12
                              12  ??? (libdispatch.dylib + 59022) [0x7ff81363068e] 1-12
                                12  ??? (libdispatch.dylib + 60002) [0x7ff813630a62] 1-12
                                  12  ??? (libdispatch.dylib + 11724) [0x7ff813624dcc] 1-12
                                    12  ??? (libsystem_kernel.dylib + 8982) [0x7ff81378e316] 1-12
                                     *12  ??? (kernel + 6667936) [0xffffff8000943ea0] 1-12

  Thread 0x1367    Thread name "com.apple.NSEventThread"    12 samples (1-12)    priority 47 (base 47)
  <process frontmost, thread QoS user interactive (requested user interactive), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (AppKit + 1657343) [0x7ff8174729ff] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x1374    Thread name "Securi:traceq0"    12 samples (1-12)    priority 31 (base 31)
  <process frontmost, thread QoS default (requested default), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (<0C7F0516-5174-31B2-BBB8-A0EBE0757A0C> + 4376249) [0x10d6686b9] 1-12
        12  ??? (<0C7F0516-5174-31B2-BBB8-A0EBE0757A0C> + 4096783) [0x10d62430f] 1-12
          12  ??? (<0C7F0516-5174-31B2-BBB8-A0EBE0757A0C> + 4375673) [0x10d668479] 1-12
            12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
             *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Thread 0x1456    Thread name "CVDisplayLink"    12 samples (1-12)    priority 54 (base 54)    cpu time 0.010s
  <process frontmost, process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      1   ??? (CoreVideo + 7885) [0x7ff81d2e5ecd] 1
        1   ??? (CoreVideo + 9523) [0x7ff81d2e6533] 1
          1   ??? (CoreVideo + 9662) [0x7ff81d2e65be] 1
           *1   ??? (kernel + 522906) [0xffffff8000367a9a] 1
             *1   ??? (kernel + 2422049) [0xffffff8000537521] 1
               *1   ??? (kernel + 2464698) [0xffffff8000541bba] 1
                 *1   ??? (kernel + 2425475) [0xffffff8000538283] (running) 1
      11  ??? (CoreVideo + 7840) [0x7ff81d2e5ea0] 2-12
        11  ??? (CoreVideo + 12041) [0x7ff81d2e6f09] 2-12
          11  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 2-12
           *11  ??? (pthread + 21156) [0xffffff80032a22a4] 2-12

  Thread 0x145c    Thread name "CVDisplayLink"    12 samples (1-12)    priority 54 (base 54)
  <process frontmost, process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (CoreVideo + 8066) [0x7ff81d2e5f82] 1-12
        12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
         *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Binary Images:
           0x107a11000 -                ???  com.apple.SecurityAgent 9.0 (55579.140.3.0.1) <63BA54DA-5A4E-3127-8567-84735DFFB20F>  /System/Library/Frameworks/Security.framework/Versions/A/MachServices/SecurityAgent.bundle/Contents/MacOS/SecurityAgent
           0x10aec7000 -                ???  ???                                           <ED7B907E-6761-3754-B3C0-98336549A3BC>
           0x10d23c000 -                ???  ???                                           <0C7F0516-5174-31B2-BBB8-A0EBE0757A0C>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                          <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff8134c2000 -     0x7ff813508bd5  libxpc.dylib                                  <A1CBBDA0-6389-3803-91FB-D18379D62FEB>  /usr/lib/system/libxpc.dylib
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                             <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                        <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib                       <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                                <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8172de000 -     0x7ff81882ec13  AppKit                                        <BDAACBEF-8C1C-3716-9E23-FB955BFFEA2A>  /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
        0x7ff81cac5000 -     0x7ff81ce7211d  QuartzCore                                    <3287B1DC-EFED-3202-8C23-1BF41AE6EA6A>  /System/Library/Frameworks/QuartzCore.framework/Versions/A/QuartzCore
        0x7ff81d2e4000 -     0x7ff81d337c73  CoreVideo                                     <C9F42E34-65A0-3E32-AB97-DCAB72812FB8>  /System/Library/Frameworks/CoreVideo.framework/Versions/A/CoreVideo
        0x7ff81f301000 -     0x7ff81f5e26ed  HIToolbox                                     <23B7558D-4EF9-3BA5-99FC-7C09357E5E5B>  /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/HIToolbox
        0x7ff91e907000 -     0x7ff91ef992c9  SceneKit                                      <02F35587-57A1-356E-A9B2-F106B290112A>  /System/Library/Frameworks/SceneKit.framework/Versions/A/SceneKit
        0x7ffb26d01000 -     0x7ffb26dc0285  AvatarKit                                     <94504D0F-C138-3D29-99D8-C6271C9B3735>  /System/Library/PrivateFrameworks/AvatarKit.framework/Versions/A/AvatarKit
        0x7ffd132c5000 -     0x7ffd132d56ec  TextInputUIMacHelper                          <7F937C49-1F06-38AB-9701-D447D03C9F84>  /System/Library/PrivateFrameworks/TextInputUIMacHelper.framework/Versions/A/TextInputUIMacHelper
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                               <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1)                     <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread


Process:          SecurityAgent [647]
UUID:             63BA54DA-5A4E-3127-8567-84735DFFB20F
Path:             /System/Library/Frameworks/Security.framework/Versions/A/MachServices/SecurityAgent.bundle/Contents/MacOS/SecurityAgent
Identifier:       com.apple.SecurityAgent
Version:          9.0 (55579.140.3.0.1)
Build Version:    90
Project Name:     SecurityAgent
Source Version:   55579140003000001
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        5860 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      4
Note:             1 idle work queue thread omitted

  Thread 0x1446    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 47 (base 47)
  <process frontmost, thread QoS user interactive (requested user interactive), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (SecurityAgent + 128792) [0x1058ba718] 1-12
      12  ??? (AppKit + 16389) [0x7ff8172e2005] 1-12
        12  ??? (AppKit + 200361) [0x7ff81730eea9] 1-12
          12  ??? (AppKit + 11328535) [0x7ff817dabc17] 1-12
            12  ??? (AppKit + 261569) [0x7ff81731ddc1] 1-12
              12  ??? (HIToolbox + 2310554) [0x7ff81f53519a] 1-12
                12  ??? (HIToolbox + 700311) [0x7ff81f3abf97] 1-12
                  12  ??? (HIToolbox + 688340) [0x7ff81f3a90d4] 1-12
                    12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                      12  ??? (CoreFoundation + 504628) [0x7ff8138b8334] 1-12
                        12  ??? (CoreFoundation + 755830) [0x7ff8138f5876] 1-12
                          12  ??? (libdispatch.dylib + 57055) [0x7ff81362fedf] 1-12
                            12  ??? (libdispatch.dylib + 57190) [0x7ff81362ff66] 1-12
                              12  ??? (libdispatch.dylib + 204688) [0x7ff813653f90] 1-12
                                12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                  12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                                    12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 113519) [0x108d6cb6f] 1-12
                                      12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 114151) [0x108d6cde7] 1-12
                                        12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 75345) [0x108d63651] 1-12
                                          12  ??? (<ED7B907E-6761-3754-B3C0-98336549A3BC> + 75586) [0x108d63742] 1-12
                                            12  ??? (LoginUIKit + 74386) [0x7ff831d95292] 1-12
                                              12  ??? (LoginUIKit + 59340) [0x7ff831d917cc] 1-12
                                                12  ??? (LoginUIKit + 327826) [0x7ff831dd3092] 1-12
                                                  12  ??? (LoginUIKit + 319588) [0x7ff831dd1064] 1-12
                                                    12  ??? (LoginUIKit + 515624) [0x7ff831e00e28] 1-12
                                                      12  ??? (LoginUIKit + 515467) [0x7ff831e00d8b] 1-12
                                                        12  ??? (AppKit + 341854) [0x7ff81733175e] 1-12
                                                          12  ??? (AppKit + 343076) [0x7ff817331c24] 1-12
                                                            12  ??? (AppKit + 345305) [0x7ff8173324d9] 1-12
                                                              12  ??? (AppKit + 1403699) [0x7ff817434b33] 1-12
                                                                12  ??? (AppKit + 13763823) [0x7ff817ffe4ef] 1-12
                                                                  12  ??? (AppKit + 350351) [0x7ff81733388f] 1-12
                                                                    12  ??? (libdispatch.dylib + 14573) [0x7ff8136258ed] 1-12
                                                                      12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                                                                        12  ??? (AppKit + 1407948) [0x7ff817435bcc] 1-12
                                                                          12  ??? (SkyLight + 2993752) [0x7ff819717e58] 1-12
                                                                            12  ??? (SkyLight + 55264) [0x7ff81944a7e0] 1-12
                                                                              12  ??? (SkyLight + 17020) [0x7ff81944127c] 1-12
                                                                                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                                                                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                                                                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                                                                     *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile waiting for WindowServer [635] after 2 hops) 1-12

  Thread 0x1454    Thread name "com.apple.NSEventThread"    12 samples (1-12)    priority 47 (base 47)
  <process frontmost, thread QoS user interactive (requested user interactive), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (AppKit + 1657343) [0x7ff8174729ff] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x145e    Thread name "Securi:traceq0"    12 samples (1-12)    priority 31 (base 31)
  <process frontmost, thread QoS default (requested default), process unclamped, process received importance donation from distnoted [147], process received importance donation from authd [204], IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (<0C7F0516-5174-31B2-BBB8-A0EBE0757A0C> + 4376249) [0x10b4f26b9] 1-12
        12  ??? (<0C7F0516-5174-31B2-BBB8-A0EBE0757A0C> + 4096783) [0x10b4ae30f] 1-12
          12  ??? (<0C7F0516-5174-31B2-BBB8-A0EBE0757A0C> + 4375673) [0x10b4f2479] 1-12
            12  ??? (libsystem_kernel.dylib + 14070) [0x7ff81378f6f6] 1-12
             *12  ??? (pthread + 21156) [0xffffff80032a22a4] 1-12

  Binary Images:
           0x10589b000 -                ???  com.apple.SecurityAgent 9.0 (55579.140.3.0.1) <63BA54DA-5A4E-3127-8567-84735DFFB20F>  /System/Library/Frameworks/Security.framework/Versions/A/MachServices/SecurityAgent.bundle/Contents/MacOS/SecurityAgent
           0x108d51000 -                ???  ???                                           <ED7B907E-6761-3754-B3C0-98336549A3BC>
           0x10b0c6000 -                ???  ???                                           <0C7F0516-5174-31B2-BBB8-A0EBE0757A0C>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                          <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                             <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                        <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib                       <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                                <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8172de000 -     0x7ff81882ec13  AppKit                                        <BDAACBEF-8C1C-3716-9E23-FB955BFFEA2A>  /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
        0x7ff81943d000 -     0x7ff81990bf2f  SkyLight                                      <14C81C57-7629-32F2-9E8C-AF32EF962700>  /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/SkyLight
        0x7ff81f301000 -     0x7ff81f5e26ed  HIToolbox                                     <23B7558D-4EF9-3BA5-99FC-7C09357E5E5B>  /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/HIToolbox
        0x7ff831d83000 -     0x7ff831e57258  LoginUIKit                                    <FA061498-EF19-38AF-A29E-6F10C9F4F22A>  /System/Library/PrivateFrameworks/LoginUIKit.framework/Versions/A/LoginUIKit
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                               <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1)                     <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread


Process:          securityd [139]
UUID:             FF5A93E9-DE68-3ADF-8E48-83ED8F981740
Path:             /usr/sbin/securityd
Codesigning ID:   com.apple.securityd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2604 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      6
Note:             1 idle work queue thread omitted

  Thread 0x334    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (securityd + 89911) [0x10f158f37] 1-12
      12  ??? (securityd + 21349) [0x10f148365] 1-12
        12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
          12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
           *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x399    Thread name "AuditMonitor"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (securityd + 19539) [0x10f147c53] 1-12
        12  ??? (securityd + 283122) [0x10f1881f2] 1-12
          12  ??? (libbsm.0.dylib + 2455) [0x7ff820ffd997] 1-12
            12  ??? (libsystem_c.dylib + 199075) [0x7ff81369d9a3] 1-12
              12  ??? (libsystem_c.dylib + 165269) [0x7ff813695595] 1-12
                12  ??? (libsystem_c.dylib + 55072) [0x7ff81367a720] 1-12
                  12  ??? (libsystem_kernel.dylib + 9298) [0x7ff81378e452] 1-12
                   *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
                     *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
                       *12  ??? (kernel + 6631249) [0xffffff800093af51] 1-12
                         *12  ??? (kernel + 6631571) [0xffffff800093b093] 1-12
                           *12  ??? (kernel + 3182633) [0xffffff80005f1029] 1-12
                             *12  ??? (kernel + 3187280) [0xffffff80005f2250] 1-12
                               *12  ??? (kernel + 3304088) [0xffffff800060ea98] 1-12
                                 *12  ??? (kernel + 6009378) [0xffffff80008a3222] 1-12
                                   *12  ??? (kernel + 6476076) [0xffffff800091512c] 1-12
                                     *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
                                       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                                         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                                           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x469    Thread name "MachServer"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (securityd + 19539) [0x10f147c53] 1-12
        12  ??? (securityd + 348494) [0x10f19814e] 1-12
          12  ??? (securityd + 21349) [0x10f148365] 1-12
            12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
              12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
               *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x46a    Thread name "MachServer"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (securityd + 19539) [0x10f147c53] 1-12
        12  ??? (securityd + 348494) [0x10f19814e] 1-12
          12  ??? (securityd + 21349) [0x10f148365] 1-12
            12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
              12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
               *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x46b    Thread name "MachServer"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (securityd + 19539) [0x10f147c53] 1-12
        12  ??? (securityd + 348494) [0x10f19814e] 1-12
          12  ??? (securityd + 21349) [0x10f148365] 1-12
            12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
              12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
               *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10f143000 -                ???  securityd               <FF5A93E9-DE68-3ADF-8E48-83ED8F981740>  /usr/sbin/securityd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81366d000 -     0x7ff8136f5a07  libsystem_c.dylib       <6B1481E5-A9C4-3315-8ADE-BE0EDA06B6EC>  /usr/lib/system/libsystem_c.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff820ffd000 -     0x7ff82100c89f  libbsm.0.dylib          <4A087E19-2A31-3516-A03B-7E17F9317FC4>  /usr/lib/libbsm.0.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          securityd_system [251]
UUID:             7744E8FE-090A-3F07-9188-A3F54ABB3653
Path:             /usr/libexec/securityd_system
Codesigning ID:   com.apple.securityd_system
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1848 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x665    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (securityd_system + 86791) [0x1040ff307] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1040ea000 -                ???  securityd_system       <7744E8FE-090A-3F07-9188-A3F54ABB3653>  /usr/libexec/securityd_system
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          SecuritySubscriber [650]
UUID:             82520552-EAFC-3BC5-8429-C5884AB73557
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/SecuritySubscriber.xpc/Contents/MacOS/SecuritySubscriber
Identifier:       com.apple.remotemanagement.SecuritySubscriber
Version:          ??? (2.0)
Build Version:    293
Project Name:     RemoteManagementSubscribers
Source Version:   560006001000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3800 KB
Time Since Fork:  52s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x149c    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10c90f000 -                ???  com.apple.remotemanagement.SecuritySubscriber (2.0) <82520552-EAFC-3BC5-8429-C5884AB73557>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/SecuritySubscriber.xpc/Contents/MacOS/SecuritySubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                   <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                              <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                     <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          sharedfilelistd [407]
UUID:             A1218504-C6B3-3752-A0E4-C0057969A73D
Path:             /System/Library/CoreServices/sharedfilelistd
Codesigning ID:   com.apple.sharedfilelistd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1228 KB
Time Since Fork:  168s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xa66    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1045ab000 -                ???  sharedfilelistd        <A1218504-C6B3-3752-A0E4-C0057969A73D>  /System/Library/CoreServices/sharedfilelistd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          smd [94]
UUID:             BA6AB3CC-B866-339E-A81D-FFE5B4695969
Path:             /usr/libexec/smd
Codesigning ID:   com.apple.xpc.smd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1140 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x6db    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10c1bd000 -                ???  smd                    <BA6AB3CC-B866-339E-A81D-FFE5B4695969>  /usr/libexec/smd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          socketfilterfw [211]
UUID:             CC2ABDDA-E43F-3662-BB73-8F173132C138
Path:             /usr/libexec/ApplicationFirewall/socketfilterfw
Identifier:       com.apple.ALF.ApplicationFirewall
Version:          1.0 (1)
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1292 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4fb    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (socketfilterfw + 27731) [0x1060aec53] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1060a8000 -                ???  com.apple.ALF.ApplicationFirewall 1.0 (1) <CC2ABDDA-E43F-3662-BB73-8F173132C138>  /usr/libexec/ApplicationFirewall/socketfilterfw
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                      <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                    <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                            <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                           <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          softwareupdated [115]
UUID:             384FEF78-D809-3C6A-8912-59E0599B97FD
Path:             /System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/Support/softwareupdated
Codesigning ID:   com.apple.mobile.softwareupdated
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2096 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3c7    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (softwareupdated + 67902) [0x10aaa993e] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10aa99000 -                ???  softwareupdated        <384FEF78-D809-3C6A-8912-59E0599B97FD>  /System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/Support/softwareupdated
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          softwareupdated [249]
UUID:             6D9716D4-A5A0-3D1F-8493-901139907735
Path:             /System/Library/CoreServices/Software Update.app/Contents/Resources/softwareupdated
Codesigning ID:   com.apple.softwareupdated
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4716 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x64d    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (softwareupdated + 3803) [0x10c81eedb] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10c81e000 -                ???  softwareupdated        <6D9716D4-A5A0-3D1F-8493-901139907735>  /System/Library/CoreServices/Software Update.app/Contents/Resources/softwareupdated
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          SoftwareUpdateSubscriber [659]
UUID:             C3214017-E8CD-3757-B1D3-0373AF00B7B6
Path:             /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/SoftwareUpdateSubscriber.xpc/Contents/MacOS/SoftwareUpdateSubscriber
Identifier:       com.apple.remotemanagement.SoftwareUpdateSubscriber
Version:          1.0 (1)
Build Version:    2
Project Name:     MobileSoftwareUpdate_SUCore
Source Version:   2171140016702001
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        4252 KB
Time Since Fork:  51s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x14ce    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x107c7f000 -                ???  com.apple.remotemanagement.SoftwareUpdateSubscriber 1.0 (1) <C3214017-E8CD-3757-B1D3-0373AF00B7B6>  /System/Library/PrivateFrameworks/RemoteManagement.framework/XPCServices/SoftwareUpdateSubscriber.xpc/Contents/MacOS/SoftwareUpdateSubscriber
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                           <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                                      <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                             <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          spindump [470]
UUID:             95C6268E-7342-3534-B009-DFF64688CCC3
Path:             /usr/sbin/spindump
Codesigning ID:   com.apple.spindump
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1188 KB
Time Since Fork:  148s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xc5f    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x107829000 -                ???  spindump               <95C6268E-7342-3534-B009-DFF64688CCC3>  /usr/sbin/spindump
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          storagekitd [217]
UUID:             0222917B-8CDD-3614-B7D0-5362FC878E00
Path:             /usr/libexec/storagekitd
Codesigning ID:   com.apple.storagekitd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2548 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      8
Note:             1 idle work queue thread omitted

  Thread 0x549    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (storagekitd + 333613) [0x10daf772d] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x570    Thread name "Storage Kit Work Thread"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 347481) [0x7ff8149f6d59] 1-12
        12  ??? (storagekitd + 357726) [0x10dafd55e] 1-12
          12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
            12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
              12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                  12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                    12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                      12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                        12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                         *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x571    Thread name "Storage Kit Disk Eject Thread"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 347481) [0x7ff8149f6d59] 1-12
        12  ??? (storagekitd + 357959) [0x10dafd647] 1-12
          12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
            12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
              12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                  12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                    12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                      12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                        12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                         *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x580    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 347481) [0x7ff8149f6d59] 1-12
        12  ??? (storagekitd + 1297925) [0x10dbe2e05] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x594    Thread name "Storage Kit Legacy Notifications Thread"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 9881907) [0x7ff81530e933] 1-12
        12  ??? (storagekitd + 357425) [0x10dafd431] 1-12
          12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
            12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
              12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                  12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                    12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                      12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                        12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                         *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x599    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 347481) [0x7ff8149f6d59] 1-12
        12  ??? (storagekitd + 1297925) [0x10dbe2e05] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x1375    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (Foundation + 347481) [0x7ff8149f6d59] 1-12
        12  ??? (storagekitd + 1297925) [0x10dbe2e05] 1-12
          12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10daa6000 -                ???  storagekitd             <0222917B-8CDD-3614-B7D0-5362FC878E00>  /usr/libexec/storagekitd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          SubmitDiagInfo [472]
UUID:             96051FF6-4EE7-3DEB-B34E-86D910763F6F
Path:             /System/Library/CoreServices/SubmitDiagInfo
Codesigning ID:   com.apple.SubmitDiagInfo
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1012 KB
Time Since Fork:  148s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xc63    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 3>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (SubmitDiagInfo + 8973) [0x1091e630d] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1091e4000 -                ???  SubmitDiagInfo         <96051FF6-4EE7-3DEB-B34E-86D910763F6F>  /System/Library/CoreServices/SubmitDiagInfo
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          suhelperd [280]
UUID:             B028760D-11B2-3C85-BEAC-B41601A95C3C
Path:             /System/Library/CoreServices/Software Update.app/Contents/Resources/suhelperd
Codesigning ID:   com.apple.suhelperd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1156 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x745    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10bbd3000 -                ???  suhelperd              <B028760D-11B2-3C85-BEAC-B41601A95C3C>  /System/Library/CoreServices/Software Update.app/Contents/Resources/suhelperd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          symptomsd [185]
UUID:             9C186D98-C268-3722-8D55-878568F5F653
Path:             /usr/libexec/symptomsd
Codesigning ID:   com.apple.symptomsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1984 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x46e    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (symptomsd + 2416) [0x10e9c3970] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10e9c3000 -                ???  symptomsd              <9C186D98-C268-3722-8D55-878568F5F653>  /usr/libexec/symptomsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          symptomsd-diag [406]
UUID:             96DB5614-6967-37EC-8E3A-2C89B0379BF4
Path:             /usr/libexec/symptomsd-diag
Codesigning ID:   com.apple.symptomsd-diag
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1932 KB
Time Since Fork:  168s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xa25    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (symptomsd-diag + 3353) [0x10323ad19] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10323a000 -                ???  symptomsd-diag         <96DB5614-6967-37EC-8E3A-2C89B0379BF4>  /usr/libexec/symptomsd-diag
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          sysextd [216]
UUID:             95E0676A-5400-373B-8807-ECEBAEB5E395
Path:             /System/Library/Frameworks/SystemExtensions.framework/Versions/A/Helpers/sysextd
Codesigning ID:   com.apple.sysextd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1612 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5b4    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x100bac000 -                ???  sysextd                <95E0676A-5400-373B-8807-ECEBAEB5E395>  /System/Library/Frameworks/SystemExtensions.framework/Versions/A/Helpers/sysextd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          syslogd [129]
UUID:             5544D068-FA57-3FA9-8652-79BFFF8B4088
Path:             /usr/sbin/syslogd
Codesigning ID:   com.apple.syslogd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        900 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x364    DispatchQueue "com.apple.root.default-qos"(13)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2 and passive>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10272) [0x7ff8137cb820] 1-12
      12  ??? (libdispatch.dylib + 69397) [0x7ff813632f15] 1-12
        12  ??? (libdispatch.dylib + 68246) [0x7ff813632a96] 1-12
          12  ??? (libdispatch.dylib + 206203) [0x7ff81365457b] 1-12
            12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
              12  ??? (libdispatch.dylib + 5817) [0x7ff8136236b9] 1-12
                12  ??? (syslogd + 30696) [0x1044127e8] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x365    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2 and passive>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10440b000 -                ???  syslogd                 <5544D068-FA57-3FA9-8652-79BFFF8B4088>  /usr/sbin/syslogd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          sysmond [395]
UUID:             D294C1E8-8E3D-37E2-A9AC-B1077DCA1DBB
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        856 KB
Time Since Fork:  173s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x9e1    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10e856000 -                ???  ???                    <D294C1E8-8E3D-37E2-A9AC-B1077DCA1DBB>
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          syspolicyd [269]
UUID:             7241A43C-556A-3E98-9DA8-B773CEBEBC05
Path:             /usr/libexec/syspolicyd
Codesigning ID:   com.apple.syspolicyd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        31.91 MB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x6ca    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (syspolicyd + 436880) [0x10530fa90] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1052a5000 -                ???  syspolicyd             <7241A43C-556A-3E98-9DA8-B773CEBEBC05>  /usr/libexec/syspolicyd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          systemsoundserverd [448]
UUID:             713F10F2-F6FF-3C40-8788-11FEEFDC5DE6
Path:             /usr/sbin/systemsoundserverd
Codesigning ID:   com.apple.systemsoundserverd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        980 KB
Time Since Fork:  157s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0xb84    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1052c4000 -                ???  systemsoundserverd     <713F10F2-F6FF-3C40-8788-11FEEFDC5DE6>  /usr/sbin/systemsoundserverd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          systemstats [102]
UUID:             E9F6C3A2-BD32-3815-A339-4E1F10C8E14B
Path:             /usr/sbin/systemstats
Codesigning ID:   com.apple.systemstats
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2000 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x344    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (systemstats + 61264) [0x10e532f50] 1-12
      12  ??? (systemstats + 97020) [0x10e53bafc] 1-12
        12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
          12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10e524000 -                ???  systemstats            <E9F6C3A2-BD32-3815-A339-4E1F10C8E14B>  /usr/sbin/systemstats
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          systemstats [396]
UUID:             E9F6C3A2-BD32-3815-A339-4E1F10C8E14B
Path:             /usr/sbin/systemstats
Codesigning ID:   com.apple.systemstats
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1728 KB
Time Since Fork:  173s
Num samples:      12 (1-12)
Num threads:      4
Note:             1 idle work queue thread omitted

  Thread 0x9e4    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (systemstats + 61264) [0x10943bf50] 1-12
      12  ??? (systemstats + 97157) [0x109444b85] 1-12
        12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
          12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
            12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
              12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                       *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x9e6    Thread name "caulk.messenger.shared:17"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested utility), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Thread 0x9e7    Thread name "caulk.messenger.shared:high"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background, thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (caulk + 5800) [0x7ff81eec36a8] 1-12
        12  ??? (caulk + 6500) [0x7ff81eec3964] 1-12
          12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
           *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x10942d000 -                ???  systemstats             <E9F6C3A2-BD32-3815-A339-4E1F10C8E14B>  /usr/sbin/systemstats
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
        0x7ff81eec2000 -     0x7ff81eee5e17  caulk                   <684B920A-BD31-31C3-A5AA-B63B6654CDA3>  /System/Library/PrivateFrameworks/caulk.framework/Versions/A/caulk
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          systemstatusd [212]
UUID:             849C936C-F6F3-3A61-9429-D75C91AE0499
Path:             /System/Library/PrivateFrameworks/SystemStatusServer.framework/Support/systemstatusd
Codesigning ID:   com.apple.systemstatusd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1572 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x526    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (systemstatusd + 6344) [0x10e77a8c8] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10e779000 -                ???  systemstatusd          <849C936C-F6F3-3A61-9429-D75C91AE0499>  /System/Library/PrivateFrameworks/SystemStatusServer.framework/Support/systemstatusd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          tccd [165]
UUID:             26047FFA-3CF7-3EB7-8CA7-3E00C0A6885A
Path:             /System/Library/PrivateFrameworks/TCC.framework/Support/tccd
Codesigning ID:   com.apple.tccd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2596 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x394    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (tccd + 27308) [0x1024dbaac] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1024d5000 -                ???  tccd                   <26047FFA-3CF7-3EB7-8CA7-3E00C0A6885A>  /System/Library/PrivateFrameworks/TCC.framework/Support/tccd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          thermalmonitord [132]
UUID:             7AA6DC2A-E364-30D1-A8CD-FF1612E12308
Path:             /usr/libexec/thermalmonitord
Codesigning ID:   com.apple.thermalmonitord
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        936 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      1

  Thread 0x32d    DispatchQueue 140704556565304    12 samples (1-12)    priority 37 (base 37)
  <thread QoS user initiated (requested user initiated), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (thermalmonitord + 52383) [0x106057c9f] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10604b000 -                ???  thermalmonitord        <7AA6DC2A-E364-30D1-A8CD-FF1612E12308>  /usr/libexec/thermalmonitord
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          timed [137]
UUID:             12246AD4-219E-3071-8218-EF869EC4CE12
Path:             /usr/libexec/timed
Codesigning ID:   com.apple.timed
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1732 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x3ca    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (timed + 11967) [0x10aba8ebf] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10aba6000 -                ???  timed                  <12246AD4-219E-3071-8218-EF869EC4CE12>  /usr/libexec/timed
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          trustd [188]
UUID:             F37ED7FD-1DF0-3A2E-82AC-22AB85DA6A4C
Path:             /usr/libexec/trustd
Codesigning ID:   com.apple.trustd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3848 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4f2    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10a927000 -                ???  trustd                 <F37ED7FD-1DF0-3A2E-82AC-22AB85DA6A4C>  /usr/libexec/trustd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          trustd [242]
UUID:             F37ED7FD-1DF0-3A2E-82AC-22AB85DA6A4C
Path:             /usr/libexec/trustd
Codesigning ID:   com.apple.trustd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3564 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x62a    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10b57c000 -                ???  trustd                 <F37ED7FD-1DF0-3A2E-82AC-22AB85DA6A4C>  /usr/libexec/trustd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          trustdFileHelper [200]
UUID:             84702AED-D66D-3AD2-A4BB-B486A61AB704
Path:             /usr/libexec/trustdFileHelper
Codesigning ID:   com.apple.trustdFileHelper
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1256 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4cb    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x106664000 -                ???  trustdFileHelper       <84702AED-D66D-3AD2-A4BB-B486A61AB704>  /usr/libexec/trustdFileHelper
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          universalaccessd [643]
UUID:             A64A1456-445D-3498-97AA-53C8CDAE15D0
Path:             /usr/sbin/universalaccessd
Codesigning ID:   com.apple.universalaccessd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3080 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x141c    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested user interactive), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (universalaccessd + 4252) [0x10e06309c] 1-12
      12  ??? (UniversalAccessCore + 37381) [0x7ff93030d205] 1-12
        12  ??? (libUADaemon.dylib + 4250) [0x7ffa1535c09a] 1-12
          12  ??? (libUADaemon.dylib + 4830) [0x7ffa1535c2de] 1-12
            12  ??? (libUADaemon.dylib + 6685) [0x7ffa1535ca1d] 1-12
              12  ??? (SkyLight + 950701) [0x7ff8195251ad] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile waiting for WindowServer [635] after 2 hops) 1-12

  Binary Images:
           0x10e062000 -                ???  universalaccessd       <A64A1456-445D-3498-97AA-53C8CDAE15D0>  /usr/sbin/universalaccessd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81943d000 -     0x7ff81990bf2f  SkyLight               <14C81C57-7629-32F2-9E8C-AF32EF962700>  /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/SkyLight
        0x7ff930304000 -     0x7ff93033eaae  UniversalAccessCore    <7327E25C-995C-38A8-B4BA-8749052E218D>  /System/Library/PrivateFrameworks/UniversalAccess.framework/Versions/A/Frameworks/UniversalAccessCore.framework/Versions/A/UniversalAccessCore
        0x7ffa1535b000 -     0x7ffa153635e0  libUADaemon.dylib      <C83AB29C-347B-3A7E-8C43-D5DDF9BDA068>  /System/Library/PrivateFrameworks/UniversalAccess.framework/Versions/A/Frameworks/UAEHCommon.framework/Versions/A/Libraries/libUADaemon.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          usbd [203]
UUID:             AB13DC51-D7F8-376A-9CE9-DC89B4F73670
Path:             /usr/libexec/usbd
Codesigning ID:   com.apple.usbd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1176 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x4c8    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (usbd + 3206) [0x1089e8c86] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1089e8000 -                ???  usbd                   <AB13DC51-D7F8-376A-9CE9-DC89B4F73670>  /usr/libexec/usbd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          usbmuxd [138]
UUID:             6CE0D4D1-FB2B-3E86-BA9A-D198E1AF144D
Path:             /Library/Apple/*/MobileDevice.framework/Versions/A/Resources/usbmuxd
Codesigning ID:   com.apple.usbmuxd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2036 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x3c5    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (usbmuxd + 5262) [0x10669648e] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x471    Thread name "com.apple.CFSocket.private"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 39422) [0x7ff8137959fe] 1-12
       *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x106695000 -                ???  usbmuxd                 <6CE0D4D1-FB2B-3E86-BA9A-D198E1AF144D>  /Library/Apple/*/MobileDevice.framework/Versions/A/Resources/usbmuxd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          UserEventAgent [95]
UUID:             6A3B82AE-D64E-353A-A88C-98C5FFEE564A
Path:             /usr/libexec/UserEventAgent
Codesigning ID:   com.apple.UserEventAgent
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2968 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.002s
Num threads:      5
Note:             2 idle work queue threads omitted

  Thread 0x319    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (UserEventAgent + 4173) [0x10b0c004d] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x422    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (<71077DC1-AD04-301E-86DC-07AF4EB84556> + 4205) [0x10b17006d] 1-12
        12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x423    Thread name "com.apple.CFSocket.private"    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (libsystem_kernel.dylib + 39422) [0x7ff8137959fe] 1-12
       *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10b0bf000 -                ???  UserEventAgent          <6A3B82AE-D64E-353A-A88C-98C5FFEE564A>  /usr/libexec/UserEventAgent
           0x10b16f000 -                ???  ???                     <71077DC1-AD04-301E-86DC-07AF4EB84556>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation              <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          UserEventAgent [642]
UUID:             6A3B82AE-D64E-353A-A88C-98C5FFEE564A
Path:             /usr/libexec/UserEventAgent
Codesigning ID:   com.apple.UserEventAgent
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1684 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x141e    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested background), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (UserEventAgent + 4173) [0x109c8c04d] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 504628) [0x7ff8138b8334] 1-12
              12  ??? (CoreFoundation + 755830) [0x7ff8138f5876] 1-12
                12  ??? (libdispatch.dylib + 57055) [0x7ff81362fedf] 1-12
                  12  ??? (libdispatch.dylib + 57190) [0x7ff81362ff66] 1-12
                    12  ??? (libdispatch.dylib + 204447) [0x7ff813653e9f] 1-12
                      12  ??? (libdispatch.dylib + 83962) [0x7ff8136367fa] 1-12
                        12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                          12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                            12  ??? (<0A7E41B4-5E3D-3FC1-B797-9A7A420BFCD5> + 4599) [0x109e021f7] 1-12
                              12  ??? (IOBluetooth + 24942) [0x7ff82780216e] 1-12
                                12  ??? (IOKit + 28156) [0x7ff8171b3dfc] 1-12
                                  12  ??? (IOKit + 29508) [0x7ff8171b4344] 1-12
                                    12  ??? (IOKit + 31510) [0x7ff8171b4b16] 1-12
                                      12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                        12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                          12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                           *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile waiting for launchd [1] send port name "com.apple.iohideventsystem" in domain 1 after 2 hops) 1-12

  Binary Images:
           0x109c8b000 -                ???  UserEventAgent         <6A3B82AE-D64E-353A-A88C-98C5FFEE564A>  /usr/libexec/UserEventAgent
           0x109e01000 -                ???  ???                    <0A7E41B4-5E3D-3FC1-B797-9A7A420BFCD5>
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
        0x7ff8171ad000 -     0x7ff81727214e  IOKit                  <C1981AE3-07A7-3982-AC88-F192A4ECF545>  /System/Library/Frameworks/IOKit.framework/Versions/A/IOKit
        0x7ff8277fc000 -     0x7ff8278b25f3  IOBluetooth            <24012E3C-1631-35B4-A32D-312CBAD474A3>  /System/Library/Frameworks/IOBluetooth.framework/Versions/A/IOBluetooth
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          usermanagerd [154]
UUID:             9F7532BD-D02D-3E92-B69D-1AE5E713D781
Path:             /usr/libexec/usermanagerd
Codesigning ID:   com.apple.usermanagerd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1192 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x354    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (usermanagerd + 109267) [0x102605ad3] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x1025eb000 -                ???  usermanagerd           <9F7532BD-D02D-3E92-B69D-1AE5E713D781>  /usr/libexec/usermanagerd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          UVCAssistant [253]
UUID:             18A2BFFB-E744-3AFA-BE55-7F04A42B53F8
Path:             /System/Library/Frameworks/CoreMediaIO.framework/Versions/A/Resources/UVCAssistant.systemextension/Contents/MacOS/UVCAssistant
Identifier:       com.apple.cmio.uvcassistantextension
Version:          1.0 (1)
Build Version:    3966
Project Name:     AppleUSBCamera
Source Version:   466080002000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1512 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x669    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (UVCAssistant + 2400) [0x107d6a960] 1-12
      12  ??? (UVCExtension + 38868) [0x7ffd16d527d4] 1-12
        12  ??? (UVCExtension + 36740) [0x7ffd16d51f84] 1-12
          12  ??? (UVCExtension + 35790) [0x7ffd16d51bce] 1-12
            12  ??? (libdispatch.dylib + 12486) [0x7ff8136250c6] 1-12
              12  ??? (libsystem_kernel.dylib + 2758) [0x7ff81378cac6] 1-12
               *12  ??? (kernel + 1237184) [0xffffff80004160c0] 1-12

  Binary Images:
           0x107d6a000 -                ???  com.apple.cmio.uvcassistantextension 1.0 (1) <18A2BFFB-E744-3AFA-BE55-7F04A42B53F8>  /System/Library/Frameworks/CoreMediaIO.framework/Versions/A/Resources/UVCAssistant.systemextension/Contents/MacOS/UVCAssistant
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                         <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                            <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                       <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ffd16d49000 -     0x7ffd16d81d01  UVCExtension                                 <3983273A-096B-3F34-821C-E654F404BF1E>  /System/Library/PrivateFrameworks/UVCExtension.framework/Versions/A/UVCExtension
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                              <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          VDCAssistant [205]
UUID:             5D31E3E3-2754-3ACD-98DE-059157554FB4
Path:             /System/Library/Frameworks/CoreMediaIO.framework/Versions/A/Resources/VDC.plugin/Contents/Resources/VDCAssistant
Codesigning ID:   com.apple.VDCAssistant
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1780 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0xd8f    DispatchQueue "com.apple.coremediaio.vdcassistant-cgsevent"(24)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default, workloop servicer override default), IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6211) [0x7ff8137ca843] 1-12
    12  ??? (libsystem_pthread.dylib + 10337) [0x7ff8137cb861] 1-12
      12  ??? (libdispatch.dylib + 70883) [0x7ff8136334e3] 1-12
        12  ??? (libdispatch.dylib + 72598) [0x7ff813633b96] 1-12
          12  ??? (libdispatch.dylib + 36269) [0x7ff81362adad] 1-12
            12  ??? (libdispatch.dylib + 33171) [0x7ff81362a193] 1-12
              12  ??? (libdispatch.dylib + 83962) [0x7ff8136367fa] 1-12
                12  ??? (libdispatch.dylib + 20675) [0x7ff8136270c3] 1-12
                  12  ??? (libdispatch.dylib + 96764) [0x7ff8136399fc] 1-12
                    12  ??? (VDCAssistant + 181787) [0x10e65c61b] 1-12
                      12  ??? (VDCAssistant + 181259) [0x10e65c40b] 1-12
                        12  ??? (VDCAssistant + 209106) [0x10e6630d2] 1-12
                          12  ??? (VDCAssistant + 208240) [0x10e662d70] 1-12
                            12  ??? (SkyLight + 3569184) [0x7ff8197a4620] 1-12
                              12  ??? (SkyLight + 3569737) [0x7ff8197a4849] 1-12
                                12  ??? (SkyLight + 12230) [0x7ff81943ffc6] 1-12
                                  12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                                    12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                                      12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                                       *12  ??? (kernel + 821648) [0xffffff80003b0990] (blocked by turnstile waiting for launchd [1] send port name "com.apple.windowserver.active" in domain 1 after 2 hops) 1-12

  Thread 0xbb4    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10e630000 -                ???  VDCAssistant            <5D31E3E3-2754-3ACD-98DE-059157554FB4>  /System/Library/Frameworks/CoreMediaIO.framework/Versions/A/Resources/VDC.plugin/Contents/Resources/VDCAssistant
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib       <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81943d000 -     0x7ff81990bf2f  SkyLight                <14C81C57-7629-32F2-9E8C-AF32EF962700>  /System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/SkyLight
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          warmd [121]
UUID:             AA5FB893-63D6-3800-88E7-252C4595175B
Path:             /usr/libexec/warmd
Codesigning ID:   com.apple.warmd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1200 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x47a    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x102bd3000 -                ???  warmd                  <AA5FB893-63D6-3800-88E7-252C4595175B>  /usr/libexec/warmd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          watchdogd [118]
UUID:             B2F547D0-9ED7-313C-AEAD-8CFF64409925
Path:             /usr/libexec/watchdogd
Codesigning ID:   com.apple.watchdogd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        8944 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
CPU Time:         0.022s
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x325    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (watchdogd + 25060) [0x10309f1e4] 1-12
      12  ??? (CoreFoundation + 987358) [0x7ff81392e0de] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x37f    Thread name "watchdogd service monitoring thread"    12 samples (1-12)    priority 97 (base 97)    cpu time 0.021s
  <IO tier 0>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (watchdogd + 29442) [0x1030a0302] 1-12
        12  ??? (watchdogd + 14722) [0x10309c982] 1-12
          12  ??? (libsystem_kernel.dylib + 47794) [0x7ff813797ab2] 1-12
           *12  ??? (kernel + 523702) [0xffffff8000367db6] 1-12
             *12  ??? (kernel + 7692331) [0xffffff8000a3e02b] 1-12
               *12  ??? (kernel + 6613109) [0xffffff8000936875] 1-12
                 *12  ??? (kernel + 693349) [0xffffff8000391465] 1-12
                   *12  ??? (kernel + 689296) [0xffffff8000390490] 1-12
                     *12  ??? (kernel + 961191) [0xffffff80003d2aa7] (running) 1-12

  Binary Images:
           0x103099000 -                ???  watchdogd               <B2F547D0-9ED7-313C-AEAD-8CFF64409925>  /usr/libexec/watchdogd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                    <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib  <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation          <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)         <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          WiFiAgent [644]
UUID:             6443D9CF-D9EF-30A4-AB23-5E40F8A1C414
Path:             /System/Library/CoreServices/WiFiAgent.app/Contents/MacOS/WiFiAgent
Identifier:       com.apple.wifi.WiFiAgent
Version:          17.0 (1728)
Build Version:    7
Project Name:     AirPort_executables
Source Version:   19075039000000000
Is First Party:   Yes
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        3852 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      3
Note:             1 idle work queue thread omitted

  Thread 0x1423    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (AppKit + 16389) [0x7ff8172e2005] 1-12
      12  ??? (AppKit + 200361) [0x7ff81730eea9] 1-12
        12  ??? (AppKit + 11328535) [0x7ff817dabc17] 1-12
          12  ??? (AppKit + 261569) [0x7ff81731ddc1] 1-12
            12  ??? (HIToolbox + 2310554) [0x7ff81f53519a] 1-12
              12  ??? (HIToolbox + 700311) [0x7ff81f3abf97] 1-12
                12  ??? (HIToolbox + 688340) [0x7ff81f3a90d4] 1-12
                  12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                    12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                      12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                        12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                          12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                            12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                             *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Thread 0x144f    Thread name "com.apple.NSEventThread"    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested user interactive), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libsystem_pthread.dylib + 6231) [0x7ff8137ca857] 1-12
    12  ??? (libsystem_pthread.dylib + 24069) [0x7ff8137cee05] 1-12
      12  ??? (AppKit + 1657343) [0x7ff8174729ff] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10eb25000 -                ???  com.apple.wifi.WiFiAgent 17.0 (1728) <6443D9CF-D9EF-30A4-AB23-5E40F8A1C414>  /System/Library/CoreServices/WiFiAgent.app/Contents/MacOS/WiFiAgent
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                 <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib               <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff8137c9000 -     0x7ff8137d4fd7  libsystem_pthread.dylib              <2410DCD3-4D92-30B2-96EC-4BD6F78139D4>  /usr/lib/system/libsystem_pthread.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                       <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8172de000 -     0x7ff81882ec13  AppKit                               <BDAACBEF-8C1C-3716-9E23-FB955BFFEA2A>  /System/Library/Frameworks/AppKit.framework/Versions/C/AppKit
        0x7ff81f301000 -     0x7ff81f5e26ed  HIToolbox                            <23B7558D-4EF9-3BA5-99FC-7C09357E5E5B>  /System/Library/Frameworks/Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/HIToolbox
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                      <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          wifianalyticsd [259]
UUID:             4299AD85-3F56-3542-8A5D-420233E7081F
Path:             /usr/libexec/wifianalyticsd
Codesigning ID:   com.apple.wifianalyticsd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        2020 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x6a1    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (wifianalyticsd + 661944) [0x107f219b8] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x107e80000 -                ???  wifianalyticsd         <4299AD85-3F56-3542-8A5D-420233E7081F>  /usr/libexec/wifianalyticsd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          WiFiCloudAssetsXPCService [225]
UUID:             3B1B867F-3B8B-3BFB-A3FB-776DC1AE7C9C
Path:             /System/Library/PrivateFrameworks/WiFiPolicy.framework/XPCServices/WiFiCloudAssetsXPCService.xpc/Contents/MacOS/WiFiCloudAssetsXPCService
Identifier:       com.apple.wifi.WiFiCloudAssetsXPCService
Version:          1.0 (1)
Build Version:    197
Project Name:     WiFiPolicy
Source Version:   935001000000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1420 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x5b2    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x1029a8000 -                ???  com.apple.wifi.WiFiCloudAssetsXPCService 1.0 (1) <3B1B867F-3B8B-3BFB-A3FB-776DC1AE7C9C>  /System/Library/PrivateFrameworks/WiFiPolicy.framework/XPCServices/WiFiCloudAssetsXPCService.xpc/Contents/MacOS/WiFiCloudAssetsXPCService
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                           <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                  <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          WiFiCloudAssetsXPCService [648]
UUID:             3B1B867F-3B8B-3BFB-A3FB-776DC1AE7C9C
Path:             /System/Library/PrivateFrameworks/WiFiPolicy.framework/XPCServices/WiFiCloudAssetsXPCService.xpc/Contents/MacOS/WiFiCloudAssetsXPCService
Identifier:       com.apple.wifi.WiFiCloudAssetsXPCService
Version:          1.0 (1)
Build Version:    197
Project Name:     WiFiPolicy
Source Version:   935001000000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1052 KB
Time Since Fork:  68s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x1451    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x10d9b1000 -                ???  com.apple.wifi.WiFiCloudAssetsXPCService 1.0 (1) <3B1B867F-3B8B-3BFB-A3FB-776DC1AE7C9C>  /System/Library/PrivateFrameworks/WiFiPolicy.framework/XPCServices/WiFiCloudAssetsXPCService.xpc/Contents/MacOS/WiFiCloudAssetsXPCService
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib                                <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                           <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                  <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          wifiFirmwareLoader [97]
UUID:             834E84ED-78C8-3234-9FA8-5AD18D1DE194
Path:             /usr/libexec/wifiFirmwareLoader
Codesigning ID:   com.apple.wifiFirmwareLoader
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        932 KB
Time Since Fork:  181s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x31c    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (wifiFirmwareLoader + 32121) [0x10b59cd79] 1-12
      12  ??? (wifiFirmwareLoader + 3435) [0x10b595d6b] 1-12
        12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
          12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
            12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
              12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                  12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                   *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10b595000 -                ???  wifiFirmwareLoader     <834E84ED-78C8-3234-9FA8-5AD18D1DE194>  /usr/libexec/wifiFirmwareLoader
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          wifip2pd [230]
UUID:             F76C0297-E185-3F7A-88E6-48EFB9A9F3FB
Path:             /usr/libexec/wifip2pd
Codesigning ID:   com.apple.wifip2pd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1200 KB
Time Since Fork:  180s
Num samples:      12 (1-12)
CPU Time:         <0.001s
Num threads:      3
Note:             2 idle work queue threads omitted

  Thread 0x5e0    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (libdispatch.dylib + 73080) [0x7ff813633d78] 1-12
    12  ??? (libsystem_kernel.dylib + 26906) [0x7ff81379291a] 1-12
     *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Binary Images:
           0x100afe000 -                ???  wifip2pd               <F76C0297-E185-3F7A-88E6-48EFB9A9F3FB>  /usr/libexec/wifip2pd
        0x7ff813622000 -     0x7ff813669339  libdispatch.dylib      <9BC18302-2A6A-38A7-8A59-F571D6721BDC>  /usr/lib/system/libdispatch.dylib
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          WirelessRadioManagerd [357]
UUID:             488A6FF1-2F2D-33C5-ACDE-6D72D1AEC6D5
Path:             /usr/sbin/WirelessRadioManagerd
Codesigning ID:   com.apple.WirelessRadioManagerd
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1820 KB
Time Since Fork:  176s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x92f    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 31 (base 31)
  <thread QoS default (requested default), IO tier 0>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (WirelessRadioManagerd + 220538) [0x10b015d7a] 1-12
      12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
        12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
          12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
            12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
              12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                  12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                    12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                     *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10afe0000 -                ???  WirelessRadioManagerd  <488A6FF1-2F2D-33C5-ACDE-6D72D1AEC6D5>  /usr/sbin/WirelessRadioManagerd
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                   <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation         <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation             <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)        <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          XProtectPluginService [243]
UUID:             86B6F897-4580-399A-A1F2-F2CE169ABDC5
Path:             /Library/Apple/*/XProtect.app/Contents/XPCServices/XProtectPluginService.xpc/Contents/MacOS/XProtectPluginService
Identifier:       com.apple.XprotectFramework.PluginService
Version:          74 (1)
Build Version:    2
Project Name:     XProtectPayloads
Source Version:   164000000000000
Shared Cache:     42EC2890-BC40-317E-BB27-45142BFF7F26 slid base address 0x7ff8118fb000, slide 0x118fb000 (System Primary)
Architecture:     x86_64
Footprint:        1832 KB
Time Since Fork:  179s
Num samples:      12 (1-12)
Num threads:      2
Note:             1 idle work queue thread omitted

  Thread 0x614    DispatchQueue "com.apple.main-thread"(1)    12 samples (1-12)    priority 4 (base 4)
  <thread QoS background (requested default), thread darwinbg, process darwinbg, IO tier 2>
  12  ??? (dyld + 25904) [0x7ff813429530] 1-12
    12  ??? (XProtectPluginService + 7008) [0x10a550b60] 1-12
      12  ??? (XProtectPluginService + 7961) [0x10a550f19] 1-12
        12  ??? (XProtectPluginService + 7749) [0x10a550e45] 1-12
          12  ??? (Foundation + 898551) [0x7ff814a7d5f7] 1-12
            12  ??? (Foundation + 372771) [0x7ff8149fd023] 1-12
              12  ??? (CoreFoundation + 500530) [0x7ff8138b7332] 1-12
                12  ??? (CoreFoundation + 503551) [0x7ff8138b7eff] 1-12
                  12  ??? (CoreFoundation + 509106) [0x7ff8138b94b2] 1-12
                    12  ??? (libsystem_kernel.dylib + 3643) [0x7ff81378ce3b] 1-12
                      12  ??? (libsystem_kernel.dylib + 31683) [0x7ff813793bc3] 1-12
                        12  ??? (libsystem_kernel.dylib + 2890) [0x7ff81378cb4a] 1-12
                         *12  ??? (kernel + 821648) [0xffffff80003b0990] 1-12

  Binary Images:
           0x10a54f000 -                ???  com.apple.XprotectFramework.PluginService 74 (1) <86B6F897-4580-399A-A1F2-F2CE169ABDC5>  /Library/Apple/*/XProtect.app/Contents/XPCServices/XProtectPluginService.xpc/Contents/MacOS/XProtectPluginService
        0x7ff813423000 -     0x7ff8134bd6c7  dyld                                             <22E15325-4DF0-3BA4-882C-314A2A7A3239>  /usr/lib/dyld
        0x7ff81378c000 -     0x7ff8137c8b1f  libsystem_kernel.dylib                           <96CC590B-A75E-3BED-A0F3-DF6C2454D6DD>  /usr/lib/system/libsystem_kernel.dylib
        0x7ff81383d000 -     0x7ff813cf2fe2  CoreFoundation                                   <C204E205-B457-333E-8DA9-3D11902A7390>  /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
        0x7ff8149a2000 -     0x7ff815794f8d  Foundation                                       <712CCDBE-DEB7-3BEF-A8DD-AEF566BF9362>  /System/Library/Frameworks/Foundation.framework/Versions/C/Foundation
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                  <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel


Process:          kernel_task [0]
UUID:             B7B3E8AD-9CA6-382C-BD56-8D52139342A4
Path:             /System/Library/Kernels/kernel
Version:          Darwin Kernel Version 24.6.0: Sun Aug 23 20:30:56 PDT 2026; root:xnu-11417.140.69.712.69~1/RELEASE_X86_64
Architecture:     x86_64
Footprint:        216 KB
Time Since Fork:  184s
Num samples:      12 (1-12)
CPU Time:         0.158s
Num threads:      253

  Thread 0x65    Thread name "VM_pageout_scan"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 1920736) [0xffffff80004bcee0] 1-12

  Thread 0x66    Thread name "idle #0"    12 samples (1-12)    priority 0 (base 0)    idle time 5.343s
  <IO tier 0>
 *1   ??? (kernel + 1186128) [0xffffff8000409950] (running) 1
 *11  ??? (kernel + 520606) [0xffffff800036719e] 2-12
   *5   ??? (kernel + 1186220) [0xffffff80004099ac] 2-6
     *5   ??? (kernel + 1185602) [0xffffff8000409742] 2-6
       *5   ??? (kernel + 2435118) [0xffffff800053a82e] (running) 2-6
   *1   ??? (kernel + 1186182) [0xffffff8000409986] (running) 7
   *5   ??? (kernel + 1186220) [0xffffff80004099ac] 8-12
     *5   ??? (kernel + 1185602) [0xffffff8000409742] 8-12
       *5   ??? (kernel + 2435118) [0xffffff800053a82e] (running) 8-12

  Thread 0x67    Thread name "sched_maintenance_thread"    12 samples (1-12)    priority 95 (base 95)    cpu time 0.001s
  <IO tier 0>
 *12  ??? (kernel + 1165904) [0xffffff8000404a50] 1-12

  Thread 0x68    Thread name "daemon.deferred-deallocation"    12 samples (1-12)    priority 80 (base 80)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 1091344) [0xffffff80003f2710] 1-12

  Thread 0x69    Thread name "daemon.thread-stack"    12 samples (1-12)    priority 93 (base 93)
  <IO tier 0>
 *12  ??? (kernel + 1091344) [0xffffff80003f2710] 1-12

  Thread 0x6a    Thread name "daemon.thread-exception"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 520256) [0xffffff8000367040] 1-12

  Thread 0x6b    Thread name "daemon.thread-backtrace"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 520256) [0xffffff8000367040] 1-12

  Thread 0x6c    Thread name "thread_call_daemon"    12 samples (1-12)    priority 94 (base 94)
  <IO tier 0>
 *12  ??? (kernel + 1353040) [0xffffff8000432550] 1-12

  Thread 0x6d    Thread name "smr.reclaim:0"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0x6e    12 samples (1-12)    priority 95 (base 95)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 2302885) [0xffffff800051a3a5] 1-12
     *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
       *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
         *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x6f    Thread name "IOServiceTerminateThread"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 8312621) [0xffffff8000ad572d] 1-12
     *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x70    Thread name "AppleACPIButton"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x72    Thread name "daemon.core-analytics-events"    12 samples (1-12)    priority 63 (base 63)
  <IO tier 0>
 *12  ??? (kernel + 520256) [0xffffff8000367040] 1-12

  Thread 0x73    Thread name "VM_reclaim_scavenger"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 1969008) [0xffffff80004c8b70] 1-12

  Thread 0x74    Thread name "VM_pageout_garbage_collect"    12 samples (1-12)    priority 31 (base 31)
  <IO tier 0>
 *12  ??? (kernel + 1904256) [0xffffff80004b8e80] 1-12

  Thread 0x76    Thread name "thread call high #1"    12 samples (1-12)    priority 93 (base 93)
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x78    Thread name "IOPMrootDomain"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x7c    Thread name "IOPCIConfigurator"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0xaf    Thread name "smr.reclaim:1"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb0    Thread name "smr.reclaim:2"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb1    Thread name "smr.reclaim:3"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb2    Thread name "smr.reclaim:4"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb3    Thread name "smr.reclaim:5"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb4    Thread name "smr.reclaim:6"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb5    Thread name "smr.reclaim:7"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb6    Thread name "smr.reclaim:8"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb7    Thread name "smr.reclaim:9"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb8    Thread name "smr.reclaim:10"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xb9    Thread name "smr.reclaim:11"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xba    Thread name "smr.reclaim:12"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xbb    Thread name "smr.reclaim:13"    12 samples (1-12)    priority 80 (base 80)
  <IO tier 0>
 *12  ??? (kernel + 1209392) [0xffffff800040f430] 1-12

  Thread 0xbd    Thread name "idle #9"    12 samples (1-12)    priority 0 (base 0)    idle time 5.518s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xbf    Thread name "idle #10"    12 samples (1-12)    priority 0 (base 0)    idle time 5.522s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xc1    Thread name "idle #11"    12 samples (1-12)    priority 0 (base 0)    idle time 5.521s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xc3    Thread name "idle #1"    12 samples (1-12)    priority 0 (base 0)    idle time 5.501s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xc5    Thread name "idle #2"    12 samples (1-12)    priority 0 (base 0)    idle time 5.516s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xc7    Thread name "idle #3"    12 samples (1-12)    priority 0 (base 0)    idle time 5.519s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xc9    Thread name "idle #4"    12 samples (1-12)    priority 0 (base 0)    idle time 5.519s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xcb    Thread name "idle #5"    12 samples (1-12)    priority 0 (base 0)    idle time 5.516s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xcd    Thread name "idle #6"    12 samples (1-12)    priority 0 (base 0)    idle time 5.518s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xcf    Thread name "idle #7"    12 samples (1-12)    priority 0 (base 0)    idle time 5.522s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xd1    Thread name "idle #8"    12 samples (1-12)    priority 0 (base 0)    idle time 5.522s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1186220) [0xffffff80004099ac] 1-12
     *12  ??? (kernel + 1185602) [0xffffff8000409742] 1-12
       *12  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 1-12

  Thread 0xd3    Thread name "idle #12"    12 samples (1-12)    priority 0 (base 0)    idle time 5.487s
  <IO tier 0>
 *12  ??? (kernel + 1186128) [0xffffff8000409950] (running) 1-12

  Thread 0xd5    Thread name "idle #13"    12 samples (1-12)    priority 0 (base 0)    idle time 5.508s
  <IO tier 0>
 *1   ??? (kernel + 1186128) [0xffffff8000409950] (running) 1
 *11  ??? (kernel + 520606) [0xffffff800036719e] 2-12
   *11  ??? (kernel + 1186220) [0xffffff80004099ac] 2-12
     *11  ??? (kernel + 1185602) [0xffffff8000409742] 2-12
       *11  ??? (kernel + 2435118) [0xffffff800053a82e] (running) 2-12

  Thread 0xd7    Thread name "sched_update_powered_cores"    12 samples (1-12)    priority 95 (base 95)
  <IO tier 0>
 *12  ??? (kernel + 1195024) [0xffffff800040bc10] 1-12

  Thread 0x13f    Thread name "VirtualSMC"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x140    Thread name "VoodooGPIOMeteorLakeP"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x141    Thread name "AppleACPIEC"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x149    Thread name "AppleACPIButton"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x157    Thread name "BatteryManager"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x158    Thread name "AppleSMC"    12 samples (1-12)    priority 81 (base 81)    cpu time 0.002s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x15b    12 samples (1-12)    priority 97 (base 97)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (AppleSMC + 67326) [0xffffff8001bda6fe] 1-12
     *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
       *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
         *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x15d    Thread name "AppleCredentialManager"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x173    Thread name "IONVMeController"    12 samples (1-12)    priority 81 (base 81)    cpu time 0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 8459190) [0xffffff8000af93b6] 1-12
     *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
       *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
         *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x176    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 8459190) [0xffffff8000af93b6] 1-12
     *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
       *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
         *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x178    Thread name "VoodooI2CDeviceNub"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x17a    Thread name "CCLogPipe"    12 samples (1-12)    priority 81 (base 81)    cpu time 0.004s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x184    Thread name "AppleKeyStore"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x185    Thread name "aks events"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (AppleKeyStore + 12126) [0xffffff80019ebf5e] 1-12
     *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x187    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (AppleLockdownMode + 5906) [0xffffff8001ab0712] 1-12
     *12  ??? (AppleLockdownMode + 5549) [0xffffff8001ab05ad] 1-12
       *12  ??? (kernel + 8284716) [0xffffff8000acea2c] 1-12
         *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
           *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
             *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
               *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x188    Thread name "AppleSSE"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x18a    Thread name "CoreAnalyticsHub"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x18c    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (EndpointSecurity + 545343) [0xffffff800207023f] 1-12
     *12  ??? (EndpointSecurity + 544543) [0xffffff800206ff1f] 1-12
       *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
         *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
           *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
             *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x18e    Thread name "IOBluetoothHCIController"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x198    Thread name "BrightnessKeys"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x199    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x19e    Thread name "AppleUSBHostResources"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1a0    Thread name "SMCSMBusController"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1a2    Thread name "thread call high #3"    12 samples (1-12)    priority 93 (base 93)
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x1a3    Thread name "VM_io_reprioritize_thread"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520256) [0xffffff8000367040] 1-12

  Thread 0x1a4    Thread name "thread call kernel #1"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1354664) [0xffffff8000432ba8] 1-12
     *12  ??? (kernel + 1350488) [0xffffff8000431b58] 1-12
       *12  ??? (AppleMobileFileIntegrity + 82389) [0xffffff8001ae91d5] 1-12
         *12  ??? (AppleMobileFileIntegrity + 30902) [0xffffff8001adc8b6] 1-12
           *12  ??? (kernel + 8284716) [0xffffff8000acea2c] 1-12
             *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
               *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
                 *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
                   *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x1a5    Thread name "thread call kernel #2"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x1a6    Thread name "AppleUSBLegacyRoot"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1a9    Thread name "thread call high #4"    12 samples (1-12)    priority 93 (base 93)    cpu time 0.004s
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x1ab    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x1ac    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 2994880) [0xffffff80005c32c0] 1-12

  Thread 0x1ad    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 2995072) [0xffffff80005c3380] 1-12

  Thread 0x1af    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6780544) [0xffffff800095f680] 1-12

  Thread 0x1b0    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6022128) [0xffffff80008a63f0] 1-12

  Thread 0x1b1    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6114992) [0xffffff80008bceb0] 1-12

  Thread 0x1b2    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6114992) [0xffffff80008bceb0] 1-12

  Thread 0x1b3    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6114992) [0xffffff80008bceb0] 1-12

  Thread 0x1b4    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6114992) [0xffffff80008bceb0] 1-12

  Thread 0x1b5    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x1b6    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x1b7    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x1b8    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 3586464) [0xffffff80006539a0] 1-12

  Thread 0x1b9    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 3522160) [0xffffff8000643e70] 1-12

  Thread 0x1ba    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 6476624) [0xffffff8000915350] 1-12

  Thread 0x1bb    Thread name "VM_memorystatus_1"    12 samples (1-12)    priority 95 (base 95)
  <IO tier 0>
 *12  ??? (kernel + 6508672) [0xffffff800091d080] 1-12

  Thread 0x1bc    Thread name "VM_memorystatus_2"    12 samples (1-12)    priority 95 (base 95)
  <IO tier 0>
 *12  ??? (kernel + 6508672) [0xffffff800091d080] 1-12

  Thread 0x1bd    Thread name "VM_memorystatus_3"    12 samples (1-12)    priority 95 (base 95)
  <IO tier 0>
 *12  ??? (kernel + 6508672) [0xffffff800091d080] 1-12

  Thread 0x1bf    Thread name "dlil_input_gif0"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 3583040) [0xffffff8000652c40] 1-12

  Thread 0x1c0    Thread name "CFIL_STATS_REPORT"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 4638880) [0xffffff80007548a0] 1-12

  Thread 0x1c1    Thread name "dlil_input_stf0"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 3583040) [0xffffff8000652c40] 1-12

  Thread 0x1c4    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1c7    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1c9    Thread name "IOHIDSystem"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1ce    Thread name "IOSurfaceRoot"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x1e3    Thread name "apfs_swcrypto_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 382855) [0xffffff8003037787] 1-12

  Thread 0x1e4    Thread name "apfs_swcrypto_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 382855) [0xffffff8003037787] 1-12

  Thread 0x1e5    Thread name "apfs_swcrypto_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 382855) [0xffffff8003037787] 1-12

  Thread 0x1e6    Thread name "apfs_swcrypto_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 382855) [0xffffff8003037787] 1-12

  Thread 0x1e7    Thread name "apfs_specread_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 1509408) [0xffffff800314a820] 1-12

  Thread 0x1e8    Thread name "apfs_specread_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 1509408) [0xffffff800314a820] 1-12

  Thread 0x1e9    Thread name "apfs_specread_worker"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (apfs + 1509408) [0xffffff800314a820] 1-12

  Thread 0x1ec    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? [0x0] 1-12
     *12  ??? [0x0] 1-12
       *12  ??? (kernel + 8188588) [0xffffff8000ab72ac] 1-12
         *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
           *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
             *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
               *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x1ed    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? [0x0] 1-12
     *12  ??? [0x0] 1-12
       *12  ??? (kernel + 8188588) [0xffffff8000ab72ac] 1-12
         *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
           *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
             *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
               *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x202    Thread name "apfs_purgatory_cleaner"    12 samples (1-12)    priority 31 (base 31)
  <IO tier 0>
 *12  ??? (apfs + 697445) [0xffffff8003084465] 1-12

  Thread 0x203    Thread name "VM_pageout_external_iothread"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0 and passive>
 *12  ??? (kernel + 1926048) [0xffffff80004be3a0] 1-12

  Thread 0x204    Thread name "VM_pressure"    12 samples (1-12)    priority 31 (base 31)
  <IO tier 0>
 *12  ??? (kernel + 1923504) [0xffffff80004bd9b0] 1-12

  Thread 0x205    Thread name "VM_object_reaper_thread"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (kernel + 1860868) [0xffffff80004ae504] 1-12
     *12  ??? (kernel + 1861115) [0xffffff80004ae5fb] 1-12
       *12  ??? (kernel + 1061182) [0xffffff80003eb13e] 1-12
         *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
           *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
             *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x206    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 1610080) [0xffffff8000471160] 1-12

  Thread 0x207    Thread name "VM_compressor"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 1927856) [0xffffff80004beab0] 1-12

  Thread 0x208    Thread name "VM_compressor"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 1927856) [0xffffff80004beab0] 1-12

  Thread 0x209    Thread name "VM_swapout"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 1641232) [0xffffff8000478b10] 1-12

  Thread 0x20a    Thread name "VM_swapfile_create"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0>
 *12  ??? (kernel + 1643440) [0xffffff80004793b0] 1-12

  Thread 0x20b    Thread name "VM_swapfile_gc"    12 samples (1-12)    priority 91 (base 91)
  <IO tier 0 and passive>
 *12  ??? (kernel + 1644384) [0xffffff8000479760] 1-12

  Thread 0x218    Thread name "AppleMultitouchTrackpadHIDEventDriver"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x21b    Thread name "AppleActuatorDevice"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x21e    Thread name "AirportItlwm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x228    Thread name "IO80211EventSource"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x22c    Thread name "AppleUSBXHCIPCI@0d000000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x22e    Thread name "dlil_input_XHC13"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 3583040) [0xffffff8000652c40] 1-12

  Thread 0x22f    Thread name "AppleUSBXHCIPCI@14000000"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x231    Thread name "dlil_input_XHC20"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 3583040) [0xffffff8000652c40] 1-12

  Thread 0x232    Thread name "AppleUSB30XHCIPort@0d100000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x234    Thread name "AppleUSB30XHCIPort@0d200000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x236    Thread name "AppleUSB30XHCIPort@0d300000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x238    Thread name "AppleUSB20XHCIPort@14100000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x23a    Thread name "AppleUSB20XHCIPort@14200000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x23c    Thread name "AppleUSB20XHCIPort@14300000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x23e    Thread name "AppleUSB20XHCIPort@14400000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x240    Thread name "AppleUSB20XHCIPort@14500000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x242    Thread name "AppleUSB20XHCIPort@14600000"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x244    Thread name "AppleUSB30XHCIPort@14700000"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x248    Thread name "thread call high #7"    12 samples (1-12)    priority 93 (base 93)
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x24c    Thread name "AppleUSBController"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x24d    Thread name "AppleUSBRootHubDevice"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x24f    Thread name "ApplePS2Controller"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x250    Thread name "ApplePS2KeyboardDevice"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x251    Thread name "ApplePS2MouseDevice"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x260    Thread name "IOWorkLoop"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x276    Thread name "apfs_purgatory_cleaner"    12 samples (1-12)    priority 31 (base 31)
  <IO tier 0>
 *12  ??? (apfs + 697445) [0xffffff8003084465] 1-12

  Thread 0x277    Thread name "apfs_transaction_flusher"    12 samples (1-12)    priority 81 (base 81)    cpu time 0.005s
  <IO tier 0 and passive>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (apfs + 1463798) [0xffffff800313f5f6] 1-12
     *12  ??? (kernel + 6475849) [0xffffff8000915049] 1-12
       *12  ??? (kernel + 1067097) [0xffffff80003ec859] 1-12
         *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
           *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
             *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x278    Thread name "apfs_purgatory_cleaner"    12 samples (1-12)    priority 31 (base 31)
  <IO tier 0>
 *12  ??? (apfs + 697445) [0xffffff8003084465] 1-12

  Thread 0x27d    Thread name "apfs_purgatory_cleaner"    12 samples (1-12)    priority 31 (base 31)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (apfs + 697445) [0xffffff8003084465] 1-12

  Thread 0x368    Thread name "SOFLOW_GC"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 7099664) [0xffffff80009ad510] 1-12

  Thread 0x3bf    Thread name "dlil_input_en0"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 3583040) [0xffffff8000652c40] 1-12

  Thread 0x3c0    Thread name "ifnet_start_en0"    12 samples (1-12)    priority 82 (base 82)
  <IO tier 0>
 *12  ??? (kernel + 3556000) [0xffffff800064c2a0] 1-12

  Thread 0x460    Thread name "skywalk_fsw_reap_en0"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 7425984) [0xffffff80009fcfc0] 1-12

  Thread 0x50e    Thread name "thread call qos-ui #1"    12 samples (1-12)    priority 47 (base 47)
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x803    Thread name "IOGraphicsSystem"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x804    Thread name "IOTimeSyncDaemonService"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x806    Thread name "X86PlatformPlugin"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x807    Thread name "AGDCBacklightControl"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x808    Thread name "AppleIntelPanelA"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x80e    Thread name "AppleMCCSParameterHandler"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x80f    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x810    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x811    Thread name "IOTimeSyncTimeSyncTimePort"    12 samples (1-12)    priority 81 (base 81)    cpu time 0.002s
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x812    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (IORDMAFamily + 119993) [0xffffff7f95d604b9] 1-12
     *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x813    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (IORDMAFamily + 70249) [0xffffff7f95d54269] 1-12
     *12  ??? (kernel + 1066897) [0xffffff80003ec791] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x817    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x818    Thread name "AppleIntelSlowAdaptiveClockingManager"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x81a    Thread name "AppleGPUWrangler"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x81d    Thread name "AppleIPDormancyHandler"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x827    Thread name "com_apple_driver_16X50PCI"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x828    Thread name "LatitudePanelClaim"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x82a    Thread name "nmx softclock"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2392941) [0xffffff7f9744336d] 1-12
     *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
       *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
         *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x82b    Thread name "nmx taskq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x830    Thread name "IOHIDPowerSource"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x831    Thread name "linuxkpi_irq_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x832    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x833    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x834    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x835    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x836    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x837    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x838    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x839    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x83a    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x83b    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x83c    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x83d    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x83e    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x83f    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x840    Thread name "linuxkpi_short_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x841    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x842    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x843    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x844    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x845    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x846    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x847    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x848    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x849    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x84a    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x84b    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x84c    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x84d    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x84e    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x84f    Thread name "linuxkpi_long_wq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x853    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x856    Thread name "i915"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x857    Thread name "i915-dp"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x858    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x859    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x85a    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x85b    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x85c    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x85d    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x85e    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x85f    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x860    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x861    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x862    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x863    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x864    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x865    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x866    Thread name "i915-unordered"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x867    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x868    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x869    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x86a    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x86b    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x86c    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x86d    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x86e    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x86f    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x870    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x871    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x872    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x873    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x874    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x875    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x876    Thread name "ttm"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (NMXGPU + 2395058) [0xffffff7f97443bb2] 1-12
     *12  ??? (NMXGPU + 2385772) [0xffffff7f9744176c] 1-12
       *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
         *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
           *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x87a    Thread name "LatitudeMTLGPUIrq"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x881    Thread name "com_apple_driver_pm_uncore_reporter"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x887    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x895    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x896    Thread name "thread call user #1"    12 samples (1-12)    priority 31 (base 31)    cpu time <0.001s
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Thread 0x8a1    Thread name "NMXEventMachine"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x8a2    Thread name "NMXAccel"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x8a3    Thread name "IOAccelFenceMachine"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x8bc    12 samples (1-12)    priority 97 (base 97)    cpu time 0.021s
  <IO tier 0>
 *12  ??? (kernel + 520606) [0xffffff800036719e] 1-12
   *12  ??? (LatitudeMTLFB + 5938) [0xffffff7f971c5732] 1-12
     *12  ??? (kernel + 1153328) [0xffffff8000401930] 1-12
       *12  ??? (kernel + 1161749) [0xffffff8000403a15] 1-12
         *12  ??? (kernel + 2400279) [0xffffff8000532017] 1-12

  Thread 0x918    Thread name "X86PlatformShim"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x981    Thread name "IOGraphics-GFX0-0x2f3"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x984    Thread name "NMXDisplayPipe"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0x985    Thread name "IOGraphics-IONDRVFramebuffer-0x4d1"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 8458880) [0xffffff8000af9280] 1-12

  Thread 0xaa9    Thread name "TRACKER_GC"    12 samples (1-12)    priority 81 (base 81)
  <IO tier 0>
 *12  ??? (kernel + 7085664) [0xffffff80009a9e60] 1-12

  Thread 0xb0b    Thread name "thread call kernel-high #3"    12 samples (1-12)    priority 92 (base 92)    cpu time 0.112s
  <IO tier 0>
 *2   ??? (kernel + 1353936) [0xffffff80004328d0] 1-2
 *1   ??? (kernel + 520606) [0xffffff800036719e] 3
   *1   ??? (kernel + 1354664) [0xffffff8000432ba8] 3
     *1   ??? (kernel + 1350488) [0xffffff8000431b58] 3
       *1   ??? (kernel + 8488662) [0xffffff8000b006d6] 3
         *1   ??? (kernel + 8488883) [0xffffff8000b007b3] 3
           *1   ??? [0x0] 3
             *1   ??? [0x0] 3
               *1   ??? [0x0] 3
                 *1   ??? (kernel + 8473618) [0xffffff8000afcc12] 3
                   *1   ??? [0x0] 3
                     *1   ??? [0x0] 3
                       *1   ??? (kernel + 8472920) [0xffffff8000afc958] 3
                         *1   ??? [0x0] 3
                           *1   ??? [0x0] 3
                             *1   ??? (kernel + 8466804) [0xffffff8000afb174] 3
                               *1   ??? (kernel + 8462016) [0xffffff8000af9ec0] 3
                                 *1   ??? (kernel + 1067097) [0xffffff80003ec859] 3
                                   *1   ??? (kernel + 1153328) [0xffffff8000401930] 3
                                     *1   ??? (kernel + 1161749) [0xffffff8000403a15] 3
                                       *1   ??? (kernel + 2400279) [0xffffff8000532017] 3
 *2   ??? (kernel + 1353936) [0xffffff80004328d0] 4-5
 *1   ??? (kernel + 520606) [0xffffff800036719e] 6
   *1   ??? (kernel + 1354664) [0xffffff8000432ba8] 6
     *1   ??? (kernel + 1350488) [0xffffff8000431b58] 6
       *1   ??? (kernel + 8488662) [0xffffff8000b006d6] 6
         *1   ??? (kernel + 8488883) [0xffffff8000b007b3] 6
           *1   ??? [0x0] 6
             *1   ??? [0x0] 6
               *1   ??? [0x0] 6
                 *1   ??? (kernel + 8473618) [0xffffff8000afcc12] 6
                   *1   ??? [0x0] 6
                     *1   ??? [0x0] 6
                       *1   ??? (kernel + 8472920) [0xffffff8000afc958] 6
                         *1   ??? [0x0] 6
                           *1   ??? [0x0] 6
                             *1   ??? (kernel + 8466804) [0xffffff8000afb174] 6
                               *1   ??? (kernel + 8462016) [0xffffff8000af9ec0] 6
                                 *1   ??? (kernel + 1067097) [0xffffff80003ec859] 6
                                   *1   ??? (kernel + 1153328) [0xffffff8000401930] 6
                                     *1   ??? (kernel + 1161749) [0xffffff8000403a15] 6
                                       *1   ??? (kernel + 2400279) [0xffffff8000532017] 6
 *2   ??? (kernel + 1353936) [0xffffff80004328d0] 7-8
 *2   ??? (kernel + 520606) [0xffffff800036719e] 9-10
   *2   ??? (kernel + 1354664) [0xffffff8000432ba8] 9-10
     *2   ??? (kernel + 1350488) [0xffffff8000431b58] 9-10
       *2   ??? (kernel + 8488662) [0xffffff8000b006d6] 9-10
         *2   ??? (kernel + 8488883) [0xffffff8000b007b3] 9-10
           *2   ??? [0x0] 9-10
             *1   ??? [0x0] 9
               *1   ??? [0x0] 9
                 *1   ??? (kernel + 8473618) [0xffffff8000afcc12] 9
                   *1   ??? [0x0] 9
                     *1   ??? [0x0] 9
                       *1   ??? (kernel + 8472920) [0xffffff8000afc958] 9
                         *1   ??? [0x0] 9
                           *1   ??? [0x0] 9
                             *1   ??? (kernel + 8466804) [0xffffff8000afb174] 9
                               *1   ??? (kernel + 8462016) [0xffffff8000af9ec0] 9
                                 *1   ??? (kernel + 1067097) [0xffffff80003ec859] 9
                                   *1   ??? (kernel + 1153328) [0xffffff8000401930] 9
                                     *1   ??? (kernel + 1161749) [0xffffff8000403a15] 9
                                       *1   ??? (kernel + 2400279) [0xffffff8000532017] 9
             *1   ??? (kernel + 1153328) [0xffffff8000401930] 10
               *1   ??? (kernel + 1161749) [0xffffff8000403a15] 10
                 *1   ??? (kernel + 2400279) [0xffffff8000532017] 10
 *1   ??? (kernel + 1353936) [0xffffff80004328d0] 11
 *1   ??? (kernel + 520606) [0xffffff800036719e] 12
   *1   ??? (kernel + 1354664) [0xffffff8000432ba8] 12
     *1   ??? (kernel + 1350488) [0xffffff8000431b58] 12
       *1   ??? (kernel + 8488662) [0xffffff8000b006d6] 12
         *1   ??? (kernel + 8488883) [0xffffff8000b007b3] 12
           *1   ??? [0x0] 12
             *1   ??? [0x0] 12
               *1   ??? [0x0] 12
                 *1   ??? (kernel + 8473618) [0xffffff8000afcc12] 12
                   *1   ??? [0x0] 12
                     *1   ??? [0x0] 12
                       *1   ??? (kernel + 8472920) [0xffffff8000afc958] 12
                         *1   ??? [0x0] 12
                           *1   ??? [0x0] 12
                             *1   ??? (kernel + 8466804) [0xffffff8000afb174] 12
                               *1   ??? (kernel + 8462016) [0xffffff8000af9ec0] 12
                                 *1   ??? (kernel + 1067097) [0xffffff80003ec859] 12
                                   *1   ??? (kernel + 1153328) [0xffffff8000401930] 12
                                     *1   ??? (kernel + 1161749) [0xffffff8000403a15] 12
                                       *1   ??? (kernel + 2400279) [0xffffff8000532017] 12

  Thread 0xe63    Thread name "thread call kernel-high #3"    12 samples (1-12)    priority 92 (base 92)    cpu time 0.002s
  <IO tier 0>
 *12  ??? (kernel + 1353936) [0xffffff80004328d0] 1-12

  Binary Images:
   *0xffffff7f95d43000 - 0xffffff7f95d6dff3  com.apple.iokit.IORDMAFamily (1.0)                                    <2FE6FB63-C7F7-35E5-A112-1F8A77CC2ADA>  /System/Library/Extensions/IORDMAFamily.kext/Contents/MacOS/IORDMAFamily
   *0xffffff7f971c4000 - 0xffffff7f971d0327  com.nullmoth.LatitudeMTLFB (0.2.3)                                    <8F005BB5-A9E1-3A5F-882A-398E3D1E1E92>  /Library/Extensions/LatitudeMTLFB.kext/Contents/MacOS/LatitudeMTLFB
   *0xffffff7f971fb000 - 0xffffff7f974e8ec9  com.nullmoth.NMXGPU (1.0.9)                                           <E770B985-14A4-3ADE-A1EE-63204C3A9996>  /Library/Extensions/NMXGPU.kext/Contents/MacOS/NMXGPU
   *0xffffff80002e8000 - 0xffffff80056defff  kernel (24.6.0)                                                       <B7B3E8AD-9CA6-382C-BD56-8D52139342A4>  /System/Library/Kernels/kernel
   *0xffffff8001909000 - 0xffffff800190affe  com.apple.AppleFSCompression.AppleFSCompressionTypeDataless (1.0.0d1) <3482FB42-25C6-3FB9-846E-A202733E43BB>  /System/Library/Extensions/AppleFSCompressionTypeDataless.kext/Contents/MacOS/AppleFSCompressionTypeDataless
   *0xffffff80019e9000 - 0xffffff8001a88ff9  com.apple.driver.AppleKeyStore (2)                                    <0B39302D-243A-3345-BAEF-C7420F95323D>  /System/Library/Extensions/AppleKeyStore.kext/Contents/MacOS/AppleKeyStore
   *0xffffff8001aaf000 - 0xffffff8001ac6ff7  com.apple.driver.AppleLockdownMode (1)                                <9044EBFC-CD08-3F31-B05C-AB17993CD59B>  /System/Library/Extensions/AppleLockdownMode.kext/Contents/MacOS/AppleLockdownMode
   *0xffffff8001ad5000 - 0xffffff8001b0fff0  com.apple.driver.AppleMobileFileIntegrity (1.0.5)                     <6E8B7BFD-07B5-329A-9745-5A22A7945A1D>  /System/Library/Extensions/AppleMobileFileIntegrity.kext/Contents/MacOS/AppleMobileFileIntegrity
   *0xffffff8001bca000 - 0xffffff8001be3ffd  com.apple.driver.AppleSMC (3.1.9)                                     <CB4E3E0B-2583-3476-A215-30C8233D2D64>  /System/Library/Extensions/AppleSMC.kext/Contents/MacOS/AppleSMC
   *0xffffff8001feb000 - 0xffffff8002092ff9  com.apple.iokit.EndpointSecurity (1)                                  <45350761-D30F-3DFF-B908-DAD7CBF0E1FF>  /System/Library/Extensions/EndpointSecurity.kext/Contents/MacOS/EndpointSecurity
   *0xffffff8002fda000 - 0xffffff80031a2fef  com.apple.filesystems.apfs (2332.140.13)                              <406F5E63-F40D-3F37-AA8E-FD5983D32A47>  /System/Library/Extensions/apfs.kext/Contents/MacOS/apfs
   *0xffffff800329d000 - 0xffffff80032a3fff  com.apple.kec.pthread (1)                                             <61A4986C-5BEF-36B7-9B2E-61F7E69AA8A6>  /System/Library/Extensions/pthread.kext/Contents/MacOS/pthread

System Profile:
Network Service: Wi-Fi, AirPort, en0
Boot Volume File System Type: apfs
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
Memory Module: Motherboard, 2 GB, RAM, 6400 MHz, Unknown, Unknown
USB Device: USB32Bus
USB Device: bluetooth_device
USB Device: Integrated_Webcam_FHD
USB Device: USB32Bus
Thunderbolt Bus: 
Model: MacBookPro16,2, BootROM 2094.80.5.0.0, 14 processors, Intel(R) Core(TM) Ultra 5 135U, 2.68 GHz, 16 GB, SMC 
Graphics: Display, Built-In
AirPort: spairport_wireless_card_type_wifi (0x8086, 0x4090), itlwm: 2.4.0 fw: 83.d24e06ed.0
Bluetooth: Version (null), 0 services, 0 devices, 0 incoming serial ports
