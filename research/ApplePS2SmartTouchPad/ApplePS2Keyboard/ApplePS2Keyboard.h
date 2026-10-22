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

#ifndef _APPLEPS2KEYBOARD_H
#define _APPLEPS2KEYBOARD_H

#include "ApplePS2KeyboardDevice.h"

#include <IOKit/IONVRAM.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

// #include <libkern/c++/OSBoolean.h>

#define MODIFIERS_STATUS_CHK_INTERVAL 4000000000 // 4 sec, going above 4 won't compile in 32bit

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Definitions used to keep track of key state.   Key up/down state is tracked
// in a bit list.  Bits are set for key-down, and cleared for key-up.  The bit
// vector and macros for it's manipulation are defined here.
//

#define KBV_NUM_KEYCODES        256
#define KBV_BITS_PER_UNIT       32     // for UInt32
#define KBV_BITS_MASK           31
#define KBV_BITS_SHIFT          5      // 1<<5 == 32, for cheap divide
#define KBV_NUNITS ((KBV_NUM_KEYCODES + \
(KBV_BITS_PER_UNIT-1))/KBV_BITS_PER_UNIT)

#define KBV_KEYDOWN(n, bits) \
(bits)[((n)>>KBV_BITS_SHIFT)] |= (1 << ((n) & KBV_BITS_MASK))

#define KBV_KEYUP(n, bits) \
(bits)[((n)>>KBV_BITS_SHIFT)] &= ~(1 << ((n) & KBV_BITS_MASK))

#define KBV_IS_KEYDOWN(n, bits) \
(((bits)[((n)>>KBV_BITS_SHIFT)] & (1 << ((n) & KBV_BITS_MASK))) != 0)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ApplePS2Keyboard Class Declaration
//

class ApplePS2Keyboard : public IOHIKeyboard
{
    OSDeclareDefaultStructors(ApplePS2Keyboard);
    
private:
    ApplePS2KeyboardDevice * _device;
    
    AbsoluteTime now;
    
    UInt8           _interruptHandlerInstalled:1;
    UInt8           _powerControlHandlerInstalled:1;
    UInt8           _TouchpadNotificationHandlerInstalled:1;
    UInt8           _ledState;
    UInt8           _debug;
    UInt8           _extendCount;
    UInt32          _keyboardTypeID;
    UInt32          _keyBitVector[KBV_NUNITS];
    UInt8           _KeysDown;

    UInt8           _osxVersion;

    clock_sec_t     _lastEventSecs;
    clock_usec_t    _lastEventMicrosecs;
    uint64_t        _lastModifiersReadTime;
    uint64_t        _lastKeyPressTime;
    
    SInt8           _kbrdBackLightLvl;
    
    bool            isKeyboardEnabled;
    bool            asusKBHas16LevelSupport;
    bool            asusFnKeyWorkAsFunction;
    bool            isTPadEnabled, featureToggleMode;
    bool            isNumPadKey, isAccInputExceptionKey;
    bool            isNumPadLocked, isCapsLocked, noNumLockLED;
    bool            isFnKeyPressed, isFnKeyComb;
    bool            hasExtendFunctions;
    bool            germanTurkishLayout;
    bool            makeKeyDotAsComma;
    bool            altFunctionEnabled, controllingFeature;
    bool            useFinerBrightnesCtrl, useFinerVolumeCtrl;
    
    UInt8           _modifierKeystate;
    UInt8           _fnKeysModeNum, _fnKeysVendorLayout;
    UInt8           _ISOLSquareBracket, _ISORSquareBracket;
    UInt8           _commandKey, _contrlKey, _altkey;
    UInt8           _letterM, _letterQ;
    UInt8           _rightCtxtMKey, _rightCntrlKey, _rightALTKey, _capsKey;
    UInt8           _brightNessUp, _brightNessDown;
    UInt8           _kbrdBrightnessUp, _kbrdBrightnessDown;
    UInt8           _volMute, _volDown, _volUp;
    UInt8           _mediaPrev, _mediaPlay, _mediaNext;
    UInt8           _sleep, _launchpadFun, _missonCtlFun, _videoMirror, _dispSleep, _touchpad;
    UInt8           _generatedCode, _tempCode1,_tempCode2;
    
    char *          fx[12]; // extended functions f13-f24 key combinations
    
    bool			macintoshMode;// swap alt and windows key?
    bool            enabledAsusKBacklight; // enable Asus Keyboard backlight support?
    bool            makeShiftCapsAsFn; // make shift + caps into Fn key?
    bool            makeDelIntoCmdBspace; // make del into command + backspace?
    bool            makeNumLockAsClear; // make Num Lockk as Clear?
    
