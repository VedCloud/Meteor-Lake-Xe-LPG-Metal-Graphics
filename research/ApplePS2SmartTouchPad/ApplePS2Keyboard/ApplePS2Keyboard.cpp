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


#include <IOKit/assert.h>
#include <IOKit/IOLib.h>
#include <IOKit/hidsystem/IOHIDTypes.h>
#include <IOKit/hidsystem/IOHIDParameter.h>
#include <IOKit/hidsystem/IOLLEvent.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <IOKit/pwr_mgt/RootDomain.h>
#include <IOKit/hidsystem/ev_keymap.h>
#include "ApplePS2Keyboard.h"
#include "ApplePS2ToADBMap.h"

#define DEBUG_KBRD 1

#if DEBUG_KBRD
#define DEBUG_LOG(fmt, args...) if (_debug == 1) IOLog(fmt, ## args)
#else
#define DEBUG_LOG(fmt, args...)
#endif

/* Fn keys mode plist keys */
#define SystemSleep     "SystemSleep"
#define BrightnessDown  "BrightnessDown"
#define BrightnessUp    "BrightnessUp"
#define MissonControl   "Misson Control"
#define Launchpad       "Launchpad"
#define SleepDisplay    "SleepDisplay"
#define VideoMirror     "VideoMirror"
#define Touchpad        "Touchpad"
#define VolumeMute      "VolumeMute"
#define VolumeDown      "VolumeDown"
#define VolumeUp        "VolumeUp"
#define MediaPrevious   "MediaPrevious"
#define MediaPlayPause  "MediaPlayPause"
#define MediaNext       "MediaNext"
#define KBBacklightUp   "KBBacklightUp"
#define KBBacklightDown "KBBacklightDown"

#define ASUS        1
#define DELL        2
#define ACER        3
#define SAMSUNG     4
#define MSI         5
#define GIGABYTE    6
#define LENOVO      7
#define HP          8
#define LG          9

/* Keyboard preferences */
#define SwapAltAndWindowKeys        "Swap alt and windows key"
#define EnableExtendedFunctions     "Enable Extended Functions"
#define MakeShiftAndCapsAsFn        "Make shift + caps into Fn key"
#define MakeDelKeyIntoCmdAndSpace   "Make delete key into cmd + backspace"
#define MakeContextMenuKeyInto      "Make context menu key into key"
#define MakeRightControlKeyInto     "Make right control into key"
#define MakeRightAltKeyInto         "Make right alt into key"
#define MakeCapsLockInto            "Make Caps Lock into key"
#define MakeISOKeypadKeyInto        "Make ISO keypad key . to ,"
#define FinerFnBrightnessControl    "FinerFnBrightnessControl"
#define FinerFnVolumeControl        "FinerFnVolumeControl"
#define NumLockEnabledAtBoot        "Num Lock enabled at boot"
#define DisableNumLockLED           "Disable Num Lock LED"
#define MakeNumLockintoClear        "Make Num Lock into Clear"
#define EnableSierraCapsLockFix     "Enable Sierra Caps Lock Fix"

// =============================================================================
// ApplePS2Keyboard Class Implementation
//

#define super IOHIKeyboard
#define APPLEPS2KEYBOARD_DEVICE_TYPE	0x1B

/* Following table is used to convert Apple USB keyboard IDs into a numbering
 scheme that can be combined with ADB handler IDs for both Cocoa and Carbon */
enum {
    kgestUSBUnknownANSIkd   = 3,       /* (0x03( Unknown ANSI keyboard */
    kgestUSBGenericANSIkd   = 40,      /* (0x28) Generic ANSI keyboard */
    kgestUSBGenericISOkd    = 41,      /* (0x29) Generic ANSI keyboard */
    kgestUSBGenericJISkd    = 42,      /* (0x2A) Generic ANSI keyboard */
    
    kgestUSBCosmoANSIKbd    = 198,      /* (0xC6) Gestalt Cosmo USB Domestic (ANSI) Keyboard */
    kgestUSBCosmoISOKbd     = 199,      /* (0xC7) Cosmo USB International (ISO) Keyboard */
    kgestUSBCosmoJISKbd     = 200,      /* (0xC8) Cosmo USB Japanese (JIS) Keyboard */
    kgestUSBAndyANSIKbd     = 204,      /* (0xCC) Andy USB Keyboard Domestic (ANSI) Keyboard */
    kgestUSBAndyISOKbd      = 205,      /* (0xCD) Andy USB Keyboard International (ISO) Keyboard */
    kgestUSBAndyJISKbd      = 206,      /* (0xCE) Andy USB Keyboard Japanese (JIS) Keyboard */
    
    kgestQ6ANSIKbd          = 31,      /* (0x1F) Apple Q6 Keyboard Domestic (ANSI) Keyboard */
    kgestQ6ISOKbd           = 32,      /* (0x20) Apple Q6 Keyboard International (ISO) Keyboard */
    kgestQ6JISKbd           = 33,      /* (0x21) Apple Q6 Keyboard Japanese (JIS) Keyboard */
    
    /* Apple Q30 Keyboard Domestic (ANSI) (or) USB Pro Keyboard w/ F16 key Domestic (ANSI) */
    kgestQ30ANSIKbd         = 34, /* (0x22) */
    /* Apple Q30 Keyboard International (ISO) (or) USB Pro Keyboard w/ F16 key International (ISO) */
    kgestQ30ISOKbd          = 35, /* (0x23) */
    /* Apple Q30 Keyboard Japanese (JIS) (or) USB Pro Keyboard w/ F16 key Japanese (JIS) */
    kgestQ30JISKbd          = 36, /* (0x24) */
    
    kgestSantaFountainANSIKbd    = 37,   /* (0x25) Apple Santa/Fountain Keyboard Domestic (ANSI) Keyboard */
    kgestSantaFountainISOKbd     = 38,   /* (0x26) Apple Santa/Fountain Keyboard International (ISO) Keyboard */
    kgestSantaFountainJISKbd     = 39,   /* (0x27) Apple Santa/Fountain Keyboard Japanese (JIS) Keyboard */
    
    kgestM90ISOKbd          = 44,      /* (0x2C) Apple M90 Wireless (ISO) Keyboard */
    kgestM89ISOKbd          = 47,      /* (0x2F) Apple M89 Wired (ISO) Keyboard */
    
    kgestCusttomISOKbd1   =  48,    /* (0x30) German and (may be Turkish) ISO Keyboard */
    kgestCusttomISOKbd2   =  49,    /* (0x31) Spanish and Italian ISO Keyboard */
    kgestCusttomISOKbd3   =  50,    /* (0x32) French 1 ISO Keyboard */
    kgestCusttomISOKbd4   =  52,    /* (0x34) French 2 ISO Keyboard */
    kgestCusttomANSIKbd1  =  51     /* (0x33) Brazilian ABNT2 Keyboard */
};

OSDefineMetaClassAndStructors(ApplePS2Keyboard, IOHIKeyboard);


UInt32 ApplePS2Keyboard::deviceType()
{
    UInt32      id = APPLEPS2KEYBOARD_DEVICE_TYPE; // Default PS2 keyboard type
    
    _keyboardTypeID = 0;
    
    // Just return
    // the already set value.  This should prevent us from mistakenly changing
    // changing a deviceType(keyboardType) value back to an unknow value after
    // it has been set via MacBuddy or the keyboardPref.
    
    //Info.plist key is <integer>, not <string>
    
    OSNumber *pID = OSDynamicCast(OSNumber, getProperty("Keyboard type (ID)"));
    
    if (pID)
        _keyboardTypeID = pID->unsigned32BitValue();
    
    if (_keyboardTypeID > 0)
        id = _keyboardTypeID;
    
    else {
        // Keyboard type ID value is not specified in plist?
        if (kOSBooleanTrue == getProperty("Use ISO Layout"))
            id = 0xC7; // Cosmo ISO keyboard
        else
            id = 0x1B; // PS2 keyboard for ANSI
    }
    
    /*
     * Custom layout made by me for some ISO keyboard keys mapping with Cosmo layout as base
     * ISO requires ALT/Option hold for [ and ]
     */
    
    switch (id) {
            // German/Turkish
        case 0x30:
            _ISOLSquareBracket = 0x17; // 0x06 Scan code
            _ISORSquareBracket = 0x16; // 0x07 Scan code
            id = 0xC7;
            break;
            // Spanish/Italian
        case 0x31:
            _ISOLSquareBracket = 0x21; // 0x1a Scan code (same as US but requires ALT)
            _ISORSquareBracket = 0x1e; // 0x1b Scan code (same as US but requires ALT)
            id = 0xC7;
            break;
            
            // French
        case 0x32:
            _letterM = 0x29; // 0x27 Scan code
            _letterQ = 0x00; // 0x1e Scan code
        case 0x34:
            _ISOLSquareBracket = 0x17; // 0x06 Scan code
            _ISORSquareBracket = 0x1b; // 0x0c Scan code
            id = 0xC7;
            break;
            
            // Brazilian ABNT2
        case 0x33:
            _ISOLSquareBracket = 0x1e; // 0x1b Scan code
            _ISORSquareBracket = 0x2a; // 0x2b Scan code
            id = 0x1B;
            break;
            
            // ANSI
        default:
            _ISOLSquareBracket = 0x21;
            _ISORSquareBracket = 0x1e;
            break;
    }
    
    DEBUG_LOG("%s :: ID: %x LS  0x%02x, RS  0x%02x, M 0x%02x, Q 0x%02x\n",this->getName(), id, _ISOLSquareBracket, _ISORSquareBracket, _letterM, _letterQ);
    
    // ISO specific mappign to match ADB keyboards
    // This should really be done in the keymaps.
    switch ( id )
    {
        case kgestUSBCosmoISOKbd:
        case kgestUSBAndyISOKbd:
        case kgestQ6ISOKbd:
        case kgestQ30ISOKbd:
        case kgestM89ISOKbd:
        case kgestUSBGenericISOkd:
            // Changing Keyboard layout with ISO Specific Mappings
            if (kOSBooleanTrue == getProperty("Use ISO Layout")) {
                PS2ToADBMap[0x29] = 0x0a;
                PS2ToADBMap[0x56] = 0x32;
                DEBUG_LOG("%s :: Keyboard layout set to ISO with ID 0x%02x\n", this->getName(), (unsigned int)id);
            }
            break;
        default:
            PS2ToADBMap[0x29] = 0x32;
            PS2ToADBMap[0x56] = 0x0a;
            DEBUG_LOG("%s :: Keyboard layout set to ANSI with ID 0x%02x\n", this->getName(), (unsigned int)id);
            break;
    }
    
    return id;
};

UInt32 ApplePS2Keyboard::interfaceID() { return NX_EVS_DEVICE_INTERFACE_ADB; };

UInt32 ApplePS2Keyboard::maxKeyCodes() { return KBV_NUM_KEYCODES; };


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2Keyboard::init(OSDictionary * properties)
{
    //
    // Initialize this object's minimal state.  This is invoked right after this
    // object is instantiated.
    //
    
    if (!super::init(properties))  return false;
    
    IOLog("STD :: Initializing keyboard...\n");
    
    _debug                     = 0;
    _device                    = 0;
    _extendCount               = 0;
    _interruptHandlerInstalled              = false;
    _TouchpadNotificationHandlerInstalled   = false;
    isKeyboardEnabled          = true;
    
    isCapsLocked               = false;
    
    isNumPadLocked             = false;
    isAccInputExceptionKey     = false;
    isTPadEnabled              = true;
    isFnKeyPressed             = false;
    isNumPadKey                = false;
    displaySleep               = false;
    releaseGestureKey          = false;
    germanTurkishLayout        = false;
    makeKeyDotAsComma          = false;
    useFinerBrightnesCtrl      = false;
    useFinerVolumeCtrl         = false;
    
    altFunctionEnabled         = false;
    controllingFeature         = false;
    
    enabledAsusKBacklight      = false;
    asusKBHas16LevelSupport    = false;
    
    fixSierraCapsLockBug       = true;
    
    _modifierKeystate       = 0;
    
    _rightCtxtMKey          = 0x5d;
    _rightCntrlKey          = 0x1d;
    _commandKey             = 0x5B;
    _contrlKey              = 0x1d;
    _altkey                 = 0x38;
    
    _letterM        =   0x2e;
    _letterQ        =   0x0c;
    
    _lastModifiersReadTime = 0;
    _lastKeyPressTime = 0;
    
    _ledState = 0;
    _generatedCode = _tempCode1 = _tempCode2 = 0;
    
    _osxVersion = 12; // Init with Sierra default
    
    bzero(_keyBitVector, sizeof(_keyBitVector));
    
    for (int i = 0; i < 12; i++)  fx[i] = 0;
    
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ApplePS2Keyboard * ApplePS2Keyboard::probe(IOService * provider, SInt32 * score)
{
    //
    // The driver has been instructed to verify the presence of the actual
    // hardware we represent. We are guaranteed by the controller that the
    // keyboard clock is enabled and the keyboard itself is disabled (thus
    // it won't send any asynchronous scan codes that may mess up the
    // responses expected by the commands we send it).  This is invoked
    // after the init.
    //
    
    ApplePS2KeyboardDevice * device  = (ApplePS2KeyboardDevice *)provider;
    PS2Request *             request = device->allocateRequest();
    bool                     success;
    
    if (!super::probe(provider, score))  return 0;
    
    DEBUG_LOG("%s :: Probing keyboard...\n", this->getName());
    
    //
    // Check to see if the keyboard responds to a basic diagnostic echo.
    //
    
    // (diagnostic echo command)
    request->commands[0].command = kPS2C_WriteDataPort;
    request->commands[0].inOrOut = kDP_TestKeyboardEcho;
    request->commands[1].command = kPS2C_ReadDataPortAndCompare;
    request->commands[1].inOrOut = 0xEE;
    request->commandsCount = 2;
    device->submitRequestAndBlock(request);
    
    // (free the request)
    success = (request->commandsCount == 2);
    
    int retryCount = 0;
    while (!success) {
        // (diagnostic echo command) - try again
        request->commands[0].command = kPS2C_WriteDataPort;
        request->commands[0].inOrOut = kDP_TestKeyboardEcho;
        request->commands[1].command = kPS2C_ReadDataPortAndCompare;
        request->commands[1].inOrOut = 0xEE;
        request->commandsCount = 2;
        device->submitRequestAndBlock(request);
        
        // (free the request)
        success = (request->commandsCount == 2);
        
        if (retryCount == 3)
            break;
        
        retryCount++;
        
        if (!success) {
            DEBUG_LOG("%s :: Echo failed, retry attempt %d\n", getName(),retryCount);
            IOSleep(1500);
        }
    }
    
    device->freeRequest(request);
    
    
    // return (success) ? this : 0;
    
    IOLog("STD :: Keyboard echo %s.\n", success?"success":"failed, still trying to enable...");
    
    return this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2Keyboard::start(IOService * provider)
{
    
    // Debug mode enabled?
    if (kOSBooleanTrue == getProperty("Debug"))
        _debug = 1;
    
    //
    // The driver has been instructed to start.   This is called after a
    // successful attach.
    //
    
    if (!super::start(provider))  return false;
    
    DEBUG_LOG("%s :: Start\n", this->getName());
    
    //
    // Maintain a pointer to and retain the provider object.
    //
    
    _device = (ApplePS2KeyboardDevice *)provider;
    _device->retain();
    
    //
    // Initialize the Keyboard
    //
    
    // Reset the keyboard to its default state.
    
    PS2Request * request = _device->allocateRequest();
    if (request)
    {
        request->commands[0].command = kPS2C_WriteDataPort;
        request->commands[0].inOrOut = kDP_SetDefaults;
        request->commands[1].command = kPS2C_ReadDataPortAndCompare;
        request->commands[1].inOrOut = kSC_Acknowledge;
        request->commandsCount = 2;
        _device->submitRequestAndBlock(request);
        _device->freeRequest(request);
    }
    
    //
    // Initialize the keyboard LED state.
    //
    
    setLEDs(_ledState);
    
    //
    // Read Keyboard preferences
    //
    
    ReadKeyboardPreferences();
    
    //
    // Read extended functions
    //
    
    if (hasExtendFunctions)
        ReadExtendedFunctionsConfig();
    
    //
    // Read Keyboard modifier positions
    //
    
    readKeyboardModifiers();
    
    //
    // Read Fn keys mode
    //
    OSNumber *fnMode = OSDynamicCast(OSNumber, getProperty("Fn keys Mode"));
    
    if (fnMode)
    {
        _fnKeysModeNum = fnMode->unsigned16BitValue();
        
        //
        // Read Fn key mapping layout from plist.
        //
        
        ReadFnKeyLayout();
        
        //
        // Disable sleep in
        // Apple Mode 1(without fn key).
        //
        
        if (_fnKeysModeNum < 2)
            _sleep = 0; // no mapping
        
        //
        // Read Fn keys config of mode 1 & 2
        //
        
        if (_fnKeysModeNum < 3)
            ReadFnKeyConfig();
    }
    else {
        // disabled fn key mode
        _fnKeysModeNum = 0;
    }
    
    //
    // Install our driver's interrupt handler, for asynchronous data delivery.
    //
    
    _device->installInterruptAction(this, OSMemberFunctionCast(PS2InterruptAction, this, &ApplePS2Keyboard::interruptOccurred));
    _interruptHandlerInstalled = true;
    
    
    //
    // Install our power control handler.
    //
    
    _device->installPowerControlAction(this, OSMemberFunctionCast(PS2PowerControlAction, this, &ApplePS2Keyboard::setDevicePowerState ));
    
    _powerControlHandlerInstalled = true;
    
    //
    // Installing Touchpad Notification Handler
    //
    _device->installPS2NotificationAction(this, OSMemberFunctionCast(PS2NotificationAction, this, &ApplePS2Keyboard::receiveTouchpadNotifications ));
    
    _TouchpadNotificationHandlerInstalled = true;
    
    
    //
    // Enable the keyboard clock (should already be so), the keyboard IRQ line,
    // and the keyboard Kscan -> scan code translation mode.
    //
    
    setCommandByte(kCB_EnableKeyboardIRQ | kCB_TranslateMode,
                   kCB_DisableKeyboardClock);
    
    //
    // Finally, we enable the keyboard itself, so that it may start reporting
    // key events.
    //
    
    setNumLockFeedback(isNumPadLocked);
    setKeyboardEnable(true);
    
    
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::stop(IOService * provider)
{
    //
    // The driver has been instructed to stop.  Note that we must break all
    // connections to other service objects now (ie. no registered actions,
    // no pointers and retains to objects, etc), if any.
    //
    
    DEBUG_LOG("%s :: Stop\n", this->getName());
    
    assert(_device == provider);
    
    //
    // Disable the keyboard itself, so that it may stop reporting key events.
    //
    
    setKeyboardEnable(false);
    
    
    //
    // Disable the keyboard clock and the keyboard IRQ line.
    //
    
    setCommandByte(kCB_DisableKeyboardClock, kCB_EnableKeyboardIRQ);
    
    
    //
    // Uninstall the interrupt handler.
    //
    
    if ( _interruptHandlerInstalled )  _device->uninstallInterruptAction();
    _interruptHandlerInstalled = false;
    
    //
    // Uninstall the power control handler.
    //
    
    if ( _powerControlHandlerInstalled ) _device->uninstallPowerControlAction();
    _powerControlHandlerInstalled = false;
    
    //
    // Unistalling our Mouse/Touchpad Notification Handler
    //
    _device->unistallPS2NotificationAction();
    _TouchpadNotificationHandlerInstalled = false;
    
    //
    // Release the pointer to the provider object.
    //
    
    OSSafeReleaseNULL(_device);
    
    
    super::stop(provider);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*IOReturn ApplePS2Keyboard::setProperties(OSObject *properties)
 {
 OSDictionary *kbd_dict;
 
 IOLog("Keyboard Set\n");
 
 if ((kbd_dict=OSDynamicCast (OSDictionary, properties)))
 return super::setParamProperties (kbd_dict);
 
	return kIOReturnError;
 
 }
 
 bool ApplePS2Keyboard::updateProperties()
 {
 IOLog("Keyboard Updated\n");
 return super::updateProperties();
 }*/


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
 * Method reading keyboard plist config on startup
 */
void ApplePS2Keyboard::ReadKeyboardPreferences()
{
    OSDictionary    *kbPrefs;
    
    kbPrefs = OSDynamicCast(OSDictionary, getProperty("Preferences"));
    if (kbPrefs)
    {
        OSIterator      *dictKeyIter = 0;
        OSBoolean       *dictKeyBool = 0;
        OSNumber        *dictKeyNum  = 0;
        const OSSymbol  *dictKey = 0;
        const char      *keyStr = NULL;
        
        bool    kbPrefBool;
        UInt32  kbPrefValue;
        
        dictKeyIter = OSCollectionIterator::withCollection(kbPrefs);
        if (!dictKeyIter) {
            kbPrefs->release();
            return;
        }
        
        while ((dictKey = (const OSSymbol *)dictKeyIter->getNextObject()))
        {
            dictKeyNum = 0; dictKeyBool = 0;
            keyStr = dictKey->getCStringNoCopy();
            
            dictKeyBool = OSDynamicCast(OSBoolean, kbPrefs->getObject(dictKey));
            
            if (dictKeyBool) {
                kbPrefBool = (bool)dictKeyBool->getValue();
                
                if (!strncmp(keyStr, SwapAltAndWindowKeys,strlen(keyStr)))
                    macintoshMode = kbPrefBool;
                
                else if (!strncmp(keyStr, EnableExtendedFunctions,strlen(keyStr)))
                    hasExtendFunctions = kbPrefBool;
                
                else if (!strncmp(keyStr, MakeShiftAndCapsAsFn, strlen(keyStr)))
                    makeShiftCapsAsFn = kbPrefBool;
                
                else if (!strncmp(keyStr, MakeDelKeyIntoCmdAndSpace, strlen(keyStr)))
                    makeDelIntoCmdBspace = kbPrefBool;
                
                else if (!strncmp(keyStr, MakeISOKeypadKeyInto, strlen(keyStr)) )
                    makeKeyDotAsComma = kbPrefBool;
                
                else if (!strncmp(keyStr, NumLockEnabledAtBoot, strlen(keyStr))) {
                    isNumPadLocked = kbPrefBool;
                }
                
                else if (!strncmp(keyStr, DisableNumLockLED, strlen(keyStr)))
                    noNumLockLED = kbPrefBool;
                
                else if (!strncmp(keyStr, MakeNumLockintoClear, strlen(keyStr)))
                    makeNumLockAsClear = kbPrefBool;
                
                else if (!strncmp(keyStr, FinerFnBrightnessControl, strlen(keyStr)))
                    useFinerBrightnesCtrl = kbPrefBool;
                
                else if (!strncmp(keyStr, FinerFnVolumeControl, strlen(keyStr)))
                    useFinerVolumeCtrl = kbPrefBool;
                
                else if (!strncmp(keyStr, EnableSierraCapsLockFix, strlen(keyStr)))
                    fixSierraCapsLockBug = kbPrefBool;
            }
            
            dictKeyNum = OSDynamicCast(OSNumber, kbPrefs->getObject(dictKey));
            if (dictKeyNum) {
                kbPrefValue = dictKeyNum->unsigned32BitValue();
                
                if (kbPrefValue > 0)
                {
                    if (!strncmp(keyStr, MakeContextMenuKeyInto, strlen(keyStr)))
                        _rightCtxtMKey = kbPrefValue;
                    
                    else if (!strncmp(keyStr, MakeRightControlKeyInto, strlen(keyStr)) )
                        _rightCntrlKey = kbPrefValue;
                    
                    else if (!strncmp(keyStr, MakeRightAltKeyInto, strlen(keyStr)) )
                        _rightALTKey = kbPrefValue;
                    
                    else if (!strncmp(keyStr, MakeCapsLockInto, strlen(keyStr)) )
                        _capsKey = kbPrefValue;
                }
            }
        }
        OSSafeReleaseNULL(dictKeyIter);
    }
}


void ApplePS2Keyboard::ReadFnKeyConfig()
{
    OSDictionary    *fnKeyConfig;
    
    // Apple Mode (without pressing fn key)
    if (_fnKeysModeNum == 1)
        fnKeyConfig = OSDynamicCast(OSDictionary, getProperty("Fn Keys Mode 1"));
    
    // Normal mode with Fn key
    else
        fnKeyConfig = OSDynamicCast(OSDictionary, getProperty("Fn Keys Mode 2"));
    
    if (fnKeyConfig)
    {
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(SystemSleep))) {
            _sleep = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(BrightnessDown))) {
            _brightNessDown = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(BrightnessUp))) {
            _brightNessUp = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(MissonControl))) {
            _missonCtlFun = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(Launchpad))) {
            _launchpadFun = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(SleepDisplay))) {
            _dispSleep = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(VideoMirror))) {
            _videoMirror = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(Touchpad))) {
            _touchpad = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(VolumeMute))) {
            _volMute = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(VolumeDown))) {
            _volDown = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(VolumeUp))) {
            _volUp = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(MediaPrevious))) {
            _mediaPrev = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(MediaPlayPause))) {
            _mediaPlay = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(MediaNext))) {
            _mediaNext = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(KBBacklightDown))) {
            _kbrdBrightnessDown = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
        
        if (OSString *keyStrVal = OSDynamicCast(OSString, fnKeyConfig->getObject(KBBacklightUp))) {
            _kbrdBrightnessUp = setupFnkeys(keyStrVal->getCStringNoCopy());
        }
    }
}

