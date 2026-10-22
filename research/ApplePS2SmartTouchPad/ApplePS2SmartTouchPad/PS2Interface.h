//
//  PS2Interface.h
//  ApplePS2SmartTouchPad
//
//  Created by EMlyDinEsH on 5/15/16.
//  Copyright (c) 2016 EMlyDinEsH. All rights reserved.
//

#ifndef __ApplePS2SmartTouchPad__PS2Interface__
#define __ApplePS2SmartTouchPad__PS2Interface__

#include "ApplePS2MouseDevice.h"

#include <IOKit/IOLib.h>

class PS2Interface
{
public:
    virtual void Get_PS2_Acesss() = 0;

    virtual void        setCommandByte(ApplePS2MouseDevice* device, UInt8 setBits, UInt8 clearBits);
    virtual IOReturn    initializeMouse(ApplePS2MouseDevice* device, UInt8   mSampleRate,
                                                                     UInt8   mResolution,
                                                                     UInt8   mScaling);
    virtual IOReturn    resetMouse(ApplePS2MouseDevice* device);
    virtual IOReturn    getStatus(ApplePS2MouseDevice* device);
    virtual IOReturn    getMouseInformation(ApplePS2MouseDevice* device, UInt8 *param);
    
    virtual IOReturn    sendSynapticsCmd(ApplePS2MouseDevice* device, UInt8 cmd, UInt8 *param);
    virtual IOReturn    sendElantechCmd(ApplePS2MouseDevice* device, UInt8 cmd, UInt8 *param);
    
    virtual IOReturn    sendSlicedPS2Cmd(ApplePS2MouseDevice* device,UInt8 cmd);
    virtual IOReturn    sendElactechPS2Cmd(ApplePS2MouseDevice* device,UInt8 cmd);
    virtual IOReturn    sendPS2Cmd(ApplePS2MouseDevice* device,UInt8 cmd);

};
#endif /* defined(__ApplePS2SmartTouchPad__PS2Interface__) */
