/*
 * Smart Touchpad driver v4.6 for Mac OSX
 *
 * Copyright © EMlyDinEsH (mg-dinesh@live.com) 2012-2015. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Trademarks are the property of their respective owners.
 *
 * Copyright (c) 2005 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 *
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef _APPLEPS2SMARTTOUCHPAD_H
#define _APPLEPS2SMARTTOUCHPAD_H

#include "PS2Interface.h"
#include "SmartTouchpadFeatures.h"

#include <IOKit/hidsystem/IOHIPointing.h>
#include <IOKit/hidsystem/IOHIKeyboard.h>
#include <IOKit/IOCommandGate.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>

/*
 * Touchpad Vendors
 */

#define STD_MOUSE           0x00
#define STD_ELAN_TECH       0x01
#define STD_FOCAL_TECH		0x02
#define STD_SYNAPTICS		0x03

/*
 * Hardware versions
 */

#define STD_MOUSE_HW            0x00

#define STD_ELAN_HW_V1          0x01
#define STD_ELAN_HW_V2          0x02
#define STD_ELAN_HW_V3          0x03
#define STD_ELAN_HW_V4          0x04

#define STD_FOCAL_TECH_HW		0x05
#define STD_SYNAPTICS_HW		0x06


#define TIME_NOW  (uint64_t *)&_now

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// ApplePS2SmartTouchpad Class Declaration
//

class ApplePS2SmartTouchpad : public IOHIPointing, PS2Interface, SmartTouchpadFeatures
{
    OSDeclareDefaultStructors( ApplePS2SmartTouchpad );
    
private:
    ApplePS2MouseDevice* _device;
    IOWorkLoop*          _stdWorkLoop;
    AbsoluteTime         _now;
    
    UInt32               _interruptHandlerInstalled:1;
    UInt32               _powerControlHandlerInstalled:1;
    UInt32               _keyboardNotificationHandlerInstalled:1;
    
    UInt8                _packetBuffer[6];
    UInt8                _packetByteCount;
    UInt8                _packetsCount;
    UInt8                _endPacketsCount;
    UInt8                _pktsize;
    
    UInt8                _debugLvl;
    UInt8                _modeSetTries;
    UInt8                _osxVersion;
    UInt8                _touchpadVendor;

    UInt16               _sleepInitDelay;

    UInt8                _hw_version;

    //
    // Mouse fields
    //
    
    struct ps2MouseStruct {
        UInt8   mSampleRate;
        UInt8   mResolution;
        UInt8   mScaling;
        
        UInt8   lastButton;
        UInt8   dragPackets;
        
        bool    isCtrlScrollSwipe;
        bool    isSwipeDone;
        bool    enableLRSwipe;
        
        uint64_t  lastSwipeTime;
    } _ps2Mouse = {0};
    
    //
    // ELAN fields
    //
    
    struct elanStruct {
        bool   v3ElanLegacyMode;
        bool   paritycheck;
        bool   jumpyCursor;
        bool   reportsPressure;
        bool   crcEnabled;
        bool   sanityCheck;
        bool   supportsTrackPoint;
        
        UInt32 singleFingerReports;
        UInt32 fw_version;
        

        UInt8 parityBit[256];
        UInt8 elanCapabs[3];
        UInt8 tracesWidth;
        
        UInt8 reg_07;
        UInt8 reg_10;
        UInt8 reg_11;
        UInt8 reg_20;
        UInt8 reg_21;
        UInt8 reg_22;
        UInt8 reg_23;
        UInt8 reg_24;
        UInt8 reg_25;
        UInt8 reg_26;
        
    } _elanTPad = {0};
    
    //
    // Synaptics fields
    //
    
    struct synapticsStruct {
        unsigned long int model_id;
        unsigned long int firmware_id;
        unsigned long int board_id;
        unsigned long int capabilities;
        unsigned long int ext_cap;		/* Extended Capabilities */
        unsigned long int ext_cap_0c;	/* Ext Caps from 0x0c query */
        unsigned long int ext_cap_10;	/* Ext Caps from 0x10 query */
        unsigned long int identity;
        unsigned long int x_res, y_res;                /* X/Y resolution in units/mm */
        