void ApplePS2Keyboard::ReadFnKeyLayout()
{
    // Real Fn keys layout
    OSString *fnKeyLayout = OSDynamicCast(OSString, getProperty("Fn keys Layout"));
    if (fnKeyLayout)
    {
        if (!strncmp(fnKeyLayout->getCStringNoCopy(),"NONE", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = 0;
            
            _sleep          = DEADKEY;
            _missonCtlFun   = DEADKEY;
            _launchpadFun   = DEADKEY;
            _dispSleep      = DEADKEY;
            _videoMirror    = DEADKEY;
            _touchpad       = DEADKEY;
            
            _brightNessDown = DEADKEY;
            _brightNessUp   = DEADKEY;
            
            _volMute        = DEADKEY;
            _volDown        = DEADKEY;
            _volUp          = DEADKEY;
            
            _mediaPrev      = DEADKEY;
            _mediaPlay      = DEADKEY;
            _mediaNext      = DEADKEY;
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "ASUS", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = ASUS;
            _fnKeysModeNum = 3;
            
            _sleep          = 0x3b; //F1
            if (enabledAsusKBacklight)
            {
                _missonCtlFun   = DEADKEY; //F3
                _launchpadFun   = DEADKEY; //F4
                _kbrdBrightnessDown   = 0x3d; //F3
                _kbrdBrightnessUp     = 0x3e; //F4
            }
            else
            {
                _missonCtlFun     = 0x3d; //F3
                _launchpadFun     = 0x3e; //F4
            }
            
            _dispSleep      = 0x41; //F7
            _videoMirror    = 0x42; //F8
            _touchpad       = 0x43; //F9
            
            _brightNessDown = 0x3f; //F5
            _brightNessUp   = 0x40; //F6
            
            _volMute        = 0x44; //F10
            _volDown        = 0x57; //F11
            _volUp          = 0x58; //F12
            
            _mediaPrev      = 0x4b; //Left Arrow
            _mediaPlay      = 0x50; //Down Arrow
            _mediaNext      = 0x4d; //Right Arrow
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "DELL", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = DELL;
            _fnKeysModeNum = 3;
            
            ExtendedPS2ToADBMap[0x08] = DEADKEY; // WiFi key
            ExtendedPS2ToADBMap[0x59] = DEADKEY; // Touchpad key
            
            _sleep          = 0x4b; //Left Arrow
            _missonCtlFun   = 0x40; //F6
            _launchpadFun   = 0x41; //F7
            _dispSleep      = 0x4d; //Right Arrow
            _videoMirror    = 0x3b; //F1
            _touchpad       = 0x3d; //F3
            
            _brightNessDown = 0x3e; //F4
            _brightNessUp   = 0x3f; //F5
            
            _volMute        = 0x50; //Down Arrow
            _volDown        = 0x57; //F11
            _volUp          = 0x58; //F12
            
            _mediaPrev      = 0x42; //F8
            _mediaPlay      = 0x43; //F9
            _mediaNext      = 0x44; //F10
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "ACER", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = ACER;
            _fnKeysModeNum = 3;
            ExtendedPS2ToADBMap[0x78] = DEADKEY; // Fn key release
            
            _sleep          = 0x3e; //F4
            _missonCtlFun   = 0x3c; //F2
            _launchpadFun   = 0x3d; //F3
            _dispSleep      = 0x40; //F6
            _videoMirror    = 0x3f; //F5
            _touchpad       = 0x41; //F7
            
            _brightNessDown = 0x4b; //Left Arrow
            _brightNessUp   = 0x4d; //Right Arrow
            
            _volMute        = 0x42; //F8
            _volDown        = 0x50; //Down Arrow
            _volUp          = 0x48; //Up Arrow
            
            _mediaPrev      = 0x44; //F10
            _mediaPlay      = 0x57; //F11
            _mediaNext      = 0x58; //F12
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "SAMSUNG", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = SAMSUNG;
            _fnKeysModeNum = 3;
            
            ExtendedPS2ToADBMap[0x77] = DEADKEY; // Touchpad key
            ExtendedPS2ToADBMap[0x29] = DEADKEY; // Fn toggle off
            
            _sleep          = 0x57; //F11
            _missonCtlFun   = 0x43; //F9
            _launchpadFun   = 0x44; //F10
            _dispSleep      = 0x3b; //F1
            _videoMirror    = 0x3e; //F4
            _touchpad       = 0x3f; //F5
            
            _brightNessDown = 0x3c; //F2
            _brightNessUp   = 0x3d; //F3
            
            _volMute        = 0x40; //F6
            _volDown        = 0x41; //F7
            _volUp          = 0x42; //F8
            
            _mediaPrev      = 0x4b; //Left Arrow
            _mediaPlay      = 0x50; //Down Arrow
            _mediaNext      = 0x4d; //Right Arrow
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "MSI", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = MSI;
            _fnKeysModeNum = 3;
            
            ExtendedPS2ToADBMap[0x6e] = DEADKEY; // Webcam key
            ExtendedPS2ToADBMap[0x6f] = DEADKEY; // P1 key
            ExtendedPS2ToADBMap[0x4e] = DEADKEY; // Battery key
            
            _sleep          = 0x58; //F12
            _missonCtlFun   = 0x41; //F7
            _launchpadFun   = 0x57; //F11
            _dispSleep      = 0x3e; //F4
            _videoMirror    = 0x3c; //F2
            _touchpad       = 0x3d; //F3
            
            _brightNessDown = 0x50; //Down Arrow
            _brightNessUp   = 0x48; //Up Arrow
            
            _volMute        = 0x52; //Keypad 0
            _volDown        = 0x4b; //Left Arrow
            _volUp          = 0x4d; //Right Arrow
            
            _mediaPrev      = 0x4b; //Left Arrow
            _mediaPlay      = 0x50; //Down Arrow
            _mediaNext      = 0x4d; //Right Arrow
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "GIGABYTE", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = GIGABYTE;
            _fnKeysModeNum = 3;
            
            _missonCtlFun   = 0x58; //F12
            _launchpadFun   = 0x57; //F11
            
            _sleep          = 0x3b; //F1
            _videoMirror    = 0x3f; //F5
            _touchpad       = 0x40; //F6
            _dispSleep      = 0x44; //F10
            
            _brightNessDown = 0x3d; //F3
            _brightNessUp   = 0x3e; //F4
            
            _volMute        = 0x41; //F7
            _volDown        = 0x42; //F8
            _volUp          = 0x43; //F9
            
            _mediaPrev      = 0x4b; //Left Arrow
            _mediaPlay      = 0x50; //Down Arrow
            _mediaNext      = 0x4d; //Right Arrow
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "LENOVO", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = LENOVO;
            _fnKeysModeNum = 3;
            
            _missonCtlFun   = 0x42; //F8
            _launchpadFun   = 0x3e; //F4
            
            _sleep          = 0x41; //F7
            _videoMirror    = 0x44; //F10
            _touchpad       = 0x40; //F6
            _dispSleep      = 0x43; //F9
            
            _brightNessDown = 0x57; //F11
            _brightNessUp   = 0x58; //F12
            
            _volMute        = 0x3b; //F1
            _volDown        = 0x3c; //F2
            _volUp          = 0x3d; //F3
            
            _mediaPrev      = 0x4b; //Left Arrow
            _mediaPlay      = 0x50; //Down Arrow
            _mediaNext      = 0x4d; //Right Arrow
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "HP", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = HP;
            _fnKeysModeNum = 3;
            
            _missonCtlFun   = 0x4b; //Left Arrow
            _launchpadFun   = 0x4d; //Right Arrow
            
            _sleep          = 0x3b; //F1
            _videoMirror    = 0x3e; //F4
            _touchpad       = 0x50; //Down Arrow
            _dispSleep      = 0x3f; //F5
            
            _brightNessDown = 0x3c; //F2
            _brightNessUp   = 0x3d; //F3
            
            _volMute        = 0x57; //F11
            _volDown        = 0x43; //F10
            _volUp          = 0x44; //F9
            
            _mediaPrev      = 0x40; //F6
            _mediaPlay      = 0x41; //F7
            _mediaNext      = 0x42; //F8
        }
        else if (!strncmp(fnKeyLayout->getCStringNoCopy(), "LG", strlen(fnKeyLayout->getCStringNoCopy())))
        {
            _fnKeysVendorLayout = LG;
            _fnKeysModeNum = 3;
            
            _missonCtlFun   = 0x42; //F8
            _launchpadFun   = 0x44; //F9
            
            _sleep          = 0x3e; //F4
            _videoMirror    = 0x41; //F7
            _touchpad       = 0x3f; //F5
            _dispSleep      = 0x40; //F6
            
            _brightNessDown = 0x3c; //F2
            _brightNessUp   = 0x3d; //F3
            
            _volMute        = 0x43; //F10
            _volDown        = 0x57; //F11
            _volUp          = 0x58; //F12
            
            _mediaPrev      = 0x4b; //Left Arrow
            _mediaPlay      = 0x50; //Down Arrow
            _mediaNext      = 0x4d; //Right Arrow
        }
        else {
            IOLog("%s :: %s Manufacturer Fn key layout is not found, using mode 2.\n", this->getName(), fnKeyLayout->getCStringNoCopy());
            
            _fnKeysModeNum = 2;
            ReadFnKeyConfig();
        }
    }
}

// Setup mapping of Fn keys to ADB code from plist
UInt8 ApplePS2Keyboard::setupFnkeys(const char * key)
{
    unsigned int codeRet = 0;
    
    if (!strncmp(key, "F1", strlen(key)))
        codeRet = 0x3b;
    else if (!strncmp(key, "F2", strlen(key)))
        codeRet = 0x3c;
    else if (!strncmp(key, "F3", strlen(key)))
        codeRet = 0x3d;
    else if (!strncmp(key, "F4", strlen(key)))
        codeRet = 0x3e;
    else if (!strncmp(key, "F5", strlen(key)))
        codeRet = 0x3f;
    else if (!strncmp(key, "F6", strlen(key)))
        codeRet = 0x40;
    else if (!strncmp(key, "F7", strlen(key)))
        codeRet = 0x41;
    else if (!strncmp(key, "F8", strlen(key)))
        codeRet = 0x42;
    else if (!strncmp(key, "F9", strlen(key)))
        codeRet = 0x43;
    else if (!strncmp(key, "F10", strlen(key)))
        codeRet = 0x44;
    else if (!strncmp(key, "F11", strlen(key)))
        codeRet = 0x57;
    else if (!strncmp(key, "F12", strlen(key)))
        codeRet = 0x58;
    else if (!strncmp(key, "ArrowLeft", strlen(key)) && _fnKeysModeNum > 1)
        codeRet = 0x4b;
    else if (!strncmp(key, "ArrowDown", strlen(key)) && _fnKeysModeNum > 1)
        codeRet = 0x50;
    else if (!strncmp(key, "ArrowUp", strlen(key)) && _fnKeysModeNum > 1)
        codeRet = 0x48;
    else if (!strncmp(key, "ArrowRight", strlen(key)) && _fnKeysModeNum > 1)
        codeRet = 0x4d;
    else if (!strncmp(key, "Pause", strlen(key)) && _fnKeysModeNum < 2)
        codeRet = 0x45;
    else
        codeRet = 0;// disabled if it has "disable" value
    
    DEBUG_LOG("%s :: Code Mapped for the Fn key %s is 0x%02x (%03d)\n", this->getName(),key,codeRet,codeRet);
    
    return codeRet;
}

void ApplePS2Keyboard::ReadExtendedFunctionsConfig()
{
    OSDictionary *extFuncConfig;
    
    // Extended Funtions F13 - F24
    extFuncConfig = OSDynamicCast(OSDictionary, getProperty("Extended Functions"));
    
    if (extFuncConfig){
        OSIterator      *dictKeyIter = 0;
        const OSSymbol  *dictKey = 0;
        OSString        *dictKeyStrObj = 0;
        const char      *dictKeyStr = NULL;
        const char      *dictKeyStrValue = NULL;
        
        dictKeyIter = OSCollectionIterator::withCollection(extFuncConfig);
        
        if (!dictKeyIter) {
            extFuncConfig->release();
            return;
        }
        
        while ((dictKey = (const OSSymbol *)dictKeyIter->getNextObject())) {
            dictKeyStrObj = 0;
            dictKeyStr = dictKey->getCStringNoCopy();
            
            dictKeyStrObj = OSDynamicCast(OSString, extFuncConfig->getObject(dictKey));
            if (dictKeyStrObj)
            {
                dictKeyStrValue = dictKeyStrObj->getCStringNoCopy();
                
                if (!strncmp(dictKeyStr, "F13", strlen(dictKeyStr)))
                    fx[0] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F14", strlen(dictKeyStr)))
                    fx[1] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F15", strlen(dictKeyStr)))
                    fx[2] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F16", strlen(dictKeyStr)))
                    fx[3] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F17", strlen(dictKeyStr)))
                    fx[4] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F18", strlen(dictKeyStr)))
                    fx[5] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F19", strlen(dictKeyStr)))
                    fx[6] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F20", strlen(dictKeyStr)))
                    fx[7] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F21", strlen(dictKeyStr)))
                    fx[8] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F22", strlen(dictKeyStr)))
                    fx[9] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F23", strlen(dictKeyStr)))
                    fx[10] = (char *)dictKeyStrValue;
                else if (!strncmp(dictKeyStr, "F24", strlen(dictKeyStr)))
                    fx[11] = (char *)dictKeyStrValue;
                
                DEBUG_LOG("%s :: Extended Funtion key %s has Key Combination : %s\n", this->getName(), dictKeyStr, dictKeyStrValue);
                
            }
            
        }
        OSSafeReleaseNULL(dictKeyIter);
    }
}
//
// Extract and dispatch the keys from the Key Combination shortcut string
//
void ApplePS2Keyboard::ProcessKeyCombinations(const char *keys, bool goingDown, bool gesture)
{
    char key[5] = {NULL};
    int k = 0, l = 0;
    UInt8 combKeyCode = DEADKEY;
    
    if (!keys)
        return;
    
    if (goingDown && !gesture)
    {
        // release shift key for Fxx key dispatch
        dispatchKeyboardKeyCode( PS2ToADBMap[0x2a], false);
    }
    
    do
    {
        // dispatch extracted key on white space/end of the string
        if (*keys == ' ' || *keys == '\0') {
            combKeyCode = DEADKEY;
            
            // get single charecter keys code
            if (k == 1)
                combKeyCode = ADBCodeForKeyChar(key[0]);
            
            // get modifier and special charecter keys code
            else {
                if (!strncmp(key, "CMD", strlen(key)))
                    combKeyCode = 0x36;
                else if (!strncmp(key, "ALT", strlen(key)))
                    combKeyCode = 0x3a;
                else if (!strncmp(key, "CTRL", strlen(key)))
                    combKeyCode = 0x3b;
                else if (!strncmp(key, "SHFT", strlen(key)))
                    combKeyCode = 0x38;
                else if (!strncmp(key, "ESC", strlen(key)))
                    combKeyCode = 0x35;
                else if (!strncmp(key, "ENTR", strlen(key)))
                    combKeyCode = 0x24;
                else if (!strncmp(key, "SPCE", strlen(key)))
                    combKeyCode = 0x31;
                else if (!strncmp(key, "DAW", strlen(key)))
                    combKeyCode = 0x7d;
                else if (!strncmp(key, "UAW", strlen(key)))
                    combKeyCode = 0x7e;
                else if (!strncmp(key, "LAW", strlen(key)))
                    combKeyCode = 0x7b;
                else if (!strncmp(key, "RAW", strlen(key)))
                    combKeyCode = 0x7c;
                else if (!strncmp(key, "TAB", strlen(key)))
                    combKeyCode = 0x30;
            }
            
            DEBUG_LOG("%s :: Dispatch key 0x%2x in Key Combination, GoingDown %s\n", this->getName(), combKeyCode,goingDown?"Yes":"No");
            
            // dispatch key
            if (combKeyCode != DEADKEY)
            {
                // dispatch Fxx
                dispatchKeyboardKeyCode( combKeyCode, goingDown);
                
                l++; // break after sending 3 keys
                if (l == 3)
                    break;
            }
            
            k = 0; // clear the key
            for (int i=0; i<5; i++) {
                key[i] = NULL;
            }
            continue;
        }
        
        // copy a charecter from the plist key combination string
        key[k] = *keys;
        k++;
        
    } while(*keys++);
    
    if (!goingDown && !gesture)
    {
        // press shift key back after Fxx dispatch
        dispatchKeyboardKeyCode( PS2ToADBMap[0x2a], true);
    }
    
}

UInt8 ApplePS2Keyboard::ADBCodeForKeyChar(const char key)
{
    UInt8 code = DEADKEY;
    
    switch (key) {
        case 'A': code = 0x00; break;
        case 'B': code = 0x0b; break;
        case 'C': code = 0x08; break;
        case 'D': code = 0x02; break;
        case 'E': code = 0x0e; break;
        case 'F': code = 0x03; break;
        case 'G': code = 0x05; break;
        case 'H': code = 0x04; break;
        case 'I': code = 0x22; break;
        case 'J': code = 0x26; break;
        case 'K': code = 0x28; break;
        case 'L': code = 0x25; break;
        case 'M': code = 0x2e; break;
        case 'N': code = 0x2d; break;
        case 'O': code = 0x1f; break;
        case 'P': code = 0x23; break;
        case 'Q': code = 0x0c; break;
        case 'R': code = 0x0f; break;
        case 'S': code = 0x01; break;
        case 'T': code = 0x11; break;
        case 'U': code = 0x20; break;
        case 'V': code = 0x09; break;
        case 'W': code = 0x0d; break;
        case 'X': code = 0x07; break;
        case 'Y': code = 0x10; break;
        case 'Z': code = 0x06; break;
            
        case '1': code = 0x12; break;
        case '2': code = 0x13; break;
        case '3': code = 0x14; break;
        case '4': code = 0x15; break;
        case '5': code = 0x17; break;
        case '6': code = 0x16; break;
        case '7': code = 0x1a; break;
        case '8': code = 0x1c; break;
        case '9': code = 0x19; break;
        case '0': code = 0x1d; break;
            
        case '`': code = 0x32; break;
        case '-': code = 0x1b; break;
        case '+': code = 0x18; break;
        case '[': code = 0x21; break;
        case ']': code = 0x1e; break;
        case '\\': code = 0x2a; break;
        case '/': code = 0x2c; break;
        case '.': code = 0x2f; break;
        case ',': code = 0x2b; break;
        case '\'': code = 0x27; break;
        case ';': code = 0x29; break;
            
        default:
            break;
    }
    
    return code;
}

void ApplePS2Keyboard::readKeyboardModifiers()
{
    
#if APPLESDK
    uint64_t timeFromLastMRead  = now -  _lastModifiersReadTime;
#else
    uint64_t timeFromLastMRead  = (*(uint64_t *)&now -  _lastModifiersReadTime);
#endif
    
    DEBUG_LOG("%s :: Modifier check Last Read %lld\n", this->getName(), timeFromLastMRead);
    
    if (timeFromLastMRead > MODIFIERS_STATUS_CHK_INTERVAL || _lastModifiersReadTime == 0)
    {
        OSDictionary * modifiersDic = 0;
        OSNumber     * modifierSrcNum = 0;
        OSNumber     * modifierDstNum = 0;
        
        OSArray * modifiers = OSDynamicCast( OSArray, getProperty(kIOHIDKeyboardModifierMappingPairsKey) );
        if (modifiers)
        {
            int modifiedNum = modifiers->getCount(), objIndx = 0;
            
            if (modifiedNum > 0)
            {
                while(modifiedNum > 0)
                {
                    modifiersDic = OSDynamicCast(OSDictionary, modifiers->getObject(objIndx));
                    modifierSrcNum = OSDynamicCast(OSNumber, modifiersDic->getObject(kIOHIDKeyboardModifierMappingSrcKey));
                    modifierDstNum = OSDynamicCast(OSNumber, modifiersDic->getObject(kIOHIDKeyboardModifierMappingDstKey));
                    
                    if (modifierDstNum->unsigned32BitValue() == NX_MODIFIERKEY_COMMAND)
                    {
                        _commandKey = getModifiersPosition(modifierSrcNum->unsigned32BitValue());
                    }
                    
                    else if (modifierDstNum->unsigned32BitValue() == NX_MODIFIERKEY_CONTROL)
                    {
                        _contrlKey = getModifiersPosition(modifierSrcNum->unsigned32BitValue());
                    }
                    else if (modifierDstNum->unsigned32BitValue() == NX_MODIFIERKEY_ALTERNATE)
                    {
                        _altkey = getModifiersPosition(modifierSrcNum->unsigned32BitValue());
                    }
                    
                    modifiedNum --;
                    objIndx ++;
                }
                
            }
            else // Default values if they are not changed
            {
                _commandKey = 0x5b; // 0x37;
                _contrlKey = 0x1d; // 0x3b;
                _altkey = 0x38; // 0x3a;
            }
        }
        
        DEBUG_LOG("%s :: Modifier Key Values Command 0x%02x Control 0x%02x, ALT 0x%02x\n", this->getName(), _commandKey, _contrlKey, _altkey);
        
#if APPLESDK
        _lastModifiersReadTime = now;
#else
        _lastModifiersReadTime  = *(uint64_t *)&now;
#endif
        
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::interruptOccurred(UInt8 scanCode)   // PS2 Interrupt
{
    //
    // This will be invoked automatically from our device when asynchronous
    // keyboard data needs to be delivered.  Process the keyboard data.  Do
    // NOT send any BLOCKING commands to our device in this context.
    //
    
    DEBUG_LOG("%s :: PS2 Keyboard Interrupt Received with Scan Code 0x%02x (%03d)\n", this->getName(),scanCode,scanCode);
    
    if (isKeyboardEnabled == false)
        return;
    
    if (scanCode == kSC_Acknowledge)
        IOLog("%s: Unexpected acknowledge from PS/2 controller.\n", getName());
    else if (scanCode == kSC_Resend)
        IOLog("%s: Unexpected resend request from PS/2 controller.\n", getName());
    else {
        dispatchKeyboardEventWithScancode(scanCode);
    }
    
}

void ApplePS2Keyboard::dispatchKeyboardEventWithScancode(UInt8 scanCode)
{
    //
    // Parses the given scan code, updating all necessary internal state, and
    // should a new key be detected, the key event is dispatched.
    //
    
    unsigned int keyCode = 0;
    bool         goingDown;
    
    
    //
    // See if this scan code introduces an extended key sequence.  If so, note
    // it and then return.  Next time we get a key we'll finish the sequence.
    //
    
    if (scanCode == kSC_Extend)
    {
        _extendCount = 1;
        return;
    }
    
    //
    // See if this scan code introduces an extended key sequence for the Pause
    // Key.  If so, note it and then return.  The next time we get a key, drop
    // it.  The next key we get after that finishes the Pause Key sequence.
    //
    // The sequence actually sent to us by the keyboard for the Pause Key is:
    //
    // 1. E1  Extended Sequence for Pause Key
    // 2. 1D  Useless Data, with Up Bit Cleared
    // 3. 45  Pause Key, with Up Bit Cleared
    // 4. E1  Extended Sequence for Pause Key
    // 5. 9D  Useless Data, with Up Bit Set
    // 6. C5  Pause Key, with Up Bit Set
    //
    // The reason items 4 through 6 are sent with the Pause Key is because the
    // keyboard hardware never generates a release code for the Pause Key and
    // the designers are being smart about it.  The sequence above translates
    // to this parser as two separate events, as it should be -- one down key
    // event and one up key event (for the Pause Key).
    //
    
    else if (scanCode == kSC_Pause)
    {
        _extendCount = 2;
        return;
    }
    
    //
    // Convert the scan code into a key code
    //
    
    keyCode = scanCode & ~kSC_UpBit;
    
    // from "The Undocumented PC" chapter 8, The Keyboard System some
    // keyboard scan codes are single byte, some are multi-byte
    // 3023805:  I want to swap alt and windows, since the windows
    // key is located where the alt/option key is on an Apple PowerBook
    // or USB keyboard, and the alt key is where the Apple/Command
    // key is on the PB or USB keyboard. Left alt is a single scan
    // code byte, right alt is a double scan code byte. Left and
    // right windows keys are double bytes.  This is all set by an
    // entry in Info.plist for ApplePS2Keyboard.kext
    
    
    //
    // Parses the normal key code.
    //
    
    if (_extendCount == 0)
    {
        
        // Clear extended mode for normal ADB codes
        extendedCode = false;
        
        switch (keyCode)
        {
                //
                // Process German/Turkish Keyboard layout on ALT/ALT+STGR(CNTRL)/ALTGR Hold
                //
                
            case 0x08: // 7 as {
                
            case 0x09: // 8 as [
                
            case 0x0a: // 9 as ]
                
            case 0x0b: // 0 as }
                
            case 0x0c: // ß as '\'
                
            case 0x1b: // + as ~
                
            case 0x10: // Q as @
                
            case 0x56: // < as |
                
                if (KBV_IS_KEYDOWN(_altkey, _keyBitVector) && _keyboardTypeID == 0x30) {
                    keyCode = MapGermanTurkishKeyboardLayout(keyCode, !(scanCode & kSC_UpBit));
                }
                break;
                
                
            case 0x3e: // F4, Fn + F4 in Lenovo
                
                // ALT is down ?
                if ((KBV_IS_KEYDOWN(_altkey, _keyBitVector) && !macintoshMode) ||
                    (KBV_IS_KEYDOWN(_commandKey, _keyBitVector) && macintoshMode))
                {
                    // Dispatch when going down as ALT + F4 to close App
                    if (!(scanCode & kSC_UpBit))
                    {
                        receiveTouchpadNotifications(kPS2C_Gesture_AppClose);
                    }
                    keyCode = 0;
                }
                break;
                
            case 0x19: // P, Fn + F10 in Lenovo
                
                // CMD is down ?
                if ((KBV_IS_KEYDOWN(_commandKey, _keyBitVector) && !macintoshMode) ||
                    (KBV_IS_KEYDOWN(_altkey, _keyBitVector) && macintoshMode))
                {
                    // Dispatch when going down as CMD + P to Display mirror
                    if (!(scanCode & kSC_UpBit))
                    {
                        dispatchKeyboardKeyCode( 0x50, true);
                        dispatchKeyboardKeyCode( 0x50, false);
                    }
                    keyCode = 0;
                }
                break;
                
            case 0x0f: // TAB, Fn + F8 in Lenovo
                
                // CMD is down ?
                if ((KBV_IS_KEYDOWN(_commandKey, _keyBitVector) && macintoshMode) ||
                    (KBV_IS_KEYDOWN(_altkey, _keyBitVector) && !macintoshMode))
                {
                    // Dispatch when going down as CMD + TAB to App switch
                    if (!(scanCode & kSC_UpBit))
                    {
                        // Release control if its down
                        if (KBV_IS_KEYDOWN(_contrlKey, _keyBitVector)) {
                            dispatchKeyboardKeyCode(PS2ToADBMap[_contrlKey], false);
                        }
                        
                        dispatchKeyboardKeyCode( 0x30, true);
                        dispatchKeyboardKeyCode( 0x30, false);
                        
                        // Trigger control if its down
                        if (KBV_IS_KEYDOWN(_contrlKey, _keyBitVector)) {
                            dispatchKeyboardKeyCode(PS2ToADBMap[_contrlKey], true);
                        }
                    }
                    keyCode = 0;
                }
                break;
                
            case 0x3a: // Capslock becomes configured key from plist
                if (_capsKey != 0)
                    keyCode = _capsKey;
                break;
                
            case 0x38: // left alt becomes left windows
                if (macintoshMode == true)
                {
                    keyCode = 0x5b;
                    extendedCode = true;
                }
                break;
                
                // Num Lock
            case 0x45:  break;
                
                // Scroll lock
            case 0x46: break;
                
                //
                // If NumLock is Enabled then read ADB code from PS2ADB for numbers
                // else read ADB code from ExtendedPS2ADB for Special keys.
                //
            case 0x47:// EXT Home, NORMAL 7
            case 0x48:// EXT Up Arrow, NOMRAL 8
            case 0x49:// EXT Page Up, NORMAL 9
            case 0x4b:// EXT Lett Arrow, NORMAL 4
            case 0x4c:// 5
            case 0x4d:// EXT Right Arrow, NORMAL 6
            case 0x4f:// EXT END, NORMAL 1
            case 0x50:// EXT Down Arrow, NORMAL 2
            case 0x51:// EXT Page down, NORMLA 3
            case 0x52:// 0
            case 0x53:// EXT Delete, NORMAL .
                
                if (!isNumPadLocked)
                    extendedCode = true;
                else {
                    
                    //
                    // Map the Numpad '.'  key which acts as ',' in OSX by default
                    // for Italian and French keyboard.
                    //
                    
                    if (keyCode == 0x53)
                    {
                        switch (_keyboardTypeID) {
                                
                            case 0x31: // Italian and Spanish
                                if (!makeKeyDotAsComma)
                                    keyCode = 0x34; // .
                                break;
                                
                            case 0x32: // French 1
                            case 0x34: // French 2
                                
                                if (makeKeyDotAsComma)
                                    break;
                                
                                //
                                // Dispatch Shift + ; for . in French
                                //
                                
                                if (!(scanCode & kSC_UpBit)) {
                                    dispatchKeyboardKeyCode( 0x3c, true);
                                    DEBUG_LOG("%s :: Shift down\n", this->getName());
                                }
                                else {
                                    DEBUG_LOG("%s :: Shift up\n", this->getName());
                                    dispatchKeyboardKeyCode( 0x3c, false);
                                }
                                
                                keyCode = 0x33; // ;
                                break;
                                
                            default:
                                break;
                        }
                    }
                }
                break;
                
            case 0x65: // F14, Fn + F3/F4 brightness up/down in Gigabyte
                if (_fnKeysVendorLayout == GIGABYTE)
                {
                    keyCode = 0;
                }
                break;
                
            default:
                break;
        }
        
        DEBUG_LOG("%s :: Received Normal Scan Code 0x%02x (%03d) as Key code 0x%02x(%03d)\n", this->getName(), scanCode, scanCode, keyCode, keyCode);
    }
    
    //
    // Parses the extended key code.
    //
    
    else
    {
        // Set exteneded mode for extended ADB Codes
        extendedCode = true;
        
        _extendCount--;
        if (_extendCount)  return;
        
        
        switch (keyCode)
        {
                // Samsung Fn keys
            case 0x02: // Video mirror
            case 0x08: // brightness up
            case 0x09: // brightness down
            case 0x4e: // Setting Help
                
                // Dell Fn keys
            case 0x05: // brightness down
            case 0x06: // brightness up
                
                // HP Fn keys
            case 0x12: // brightness down
            case 0x17: // brightness up
                
                // Acer Fn keys
            case 0x6f: // brightness down
            case 0x6e: // brightness up
                
                // LG and MSI Fn keys
            case 0x77: // brightness down
            case 0x78: // brightness up, received when Fn key is released too in Acer
                
                // Common volume controls for Fn
            case 0x20: // Mute
            case 0x2e: // Vol down
            case 0x30: // Vol up
                
                if (asusFnKeyWorkAsFunction)
                {
                    switch (keyCode) {
                        case 0x20: // F10
                            keyCode = 0x44;
                            break;
                            
                        case 0x2e: // F11
                            keyCode = 0x57;
                            break;
                            
                        case 0x30: // F12
                            keyCode = 0x58;
                            break;
                    }
                }
                else {
                    keyCode = ExtendedPS2ToADBMap[keyCode];
                }
                
                // Dispatch when going down
                if (!(scanCode & kSC_UpBit))
                {
                    DEBUG_LOG("%s :: Dispatching Fn Keyboard Scan Code 0x%02x (%03d) as Key code 0x%02x (%03d), Finer Brightness %d, Finer Volume %d\n", this->getName(), scanCode, scanCode, keyCode, keyCode, useFinerBrightnesCtrl, useFinerVolumeCtrl);

                    switch (keyCode) {
                        case 0x4d: // Brit up
                            if (useFinerBrightnesCtrl) {
                                processFnKeys(0x10, false);
                                keyCode = 0;
                            }
                            break;
                            
                        case 0x4f: // Brit down
                            if (useFinerBrightnesCtrl) {
                                processFnKeys(0x20, false);
                                keyCode = 0;
                            }
                            break;
                            
                        case 0x48: // Vol up
                            if (useFinerVolumeCtrl) {
                                processFnKeys(0x4c, false);
                                keyCode = 0;
                            }
                            break;
                            
                        case 0x49: // Vol down
                            if (useFinerVolumeCtrl) {
                                processFnKeys(0x49, false);
                                keyCode = 0;
                            }
                            break;
                            
                        default:
                            break;
                    }
                    
                    if (keyCode != 0) {
                        dispatchKeyboardKeyCode( keyCode, true);
                        dispatchKeyboardKeyCode( keyCode, false);
                    }
                }
                keyCode = 0;
                break;
                
            case 0x32: // HP (Internet Fn + F5) as touchpad on/off
            case 0x59: // Dell touchpad on/off
                if (!(scanCode & kSC_UpBit)) {
                    isTPadEnabled = !isTPadEnabled;
                    if (!isTPadEnabled)
                        _device->dispatchPS2Notification(kPS2C_DisableTouchpad);
                    else
                        _device->dispatchPS2Notification(kPS2C_EnableTouchpad);
                }
                keyCode = 0;
                break;
                
            case 0x33: // Samsung Sleep Fn + F11
            case 0x5f: // System sleep
                if (!(scanCode & kSC_UpBit))
                {
                    processFnkeySleep();
                }
                keyCode = 0;
                break;
                
            case 0x1d: // Right control becomes configured key from plist
                if (_rightCntrlKey != 0)
                    keyCode = _rightCntrlKey;
                break;
                
            case 0x38: // right alt may become right windows/command or plist config key
                if (macintoshMode == true)
                    keyCode = 0x5c; // right windows/command
                
                // Right control becomes configured key from plist
                if (_rightALTKey != 0)
                    keyCode = _rightALTKey;
                break;
                
            case 0x5b: // left Windows key may become left alt
                if (macintoshMode == true)
                {
                    keyCode = 0x38; // left alt
                    extendedCode = false;
                }
                break;
                
            case 0x5c:  // right windows may become right alt
                if (macintoshMode == true)
                    keyCode = 0x38; // right alt
                break;
                
                
            case 0x52:  break; // insert becomes Help
                
            case 0x53: // make Del into Command + backspace to file/folder delete
                if (makeDelIntoCmdBspace && !(scanCode & kSC_UpBit))
                {
                    dispatchKeyboardKeyCode( ExtendedPS2ToADBMap[_commandKey], true);
                    dispatchKeyboardKeyCode( 0x33, true);
                    dispatchKeyboardKeyCode( 0x33, false);
                    dispatchKeyboardKeyCode( ExtendedPS2ToADBMap[_commandKey], false);
                    keyCode = 0;
                }
                break;
                
            case 0x37:   // PrintScreen
                
                if (!(scanCode & kSC_UpBit) && !displaySleep) // trigger only on key down
                {
                    // Print Screen Simulation with command + shift + 4
                    
                    dispatchKeyboardKeyCode( ExtendedPS2ToADBMap[_commandKey], true);
                    dispatchKeyboardKeyCode( 0x38, true);
                    dispatchKeyboardKeyCode( 0x15, true);
                    dispatchKeyboardKeyCode( 0x15, false);
                    dispatchKeyboardKeyCode( 0x38, false);
                    dispatchKeyboardKeyCode( ExtendedPS2ToADBMap[_commandKey], false);
                }
                keyCode = 0;
                break;
                
            case 0x45:  break; // Pause/Break key
                
                
            case 0x46:  // Break (control + Pause/Break) as Eject
                
                // release ctrl key for eject to work otherwise power button will be triggered
                if (!(scanCode & kSC_UpBit)) {
                    dispatchKeyboardKeyCode( PS2ToADBMap[_contrlKey], false);
                }
                break;
                
            case 0x2a:  // header or trailer for PrintScreen
                keyCode = 0;
                break;
                
            default:
                break;
        }
        DEBUG_LOG("%s :: Received Extended Scan Code 0x%02x (%03d) as Key code 0x%02x (%03d)\n", this->getName(), scanCode, scanCode, keyCode, keyCode);
    }
    
    
    //
    // Stop further precessing.
    //
    
    if (keyCode == 0)  {
        dispatchKeyboardBacklightNotification(goingDown);
        return;
    }
    
    
    //
    // Update our key bit vector, which maintains the up/down status of all keys.
    //
    
    goingDown = !(scanCode & kSC_UpBit);
    
    if (goingDown)
    {
    
        //
        // Verify that this is not an autorepeated key -- discard if it is.
        //
        
        if (KBV_IS_KEYDOWN(keyCode, _keyBitVector)) {
            
            //
            // Allow repeated brightness and Volume Fn keys
            // so they can be controlled on key hold. Also ignore ALT + Shift which is
            // used to control finer brightness and volume levels.
            //
            
            if ((keyCode == _brightNessDown || keyCode == _brightNessUp ||
                 keyCode == _volDown ||  keyCode == _volUp)
                
                && !isNumPadKey && !displaySleep
                && !(KBV_IS_KEYDOWN(_contrlKey, _keyBitVector))
                && !(KBV_IS_KEYDOWN(_commandKey, _keyBitVector))
                
                && (!(KBV_IS_KEYDOWN(_altkey, _keyBitVector)) ||
                    
                    (KBV_IS_KEYDOWN(_altkey, _keyBitVector) // ALT +
                     && (KBV_IS_KEYDOWN(0x2a, _keyBitVector) || // L SHIFT
                         KBV_IS_KEYDOWN(0x36, _keyBitVector))))) // R SHIFT
            {
                ; // Continue processing
            }
            else {
                dispatchKeyboardBacklightNotification(goingDown);
                return;
            }
        }
        
        KBV_KEYDOWN(keyCode, _keyBitVector);
    }
    else
    {
        KBV_KEYUP(keyCode, _keyBitVector);
    }
    

    //
    // Parse Extended functions F13 - F24 when Shift and F1 - 12 are down.
    // Shift + F1- F12 acts as F13 - F24
    //
    
    if (hasExtendFunctions && !featureToggleMode
        && (KBV_IS_KEYDOWN(0x2a, _keyBitVector) || KBV_IS_KEYDOWN(0x36, _keyBitVector)) // Shift is down
        && keyCode != 0x2a  // Not Left shift in second keycode
        && keyCode != 0x36  // Not Right Shift in second keycode
        && !(KBV_IS_KEYDOWN(_altkey, _keyBitVector))  // Skip if alt key is down
        && !(KBV_IS_KEYDOWN(_contrlKey, _keyBitVector)) // Skip if ctrl key is down
        && !(KBV_IS_KEYDOWN(_commandKey, _keyBitVector))) // Skip if cmd key is down
    {
        if (ProcessExtendedFunctions(keyCode, goingDown))
        {
            dispatchKeyboardBacklightNotification(goingDown);
            return;
        }
    }
    
    //
    // Parse special keys, Control Keypad and Accidental Input exception Key states.
    //
    
    switch (keyCode)
    {
        case 0x3d: // F3
            
            /*
             * Acts as Desktop (Fn + Control + F3) and App Window (Fn + Cmd + F3)
             */
            
            if (isFnKeyPressed &&
                (KBV_IS_KEYDOWN(_contrlKey, _keyBitVector) ||
                 KBV_IS_KEYDOWN(_commandKey, _keyBitVector)))
            {
                if (KBV_IS_KEYDOWN(_commandKey, _keyBitVector) && goingDown)
                    receiveTouchpadNotifications(kPS2C_Gesture_MissionControl);
                
                else if (KBV_IS_KEYDOWN(_contrlKey, _keyBitVector) && goingDown)
                    receiveTouchpadNotifications(kPS2C_Gesture_MissionControl);
                
                keyCode = 0;
            }
            break;
            
            
        case 0x45: // NumLock & Pause/Break (extended) key
            
            /*
             * NumLock does not work in OSX, so we manually control.
             */
            
            if (!extendedCode)
            {
                if (makeNumLockAsClear) {
                    // Just toggle LED and Work as Numpad clear key
                    setNumLockFeedback(goingDown);
                }
                else {
                    if (goingDown) {
                        isNumPadLocked = !isNumPadLocked;
                        setNumLockFeedback(isNumPadLocked);
                    }
                    keyCode = 0;
                }
            }
            // Acts as Context Menu key for Fn key mode > 1
            else if (extendedCode && _fnKeysModeNum > 1)
            {
                keyCode = 0x5d;
            }
            break;
            
        case 0x46: // Scroll Lock and Pause/Break key (extended)
            
            /*
             * Toggle Inertial and Continuous Scroll(on ALT Hold) for Scroll Lock Key.
             */
            
            if (!extendedCode)
            {
                if (KBV_IS_KEYDOWN(_altkey, _keyBitVector) && goingDown)
                    _device->dispatchPS2Notification(kPS2C_ScrollLock_Continuous);
                
                else if (goingDown)
                    _device->dispatchPS2Notification(kPS2C_ScrollLock_Inertia);
            }
            break;
            
        case 0x1c: // Enter
        case 0x39: // Space
        case 0x01: // ESC
            
            /*
             * Release drag, Toggle feature mode, right click on ESC and
             * if releaseGestureKey is true for space and Enter keys.
             */
            
            if (releaseGestureKey || keyCode == 0x01) {
                
                // Release command key for cmd + tab shortcut
                if (KBV_IS_KEYDOWN(_commandKey, _keyBitVector))
                {
                    
                    dispatchKeyboardKeyCode( ExtendedPS2ToADBMap[_commandKey], false);
                }
                _device->dispatchPS2Notification(kPS2C_ESC);
                releaseGestureKey = false;
            }
            isAccInputExceptionKey = true;
            break;
         
        case 0x2a: // left shift
            
            if (goingDown) {
                
                // CapsLock is down ?
                if (KBV_IS_KEYDOWN(0x3a, _keyBitVector))
                {
                    featureToggleMode = true;
                    
                    // Dispatch Notification to enable logs of Pressure and Width of the touch
                    _device->dispatchPS2Notification(kPS2C_ShowTouchPressureWidth);
                    
                    // Release shift key
                    releaseGestureKey = false;
                    dispatchKeyboardKeyCode(0x38, false);
                    
                    // Send ESC key event to clear Feature toggle mode
                    _device->dispatchPS2Notification(kPS2C_ESC);
                    
                    // Disable Caps LED status
                    isCapsLocked = false;
                    setAlphaLockFeedback(isCapsLocked);
                }
                
                /*
                 * Send Shift down event to touchpad
                 * for Toggle Feature mode and Scroll direction.
                 */
                else {
                    _device->dispatchPS2Notification(kPS2C_ShiftKey);
                    releaseGestureKey = true;
                }
            }
            else if (!goingDown && !featureToggleMode) {
                _device->dispatchPS2Notification(kPS2C_ESC); // Clear
            }
            
            isAccInputExceptionKey = true;
            break;
            
        case 0x3a: // CapsLock
            
            // Check Modifiers position
            readKeyboardModifiers();
            
            // Left shift is down ?
            if (KBV_IS_KEYDOWN(0x2a, _keyBitVector) && goingDown)
            {
                featureToggleMode = true;
                
                // Dispatch Notification to enable logs of Pressure and Width of the touch
                _device->dispatchPS2Notification(kPS2C_ShowTouchPressureWidth);
                
                // Release shift key
                releaseGestureKey = false;
                dispatchKeyboardKeyCode(0x38, false);
                
                // Send ESC key event to clear Feature toggle mode
                _device->dispatchPS2Notification(kPS2C_ESC);
                
                // Disable Caps LED status
                setAlphaLockFeedback(false);
            }
            else {
                
                //
                // Sierra has a bug with Caps Lock where it requires 3 presses to turn if off,
                // so a fix for this bug is using workaround of reporting that key press 3 times
                // instead of once.
                //

               if (_osxVersion > 11 && !featureToggleMode && fixSierraCapsLockBug)
                {
                    dispatchKeyboardKeyCode( 0x39, goingDown);
                    dispatchKeyboardKeyCode( 0x39, goingDown);
                    
                    if (!goingDown) {
                        isCapsLocked = !isCapsLocked;
                        setAlphaLock(isCapsLocked);
                        setAlphaLockFeedback(isCapsLocked);
                    }
                }
               else {
                   isCapsLocked = !isCapsLocked;
               }
            }
            isAccInputExceptionKey = true;
            break;
            
        case 0x5d: // right context menu key
            switch (_rightCtxtMKey) {
                case 0x5c:
                    if (macintoshMode)
                        keyCode = 0x38; // may become right alt
                    else
                        keyCode = 0x5c; // may become right windows
                    break;
                    
                case 0x54:  // Apple Fn
                    if (_fnKeysModeNum > 1)
                    {
                        /*
                         * Set Fn Key state on key down and
                         * generate ADB code on release.
                         */
                        
                        if (goingDown) {
                            isFnKeyPressed = true;
                        }
                        else if (!goingDown)
                        {
                            isFnKeyPressed = false;
                            
                            // Send Generated ADB Code on key release if keypad is on
                            if (isNumPadLocked && !displaySleep)
                                generateAndSeeADBCode(keyCode, goingDown);
                        }
                    }
                    
                default:
                    keyCode = _rightCtxtMKey; // may become configured key from plist
                    break;
            }
            break;
            
            
            /* Accidental input exception keys */
        case 0x1d: // left control
        case 0x5b: // left Window
        case 0x5c: // right Window
        case 0x38: // Alt both
            //check for Modifiers position
            readKeyboardModifiers();
            
        case 0x36:// right shift
        case 0X0f:// Tab
        case 0x0e:// Backspace
        case 0x37:// "keypad *"
        case 0x4a:// "keypad -"
        case 0x4e:// "keypad +"
        case 0x35:// "keypad /" in ext mode
            isAccInputExceptionKey = true;
            break;
            
            /* Numpad */
        case 0x47:// EXT Home, NORMAL 7
        case 0x48:// EXT Up Arrow, NOMRAL 8
        case 0x49:// EXT Page Up, NORMAL 9
        case 0x4b:// EXT Lett Arrow, NORMAL 4
        case 0x4c:// 5
        case 0x4d:// EXT Right Arrow, NORMAL 6
        case 0x4f:// EXT END, NORMAL 1
        case 0x50:// EXT Down Arrow, NORMAL 2
        case 0x51:// EXT Page down, NORMLA 3
        case 0x52:// 0
        case 0x53:// EXT Delete, NORMAL .
            
            if (!extendedCode) // Numbers
            {
                isNumPadKey = true;
                
                // Try to generate ADB code if Fn key was pressed and keypad is on
                if (isFnKeyPressed && isNumPadLocked && goingDown
                    && keyCode != 0x53 && !displaySleep)
                {
                    generateAndSeeADBCode(keyCode, goingDown);
                }
            }
            else // Special keys
            {
                isNumPadKey = false;
            }
            
            isAccInputExceptionKey = true;
            break;
            
        default:
            // Skip if command key is down for command + x shortcuts
            if (KBV_IS_KEYDOWN(_commandKey, _keyBitVector))
                isAccInputExceptionKey = true;
            else
                isNumPadKey = isAccInputExceptionKey = false;
            break;
    }
    
    //
    // Parse Fn key combination.
    //
    
    if ((keyCode == _brightNessDown || keyCode == _brightNessUp ||
         keyCode == _kbrdBrightnessDown || keyCode == _kbrdBrightnessUp ||
         keyCode == _sleep || keyCode == _missonCtlFun || keyCode == _launchpadFun ||
         keyCode == _dispSleep || keyCode == _videoMirror || keyCode == _touchpad ||
         keyCode == _volMute || keyCode == _volDown ||  keyCode == _volUp ||
         keyCode == _mediaPrev || keyCode == _mediaPlay || keyCode == _mediaNext)
        
        && !isNumPadKey && (!displaySleep || keyCode == _dispSleep)
        
        && !(KBV_IS_KEYDOWN(_contrlKey, _keyBitVector))
        && !(KBV_IS_KEYDOWN(_commandKey, _keyBitVector))
        
        && (!(KBV_IS_KEYDOWN(_altkey, _keyBitVector)) ||
            
            (KBV_IS_KEYDOWN(_altkey, _keyBitVector)
             && (KBV_IS_KEYDOWN(0x2a, _keyBitVector) ||
                 KBV_IS_KEYDOWN(0x36, _keyBitVector)))))
    {
        isFnKeyComb = true;
    }
    else
        isFnKeyComb = false;
    
    
    //
    // Process Touchpad Features toggle and Fn keys on Shift + Caps Lock.
    //
    
    if (featureToggleMode)
    {
        if (KBV_IS_KEYDOWN(0x2a, _keyBitVector) && KBV_IS_KEYDOWN(0x3a, _keyBitVector)) {
            
            if (isFnKeyComb && makeShiftCapsAsFn) {
                switch (_fnKeysModeNum) {
                    case 1:
                        // Choose ADB code to dispatch
                        if (extendedCode) {
                            keyCode = ExtendedPS2ToADBMap[keyCode];
                        }
                        else {
                            keyCode = PS2ToADBMap[keyCode];
                        }
                        
                        // Dispatch Function key
                        dispatchKeyboardKeyCode(keyCode, goingDown);
                        break;
                        
                    case 2 ... 3:
                        // Dispatch Fn key
                        processFnKeys(keyCode, goingDown);
                        break;
                        
                    default:
                        break;
                }
            }
            // Dispatch Touchpad toggle event
            else {
                ToggleTouchpadFeatures(keyCode, goingDown);
            }
        }
        
        else if (!(KBV_IS_KEYDOWN(0x2a, _keyBitVector)) && !(KBV_IS_KEYDOWN(0x3a, _keyBitVector)))
        {
            // Restore capslock state
            setAlphaLockFeedback(isCapsLocked);
            featureToggleMode = false;
            
            // Dispatch Notification to hide log of Pressure and Width of the touch
            _device->dispatchPS2Notification(kPS2C_ShowTouchPressureWidth);
            
            DEBUG_LOG("%s :: Feature Toggle mode ended.\n", this->getName());
        }
        keyCode = 0;
        DEBUG_LOG("%s :: Ignored Key on Feature Toggle mode 0x%02x (%d)\n", this->getName(), keyCode,keyCode);
    }
    
    
    //
    // Stop further processing.
    //
    
    if (keyCode == 0)
    {
        dispatchKeyboardBacklightNotification(goingDown);
        return;
    }
    
    
    //
    //  Dispatch Fn Keys on right Context Menu key Hold in fn keys mode 2 and 3,
    //  without Fn key in Mode 1.
    //
    
    if (isFnKeyComb && (isFnKeyPressed || _fnKeysModeNum == 1))
    {
        processFnKeys(keyCode, goingDown);
    }
    
    //
    // Dispatch Modifier Keys
    //
    
    else if (!displaySleep &&
             (keyCode == _commandKey || keyCode == _altkey || keyCode == _contrlKey ||
              keyCode == 0x54 /*Fn*/ || (keyCode == 0x5d && !isFnKeyPressed) /*Context*/ ||
              keyCode == 0x2a || keyCode == 0x36 /*Shift*/))
    {
        //
        //  Modifier Keys state
        //  7   6   5   4   3   2   1   0
        //  CF  AF  0  0   S   A   C   CMD
        //
        //  CF = Controlling Feature
        //  AF = Alternate function
        //  S  = Shift
        //  A  = ALT/Option
        //  C  = Control
        //  CMD = command
        //
        
        // Send Control feature event to touchpad (CNTRL + CMD + ALT)
        if (!(_modifierKeystate & 0x80) &&
                 KBV_IS_KEYDOWN(_commandKey, _keyBitVector) &&
                 KBV_IS_KEYDOWN(_contrlKey, _keyBitVector) &&
                 KBV_IS_KEYDOWN(_altkey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_CNTRL_Feature_on);
            controllingFeature = true;
            _modifierKeystate &= 0xBF; // clear bit 6
            _modifierKeystate |= 0x80; // set bit 7
        }
        else if ((_modifierKeystate & 0x80) &&
                 (!KBV_IS_KEYDOWN(_commandKey, _keyBitVector) ||
                  !KBV_IS_KEYDOWN(_contrlKey, _keyBitVector) ||
                  !KBV_IS_KEYDOWN(_altkey, _keyBitVector)))
        {
            _device->dispatchPS2Notification(kPS2C_CNTRL_Feature_off);
            controllingFeature = false;
            _modifierKeystate &= 0x7F; // clear bit 7
        }
        
        // Send alternate function event to touchpad (CMD + ALT)
        else if (!(_modifierKeystate & 0x60) &&
                 KBV_IS_KEYDOWN(_commandKey, _keyBitVector) &&
                 KBV_IS_KEYDOWN(_altkey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_ALT_Funtion_on);
            altFunctionEnabled = true;
            _modifierKeystate |= 0x60; // set bit 6
        }
        else if ((_modifierKeystate & 0x80) &&
                 !KBV_IS_KEYDOWN(_commandKey, _keyBitVector) &&
                 !KBV_IS_KEYDOWN(_altkey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_ALT_Funtion_off);
            altFunctionEnabled = false;
            _modifierKeystate &= 0xBF; // clear bit 6
        }
        
        // Send Command key up and down event to touchpad
        else if (!(_modifierKeystate & 0x01) &&
                 KBV_IS_KEYDOWN(_commandKey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_CMDKeyDown);
            _modifierKeystate |= 0x01; // set bit 0
        }
        
        else if ((_modifierKeystate & 0x01) &&
                 !KBV_IS_KEYDOWN(_commandKey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_CMDKeyUp);
            _modifierKeystate &= 0xFE; // clear bit 0
        }
        
        // Send Control key up and down event to touchpad
        else if (!(_modifierKeystate & 0x02) &&
                 KBV_IS_KEYDOWN(_contrlKey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_CNTRLKeyDown);
            _modifierKeystate |= 0x02; // set bit 1
        }
        
        else if ((_modifierKeystate & 0x02) &&
                 !KBV_IS_KEYDOWN(_contrlKey, _keyBitVector))
        {
            _device->dispatchPS2Notification(kPS2C_CNTRLKeyUp);
            _modifierKeystate &= 0xFD; // clear bit 1
        }
        
        // Dispatch key
        switch (keyCode) {
            case 0x5d: // Send Context menu event to touchpad
                if (goingDown) {
                    _device->dispatchPS2Notification(kPS2C_DoContextMenu);
                }
                break;
                
            default: // Choose ADB code to dispatch
                if (extendedCode)
                    keyCode = ExtendedPS2ToADBMap[keyCode];
                else
                    keyCode = PS2ToADBMap[keyCode];
                
                dispatchKeyboardKeyCode( keyCode, goingDown);
                break;
        }
        
        DEBUG_LOG("%s :: Dispatched Modifier Key code 0x%02x (%03d) as Key code 0x%02x (%03d), ExtendedKey %s, GoingDown %s\n", this->getName(), scanCode, scanCode, keyCode, keyCode, extendedCode?"YES":"NO", goingDown?"YES":"NO");
    }
    
    //
    // Dispatch normal Keys
    //
    
    else if (!displaySleep && !isFnKeyPressed)
    {
        // Choose ADB code to dispatch
        if (extendedCode)
            keyCode = ExtendedPS2ToADBMap[keyCode];
        else
            keyCode = PS2ToADBMap[keyCode];
        
        // Dispatch key
        dispatchKeyboardKeyCode( keyCode, goingDown);
        
        
        DEBUG_LOG("%s :: Dispatched Scan code 0x%02x (%03d) as Key code 0x%02x (%03d), ExtendedKey %s, GoingDown %s\n", this->getName(), scanCode, scanCode, keyCode, keyCode, extendedCode?"YES":"NO", goingDown?"YES":"NO");
        
        /*
         * Send PS2 Notification to touchpad
         * for disabling (key down) touchpad on keyboard input of letters, numbers and symbols
         * to prevent accidental touchpad inputs while typing except for Acccidental and number pad keys(enable).
         */
        
        if (goingDown && !isAccInputExceptionKey && !isNumPadKey && isTPadEnabled) {
            _device->dispatchPS2Notification(kPS2C_AccidentalInput);
        }
        else if ((isAccInputExceptionKey || isNumPadKey) && isTPadEnabled)
        {
            _device->dispatchPS2Notification(kPS2C_EnableTouchpad);
        }
    }
    
    dispatchKeyboardBacklightNotification(goingDown);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::dispatchKeyboardBacklightNotification(bool keyState)
{
    if (enabledAsusKBacklight) {
        
        // Cancel timeout for keyboard backlight off and turn on if its off
        if (keyState) {
            _device->dispatchPS2Notification(kPS2C_KeybrdBackLightOn);
        }
        // Trigger timeout for keyboard backlight off if backlight is on
        else {
            _device->dispatchPS2Notification(kPS2C_KeybrdBackLightOff);
        }
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
 * Process Fn Keys for Fn Keys mode
 */
void ApplePS2Keyboard::processFnKeys(UInt32 keyCode, bool keyDown)
{
    /*
     * F1........F12, Left, Down and Right Arrows, Pause keys
     * 0x3b: F1, 0x3c: F2, 0x3d: F3, 0x3e: F4, 0x3f: F5, 0x40: F6,
     * 0x41: F7, 0x42: F8, 0x43: F9, 0x44: F10, 0x57: F11, 0x58: F12,
     * 0x4b: LEFT ARROW, 0x50: DOWN ARROW, 0x4d: RIGHT ARROW, 0x48: UP ARROW,0x45: Pause
     */
    
    bool    rateLimit = false;
    UInt8   fnKeyCode = 0;
    clock_sec_t  secs, deltaSecs;
    clock_usec_t microsecs, deltaMicrosecs;
    
    clock_get_system_microtime(&secs, &microsecs);
    deltaSecs = secs - _lastEventSecs;
    
    // Limits the rate of event send to HID stack,
    // otherwise the system slow down and the sound/sun bezel lags.
    if (deltaSecs < 2)
    {
        deltaMicrosecs =(clock_usec_t) (microsecs + (1000000 * deltaSecs) - _lastEventMicrosecs);
        
        if (deltaMicrosecs < 2000000) // rate limiter to 200 ms
        {
            rateLimit = true;
            
        }
    }
    
    if (keyDown) {
        
        if (keyCode == _kbrdBrightnessDown && enabledAsusKBacklight) {
            processAsusKBacklight(false);
            _kbrdBackLightLvl = readAsusKBlight(0);
        }
        
        else if (keyCode == _kbrdBrightnessUp && enabledAsusKBacklight) {
            processAsusKBacklight(true);
            _kbrdBackLightLvl = readAsusKBlight(0);
        }
        
        // Media Previous/Next acts as Media Rewind/Fast on key Hold by not releasing key
        else if (keyCode == _mediaPrev)
        {
            dispatchKeyboardKeyCode( 0x34, true);
        }
        else if (keyCode == _mediaNext)
        {
            dispatchKeyboardKeyCode( 0x40, true);
        }
        
        else if (keyCode == _brightNessDown || keyCode == _brightNessUp)
        {
            
            if (keyCode == _brightNessUp)
                fnKeyCode = 0x10;
            else
                fnKeyCode = 0x20;
            
            //
            // Using Asus Backlight driver?
            //
            // If yes, then read AppleBezel Values from Asus Backlight driver
            // for controlling the Apple bezel levels and
            // mapping of 16 bezel values to 11 Asus levels.
            //
            
            IORegistryEntry *asusDeviceEntry = IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/PNLF/AsusACPIBacklightPanel");
            
            if (asusDeviceEntry) {
                
                UInt8 loopCount = 1, bLevel = 0;
                
                OSNumber *ioregAsusTLvls = OSDynamicCast(OSNumber, asusDeviceEntry->getProperty("AsusBrightnessLevelsTotal"));
                OSNumber *ioregAsusLvl = OSDynamicCast(OSNumber, asusDeviceEntry->getProperty("AsusBrightnessLevel"));
                
                if(ioregAsusTLvls && ioregAsusLvl) {
                    
                    // Dislable finer brightness control
                    useFinerBrightnesCtrl = false;
                    
                    if (keyCode == _brightNessUp) {
                        bLevel++;
                        fnKeyCode = 0x4d;
                    }
                    else
                        fnKeyCode = 0x4f;
                    
                    if (ioregAsusTLvls->unsigned8BitValue() == 0x0B)
                    {
                        switch (bLevel) {
                            case 11:
                            case 10:
                                loopCount = 3;
                                break;
                                
                            case 9:
                            case 8:
                                loopCount = 2;
                                break;
                                
                            default:
                                loopCount = 1;
                                break;
                        }
                    }
                    else
                        loopCount = 1;
                    
                    // Dispatch brightness
                    for (int loop = 0; loop < loopCount; loop++)
                    {
                        dispatchKeyboardKeyCode( fnKeyCode, true);
                        dispatchKeyboardKeyCode( fnKeyCode, false);
                        IOSleep(25);
                    }
                    
                    fnKeyCode = 0; // skip further processing
                }
                else
                    DEBUG_LOG("%s: Asus Backlight brightness level not Found.\n",this->getName());
                
                DEBUG_LOG("%s :: Asus backlight device found with Level %d\n",this->getName(),bLevel);
                
                asusDeviceEntry->release();
            }
            
        }
        
        else if (keyCode == _sleep && !rateLimit)
            processFnkeySleep();
        
        else if (keyCode == _missonCtlFun &&
                 (!enabledAsusKBacklight || keyCode != _kbrdBrightnessDown)) {
            receiveTouchpadNotifications(kPS2C_Gesture_MissionControl);
        }
        
        else if (keyCode == _launchpadFun &&
                 (!enabledAsusKBacklight || keyCode != _kbrdBrightnessUp )) {
            receiveTouchpadNotifications(kPS2C_Gesture_Launchpad);
        }
        
        else if (keyCode == _videoMirror)
            fnKeyCode = 0x50;
        
        else if (keyCode == _dispSleep && !rateLimit)
            fnKeyCode = _dispSleep;
        
        else if (keyCode == _touchpad)
        {
            isTPadEnabled = !isTPadEnabled;
            if (!isTPadEnabled)
                _device->dispatchPS2Notification(kPS2C_DisableTouchpad);
            else
                _device->dispatchPS2Notification(kPS2C_EnableTouchpad);
            
        }
        else if (keyCode == _volMute)
            fnKeyCode = 0x4a;
        
        else if (keyCode == _volDown)
            fnKeyCode = 0x49;
        
        else if (keyCode == _volUp)
            fnKeyCode = 0x4c;
        
        else if (keyCode == _mediaPlay)
            fnKeyCode = 0x42;
    }
    // Media key release
    else {
        
        if (keyCode == _mediaPrev)
        {
            dispatchKeyboardKeyCode( 0x34, false);
        }
        else if (keyCode == _mediaNext)
        {
            dispatchKeyboardKeyCode( 0x40, false);
        }
        else
        {
            switch (keyCode) {
                case 0x10:
                case 0x20:
                case 0x49:
                case 0x4c:
                    fnKeyCode = keyCode;
                    break;
                    
                default:
                    break;
            }
        }
    }
    
    _lastEventSecs =		 secs;
    _lastEventMicrosecs = microsecs;
    
    switch (fnKeyCode) {
            
        case 0x10:
            
            // Brightness up finer increments Shift +  Option +
            
            if (useFinerBrightnesCtrl) {
                
                dispatchKeyboardKeyCode( 0x38, true);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], true);
                
                dispatchKeyboardKeyCode( 0x4d, true);
                
                dispatchKeyboardKeyCode( 0x4d, false);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], false);
                
                dispatchKeyboardKeyCode( 0x38, false);
            }
            // normal
            else {
                dispatchKeyboardKeyCode( 0x4d, true);
                dispatchKeyboardKeyCode( 0x4d, false);
            }
            break;
            
        case 0x20:
            
            // Brightness down finer increments with Shift + Option +
            
            if (useFinerBrightnesCtrl) {
                
                dispatchKeyboardKeyCode( 0x38, true);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], true);
                
                dispatchKeyboardKeyCode( 0x4f, true);
                
                dispatchKeyboardKeyCode( 0x4f, false);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], false);
                
                dispatchKeyboardKeyCode( 0x38, false);
            }
            // normal
            else {
                dispatchKeyboardKeyCode( 0x4f, true);
                dispatchKeyboardKeyCode( 0x4f, false);
            }
            break;
            
        case 0x4c:// Volume up
            
            // finer increments with Shift + Option +
            
            if (useFinerVolumeCtrl) {
                
                dispatchKeyboardKeyCode( 0x38, true);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], true);
                
                dispatchKeyboardKeyCode( 0x48, true);
                
                dispatchKeyboardKeyCode( 0x48, false);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], false);
                
                dispatchKeyboardKeyCode( 0x38, false);
            }
            // normal
            else {
                dispatchKeyboardKeyCode( 0x48, true);
                dispatchKeyboardKeyCode( 0x48, false);
            }
            break;
            
        case 0x49:// Volume down
            
            // finer increments with Shift + Option +
            
            if (useFinerVolumeCtrl) {
                
                dispatchKeyboardKeyCode( 0x38, true);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], true);
                
                dispatchKeyboardKeyCode( 0x49, true);
                
                dispatchKeyboardKeyCode( 0x49, false);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_altkey], false);
                
                dispatchKeyboardKeyCode( 0x38, false);
            }
            // normal
            else {
                dispatchKeyboardKeyCode( 0x49, true);
                dispatchKeyboardKeyCode( 0x49, false);
            }
            break;
            
        case 0: // do nothing
            break;
            
        default:
            if (fnKeyCode == _dispSleep) {
                //
                // Sleeps the display by sending Control + Shift + Eject keys
                //
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_contrlKey], true);
                
                dispatchKeyboardKeyCode( 0x38, true);
                
                dispatchKeyboardKeyCode( 0x70, true);
                
                dispatchKeyboardKeyCode( 0x70, false);
                
                dispatchKeyboardKeyCode( 0x38, false);
                
                dispatchKeyboardKeyCode( PS2ToADBMap[_contrlKey], false);
                
                isTPadEnabled = !isTPadEnabled;
                
                if (!isTPadEnabled)
                    _device->dispatchPS2Notification(kPS2C_DisableTouchpad);
                else
                    _device->dispatchPS2Notification(kPS2C_EnableTouchpad);
                
                displaySleep = !displaySleep;
                
            }
            // Dispatch Fn key
            else {
                dispatchKeyboardKeyCode( fnKeyCode, true);
                dispatchKeyboardKeyCode( fnKeyCode, false);
            }
            break;
    }
    
    DEBUG_LOG("%s :: Dispatched Fn key for Key code 0x%02x (%03d) with 0x%02x, ExtendedKey %s, Key Down %s, TP %d, DP %d\n", this->getName(), (unsigned int)keyCode, (unsigned int)keyCode, fnKeyCode, extendedCode?"YES":"NO",keyDown?"YES":"NO",isTPadEnabled, displaySleep);
    
}

