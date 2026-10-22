/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef _APPLEPS2TOADBMAP_H
#define _APPLEPS2TOADBMAP_H

#define DEADKEY 0xa2

// Normal PS2 ScanCodes to ADB Mapping
unsigned int PS2ToADBMap[0x80] =
{
/*  ADB       AT  Key-Legend
    ======================== */
    DEADKEY,  // 00
    0x35,  // 01  Escape
    0x12,  // 02  1
    0x13,  // 03  2
    0x14,  // 04  3
    0x15,  // 05  4
    0x17,  // 06  5
    0x16,  // 07  6
    0x1a,  // 08  7
    0x1c,  // 09  8
    0x19,  // 0a  9
    0x1d,  // 0b  0
    0x1b,  // 0c  -_
    0x18,  // 0d  =+
    0x33,  // 0e  Backspace
    0x30,  // 0f  Tab
    0x0c,  // 10  Q
    0x0d,  // 11  W
    0x0e,  // 12  E
    0x0f,  // 13  R
    0x11,  // 14  T
    0x10,  // 15  Y
    0x20,  // 16  U
    0x22,  // 17  I
    0x1f,  // 18  O
    0x23,  // 19  P
    0x21,  // 1a  [{
    0x1e,  // 1b  ]}
    0x24,  // 1c  Enter
    0x3b,  // 1d  Left Ctrl
    0x00,  // 1e  A
    0x01,  // 1f  S
    0x02,  // 20  D
    0x03,  // 21  F
    0x05,  // 22  G
    0x04,  // 23  H
    0x26,  // 24  J
    0x28,  // 25  K
    0x25,  // 26  L
    0x29,  // 27  ;:
    0x27,  // 28  '"
    0x32,  // 29  `~
    0x38,  // 2a  Left Shift
    0x2a,  // 2b  \|
    0x06,  // 2c  Z
    0x07,  // 2d  X
    0x08,  // 2e  C
    0x09,  // 2f  V
    0x0b,  // 30  B
    0x2d,  // 31  N
    0x2e,  // 32  M
    0x2b,  // 33  ,<
    0x2f,  // 34  .>
    0x2c,  // 35  /?
    0x3c,  // 36  Right Shift
    0x43,  // 37  Keypad *
    0x3a,  // 38  Left Alt
    0x31,  // 39  Space
    0x39,  // 3a  Caps Lock
    0x7a,  // 3b  F1
    0x78,  // 3c  F2
    0x63,  // 3d  F3
    0x76,  // 3e  F4
    0x60,  // 3f  F5
    0x61,  // 40  F6
    0x62,  // 41  F7
    0x64,  // 42  F8
    0x65,  // 43  F9
    0x6d,  // 44  F10
    0x47,  // 45  Num Lock
    DEADKEY,  // 46  Scroll Lock
    0x59,  // 47  Keypad 7
    0x5b,  // 48  Keypad 8
    0x5c,  // 49  Keypad 9
    0x4e,  // 4a  Keypad -
    0x56,  // 4b  Keypad 4
    0x57,  // 4c  Keypad 5
    0x58,  // 4d  Keypad 6
    0x45,  // 4e  Keypad +
    0x53,  // 4f  Keypad 1
    0x54,  // 50  Keypad 2
    0x55,  // 51  Keypad 3
    0x52,  // 52  Keypad 0
    0x41,  // 53  Keypad .
    0x3f,  // 54  SysReq (ALT + Print Screen) set to Apple Fn
    DEADKEY,  // 55
    0x0a,  // 56  Europe (ISO)
    0x67,  // 57  F11
    0x6f,  // 58  F12
    0x51,  // 59  Keypad =
    DEADKEY,  // 5a
    0x37,  // 5b Using Windows key for my custom shorcuts
    0x5f,  // 5c Keyboard Int'l 6 (PC9800 Keypad , )
    DEADKEY,  // 5d 
    DEADKEY,  // 5e 
    DEADKEY,  // 5f 
    DEADKEY,  // 60
    DEADKEY,  // 61
    DEADKEY,  // 62
    DEADKEY,  // 63
    0x69,  // 64  F13
    0x6b,  // 65  F14 Brightness down, Fn + F3 Brightnes down/up
    0x71,  // 66  F15 Brightness up
    0x6a,  // 67  F16
    0x6c,  // 68  F17, Fn + F5 Gigabyte Video mirror
    0x6e,  // 69  F18
    0x66,  // 6a  F19
    0x68,  // 6b  F20
    DEADKEY,  // 6c  F21, Fn + F12 Gigabyte Web cam off
    DEADKEY,  // 6d  F22, Fn + F11 Gigabyte BT off
    DEADKEY,  // 6e  F23, Fn + F11 Gigabyte BT on
    DEADKEY,  // 6f  
    DEADKEY,  // 70
    DEADKEY,  // 71
    DEADKEY,  // 72
    0x5e,  // 73 Brazilian ABNT2 /? and Keyboard Int'l 1 (Japanese Ro)
    DEADKEY,  // 74
    DEADKEY,  // 75
    DEADKEY,  // 76 F24, Fn + F12 Gigabyte Web cam on
    0x5a,  // 77 Keyboard Lang 4 (Japanese Hiragana)
    0x5a,  // 78 Keyboard Lang 3 (Japanese Katakana)
    0x5a,  // 79 Keyboard Int'l 4 (Japanese Henkan)
    DEADKEY,  // 7a
    0x5f,  // 7b Keyboard Int'l 5 (Japanese Muhenkan)
    DEADKEY,  // 7c
    0x5d,  // 7d Keyboard Int'l 3 (Japanese Yen)
    0x5f,  // 7e Brazilian ABNT2 Keypad .
    DEADKEY,  // 7f Power Button
};