        UInt8   pkt_type;                 /* packet type - old, new, etc */
        UInt8   mode;                     /* current mode byte */
        UInt8   ext_buttons;
        UInt8   w;
        UInt8   secondary_pressure;
        UInt32  secondary_x, secondary_y;
        UInt32  m_scroll;
        
        bool    absolute_mode;
        bool    disable_gesture;
        bool    touch_ended;
        bool    useGenericBounds;
        bool    secondary_wait;        /* Waiting for primary packet */
        
        // ForcePad handling
        UInt32      press_start;
        bool        press;
        bool		report_press;
    } _synTPad = {0};
    
    //
    // Plist key type
    //
    
    enum {
        INT8_TYPE,
        INT16_TYPE,
        INT32_TYPE,
        BOOL_TYPE
    } _keyType;
    
    // Controls the driver power state
    void   setDevicePowerState(UInt32 whatToDo);
    
    // Interrupt handler for receiving the touchpad data
    void   interruptOccurred( UInt8 data );
    
    // Notifications handler for receiving keyboard notifications
    void   receiveKeyboardNotifications(UInt32 data);
    
    // Asus Keybaord backlight control
    void ControlfKeybrdBackLight();
    
    //
    // ELAN & Focaltech reg methods to init
    //
    
    IOReturn Elantech_read_reg(UInt8 reg, UInt8 *val);
    IOReturn Elantech_write_reg(UInt8 reg, UInt8 *val);
    
    IOReturn Focaltech_read_reg(int reg, UInt8 *param);
    
    //
    // Methods to process absolute touch and relative mouse/trackpoint events
    //
    
    void processMousePacket(UInt8 *packet);

    void Elantech_input_sync_v4();
    void Process_ELANv2v3_Checks(UInt8 *packet);
    void Process_absolute_events_ELANv1(UInt8 *packet);
    void Process_absolute_events_ELANv2v3(UInt8 packet_type, UInt8 *packet);
    void Process_absolute_events_ELANv4AndFocalTech(UInt8 packet_type, UInt8 *packet);
    void Process_trackpoint_events_ELAN(UInt8 packet_type, UInt8 *packet);

    void Synaptics_report_absolute(UInt8 *packet);
    void Process_passthrough_events_Synaptics(UInt8 *packet, int AbsX, int AbsY);
    IOReturn Synaptics_validate_byte(UInt8 *packet, int idx, UInt8 pkt_type);

    //
    // Methods which processes all the features
    //
    
    void Process_singlefinger_touch(int x, int y, int fid);
    void Process_twofingers_touch(int x1, int x2, int y1, int y2);
    void Process_multiFingers_touch(int m_x, int m_y, int fid);
    void Process_touch_end();
    void Process_fingers_change();
    void Process_scroll(UInt8 sMode, int sdx, int sdy, bool isZoomConflict);
    void Proces_Fingers_Press(UInt8 fingers);
    void Process_ClickPad_2F(UInt8 *packet, UInt8 packetType, int dx1, int dy1, int dx2, int dy2);
    void Process_edgeSwipe(int x, int y, int fid);
    void Dispatch_edgeSwipe(UInt8 gestureAction, UInt8 direction);
    void Is2FingEdgeSwipe();
    
    void Process_Physical_Buttons(UInt8 *packet);
    bool Process_ClickPad_Virtual_Buttons(UInt8 *packet, UInt8 packetType, UInt8 fid, UInt8 fCount);
    void Release_ButtonsAndDrag();
    
    void ResetPointerToMiddle();
    void Open_NC();
    
    void EnhancePointerAcceleration();
    void EnhanceScrollAcceleration();
    void ProcessFeaturesToggle();
    void ProcessInertia(TouchPadModes mode);

    // ACPI and IOREG Polling
    void Process_ACPI_IOREG_Polling();
    
protected:

    // Touchpad Enable methods
    IOReturn    Detect_touchpad();
    void        Reconnect_touchpad();
    void        Enable_touchpad(bool enable);
    
    // ELAN methods
    IOReturn    Elantech_set_properties();
    IOReturn    Elantech_set_absolute_mode();
    IOReturn    Elantech_set_range();
    bool        Elantech_setup_touchpad();
    bool        Elantech_is_signatures_valid(const UInt8 *param);
    