/*
 * Process System sleep for Fn keys
 */
void ApplePS2Keyboard::processFnkeySleep()
{
    /*
     * Notify sleep device 'SLPB' in DSDT for Sleeping
     * Have to add the method SLPN to the SLPB device in DSDT using my DSDT patch
     * This does BIOS Level sleep
     * otherwise Sleep will be interrupted if we try to operate OSX after sleep key(for me in Asus)
     */
    
    IOACPIPlatformDevice * acpiProvider;
    acpiProvider = (IOACPIPlatformDevice*)IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/SLPB");
    if (acpiProvider)
        acpiProvider->retain();
    
    if (acpiProvider) {
        UInt32 res = 0;
        if (kIOReturnSuccess == acpiProvider->validateObject("SLPN")) {
            // on success we receive the result '1'
            acpiProvider->evaluateInteger("SLPN", &res);
            DEBUG_LOG("%s: Sleep button device notified, Result %d\n",this->getName(),(unsigned int)res);
        }
        else
            DEBUG_LOG("%s: Sleep notifier method not found\n",this->getName());
        
    }
    OSSafeRelease(acpiProvider);
    
    IOPMrootDomain * rootDomain = getPMRootDomain();
    if (rootDomain)
        rootDomain->receivePowerNotification( kIOPMSleepNow );
    
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * Check the Asus Fn keys driver ioreg to find its mode
 */
void ApplePS2Keyboard::checkAsusFnKeysMode()
{
    IORegistryEntry *fnDeviceEntry = IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/ATKD/AsusNBFnKeys");
    
    if (fnDeviceEntry != NULL) {
        
        if(OSDynamicCast(OSBoolean, fnDeviceEntry->getProperty("FunctionKeysMode")))
            asusFnKeyWorkAsFunction = true;
        else
            asusFnKeyWorkAsFunction = false;
        
        fnDeviceEntry->release();
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * Process Asus Keyboard backlight
 */


SInt16 ApplePS2Keyboard::readAsusKBlight(UInt8 reqParm)
{
    OSObject * params[1];
    
   	params[0] =OSNumber::withNumber(reqParm, 8);
    
    UInt32 kblvl = 0;
    enabledAsusKBacklight = false;
    
    IOACPIPlatformDevice *atkd = (IOACPIPlatformDevice*)IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/ATKD");
    
    //
    // Call Asus WMI DSDT Method 'GKBL' to read the keyboard backlight
    //
    
    if (atkd) {
        if (kIOReturnSuccess == atkd->validateObject("GKBL")) {
            
            if (kIOReturnSuccess == atkd->evaluateInteger("GKBL", &kblvl, params, 1))
            {
                //
                // Some systems have the backlight value in 2's complement,
                // so need to convert.
                //
                
                kblvl = kblvl & 0xFF;
                IOLog("%s: Keyboard backight value from DSDT is 0x%x\n",this->getName(), kblvl);
                enabledAsusKBacklight = true;
            }
            else {
                IOLog("%s: Failed to get the Keyboard backight level\n",this->getName());
            }
        }
        else {
            IOLog("%s: Keyboard backlight method not found\n",this->getName());
        }
        
        atkd->release();
    }
    else {
        IOLog("%s: Failed to get ATKD device\n",this->getName());
    }
    
    if (enabledAsusKBacklight)
        return kblvl;
    else
        return -1;
}

SInt16 ApplePS2Keyboard::getMaxKBlightLevel()
{
    UInt32 kbMlvl = 3; // Default set to 4 (0-3)
    
    IOACPIPlatformDevice *atkd = (IOACPIPlatformDevice*)IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/ATKD");
    
    //
    // Call Asus WMI DSDT Method 'GKLM' to get max keyboard backlight level
    //
    
    if (atkd) {
        if (kIOReturnSuccess == atkd->validateObject("GKLM"))
        {
            
            if (kIOReturnSuccess == atkd->evaluateInteger("GKLM", &kbMlvl))
            {
                //
                // Some systems have the backlight value in 2's complement,
                // so need to convert.
                //
                
                kbMlvl = kbMlvl & 0xFF;
                IOLog("%s: Max Keyboard backight value from DSDT is 0x%x\n",this->getName(), kbMlvl);
                
                // Invalid value ?
                if (kbMlvl > 254) {
                    kbMlvl = 3;
                }
            }
            else {
                IOLog("%s: Failed to get the Keyboard backight max level\n",this->getName());
            }
        }
        else {
            IOLog("%s: Keyboard backlight method not found\n",this->getName());
        }
        
        atkd->release();
    }
    else {
        IOLog("%s: Failed to get ATKD device\n",this->getName());
    }
    
    return kbMlvl;
}

void ApplePS2Keyboard::setAsusKBlight(UInt8 lvl)
{
    
    IOACPIPlatformDevice *atkd = (IOACPIPlatformDevice*)IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/ATKD");
    
    //
    // Call Asus WMI DSDT Method 'SKBL' to set the keyboard backlight
    //
    
    if (atkd) {
        if (kIOReturnSuccess == atkd->validateObject("SKBL")) {
            OSObject * params[1];
            OSObject * ret = NULL;
            
            params[0] =OSNumber::withNumber(lvl, 8);
            
            if (kIOReturnSuccess == atkd->evaluateObject("SKBL", &ret, params, 1))
            {
                IOLog("%s: Keyboard backight level set to 0x%x\n",this->getName(), lvl);
                
                //
                // Save level in NVRAM (Ignore Auto off control values)
                //
                
                if(lvl != 0xED && lvl != 0xEA) {
                    saveValueToNVRAM("AsusKeyboardBackLightLvl", lvl);
                }
            }
            else
                IOLog("%s: Failed to set Keyboard backight level to %d\n",this->getName(), lvl);
            
        }
        else {
            IOLog("%s: Keyboard backlight method not found\n",this->getName());
            enabledAsusKBacklight = false;
        }
        
        atkd->release();
    }
    else
        IOLog("%s: Failed to get ATKD device\n",this->getName());
    
}

void ApplePS2Keyboard::processAsusKBacklight(bool LvlUp)
{
    DEBUG_LOG("%s: Setting Asus keyboard backlight...\n",this->getName());
    /*
     * Asus Keyboard backlight controls
     * using DSDT below backlight methods GKBL and SKBL in ATKD device scope
     *
     Method (SKBL, 1, NotSerialized)
     {
         Store (Arg0, Local0)
         Store (Arg0, KBLV)
         Store (DerefOf (Index (PWKB, Local0)), Local1)
         ^^PCI0.LPCB.EC0.WRAM (0x04B1, Local1) // haswell
         ^^PCI0.LPCB.EC0.WRAM (0x044B, Local1) // Sandy/Ivy
         Return (One)
     }
     Method (GKBL, 0, NotSerialized)
     {
        Return (KBLV)
     }
     
     Method (GKLM, 0, NotSerialized)
     {
        Return (0x03) // Set Max Level here
     }
     
     // For 16 Levels
     Name (KBPW, Buffer (0x10)
     {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
     })
     */
    SInt8 blvl = -1;
    UInt8 bMlvl = getMaxKBlightLevel();
    
    blvl = readAsusKBlight(0);
    
    // Invalid level ?
    if (blvl > bMlvl)
    {
        blvl = bMlvl;
        
    }
    
    // Valid level ?
    if (blvl >= 0)
    {
        if (((asusKBHas16LevelSupport && blvl < 15) ||
             (!asusKBHas16LevelSupport && blvl < bMlvl))
            
            && LvlUp)
        {
            blvl++;
        }
        else if (blvl > 0 && !LvlUp) {
            blvl--;
        }
        
        setAsusKBlight(blvl);
        setAsusKBlight(0xEA); // set the BOFF to 0xEA so last idle off from 'Fn' can be stopped
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * Process Touchpad features toggle on Shift + Caps + x
 */
void ApplePS2Keyboard::ToggleTouchpadFeatures(unsigned int keyCode, bool goingDown)
{
    bool isFeatureKey = true;
    UInt8 feature = 0;
    char* featureString =(char*) "None";
    
    DEBUG_LOG("%s :: Received Toggle Key code 0x%02x (%03d), ExtendedKey %s\n", this->getName(), keyCode, keyCode,extendedCode?"YES":"NO");
    
    switch (keyCode) {
        case 0x1e: // Auto scroll 'A'
            feature = kPS2C_ScrollLock_Auto;
            featureString = (char *) "Auto scroll";
            break;
        case 0x17: // Inertia 'I'
            feature = kPS2C_ScrollLock_Inertia;
            featureString = (char *) "Inertia scroll effect";
            break;
        case 0x2e: // Continuous 'C'
            feature = kPS2C_ScrollLock_Continuous;
            featureString = (char *) "Continuous scroll";
            break;
        case 0x1f: // Edge scroll 'S'
            feature = kPS2C_EdgeScroll;
            featureString = (char *) "Edge scroll";
            break;
        case 0x12: // 1F Edge swipes 'E'
            feature = kPS2C_Gestures_1FSwipes;
            featureString = (char *) "Edge swipes";
            break;
        case 0x14: // Double Taps 'T'
            feature = kPS2C_Gestures_DoubleTaps;
            featureString = (char *) "Double Taps";
            break;
        case 0x32: // 3F & 4F Swipes 'M'
            feature = kPS2C_Gestures_3FSwipes;
            featureString = (char *) "Three and Four Fingers swipes";
            break;
        case 0x10: // Rotate 'Q'
            feature = kPS2C_Gestures_Rotate;
            featureString = (char *) "Rotate";
            break;
        case 0x2c: // Zoom 'Z'
            feature = kPS2C_Gestures_Zoom;
            featureString = (char *) "Zoom";
            break;
        case 0x20:// Debug logs 'D'
            if (goingDown)
                _debug = !_debug;
            featureString = (char *) "Debug logs";
            break;
            
        case 0x2d:// Extended Funtions 'X'
            if (goingDown)
                hasExtendFunctions = !hasExtendFunctions;
            
            //
            // Read extended functions
            //
            if (hasExtendFunctions)
                ReadExtendedFunctionsConfig();
            
            featureString = (char *) "Extended functions";
            break;
            
        case 0x25: // Key swap 'K'
            if (goingDown)
                macintoshMode = !macintoshMode;
            
            featureString = (char *) "Swap ALT and Windows";
            break;
            
        case 0x21: // FnKeys modes 'F'
            if (_fnKeysModeNum != 0 && goingDown)
            {
                if (_fnKeysModeNum<3)
                    _fnKeysModeNum++;
                else if (_fnKeysModeNum == 3)
                    _fnKeysModeNum = 1;
                
                // Apple Mode 1(without fn key)
                if (_fnKeysModeNum < 2)
                    _sleep = 0; // no mapping
                
                IOLog("%s :: Fn keys set to mode %d\n", this->getName(),_fnKeysModeNum);
                
                //
                // Read Fn keys config
                //
                if (_fnKeysModeNum != 0)
                    ReadFnKeyConfig();
                
                //
                // Read real Fn keys layout of vendor
                //
                if (_fnKeysModeNum == 3)
                    ReadFnKeyLayout();
                
            }
            featureString = (char *) "Changing Fn key mode";
            break;
            
        case 0x01: // restore touchpad 'ESC'
            feature = kPS2C_RestoreTouchpad;
            featureString = (char *) "Reconnect touchpad";
            break;
            
        case 0x2f: // Key swap 'V'
            if (goingDown)
                useFinerVolumeCtrl = !useFinerVolumeCtrl;
            
            featureString = (char *) "Finer Volume control";
            break;
            
        case 0x30: // Key swap 'B'
            if (goingDown)
                useFinerBrightnesCtrl = !useFinerBrightnesCtrl;
            
            featureString = (char *) "Finer Brightnes control";
            break;
            
        default:
            isFeatureKey = false;
            break;
    }
    if (isFeatureKey && goingDown) {
        if (feature != 0)
            _device->dispatchPS2Notification(feature);
        
        setAlphaLockFeedback(true);
    }
    
    else if (isFeatureKey){
        setAlphaLockFeedback(false);
        IOLog("%s :: Toggled Feature %s.\n", this->getName(),featureString);
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * Process Extended Functions generated on Shift + Fx
 */
bool ApplePS2Keyboard::ProcessExtendedFunctions(UInt8 keyCode, bool goingDown)
{
    
    DEBUG_LOG("%s :: Processing Extended Function for the key 0x%x (%d), goingDown %s.\n", this->getName(),keyCode,keyCode,goingDown?"Yes":"No");
    bool extFunction = true;
    
    switch (keyCode) {
        case 0x3b:
            if (strncmp(fx[0], "", strlen(fx[0]))) {
                ProcessKeyCombinations(fx[0], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x64;
            break;
            
        case 0x3c:
            if (strncmp(fx[1], "", strlen(fx[1]))) {
                ProcessKeyCombinations(fx[1], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x65;
            break;
            
        case 0x3d:
            if (strncmp(fx[2], "", strlen(fx[2]))) {
                ProcessKeyCombinations(fx[2], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x66;
            break;
            
        case 0x3e:
            if (strncmp(fx[3], "", strlen(fx[3]))) {
                ProcessKeyCombinations(fx[3], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x67;
            break;
            
        case 0x3f:
            if (strncmp(fx[4], "", strlen(fx[4]))) {
                ProcessKeyCombinations(fx[4], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x68;
            break;
            
        case 0x40:
            if (strncmp(fx[5], "", strlen(fx[5]))) {
                ProcessKeyCombinations(fx[5], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x69;
            break;
            
        case 0x41:
            if (strncmp(fx[6], "", strlen(fx[6]))) {
                ProcessKeyCombinations(fx[6], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x6a;
            break;
            
        case 0x42:
            if (strncmp(fx[7], "", strlen(fx[7]))) {
                ProcessKeyCombinations(fx[7], goingDown, false);
                keyCode = 0;
            }
            else
                keyCode = 0x6b;;
            break;
            
        case 0x43: // F21
            if (strncmp(fx[8], "", strlen(fx[8]))) {
                ProcessKeyCombinations(fx[8], goingDown, false);
            }
            keyCode = 0;
            break;
            
        case 0x44: // F22
            if (strncmp(fx[9], "", strlen(fx[9]))) {
                ProcessKeyCombinations(fx[9], goingDown, false);
            }
            keyCode = 0;
            break;
            
        case 0x57: // F23
            if (strncmp(fx[10], "", strlen(fx[10]))) {
                ProcessKeyCombinations(fx[10], goingDown, false);
            }
            keyCode = 0;
            break;
            
        case 0x58: // F24
            if (strncmp(fx[11], "", strlen(fx[11]))) {
                ProcessKeyCombinations(fx[11], goingDown, false);
            }
            keyCode = 0;
            break;
            
        default:
            keyCode = 0;
            extFunction = false;
            break;
    }
    
    if (keyCode) {
        
        DEBUG_LOG("%s :: Dispatching Extended Normal Function for the key 0x%x (%d), goingDown %s.\n", this->getName(), keyCode, keyCode, goingDown?"Yes":"No");
        
        // Dispatch key
        if (goingDown)
        {
            // release shift key for Fxx
            dispatchKeyboardKeyCode( PS2ToADBMap[0x2a], !goingDown);
            // dispatch Fxx
            dispatchKeyboardKeyCode( PS2ToADBMap[keyCode], goingDown);
        }
        else
        {   // release Fxx
            dispatchKeyboardKeyCode( PS2ToADBMap[keyCode], goingDown);
            // press shift key after Fxx
            dispatchKeyboardKeyCode( PS2ToADBMap[0x2a], !goingDown);
        }
    }
    
    return extFunction;
    
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * Manual ADB Code Generator
 */
void ApplePS2Keyboard::generateAndSeeADBCode(UInt32 keyCode, bool goingDown)
{
    
    /*
     * This is useful to know what a particular ADB code does in OSX
     * we enter the ADB code using keypad when fn key is pressed down and
     * the generated code is sent when fn key is released
     */
    if (goingDown)
    {
        
        if (keyCode == 0x52)
            _generatedCode = 0;
        if (keyCode ==  0x4f)
            _generatedCode = 1;
        if (keyCode ==  0x50)
            _generatedCode = 2;
        if (keyCode ==  0x51)
            _generatedCode = 3;
        if (keyCode ==  0x4b)
            _generatedCode = 4;
        if (keyCode ==  0x4c)
            _generatedCode = 5;
        if (keyCode ==  0x4d)
            _generatedCode = 6;
        if (keyCode ==  0x47)
            _generatedCode = 7;
        if (keyCode ==  0x48)
            _generatedCode = 8;
        if (keyCode ==  0x49)
            _generatedCode = 9;
        
        
        // Store the three digits in each of the temp variables
        // to calculate the three didigt ADB code
        if (_tempCode1 == 0)
        {
            _tempCode1 = _generatedCode;
            _tempCode2 = 0;
        }
        else if (_tempCode1 > 0 && _tempCode2 == 0)
        {
            if (_generatedCode == 0)// Entered number is 0
                _tempCode2 = 10;
            else
                _tempCode2 = _generatedCode;
            
            _generatedCode = (_tempCode1 * 10) + _generatedCode;
            
        }
        else if (_tempCode1 > 0 && _tempCode2 > 0)
        {
            if (_tempCode2 == 10)
                _tempCode2 = 0;
            
            _generatedCode = (_tempCode1*100) + (_tempCode2*10) + _generatedCode;
        }
        
        IOLog("Received code for ADB key generation: 0x%02x (%03d)\n",_generatedCode, _generatedCode);
    }
    
    // Dispatching Generated ADB code
    if (!goingDown && _generatedCode != 0)
    {
        dispatchKeyboardKeyCode( _generatedCode, true);
        dispatchKeyboardKeyCode( _generatedCode, false);
        
        IOLog("Sent generated ADB Key: 0x%02x (%03d)\n",_generatedCode, _generatedCode);
        _generatedCode = _tempCode1 = _tempCode2 = 0;
        
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
 * Process German/Turkish Keyboard layout for ALT/ALT+STGR(CNTRL)/ALTGR
 */
UInt8 ApplePS2Keyboard::MapGermanTurkishKeyboardLayout(UInt8 keyCode,bool goingDown)
{
    DEBUG_LOG("%s :: Processing German layout for the key 0x%x (%d), GoingDown %s.\n", this->getName(),keyCode,keyCode,goingDown?"Yes":"No");
    
    germanTurkishLayout = true;
    switch (keyCode) {
            
        case 0x08: // 7 as {
            keyCode = 0x09; // 8 does this in OSX
            break;
            
        case 0x09: // 8 as [
            keyCode = 0x06; // 5 does this in OSX
            break;
            
        case 0x0a: // 9 as ]
            keyCode = 0x07; // 6 does this in OSX
            break;
            
        case 0x0b: // 0 as }
            keyCode = 0x0a; // 9 does this in OSX
            break;
            
        case 0x0c: // ß as '\'
            // press shift key on key down
            if (goingDown) {
                
                dispatchKeyboardKeyCode( PS2ToADBMap[0x2a], true);
            } // release shift key after on key up
            else {
                
                dispatchKeyboardKeyCode( PS2ToADBMap[0x2a], false);
            }
            keyCode = 0x08; //7, Shift + 7 does this in OSX
            break;
            
        case 0x1b: // + as ~
            keyCode = 0x31; // N does this in OSX
            break;
            
        case 0x10: // Q as @
            keyCode = 0x26; // L does this in OSX
            break;
            
        case 0x56: // < as |
            keyCode = 0x08; // 7 does this in OSX
            break;
            
        default:
            germanTurkishLayout = false;
            break;
    }
    
    if (germanTurkishLayout)
    {
        //
        // Remove STGR (control) from the dispatch of X in (STGR + ALT + X) to make this ALT + X
        // Add STGR (control) after the dispatch of X in (STGR + ALT + X) in German/Turkish layout
        //
        
        // Dispatch key
        if (goingDown)
        {
            // release control key from STGR + ALT + X
            if (KBV_IS_KEYDOWN(_contrlKey, _keyBitVector)) {
                dispatchKeyboardKeyCode( PS2ToADBMap[_contrlKey], !goingDown);
            }
            
            // dispatch key
            dispatchKeyboardKeyCode( PS2ToADBMap[keyCode], goingDown);
        }
        else
        {   // dispatch key
            dispatchKeyboardKeyCode( PS2ToADBMap[keyCode], goingDown);
            
            // press control key back after ALT + X
            if (KBV_IS_KEYDOWN(_contrlKey, _keyBitVector)) {
                dispatchKeyboardKeyCode( PS2ToADBMap[_contrlKey], !goingDown);
            }
        }
        keyCode = 0;
    }
    
    return keyCode;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if _osxVersion > 11

bool ApplePS2Keyboard::doesKeyLock(unsigned key) {
    return (key == NX_KEYTYPE_CAPS_LOCK) || super::doesKeyLock(key);
}

#endif

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::setAlphaLockFeedback(bool locked)
{
    //
    // Set the keyboard LEDs to reflect the state of alpha (caps) lock.
    //
    // It is safe to issue this request from the interrupt/completion context.
    //
    
    _ledState = locked ? (_ledState | kLED_CapsLock):(_ledState & ~kLED_CapsLock);
    setLEDs(_ledState);
    
    DEBUG_LOG("%s :: Caps Lock set to %x\n",this->getName(), _ledState);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::setNumLockFeedback(bool locked)
{
    if (noNumLockLED) return;
    
    //
    // Set the keyboard LEDs to reflect the state of num lock.
    //
    // It is safe to issue this request from the interrupt/completion context.
    //
    
    _ledState = locked ? (_ledState | kLED_NumLock):(_ledState & ~kLED_NumLock);
    setLEDs(_ledState);
    
    DEBUG_LOG("%s :: Num Lock set to %x\n",this->getName(), _ledState);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::setLEDs(UInt8 ledState)
{
    //
    // Asynchronously instructs the controller to set the keyboard LED state.
    //
    // It is safe to issue this request from the interrupt/completion context.
    //
    
    PS2Request * request = _device->allocateRequest();
    
    // (set LEDs command)
    request->commands[0].command = kPS2C_WriteDataPort;
    request->commands[0].inOrOut = kDP_SetKeyboardLEDs;
    request->commands[1].command = kPS2C_ReadDataPortAndCompare;
    request->commands[1].inOrOut = kSC_Acknowledge;
    request->commands[2].command = kPS2C_WriteDataPort;
    request->commands[2].inOrOut = ledState;
    request->commands[3].command = kPS2C_ReadDataPortAndCompare;
    request->commands[3].inOrOut = kSC_Acknowledge;
    request->commandsCount = 4;
    _device->submitRequest(request); // asynchronous, auto-free'd
    
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::setKeyboardEnable(bool enable)
{
    //
    // Instructs the keyboard to start or stop the reporting of key events.
    // Be aware that while the keyboard is enabled, asynchronous key events
    // may arrive in the middle of command sequences sent to the controller,
    // and may get confused for expected command responses.
    //
    // It is safe to issue this request from the interrupt/completion context.
    //
    
    PS2Request * request = _device->allocateRequest();
    
    // (keyboard enable/disable command)
    request->commands[0].command = kPS2C_WriteDataPort;
    request->commands[0].inOrOut = (enable)?kDP_Enable:kDP_SetDefaultsAndDisable;
    request->commands[1].command = kPS2C_ReadDataPortAndCompare;
    request->commands[1].inOrOut = kSC_Acknowledge;
    request->commandsCount = 2;
    _device->submitRequest(request); // asynchronous, auto-free'd
    
    if (request->commandsCount == 2) {
        if (enable)
            IOLog("STD :: Keyboard enabled.\n");
        else
            IOLog("STD :: Keyboard disabled.\n");
    }
    else
        IOLog("STD :: Keyboard enable failed.\n");
    
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::setCommandByte(UInt8 setBits, UInt8 clearBits)
{
    //
    // Sets the bits setBits and clears the bits clearBits "atomically" in the
    // controller's Command Byte.   Since the controller does not provide such
    // a read-modify-write primitive, we resort to a test-and-set try loop.
    //
    // Do NOT issue this request from the interrupt/completion context.
    //
    
    UInt8        commandByte;
    UInt8        commandByteNew;
    PS2Request * request = _device->allocateRequest();
    
    do
    {
        // (read command byte)
        request->commands[0].command = kPS2C_WriteCommandPort;
        request->commands[0].inOrOut = kCP_GetCommandByte;
        request->commands[1].command = kPS2C_ReadDataPort;
        request->commands[1].inOrOut = 0;
        request->commandsCount = 2;
        _device->submitRequestAndBlock(request);
        
        //
        // Modify the command byte as requested by caller.
        //
        
        commandByte    = request->commands[1].inOrOut;
        commandByteNew = (commandByte | setBits) & (~clearBits);
        
        // ("test-and-set" command byte)
        request->commands[0].command = kPS2C_WriteCommandPort;
        request->commands[0].inOrOut = kCP_GetCommandByte;
        request->commands[1].command = kPS2C_ReadDataPortAndCompare;
        request->commands[1].inOrOut = commandByte;
        request->commands[2].command = kPS2C_WriteCommandPort;
        request->commands[2].inOrOut = kCP_SetCommandByte;
        request->commands[3].command = kPS2C_WriteDataPort;
        request->commands[3].inOrOut = commandByteNew;
        request->commandsCount = 4;
        _device->submitRequestAndBlock(request);
        
        //
        // Repeat this loop if last command failed, that is, if the old command byte
        // was modified since we first read it.
        //
        
    } while (request->commandsCount != 4);
    
    _device->freeRequest(request);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const unsigned char * ApplePS2Keyboard::defaultKeymapOfLength(UInt32 * length)
{
    //
    // Keymap data borrowed and modified from IOUSBFamily/AppleUSBKeyboard.
    //
    //Credits to Eric Sunshine <sunshine@sunshineco.com> for details about keymapping and rehabman
    //http://www.xfree.org/current/dumpkeymap.1.html
    //http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
    
    static const unsigned char appleUSAKeyMap[] = {
        // The first 16 bits are always read first, to determine if the rest of
        // the keymap is in shorts (16 bits) or bytes (8 bits). If the first 16 bits
        // equals 0, data is in bytes; if first 16 bits equal 1, data is in shorts.
        0x00,0x00, // data is in bytes
        
        // The next value is the number of modifier keys.
        0x0b,
        
        // modifier definitions
        /******
         Syntax: Modifier key code, No of keys, ADB Code // Info
         
         Modifiers are Shift, Control, Option and Command/Windows
         ******/
        //0x00,0x01,0x39,  //CAPSLOCK, uses one byte.
        0x01,0x01,0x38,// Left Shift
        0x02,0x01,0x3b,// Left Control
        0x03,0x01,0x3a,// Left Option/Alt
        0x04,0x01,0x37,// Left Command/Windows
        0x05,0x15,0x52,0x41,0x4c,0x53,0x54,0x55,0x45,0x58,0x57,0x56,0x5b,0x5c,
        0x43,0x4b,0x51,0x7b,0x7d,0x7e,0x7c,0x4e,0x59,//Keypad
        0x06,0x01,0x72,// HELP,
        0x07,0x01,0x3f,// Apple Fn key
        //0x08,0x01,0x47, Num Lock
        0x09,0x01,0x3c, //Right shift
        0x0a,0x01,0x3e, //Right control
        0x0b,0x01,0x3d, //Right Option
        0x0c,0x01,0x36, //Right Command/Windows/Context Menu
        
        
        // The next value is number of key definitions
        0xa2,
        /*****
         Key definitions Syntax:
         Mask value (0xXX),
         Chartecter set (0xXX), charecter generated when no modifers is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#1 is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#2 is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#1+2 is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#3 is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#3+1 is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#3+2 is used(0xXX),
         Chartecter set (0xXX), charecter generated when modifer#3+2+1 is used(0xXX),
         
         Mask value : is determined based on number of modifiers used through bit setting and
         this determines no of charecters generated depending on the modifier number
         
         Modifer#1 defines both Shift/ALPHA LOCK since both generates same charecters when they are used
         where ALPH LOCK is used only for ALPHA charecters then we set this value for Mask bit value
         Shift is set for others than ALPHA Charecters in Mask bit value
         
         bit values for modifiers:
         bit 3           bit 2           bit 1           bit 0
         Option(m#3)     Control(m#2)    Shift(m#1)      ALPHA LOCK(CAPS)(m#1)
         
         Charecter set: this defines the symbols (or) charectes generated based on ASCII(0x00) or Sysmbol(0x01) set
         
         ******/
        // key defintions
        /*0x00*/0x0d,0x00,0x61,0x00,0x41,0x00,0x01,0x00,0x01,0x00,0xca,0x00,0xc7,0x00,0x01,0x00,0x01,// 0x00, A
        0x0d,0x00,0x73,0x00,0x53,0x00,0x13,0x00,0x13,0x00,0xfb,0x00,0xa7,0x00,0x13,0x00,0x13,// 0x01, S
        0x0d,0x00,0x64,0x00,0x44,0x00,0x04,0x00,0x04,0x01,0x44,0x01,0xb6,0x00,0x04,0x00,0x04,// 0x02, D
        0x0d,0x00,0x66,0x00,0x46,0x00,0x06,0x00,0x06,0x00,0xa6,0x01,0xac,0x00,0x06,0x00,0x06,// 0x03, F
        0x0d,0x00,0x68,0x00,0x48,0x00,0x08,0x00,0x08,0x00,0xe3,0x00,0xeb,0x00,0x00,0x18,0x00,// 0x04, H
        0x0d,0x00,0x67,0x00,0x47,0x00,0x07,0x00,0x07,0x00,0xf1,0x00,0xe1,0x00,0x07,0x00,0x07,// 0x05, G
        0x0d,0x00,0x7a,0x00,0x5a,0x00,0x1a,0x00,0x1a,0x00,0xcf,0x01,0x57,0x00,0x1a,0x00,0x1a,// 0x06, Z
        0x0d,0x00,0x78,0x00,0x58,0x00,0x18,0x00,0x18,0x01,0xb4,0x01,0xce,0x00,0x18,0x00,0x18,// 0x07, X
        0x0d,0x00,0x63,0x00,0x43,0x00,0x03,0x00,0x03,0x01,0xe3,0x01,0xd3,0x00,0x03,0x00,0x03,// 0x08, C
        0x0d,0x00,0x76,0x00,0x56,0x00,0x16,0x00,0x16,0x01,0xd6,0x01,0xe0,0x00,0x16,0x00,0x16,// 0x09, V
        0x02,0x00,0x3c,0x00,0x3e,// 0x0a, ISO Keyboard extra key between Z and Shift
        0x0d,0x00,0x62,0x00,0x42,0x00,0x02,0x00,0x02,0x01,0xe5,0x01,0xf2,0x00,0x02,0x00,0x02,// 0x0b, B
        0x0d,0x00,0x71,0x00,0x51,0x00,0x11,0x00,0x11,0x00,0xfa,0x00,0xea,0x00,0x11,0x00,0x11,// 0x0c, Q
        0x0d,0x00,0x77,0x00,0x57,0x00,0x17,0x00,0x17,0x01,0xc8,0x01,0xc7,0x00,0x17,0x00,0x17,// 0x0d, W
        0x0d,0x00,0x65,0x00,0x45,0x00,0x05,0x00,0x05,0x00,0xc2,0x00,0xc5,0x00,0x05,0x00,0x05,// 0x0e, E
        0x0d,0x00,0x72,0x00,0x52,0x00,0x12,0x00,0x12,0x01,0xe2,0x01,0xd2,0x00,0x12,0x00,0x12,// 0x0f, R
        /*0x10*/0x0d,0x00,0x79,0x00,0x59,0x00,0x19,0x00,0x19,0x00,0xa5,0x01,0xdb,0x00,0x19,0x00,0x19,// 0x10, Y
        0x0d,0x00,0x74,0x00,0x54,0x00,0x14,0x00,0x14,0x01,0xe4,0x01,0xd4,0x00,0x14,0x00,0x14,// 0x11, T
        0x0a,0x00,0x31,0x00,0x21,0x01,0xad,0x00,0xa1,// 0x12, digit 1
        0x0e,0x00,0x32,0x00,0x40,0x00,0x32,0x00,0x00,0x00,0xb2,0x00,0xb3,0x00,0x00,0x00,0x00,// 0x13, 2
        0x0a,0x00,0x33,0x00,0x23,0x00,0xa3,0x01,0xba,// 0x14, 3
        0x0a,0x00,0x34,0x00,0x24,0x00,0xa2,0x00,0xa8,// 0x15, 4
        0x0e,0x00,0x36,0x00,0x5e,0x00,0x36,0x00,0x1e,0x00,0xb6,0x00,0xc3,0x00,0x1e,0x00,0x1e,// 0x16, 6
        0x0a,0x00,0x35,0x00,0x25,0x01,0xa5,0x00,0xbd,// 0x17, 5
        0x0a,0x00,0x3d,0x00,0x2b,0x01,0xb9,0x01,0xb1,// 0x18, =+
        0x0a,0x00,0x39,0x00,0x28,0x00,0xac,0x00,0xab,// 0x19, 9
        0x0a,0x00,0x37,0x00,0x26,0x01,0xb0,0x01,0xab,// 0x1a, 7
        0x0e,0x00,0x2d,0x00,0x5f,0x00,0x1f,0x00,0x1f,0x00,0xb1,0x00,0xd0,0x00,0x1f,0x00,0x1f,// 0x1b, -_
        0x0a,0x00,0x38,0x00,0x2a,0x00,0xb7,0x00,0xb4,// 0x1c, 8
        0x0a,0x00,0x30,0x00,0x29,0x00,0xad,0x00,0xbb,// 0x1d, 0
        0x0e,0x00,0x5d,0x00,0x7d,0x00,0x1d,0x00,0x1d,0x00,0x27,0x00,0xba,0x00,0x1d,0x00,0x1d,// 0x1e, ]}
        0x0d,0x00,0x6f,0x00,0x4f,0x00,0x0f,0x00,0x0f,0x00,0xf9,0x00,0xe9,0x00,0x0f,0x00,0x0f,// 0x1f, O
        /*0x20*/0x0d,0x00,0x75,0x00,0x55,0x00,0x15,0x00,0x15,0x00,0xc8,0x00,0xcd,0x00,0x15,0x00,0x15,// 0x20, U
        0x0e,0x00,0x5b,0x00,0x7b,0x00,0x1b,0x00,0x1b,0x00,0x60,0x00,0xaa,0x00,0x1b,0x00,0x1b,// 0x21, [{
        0x0d,0x00,0x69,0x00,0x49,0x00,0x09,0x00,0x09,0x00,0xc1,0x00,0xf5,0x00,0x09,0x00,0x09,// 0x22, I
        0x0d,0x00,0x70,0x00,0x50,0x00,0x10,0x00,0x10,0x01,0x70,0x01,0x50,0x00,0x10,0x00,0x10,// 0x23, P
        0x10,0x00,0x0d,0x00,0x03,// 0x24, Return
        0x0d,0x00,0x6c,0x00,0x4c,0x00,0x0c,0x00,0x0c,0x00,0xf8,0x00,0xe8,0x00,0x0c,0x00,0x0c,// 0x25, L
        0x0d,0x00,0x6a,0x00,0x4a,0x00,0x0a,0x00,0x0a,0x00,0xc6,0x00,0xae,0x00,0x0a,0x00,0x0a,// 0x26, J
        0x0a,0x00,0x27,0x00,0x22,0x00,0xa9,0x01,0xae,// 0x27, '"
        0x0d,0x00,0x6b,0x00,0x4b,0x00,0x0b,0x00,0x0b,0x00,0xce,0x00,0xaf,0x00,0x0b,0x00,0x0b,// 0x28, K
        0x0a,0x00,0x3b,0x00,0x3a,0x01,0xb2,0x01,0xa2,// 0x29, :;
        0x0e,0x00,0x5c,0x00,0x7c,0x00,0x1c,0x00,0x1c,0x00,0xe3,0x00,0xeb,0x00,0x1c,0x00,0x1c,// 0x2a, \|
        0x0a,0x00,0x2c,0x00,0x3c,0x00,0xcb,0x01,0xa3,// 0x2b, ,<
        0x0a,0x00,0x2f,0x00,0x3f,0x01,0xb8,0x00,0xbf,// 0x2c, /?
        0x0d,0x00,0x6e,0x00,0x4e,0x00,0x0e,0x00,0x0e,0x00,0xc4,0x01,0xaf,0x00,0x0e,0x00,0x0e,// 0x2d, N
        0x0d,0x00,0x6d,0x00,0x4d,0x00,0x0d,0x00,0x0d,0x01,0x6d,0x01,0xd8,0x00,0x0d,0x00,0x0d,// 0x2e, M
        0x0a,0x00,0x2e,0x00,0x3e,0x00,0xbc,0x01,0xb3,// 0x2f, .>
        /*0x30*/0x02,0x00,0x09,0x00,0x19,// 0x30, Tab
        0x0c,0x00,0x20,0x00,0x00,0x00,0x80,0x00,0x00,// 0x31, Space
        0x0a,0x00,0x60,0x00,0x7e,0x00,0x60,0x01,0xbb,// 0x32, `~
        0x02,0x00,0x7f,0x00,0x08,// 0x33, Backspace
        0xff,// 0x34,(DEAD) Media Rewind
        0x02,0x00,0x1b,0x00,0x7e,// 0x35, ESC
        0xff,//  0x36, Right Windows
        0xff,//  0X37, Left Windows
        0xff,//  0x38, Left Shift
        0xff,//  0x39, Caps Locks
        0xff,//  0x3a, Left ALT
        0xff,//  0x3b, Left Control
        0xff,//  0x3c, Right Shift
        0xff,//  0x3d, Right ALT
        0xff,//  0x3e, Right Control
        0xff,//  0x3f, (DEAD) Apple Fn key
        /*0x40*/0xff,//  0x40, (DEAD) Media Fast
        0x00,0x00,0x2e, // 0x41, Keypad ., Delete
        0xff,//  0x42,(DEAD) Media Play/Pause
        0x00,0x00,0x2a,// 0x43, Keyboad *
        0xff,//  0x44,(DEAD) Media Next
        0x00,0x00,0x2b,// 0x45, Keypard +
        0xff,//  0x46,(DEAD) Media Previous
        0x00,0x00,0x1b,//  0x47, Clear
        0xff,//  0x48 Volume Up
        0xff,//  0x49 Volume Down
        0xff,//  0x4a Volume Mute
        0x0e,0x00,0x2f,0x00,0x5c,0x00,0x2f,0x00,0x1c,0x00,0x2f,0x00,0x5c,0x00,0x00,0x0a,0x00,// 0x4b, Keypad /
        0x00,0x00,0x0d,// 0x4c, Keypad enter/ Apple fn + Return = enter
        0xff,//  0x4d,(DEAD) Brightness Up
        0x00,0x00,0x2d,// 0x4e, Keypad -
        0xff,//  0x4f,(DEAD) Brightness Down
        /*0x50*/0xff,//  0x50,(DEAD) Video Mirror
        0x0e,0x00,0x3d,0x00,0x7c,0x00,0x3d,0x00,0x1c,0x00,0x3d,0x00,0x7c,0x00,0x00,0x18,0x46,// 0x51, Keypad =
        0x00,0x00,0x30,// 0x52, Keypad 0, Insert
        0x00,0x00,0x31,// 0x53, Keypad 1, End
        0x00,0x00,0x32,// 0x54, Keypad 2, Down Arrow
        0x00,0x00,0x33,// 0x55, Keypad 3, Page Down
        0x00,0x00,0x34,// 0x56, Keypad 4, Let Arrow
        0x00,0x00,0x35,// 0x57, Keypad 5, Clear
        0x00,0x00,0x36,// 0x58, Keypad 6, Right Arrow
        0x00,0x00,0x37,// 0x59, Keypad 7, Home
        0xff,// 0x5a, (DEAD) Keyboard Lang 3/4 (or) 0x00,0xfe,0x3a, F21
        0x00,0x00,0x38,// 0x5b, Keypad 8, Up Arrow
        0x00,0x00,0x39,// 0x5c, Keypad 9, Page Up
        0xff,//  0x5d (DEAD) Keyboard Int'l 3  (or) 0x00,0xfe,0x3b, F22
        0xff,//  0x5e (DEAD) Brazilian ABNT2 /? (or) 0x00,0xfe,0x3c, F23
        0xff,//  0x5f (DEAD) Brazilian ABNT2 Keypad . (or) 0x00,0xfe,0x3d, F24
        /*0x60*/0x00,0xfe,0x24,//  0x60, F5
        0x00,0xfe,0x25,//  0x61, F6
        0x00,0xfe,0x26,//  0x62, F7
        0x00,0xfe,0x22,//  0x63, F3
        0x00,0xfe,0x27,//  0x64, F8
        0x00,0xfe,0x28,//  0x65, F9
        0x00,0xfe,0x38,//  0x66, (DEAD) F19
        0x00,0xfe,0x2a,//  0x67, F11
        0x00,0xfe,0x39,//  0x68, (DEAD) F20
        0x00,0xfe,0x32,// 0x69 F13
        0x00,0xfe,0x35,// 0x6a F16
        0x00,0xfe,0x33,// 0x6b F14
        0x00,0xfe,0x36,// 0x6c,(DEAD) F17
        0x00,0xfe,0x29,// 0x6d, F10
        0x00,0xfe,0x37,// 0x6e,(DEAD) F18
        0x00,0xfe,0x2b,// 0x6f, F12
        /*0x70*/0xff,//  0x70, (DEAD) Eject
        0x00,0xfe,0x34,//  0x71 F15
        0xff,//  0x72, (DEAD) Help
        0x00,0xfe,0x2e,//  0x73, Keypad Home
        0x00,0xfe,0x30,//  0x74, Keypad PgUp
        0x00,0xfe,0x2d,//  0x75, Keypad Del
        0x00,0xfe,0x23,//  0x76, F4
        0x00,0xfe,0x2f,//  0x77, Keypad End
        0x00,0xfe,0x21,//  0x78, F2
        0x00,0xfe,0x31,//  0x79, Keypad PgDn
        0x00,0xfe,0x20,//  0x7a, F1
        0x00,0x01,0xac, // 0x7b  Left arrow
        0x00,0x01,0xae, // 0x7c  RDight arrow
        0x00,0x01,0xaf, // 0x7d  Down arrow.
        0x00,0x01,0xad, // 0x7e  Up arrow
        /* Extended for Mapping */
        0x00,0x00,0x00, // 0x7f Power button
        0x00,0x00,0x00,
        0x00,0x00,0x00, // 0x81 Spotlight
        0x00,0x00,0x00, // 0x82 Dashboard
        0x00,0x00,0x00, // 0x83 Launchpad
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00, // 0x90 Brightness Up
        0x00,0x00,0x00, // 0x90 Brightness Down
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00,
        0x00,0x00,0x00, // 0xa0 Misson control
        0x00,0x00,0x00, // 0xa1 Expose Desktop
        
        
        // the next value is number of sequence definitions
        0x0f,
        /*****
         Synatx:     Number of Charecters, 0xff, Modifer Key, Charecter set, Charecter generated
         *****/
        //seq deff
        0x02,0xff,0x04,0x00,0x31,
        0x02,0xff,0x04,0x00,0x32,
        0x02,0xff,0x04,0x00,0x33,
        0x02,0xff,0x04,0x00,0x34,
        0x02,0xff,0x04,0x00,0x35,
        0x02,0xff,0x04,0x00,0x36,
        0x02,0xff,0x04,0x00,0x37,
        0x02,0xff,0x04,0x00,0x38,
        0x02,0xff,0x04,0x00,0x39,
        0x02,0xff,0x04,0x00,0x30,
        0x02,0xff,0x04,0x00,0x2d,
        0x02,0xff,0x04,0x00,0x3d,
        0x02,0xff,0x04,0x00,0x70,
        0x02,0xff,0x04,0x00,0x5d,
        0x02,0xff,0x04,0x00,0x5b,
        
        // The next value is the number of special keys
        0x10,
        /*****
         Synatx:     Special Key,    ADB Code
         *****/
        
        NX_KEYTYPE_SOUND_UP,		0x48,
        NX_KEYTYPE_SOUND_DOWN,		0x49,
        NX_KEYTYPE_BRIGHTNESS_UP,	0x4d,
        NX_KEYTYPE_BRIGHTNESS_DOWN,	0x4f,
        NX_KEYTYPE_CAPS_LOCK,		0x39,
        NX_KEYTYPE_HELP,		0x72,
        NX_POWER_KEY,			0x7F,
        NX_KEYTYPE_MUTE,		0x4a,
        NX_KEYTYPE_NUM_LOCK,		0x47,
        NX_KEYTYPE_EJECT,		0x70,
        NX_KEYTYPE_VIDMIRROR,		0x50,
        NX_KEYTYPE_PLAY,		0x42,
        NX_KEYTYPE_NEXT,		0x44,
        NX_KEYTYPE_PREVIOUS,		0x46,
        NX_KEYTYPE_FAST,			0x40,
        NX_KEYTYPE_REWIND,          0x34,
        
    };
    
    *length = sizeof(appleUSAKeyMap);
    return appleUSAKeyMap;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2Keyboard::setDevicePowerState( UInt32 whatToDo )
{
    switch ( whatToDo )
    {
        case kPS2C_DisableDevice:
            
            //
            // Disable keyboard.
            //
            
            isKeyboardEnabled = false;
            
            setKeyboardEnable(false);
            
            break;
            
        case kPS2C_EnableDevice:
            
            //
            // Enable Keyboard.
            //
            
            //
            // Reset keyBitVector for key press states to start with all keys up
            //
            
            bzero(_keyBitVector, sizeof(_keyBitVector));
            
            // Release if there are any keys down before sleep
            for (int sCode = 0; sCode < KBV_NUM_KEYCODES; sCode++)
            {
                if (KBV_IS_KEYDOWN(sCode, _keyBitVector)) {
                    dispatchKeyboardKeyCode(sCode, false);
                }
            }
            
            //
            // Initialize the keyboard LED state.
            //
            
            setLEDs(_ledState);
            
            //
            // Enable the keyboard clock (should already be so), the keyboard IRQ line,
            // and the keyboard Kscan -> scan code translation mode.
            //
            
            setCommandByte(kCB_EnableKeyboardIRQ | kCB_TranslateMode,
                           kCB_DisableKeyboardClock);
            
            //
            // Finally, we enable the keyboard itself, so that it may start reporting
            // key events.
            //
            
            setKeyboardEnable(true);
            
            isKeyboardEnabled = true;
            
            break;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// Method for receiving PS2 Notifications from Touchpad/Mouse

void ApplePS2Keyboard::receiveTouchpadNotifications(UInt32 data)
{
    //
    // In order to get the OS X version number from touchpad,
    // First OS X version is initialized with Zero using the event kPS2C_OSXVersionNotify,
    // and then next notification we are going to get will give us OS X version data.
    //
    
    if (_osxVersion == 0)
    {
        _osxVersion = data;
    }
    
    /*
     * Check for the Modifiers position
     * for the gestures which are using modifiers
     */
    switch (data) {
        case 1:
        case 2:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 53:
        case 54:
        case 55:
        case 70:
        case 71:
            readKeyboardModifiers();
            break;
            
        default:
            break;
    }
    
    switch (data) {
            
        case kPS2C_BrightnessDown:
            // IOLog("STD :: PS2 Brightness down Notification %d\n", data);
            processFnKeys(0x20, false);
            break;
            
        case kPS2C_BrightnessUp:
            // IOLog("STD :: PS2 Brightness up Notification %d\n", data);
            processFnKeys(0x10, false);
            break;
            
        case kPS2C_ReleaseCmdKey:// Just to release cmd after sleep and swipe down
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            releaseGestureKey = false;
            break;
            
        case kPS2C_ZoomPlus:// Zoom In, cmd + '+'
            
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x45, true);
            dispatchKeyboardKeyCode(0x45, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            IOSleep(100);
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_ZoomMinus:// Zoom Out, cmd + '-'
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x4e, true);
            dispatchKeyboardKeyCode(0x4e, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            IOSleep(100);
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_Gesture_ZoomReset:
        case kPS2C_ZoomReset:// CMD + 0
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x1d, true);
            dispatchKeyboardKeyCode(0x1d, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_RotateLeft:// CMD + L
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x25, true);
            dispatchKeyboardKeyCode(0x25, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_RotateRight:// CMD + R
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x0f, true);
            dispatchKeyboardKeyCode(0x0f, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_Gesture_AppSwitch:// CMD + TAB
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x30, true);
            dispatchKeyboardKeyCode(0x30, false);
            
            releaseGestureKey = true;
            break;
            
        case kPS2C_Gesture_AppClose:// CMD + Q
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(_letterQ, true);
            dispatchKeyboardKeyCode(_letterQ, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case  kPS2C_Gesture_Launchpad:
            if (_osxVersion < 12)
            {
                dispatchKeyboardKeyCode( 131, true);
                dispatchKeyboardKeyCode( 131, false);
            }
            // CMD + CNTRL + SHIFT + L which needs to be configured in Keyboard shortcuts
            else {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(0x38, true);
                dispatchKeyboardKeyCode(0x25, true);
                dispatchKeyboardKeyCode(0x25, false);
                dispatchKeyboardKeyCode(0x38, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            }
            break;
            
        case kPS2C_Gesture_MissionControl:
            if (_osxVersion < 12)
            {
                dispatchKeyboardKeyCode( 160, true);
                dispatchKeyboardKeyCode( 160, false);
            }
            // CNTRL + UP
            else {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(0x7e, true);
                dispatchKeyboardKeyCode(0x7e, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            }
            break;
            
        case kPS2C_Gesture_Dashboard:
            if (_osxVersion < 12)
            {
                dispatchKeyboardKeyCode( 130, true);
                dispatchKeyboardKeyCode( 130, false);
            }
            // F12
            else {
                dispatchKeyboardKeyCode( 0x6f, true);
                dispatchKeyboardKeyCode( 0x6f, false);
            }
            break;
            
        case kPS2C_Gesture_DesktopSwitchLeft:// CNTRL + LEFT
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
            dispatchKeyboardKeyCode(0x7b, true);
            dispatchKeyboardKeyCode(0x7b, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_contrlKey]);
            break;
            
        case kPS2C_Gesture_DesktopSwitchRight:// CNTRL + RIGHT
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
            dispatchKeyboardKeyCode(0x7c, true);
            dispatchKeyboardKeyCode(0x7c, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_contrlKey]);
            break;
            
        case kPS2C_Gesture_AppWindows:// Show Application Windows, Control + F3 of apple (control + mission control)
            if (_osxVersion < 12)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(0xa0, true);
                dispatchKeyboardKeyCode(0xa0, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
                
                // key release fix
                releaseKey(ExtendedPS2ToADBMap[_contrlKey]);
            }
            // CMD + CNTRL + SHIFT + A which needs to be configured in Keyboard shortcuts
            else {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(0x38, true);
                dispatchKeyboardKeyCode(0x00, true);
                dispatchKeyboardKeyCode(0x00, false);
                dispatchKeyboardKeyCode(0x38, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            }
            break;
            
        case kPS2C_Gesture_Minimize:// CMD + M
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(_letterM, true);
            dispatchKeyboardKeyCode(_letterM, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_Gesture_FullScreen:// CMD + CNTRL + F
            
            // Extended ISO keyboard shortcut, CMD + ALT + key
            if (_keyboardTypeID == 0x30 || _keyboardTypeID == 0x31)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], true);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(0x03, true);
                dispatchKeyboardKeyCode(0x03, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], false);
            }
            else {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(0x03, true);
                dispatchKeyboardKeyCode(0x03, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            }
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_contrlKey]);
            break;
            
        case kPS2C_Gesture_Backward:
            
            // Extended ISO keyboard shortcut, CMD + ALT + key
            if (_keyboardTypeID == 0x30 || _keyboardTypeID == 0x31 || _keyboardTypeID == 0x34)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], true);
                dispatchKeyboardKeyCode(_ISOLSquareBracket, true);
                dispatchKeyboardKeyCode(_ISOLSquareBracket, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            }
            // French 1 ISO keyboard acts as Control + right arrow to work
            else if (_keyboardTypeID == 0x32)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(0x7b, true);
                dispatchKeyboardKeyCode(0x7b, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            }
            // Normal keyboards, cmd + [
            else {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(_ISOLSquareBracket, true);
                dispatchKeyboardKeyCode(_ISOLSquareBracket, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
                
            }
            break;
            
        case kPS2C_Gesture_Forward:// Forward
            
            // Extended ISO keyboard shortcut, CMD + ALT + key
            if (_keyboardTypeID == 0x30 || _keyboardTypeID == 0x31 || _keyboardTypeID == 0x34)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], true);
                dispatchKeyboardKeyCode(_ISORSquareBracket, true);
                dispatchKeyboardKeyCode(_ISORSquareBracket, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            }
            // French 1 ISO keyboard acts as Control + left arrow to work
            else if (_keyboardTypeID == 0x32)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
                dispatchKeyboardKeyCode(0x7c, true);
                dispatchKeyboardKeyCode(0x7c, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            }
            // Normal keyboards, cmd + ]
            else {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(_ISORSquareBracket, true);
                dispatchKeyboardKeyCode(_ISORSquareBracket, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
                
            }
            break;
            
        case kPS2C_Gesture_Desktop:// Desktop, CMD + F3 of apple (CMD + Mission control)
            if (_osxVersion < 12)
            {
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
                dispatchKeyboardKeyCode(0xa0, true);
                dispatchKeyboardKeyCode(0xa0, false);
                dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
                
                // key release fix
                releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            }
            // F11
            else {
                dispatchKeyboardKeyCode( 0x67, true);
                dispatchKeyboardKeyCode( 0x67, false);
            }
            break;
            
        case kPS2C_Gesture_NC_Shortcut: // Notification Centre
            // CMD + CNTRL + SHIFT + N which needs to be configured in Keyboard shortcuts
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], true);
            dispatchKeyboardKeyCode(0x38, true);
            dispatchKeyboardKeyCode(0x2d, true);
            dispatchKeyboardKeyCode(0x2d, false);
            dispatchKeyboardKeyCode(0x38, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_contrlKey], false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
            
        case kPS2C_Gesture_GetInfo:// CMD + I to show info
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x22, true);
            dispatchKeyboardKeyCode(0x22, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_commandKey]);
            break;
            
        case kPS2C_Gesture_ToggleDock:// ALT + Cmd + D
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], true);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x02, true);
            dispatchKeyboardKeyCode(0x02, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_altkey]);
            break;
            
        case kPS2C_Gesture_NC_Workaround:
            // Soft NC which is done at touchpad itself
            break;
            
        case kPS2C_Gesture_Finder: // ALT + Cmd + Space
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], true);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x31, true);
            dispatchKeyboardKeyCode(0x31, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_altkey]);
            break;
            
        case kPS2C_Gesture_ForceQuit:// ALT + Cmd + ESC
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], true);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], true);
            dispatchKeyboardKeyCode(0x35, true);
            dispatchKeyboardKeyCode(0x35, false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_commandKey], false);
            dispatchKeyboardKeyCode(ExtendedPS2ToADBMap[_altkey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_altkey]);
            break;
            
            // Process Extended Functions
        case kPS2C_ExtFunF13:
            ProcessKeyCombinations(fx[0], true, true);
            ProcessKeyCombinations(fx[0], false, true);
            break;
            
        case  kPS2C_ExtFunF14:
            ProcessKeyCombinations(fx[1], true, true);
            ProcessKeyCombinations(fx[1], false, true);
            break;
            
        case  kPS2C_ExtFunF15:
            ProcessKeyCombinations(fx[2], true, true);
            ProcessKeyCombinations(fx[2], false, true);
            break;
            
        case  kPS2C_ExtFunF16:
            ProcessKeyCombinations(fx[3], true, true);
            ProcessKeyCombinations(fx[3], false, true);
            break;
            
        case  kPS2C_ExtFunF17:
            ProcessKeyCombinations(fx[4], true, true);
            ProcessKeyCombinations(fx[4], false, true);
            break;
            
        case  kPS2C_ExtFunF18:
            ProcessKeyCombinations(fx[5], true, true);
            ProcessKeyCombinations(fx[5], false, true);
            break;
            
        case  kPS2C_ExtFunF19:
            ProcessKeyCombinations(fx[6], true, true);
            ProcessKeyCombinations(fx[6], false, true);
            break;
            
        case  kPS2C_ExtFunF20:
            ProcessKeyCombinations(fx[7], true, true);
            ProcessKeyCombinations(fx[7], false, true);
            break;
            
        case  kPS2C_ExtFunF21:
            ProcessKeyCombinations(fx[8], true, true);
            ProcessKeyCombinations(fx[8], false, true);
            break;
            
        case  kPS2C_ExtFunF22:
            ProcessKeyCombinations(fx[9], true, true);
            ProcessKeyCombinations(fx[9], false, true);
            break;
            
        case  kPS2C_ExtFunF23:
            ProcessKeyCombinations(fx[10], true, true);
            ProcessKeyCombinations(fx[10], false, true);
            break;
            
        case  kPS2C_ExtFunF24:
            ProcessKeyCombinations(fx[11], true, true);
            ProcessKeyCombinations(fx[11], false, true);
            break;
            
        case kPS2C_CNTRLKeyDown:
            //
            // Read Keyboard modifier positions
            //
            readKeyboardModifiers();
            
            dispatchKeyboardKeyCode(PS2ToADBMap[_contrlKey], true);
            break;
            
        case kPS2C_CNTRLKeyUp:
            //
            // Read Keyboard modifier positions
            //
            readKeyboardModifiers();
            
            dispatchKeyboardKeyCode(PS2ToADBMap[_contrlKey], false);
            
            // key release fix
            releaseKey(ExtendedPS2ToADBMap[_contrlKey]);
            break;
            
        case kPS2C_EnableAsusKBacklight:
            enabledAsusKBacklight = true;
            break;
            
        case kPS2C_EnableAsusKBacklight16Levels:
            asusKBHas16LevelSupport = true;
            break;
            
        case kPS2C_InitKeyboard:
            
            //
            // Set Keyboard string which is displayed at Keyboard Preferences (Optional).
            //
            
            if (OSDynamicCast(OSString, getProperty("Product")) != NULL
                && !strncmp(OSDynamicCast(OSString, getProperty("Product"))->getCStringNoCopy(), "Keyboard", strlen(OSDynamicCast(OSString, getProperty("Product"))->getCStringNoCopy())))
            {
                setProperty("Product", "PS/2 Keyboard / Smart-Pad");
            }
            
            //
            // Initialize Asus Keyboard backlight
            //
            
            if (enabledAsusKBacklight)
            {
                SInt8 nvramValue = -1;
                
                // Load backlight level from NVRAM
                LoadValuesFromNVRAM();
                nvramValue = _kbrdBackLightLvl;
                
                _kbrdBackLightLvl = readAsusKBlight(0);
                setAsusKBlight(0xEA);// init BOFF variable in DSDT
                
                // Restore backlight level
                if (nvramValue != _kbrdBackLightLvl
                    && _kbrdBackLightLvl >= 0
                    && nvramValue >= 0)
                {
                    DEBUG_LOG("STD :: Asus Keyboard backlight NVRAM Level %d\n", nvramValue);
                    
                    // If nvram value was corrupted/missing then set the level to 1
                    if (nvramValue > 16) {
                        _kbrdBackLightLvl = 1;
                        DEBUG_LOG("STD :: Found corrupted/invalid NVRAM Level %d, Keyboard level set to 1\n", nvramValue);
                    }
                    
                    setAsusKBlight(_kbrdBackLightLvl);
                }
                // Just read current blevel
                else {
                    _kbrdBackLightLvl = readAsusKBlight(0);
                }
                
                if (_kbrdBackLightLvl > 0)
                    _device->dispatchPS2Notification(kPS2C_KeybrdBackLightOff);
                
                DEBUG_LOG("STD :: Asus Keyboard backlight Level at boot %d\n", _kbrdBackLightLvl);
            }
            break;
            
        case kPS2C_KeybrdBackLightOff:
            if (enabledAsusKBacklight) {
                setAsusKBlight(0xED); // turn off
                _device->dispatchPS2Notification(kPS2C_KeybrdBackLightToggleOff);
                DEBUG_LOG("STD :: Keyboard backlight off triggered at Lvl %d for Idle timeout\n", _kbrdBackLightLvl);
            }
            break;
            
        case kPS2C_KeybrdBackLightOn:
            // IOLog("STD :: Keyboard backlight turned on event received %d\n",enabledAsusKBacklight);
            
            if (enabledAsusKBacklight) {
                
                UInt8 clvl = readAsusKBlight(0xFF);
                
                if (clvl == 0xFD || clvl == 0xED) {
                    _kbrdBackLightLvl = readAsusKBlight(0);
                    
                    if (clvl > 0) {
                        setAsusKBlight(0xEA);// turn on to last level
                        _device->dispatchPS2Notification(kPS2C_KeybrdBackLightToggleOn);
                        DEBUG_LOG("STD :: Keyboard backlight turned on to Lvl %d from idle timeout off\n", _kbrdBackLightLvl);
                    }
                }
            }
            break;
            
            // Dispatch ESC key
        case kPS2C_ESC:
            dispatchKeyboardKeyCode(0x35, true);
            dispatchKeyboardKeyCode(0x35, false);
            DEBUG_LOG("STD :: NC released\n");
            break;
            
        case kPS2C_EnableTouchpad:
            isTPadEnabled = true;
            break;
            
        case kPS2C_OSXVersionNotify:
            _osxVersion = 0;
            break;
            
        default:
            break;
    }
    
}

/*
 * Sometimes the last key is not being released for some reason
 * so we have to dispatch key press and release for that
 */
void ApplePS2Keyboard::releaseKey(UInt8 keyCode)
{
    dispatchKeyboardKeyCode( keyCode, true);
    dispatchKeyboardKeyCode( keyCode, false);
}

/*
 * NVRAM Load and Save methods
 */
void ApplePS2Keyboard::LoadValuesFromNVRAM()
{
    _kbrdBackLightLvl = -1;
    
    if (OSDictionary *nvramServiceMatch = serviceMatching("IODTNVRAM"))
    {
        if (IODTNVRAM *nvram = OSDynamicCast(IODTNVRAM, waitForMatchingService(nvramServiceMatch, 1000000000ULL * 15))) {
            
            IOLog("%s: NVRAM name->%s\n", this->getName(),nvram->getName());
            
            OSData *bval = 0;
            UInt8 rValue = 0;
            
            //
            // Get the object for NVRAM options
            //
            
            IORegistryEntry *nvramEntry = IORegistryEntry::fromPath("IODeviceTree:/options");
            
            if (!nvramEntry)
                IOLog("%s: Failed to find NVRAM options\n", this->getName());
            else {
                IOLog("%s: Found NVRAM options\n", this->getName());
                
                //
                // Read keyboard backlight value from NVRAM
                //
                
                bval = OSDynamicCast(OSData, nvramEntry->getProperty("AsusKeyboardBackLightLvl"));
                
                if (bval) {
                    memcpy(&rValue, bval->getBytesNoCopy(), bval->getLength());
                    IOLog("%s: Keyboard Backlight value %d\n", this->getName(), rValue);
                    _kbrdBackLightLvl = rValue;
                }
                else {
                    IOLog("%s: Keyboard Backlight value not found\n", this->getName());
                }
                
                nvramEntry->release();
            }
            nvram->release();
        }
        else
            IOLog("%s: NVRAM not available\n", this->getName());
        
        nvramServiceMatch->release();
    }
    
}

void ApplePS2Keyboard::saveValueToNVRAM(const char * symbol, UInt8 value)
{
    IORegistryEntry *nvramEntry = IORegistryEntry::fromPath("IODeviceTree:/options");
    
    if (!nvramEntry)
    {
        IOLog("%s: Failed to find NVRAM options\n", this->getName());
    }
    else {
        DEBUG_LOG("%s: Found NVRAM options\n", this->getName());
        
        //
        // Save backlight value to NVRAM
        //
        
        if (nvramEntry->IORegistryEntry::setProperty(symbol, &value, sizeof(value)) == false)
        {
            IOLog("%s: Failed to save %s value %d in NVRAM\n", this->getName(), symbol, value);
        }
        else
            DEBUG_LOG("%s: %s value %d saved in NVRAM\n", this->getName(), symbol, value);
        
        nvramEntry->release();
    }
}