// Extended PS2 ScanCodes to ADB Mapping
unsigned int ExtendedPS2ToADBMap[0x80] =
{
/*  ADB             AT  Key-Legend
    ======================== */
    DEADKEY,  // e0 00
    0x35,     // e0 01  Escape, Default DEAD
    0x50   ,  // e0 02  Samsung display modes Fn + F4
    DEADKEY,  // e0 03
    DEADKEY,  // e0 04
    0x4f,     // e0 05  Dell brightness down
    0x4d,     // e0 06  Dell brightness up
    DEADKEY,  // e0 07
    0x4d,     // e0 08  Samsung brightness up Fn + F2, Dell WiFi
    0x4f,     // e0 09  Samsung brightness down Fn + F3
    DEADKEY,  // e0 0a
    DEADKEY,  // e0 0b
    DEADKEY,  // e0 0c
    DEADKEY,  // e0 0d
    DEADKEY,  // e0 0e
    DEADKEY,  // e0 0f
    0x46,     // e0 10  Media Previous - Almost every vendor
    DEADKEY,  // e0 11
    0x4f,     // e0 12  HP brightness down Fn + F2
    DEADKEY,  // e0 13
    DEADKEY,  // e0 14
    DEADKEY,  // e0 15
    DEADKEY,  // e0 16  Samsung KbLight Up Fn + F9
    0x4d,     // e0 17  Samsung KbLight Down Fn + F10, HP brightness up Fn + F3
    DEADKEY,  // e0 18
    0x44,     // e0 19  Media Next - Almost every vendor
    DEADKEY,  // e0 1a
    DEADKEY,  // e0 1b
    0x4c,     // e0 1c  Keypad enter
    0x3e,     // e0 1d  Right Control
    DEADKEY,  // e0 1e
    DEADKEY,  // e0 1f
    0x4a,     // e0 20  Volume Mute  - Almost every vendor
    DEADKEY,  // e0 21  Calculator
    0x42,     // e0 22  Media Play/Pause - Almost every vendor
    DEADKEY,  // e0 23
    DEADKEY,  // e0 24  Media Stop - Almost every vendor
    0x72   ,  // e0 25  Acer Help Fn + F1
    DEADKEY,  // e0 26  Acer eSetting Fn + F2
    DEADKEY,  // e0 27  Acer Power managmenet Fn + F3
    DEADKEY,  // e0 28  Samsung Fn Toggle on
    0x50   ,  // e0 29  Acer display modes Fn + F5, Samsung Fn Toggle off
    DEADKEY,  // e0 2a
    DEADKEY,  // e0 2b  MSI Turbo battery key
    DEADKEY,  // e0 2c  
    DEADKEY,  // e0 2d
    0x49,     // e0 2e  Volume Down - Every vendor
    DEADKEY,  // e0 2f
    0x48,     // e0 30  Volume Up - Every vendor
    0x72,     // e0 31  HP Help Fn + F1
    DEADKEY,  // e0 32  HP Internet Fn + F1
    DEADKEY,  // e0 33  Samsung Fan Fn + F11
    DEADKEY,  // e0 34
    0x4b,     // e0 35  Keypad /
    DEADKEY,  // e0 36  MSI display off
    DEADKEY,  // e0 37  Print Screen
    0x3d,     // e0 38  Right Option/Alt
    DEADKEY,  // e0 39  MSI Device key
    DEADKEY,  // e0 3a
    0x50,     // e0 3b  MSI Cinema Pro
    DEADKEY,  // e0 3c
    DEADKEY,  // e0 3d
    DEADKEY,  // e0 3e
    0x3f,     // e0 3f  Apple Fn Key
    DEADKEY,  // e0 40
    DEADKEY,  // e0 41
    0x70,     // e0 42  MSI Eject
    DEADKEY,  // e0 43
    DEADKEY,  // e0 44  MSI 3G
    0xa1,     // e1 45  Pause
    0x70,     // e0 46  Break (Control + Pause) as Eject
    0x73,     // e0 47  Keypad Home
    0x7e,     // e0 48  Keypad Up Arrow
    0x74,     // e0 49  Keypad PgUp
    DEADKEY,  // e0 4a  
    0x7b,     // e0 4b  Keypad Left Arrow
    DEADKEY,  // e0 4c  Clear
    0x7c,     // e0 4d  Keypad Right Arrow
    0x72   ,  // e0 4e  Samsung settings Fn + F1, MSI Power/Battery saver Fn + F5
    0x77,     // e0 4f  Keypad End
    0x7d,     // e0 50  Keypad Down Arrow
    0x79,     // e0 51  Keypad PgDn
    0x72,     // e0 52  Keypad Insert
    0x75,     // e0 53  Keypad Del
    0x3f,     // e0 54  Apple Fn
    DEADKEY,  // e0 55  Samsung WiFi Fn + F12
    DEADKEY,  // e0 56
    DEADKEY,  // e0 57
    DEADKEY,  // e0 58  Lenovo Fn + F6 Touchpd On
    0x4d,     // e0 59  Acer brightness up, Dell touchpad on/off, Lenovo Fn + F6 Touchpad off
    DEADKEY,  // e0 5a
    0x37,     // e0 5b  Left Windows
    0x36,     // e0 5c  Right Windows
    0x3f,     // e0 5d  Right Context Menu as Apple Fn in Fn keys mode
    0x7f,     // e0 5e  System Power
    DEADKEY,  // e0 5f  System Sleep, MSI Sleep Fn + F12
    DEADKEY,  // e0 60  
    DEADKEY,  // e0 61  
    DEADKEY,  // e0 62  MSI Bluetooth on/off Fn + F9
    DEADKEY,  // e0 63  System Wake
    DEADKEY,  // e0 64  MSI touchpad on/off Fn + F3
    DEADKEY,  // e0 65  WWW Search
    DEADKEY,  // e0 66  WWW Favorites, HP Display switch Fn + F4
    DEADKEY,  // e0 67  WWW Refresh
    DEADKEY,  // e0 68  WWW Stop
    DEADKEY,  // e0 69  WWW Forward
    DEADKEY,  // e0 6a  WWW Back
    DEADKEY,  // e0 6b  My Computer
    DEADKEY,  // e0 6c  Mail
    DEADKEY,  // e0 6d  Media Select
    0x4d,     // e0 6e  Acer brightness up, MSI Webcam Fn + F6
    0x4f,     // e0 6f  Acer brightnessb down, MSI P1 Fn + F4
    DEADKEY,  // e0 70
    DEADKEY,  // e0 71  Acer Touchpad on Fn + F7
    DEADKEY,  // e0 72  Acer Touchpad off Fn + F7
    DEADKEY,  // e0 73
    DEADKEY,  // e0 74
    DEADKEY,  // e0 75 
    DEADKEY,  // e0 76  MSI WiFi Fn + F8
    0x4f,     // e0 77  MSI brightness down, Samsung touchpad on/off Fn + F5
    0x4d,     // e0 78  MSI brightness up, received when Fn key is released too in Acer
    DEADKEY,  // e0 79
    DEADKEY,  // e0 7a 
    DEADKEY,  // e0 7b 
    DEADKEY,  // e0 7c
    DEADKEY,  // e0 7d 
    DEADKEY,  // e0 7e 
    DEADKEY   // e0 7f 
    
};
/*  Useful ADB codes found
    //0x82, 130 Dashboard
    //0x83, 131 Launchpad
    //0xa0, 160 Misson Control
    //0x71, 113 Brightness Up F15 Key
    //0x6B, 107 Brightness Down F14 Key
    //0x7F, Power Button

 Modifier Keys Codes - Hex ScanCode (dec) -> Hex ADB Code (dec)
   Left Control     -  1d (29)  -> 3b (59)
   Right Control    -  60 (96)  -> 3e (62)
   Left Window      -  70 (112) -> 37 (55)
   Right Window     -  71 (113) -> 36 (54)
   Left ALT         -  38 (56)  -> 3a (58)
   Right ALT        -  61 (97)  -> 3d (61)
*/

/*
Usage of Apple function keys (from wikipedia)
F1	F2	F3	F4	F5	F6	F7	F8	F9	F10	F11	F12

(2003-2006)	
Brightness Down
Brightness Up
Mute
Volume Down
Volume Up
Num Lock
Display Switch
All windows (Exposé)
Application windows (Exposé)
Show Desktop (Exposé)
Dashboard

(2006-2011)
Brightness Down
Brightness Up
Exposé
Dashboard
Rewind
Play/Pause
Fast Forward
Mute
Volume Down
Volume Up

MacBook Air (2010-2011)
Brightness Down
Brightness Up
Exposé
Dashboard
Rewind
Play/Pause
Fast Forward
Mute
Volume Down
Volume Up
Eject

(2011-)
Brightness Down
Brightness Up
Mission Control
Launchpad
Keyboard Backlit Down
Keyboard Backlit Up
Rewind 
Play/Pause
Fast Forward
Mute
Volume Down
Volume Up
*/

#endif /* !_APPLEPS2TOADBMAP_H */