    bool            displaySleep; // display sleep status
    bool            extendedCode; // extended scan code (with escape sequence) is received?
    bool            releaseGestureKey; // touchpad notified to release gesture key?
    bool            fixSierraCapsLockBug;
    
    virtual void setCommandByte(UInt8 setBits, UInt8 clearBits);
    virtual void setLEDs(UInt8 ledState);
    virtual void setKeyboardEnable(bool enable);
    virtual void setDevicePowerState(UInt32 whatToDo);
    
    virtual void releaseKey(UInt8 code);
    virtual void dispatchKeyboardEventWithScancode(UInt8 scanCode);
    
    // inline to improve the performance
    void dispatchKeyboardKeyCode(unsigned int keyCode, bool goingDown)
    
    {
#if APPLESDK
        clock_get_uptime(&now);
#else
        clock_get_uptime((uint64_t *)(&now));
#endif
        
        // IOLog("%s :: DISPATCHED KEY %x, State %d.\n", this->getName(), keyCode, goingDown);

        dispatchKeyboardEvent(/* Keyboard ADB code */ keyCode,
                              /* direction */ goingDown,
                              /* timeStamp */ now );
    }
    
    // configurations
    void ReadKeyboardPreferences();
    void ReadFnKeyConfig();
    void ReadFnKeyLayout();
    void ReadExtendedFunctionsConfig();
    
    // German/Turkish custom layout mapping
    UInt8 MapGermanTurkishKeyboardLayout(UInt8 keyCode, bool goingDown);
    
    // Extended Functions
    bool  ProcessExtendedFunctions(UInt8 keyCode, bool goingDown);
    void  ProcessKeyCombinations(const char *keys, bool goingDown, bool gesture);
    UInt8 ADBCodeForKeyChar(const char key);
    
    // Touchpad handler
    void receiveTouchpadNotifications(UInt32 data);
    void ToggleTouchpadFeatures(unsigned int keyCode, bool goingDown);
    
    // Fn Notifications
    UInt8 setupFnkeys(const char * key);
    void  processFnKeys(UInt32 keyCode, bool keyDown);
    void  processFnkeySleep();
    void  checkAsusFnKeysMode();
    
    // Asus Keyboard backlight
    void  processAsusKBacklight(bool LvlUp);
    SInt16 getMaxKBlightLevel();
    SInt16 readAsusKBlight(UInt8 reqParm);
    void  setAsusKBlight(UInt8 lvl);
    void  dispatchKeyboardBacklightNotification(bool keyState);
    
    // keyboard Modifiers status
    void readKeyboardModifiers();
    UInt8 getModifiersPosition(UInt32 modifierPos)
    {
        if(modifierPos == NX_MODIFIERKEY_ALPHALOCK)
            return 0x3A;
        else if(modifierPos == NX_MODIFIERKEY_SHIFT)
            return 0x2A;
        else if(modifierPos == NX_MODIFIERKEY_CONTROL)
            return 0x1D;
        else if(modifierPos == NX_MODIFIERKEY_ALTERNATE)
            return 0x38;
        else if(modifierPos == NX_MODIFIERKEY_COMMAND)
            return 0x5B;
        else
            return 0x0;
    }
    
    // Manual ADB Code generate method, just for knowing what a particular ADB code does in OSX
    void generateAndSeeADBCode(UInt32 keycode, bool goingDown);
    
    
protected:
    void Get_SmartKeyboard_Access() {};
    
    // Keyboard methods
    virtual const unsigned char * defaultKeymapOfLength(UInt32 * length);
    virtual void setAlphaLockFeedback(bool locked);
    virtual void setNumLockFeedback(bool locked);
#if _osxVersion > 11
    virtual bool doesKeyLock(unsigned key);
#endif
    virtual UInt32 maxKeyCodes();
    
    // NVRAM
    void LoadValuesFromNVRAM();
    void saveValueToNVRAM(const char * symbol, UInt8 value);
    
public:
    // IOKit methods
    virtual bool init(OSDictionary * properties);
    virtual ApplePS2Keyboard * probe(IOService * provider, SInt32 * score);
    virtual bool start(IOService * provider);
    virtual void stop(IOService * provider);
    
    // virtual bool 	 updateProperties( void );
    // virtual IOReturn  setParamProperties(OSDictionary * dict);
    // virtual IOReturn  setProperties( OSObject * properties );
    
    // IOHIDevice methods
    virtual UInt32 deviceType();
    virtual UInt32 interfaceID();
    
    // PS2 interrupt handler
    void interruptOccurred(UInt8 scanCode);
    
};

#endif /* _APPLEPS2KEYBOARD_H */