    // FocualTech methods
    bool        Focaltech_setup_touchpad();
    IOReturn    Focaltech_set_absolute_mode();
    
    // Synaptics methods
    void     Synaptics_reset();
    IOReturn Synaptics_detect();
    IOReturn Synaptics_mode_cmd(UInt8 mode);
    IOReturn Synaptics_set_advanced_gesture_mode();
    IOReturn Synaptics_set_absolute_mode();
    bool     Synaptics_setup_touchpad();
    
    // Methods to read plist config
    void    ReadSTDPlistConfig();
    void    ReadAccelPlistConfig();
    UInt8   ReadPlistDictionaryKey(OSDictionary *dict);
    template <class pType> void readPlistKey(const void* keyDict, const char* keyStr, const UInt8 keyType, pType& value);

    // Methods which log the data
    void LogSwipeGestureAction(int action);
    void LogPacketData(UInt8 *packet, int packetType);
    
    // inline methods to dispatch pointer and scroll
    void dispatchRelativePointerPosition(int        dx,
                                         int        dy,
                                         UInt32     buttonState)
    {   clock_get_uptime(TIME_NOW);
        dispatchRelativePointerEvent(dx, dy, buttonState, _now);
        // IOLog("STD :: Position DX %d, DY %d, Button state %x\n", dx, dy, buttonState);
    }
    
    void dispatchScrollDifference(short deltaAxis1,
                                  short deltaAxis2,
                                  short deltaAxis3)
    {
        clock_get_uptime(TIME_NOW);
        dispatchScrollWheelEvent(deltaAxis1, deltaAxis2, deltaAxis3, _now);
        // IOLog("STD :: Scroll DY %d, DX %d, DZ %x\n", deltaAxis1, deltaAxis2, deltaAxis3);
    }
    
    void StopPointerEnhanceTimer()
    {
        _pDx = 0;
        _pDy = 0;
        _pContinuousMove = false;
        _pointerTimer->cancelTimeout();
        // IOLog("STD :: Disabled pointer enhancement timer.\n");
    }
    
    void StopScrollEnhanceTimer()
    {
        if(_scrollTimer) {
            _scrollTimer->cancelTimeout();
           // IOLog("STD :: Disabled Scroll enhancement timer.\n");
        }
        
        _scroll.inertiaDeltaX =_scroll.inertiaDeltaY = 0;
        _doTimerJob = false;

        if (!_scroll.doCont)
        {
            _scroll.doSling = false;
        }
        
        _scroll.doAuto  = false;
        _scroll.doCont  = false;
    }
    
    bool IsFingerOnButtonArea(int y)
    {
        IOLog("STD :: Skipping for finger position Y %d on button area\n", y);
        return (y > (_ymax - _button.clickPadVBtnArea));
    }

public:
    // override pure virtual function to acess abstract classes
    void Get_Gestures_Data() { }
    void Get_PS2_Acesss() { }
    
    // IOKit methods
    virtual bool init( OSDictionary * properties );
    virtual ApplePS2SmartTouchpad * probe( IOService * provider,
                                          SInt32 *    score );
    
    virtual bool start( IOService * provider );
    virtual void stop( IOService * provider );
    virtual void free();
    
    // IOHIPointing methods
    virtual IOReturn setParamProperties( OSDictionary * dict );
    virtual IOFixed resolution();
    
    virtual UInt32 deviceType();
    virtual UInt32 interfaceID();
    
    // Utilities
    virtual void detectOSXVersion();
    virtual void detectTouchpadVendor();
    virtual void detectExternalMouse();
    
    void setAccelAndResolution();
    
    template<typename T, size_t N>
    size_t elementSizeOfArray(T (&arr)[N])
    {
        return N;
        // We can even do this now:
        // size_t count = sizeof(arr)/sizeof(arr[0]);
    }
    
    static UInt32 getAbsoluteValue(int value)
    {
        if (value < 0)
            value = 0 - value;
        
        return value;
    }
    
    static uint64_t absoluteToUINT64(AbsoluteTime	abstime)
    {
        return (*(uint64_t *)&abstime);
    }
};

#endif /* _APPLEPS2SMARTTOUCHPAD_H */