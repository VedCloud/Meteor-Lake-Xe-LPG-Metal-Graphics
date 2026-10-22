//
//  PS2Interface.cpp
//  PS2Interface
//
//  Created by EMlyDinEsH on 5/15/16.
//  Copyright (c) 2016 EMlyDinEsH. All rights reserved.
//

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// PS2 methods for interacting with the device.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

#include "PS2Interface.h"
#include "ELAN.h"

#define DEBUG_ENABLE 0

#if DEBUG_ENABLE
#define DEBUG_LOG(fmt, args...)  IOLog(fmt, ## args)
#else
#define DEBUG_LOG(fmt, args...)
#endif

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Send a Synaptics style sliced query command.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::sendSynapticsCmd(ApplePS2MouseDevice* device, UInt8 cmd, UInt8 *param)
{
    DEBUG_LOG("STD :: Synaptics command method called for the command 0x%d\n", cmd);
    
    if (sendSlicedPS2Cmd(device, cmd) || getMouseInformation(device, param))
        return kIOReturnError;
    
    return kIOReturnSuccess;
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// v3 hw and later support this fast Elan style command.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::sendElantechCmd(ApplePS2MouseDevice* device, UInt8 cmd, UInt8 *param)
{
    DEBUG_LOG("STD :: Elantech command method called for the command 0x%d\n", cmd);
    
    IOReturn ret = kIOReturnSuccess;
    PS2Request *request = device->allocateRequest();
    
    request->commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut  = ETD_PS2_CUSTOM_COMMAND;
    request->commands[1].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut  = cmd;
    request->commands[2].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut  = kDP_GetMouseInformation;
    request->commands[3].command = kPS2C_ReadDataPort;
    request->commands[3].inOrOut = 0;
    request->commands[4].command = kPS2C_ReadDataPort;
    request->commands[4].inOrOut = 0;
    request->commands[5].command = kPS2C_ReadDataPort;
    request->commands[5].inOrOut = 0;
    request->commandsCount = 6;
    
    device->submitRequestAndBlock(request);
    
    //
    // Get the data read from the ports
    //
    
    param[0] = request->commands[3].inOrOut;
    param[1] = request->commands[4].inOrOut;
    param[2] = request->commands[5].inOrOut;
    
    if (!(request->commandsCount == 6)) {
        IOLog("STD :: %s query 0x%02x failed.\n", __func__, cmd);
        device->freeRequest(request);
        ret = kIOReturnIOError;
    }
    device->freeRequest(request);
    
    return ret;
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// A retrying version of standard PS2 command for ELAN.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

int  PS2Interface::sendElactechPS2Cmd(ApplePS2MouseDevice* device, UInt8 cmd)
{
    DEBUG_LOG("STD :: Elantech PS2 command method called for the command 0x%x\n", cmd);
    
    IOReturn ret = kIOReturnIOError;
    int tries = ETD_PS2_COMMAND_TRIES;
    
    PS2Request *request = device->allocateRequest();
    
    do {
        request->commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
        request->commands[0].inOrOut  = cmd;
        request->commandsCount = 1;
        
        device->submitRequestAndBlock(request);
        
        if (request->commandsCount == 1)
            ret = kIOReturnSuccess;
        
        device->freeRequest(request);
        
        if (ret == kIOReturnSuccess)
            break;
        
        tries--;
        IOLog("STD :: Retrying PS2 command 0x%02x (%d)...\n", cmd, tries);
        
        IOSleep(ETD_PS2_COMMAND_DELAY);
        
    } while (tries > 0);
    
    if (ret)
        IOLog("STD :: PS2 command 0x%02x failed.\n", cmd);
    
    return ret;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Standard PS2 command.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::sendPS2Cmd(ApplePS2MouseDevice* device, UInt8 cmd)
{
    DEBUG_LOG("STD :: PS2 command method called for the command 0x%x\n", cmd);
    
    IOReturn ret = kIOReturnIOError;
    
    PS2Request *request = device->allocateRequest();
    
    request->commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut  = cmd;
    request->commandsCount = 1;
    
    device->submitRequestAndBlock(request);
    
    if (request->commandsCount == 1)
        ret = kIOReturnSuccess;
    
    device->freeRequest(request);
    
    return ret;
}


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// An extended PS/2 command to the mouse
// using sliced syntax, understood by advanced devices, such as Logitech
// or Synaptics touchpads. The command is encoded as:
// 0xE6 0xE8 rr 0xE8 ss 0xE8 tt 0xE8 uu where (rr*64)+(ss*16)+(tt*4)+uu
// is the command.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::sendSlicedPS2Cmd(ApplePS2MouseDevice* device, UInt8 cmd)
{
    int i, cmdIndex = 0;
    IOReturn ret = kIOReturnSuccess;
    PS2Request *request = device->allocateRequest();
    
    request->commands[cmdIndex].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[cmdIndex].inOrOut  = kDP_SetMouseScaling1To1;
    cmdIndex++;
    
    for (i = 6; i >= 0; i -= 2) {
        unsigned char d = (cmd >> i) & 3;
        
        request->commands[cmdIndex].command  = kPS2C_SendMouseCommandAndCompareAck;
        request->commands[cmdIndex].inOrOut  = kDP_SetMouseResolution;
        cmdIndex++;
        request->commands[cmdIndex].command  = kPS2C_SendMouseCommandAndCompareAck;
        request->commands[cmdIndex].inOrOut  = d;
        cmdIndex++;
    }
    request->commandsCount = cmdIndex;
    
    device->submitRequestAndBlock(request);
    
    if (!(request->commandsCount == cmdIndex)) {
        device->freeRequest(request);
        ret = kIOReturnIOError;
    }
    
    device->freeRequest(request);
    
    return ret;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to set the controller Command Byte.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void PS2Interface::setCommandByte( ApplePS2MouseDevice* device, UInt8 setBits, UInt8 clearBits )
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
    PS2Request * request = device->allocateRequest();
    
    if ( !request ) return;
    
    do
    {
        // (read command byte)
        request->commands[0].command = kPS2C_WriteCommandPort;
        request->commands[0].inOrOut = kCP_GetCommandByte;
        request->commands[1].command = kPS2C_ReadDataPort;
        request->commands[1].inOrOut = 0;
        request->commandsCount = 2;
        device->submitRequestAndBlock(request);
        
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
        device->submitRequestAndBlock(request);
        
        //
        // Repeat this loop if last command failed, that is, if the
        // old command byte was modified since we first read it.
        //
        
    } while (request->commandsCount != 4);
    
    device->freeRequest(request);
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to read the data from the mouse controller.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::getMouseInformation(ApplePS2MouseDevice* device, UInt8 *param)
{
    IOReturn ret = kIOReturnSuccess;
    PS2Request * request = device->allocateRequest();
    
    if ( !request ) return kIOReturnNoMemory;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_GetMouseInformation;
    request->commands[1].command = kPS2C_ReadDataPort;
    request->commands[1].inOrOut = 0;
    request->commands[2].command = kPS2C_ReadDataPort;
    request->commands[2].inOrOut = 0;
    request->commands[3].command = kPS2C_ReadDataPort;
    request->commands[3].inOrOut = 0;
    request->commandsCount = 4;
    
    device->submitRequestAndBlock(request);
    
    if (request->commandsCount == 4)
    {
        DEBUG_LOG("STD :: MouseInformation { 0x%02x, 0x%02x, 0x%02x }\n", request->commands[1].inOrOut, request->commands[2].inOrOut, request->commands[3].inOrOut);
        //
        // Get the data read from the ports
        //
        
        if (param) {
            param[0] = request->commands[1].inOrOut;
            param[1] = request->commands[2].inOrOut;
            param[2] = request->commands[3].inOrOut;
        }
        
    } else {
        IOLog("STD :: Failed to get mouse information\n");
        ret = kIOReturnIOError;
    }
    
    device->freeRequest(request);
    
    return ret;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to read the status of the controller.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::getStatus(ApplePS2MouseDevice* device)
{
    IOReturn ret = kIOReturnSuccess;
    PS2Request * request = device->allocateRequest();
    
    if ( !request ) return kIOReturnNoMemory;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetDefaultsAndDisable;
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = kDP_SetDefaultsAndDisable;
    request->commands[2].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut = kDP_SetDefaultsAndDisable;
    request->commands[3].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut = kDP_GetMouseInformation;
    request->commands[4].command = kPS2C_ReadDataPort;
    request->commands[4].inOrOut = 0;
    request->commands[5].command = kPS2C_ReadDataPort;
    request->commands[5].inOrOut = 0;
    request->commands[6].command = kPS2C_ReadDataPort;
    request->commands[6].inOrOut = 0;
    request->commandsCount = 7;
    device->submitRequestAndBlock(request);
    
    if (request->commandsCount == 7)
    {
        DEBUG_LOG("STD :: Status { 0x%02x, 0x%02x, 0x%02x }\n", request->commands[4].inOrOut, request->commands[5].inOrOut, request->commands[6].inOrOut);
    } else {
        IOLog("STD :: Failed to get status\n");
        ret = kIOReturnIOError;
    }
    
    device->freeRequest(request);
    
    return ret;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to initialize the mouse
// with sample rate, resoultion and scaling  of the mouse.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::initializeMouse(ApplePS2MouseDevice* device, UInt8   mSampleRate,
                                       UInt8   mResolution,
                                       UInt8   mScaling)
{
    IOReturn ret = kIOReturnSuccess;
    
    PS2Request * request = device->allocateRequest();
    
    if ( !request ) return kIOReturnNoMemory;
    
    DEBUG_LOG("STD :: Initializing mouse with Resolution 0x%x, Sample Rate %d and Scaling %s.\n", mResolution, mSampleRate, (mScaling == 0xE6)?"1->1":"2->1");
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetMouseSampleRate;
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = mSampleRate;           // default 0x64 (100 dpi)
    request->commands[2].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut = kDP_SetMouseResolution;
    request->commands[3].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut = mResolution;           // default 0x02 (04 counts/mm)
    request->commands[4].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[4].inOrOut = mScaling;
    
    request->commandsCount = 5;
    device->submitRequestAndBlock(request);
    
    if (request->commandsCount != 5) {
        IOLog("STD :: Failed to Initialize mouse.\n");
        ret = kIOReturnError;
    }
    
    device->freeRequest(request);
    
    //
    // Read mouse information and status
    //
    
    if (ret == kIOReturnSuccess) {
        getMouseInformation(device, NULL);
        getStatus(device);
    }
    
    return ret;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to reset the mouse.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn PS2Interface::resetMouse(ApplePS2MouseDevice* device)
{
    IOReturn ret = kIOReturnSuccess;
    PS2Request * request = device->allocateRequest();
    
    if ( !request ) return kIOReturnNoMemory;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetDefaultsAndDisable; // Disable stream mode
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = kDP_Reset;   // 0xFF
    request->commands[2].command = kPS2C_ReadMouseDataPort;
    request->commands[2].inOrOut = 0;
    request->commands[3].command = kPS2C_ReadMouseDataPort;
    request->commands[3].inOrOut = 0;
    request->commands[4].command = kPS2C_ReadMouseDataPort;
    request->commands[4].inOrOut = 0;
    
    request->commandsCount = 5;
    device->submitRequestAndBlock(request);
    
    // KSC_Reset is received either in first (or) second read after reset.
    if (request->commandsCount == 5
        && (request->commands[2].inOrOut == kSC_Reset ||
            request->commands[3].inOrOut == kSC_Reset ||
            request->commands[4].inOrOut == kSC_Reset))
    {
        DEBUG_LOG("STD :: Reset success with response 0x%x 0x%x 0x%x\n", request->commands[2].inOrOut, request->commands[3].inOrOut, request->commands[4].inOrOut);
    }
    else {
        IOLog("STD :: Mouse reset failed with response 0x%x 0x%x 0x%x, ACK %d.\n", request->commands[2].inOrOut, request->commands[3].inOrOut, request->commands[4].inOrOut, request->commandsCount);
        ret = kIOReturnIOError;
    }
    
    device->freeRequest(request);
    
    return ret;
}