//
//  VoodooI2CHIDDevice.cpp
//  VoodooI2CHID
//
//  Created by Alexandre on 25/08/2017.
//  Copyright © 2017 Alexandre Daoud. All rights reserved.
//

#include <IOKit/hid/IOHIDDevice.h>
#include <kern/locks.h>
#include "VoodooI2CHIDDevice.hpp"
#include "../../../VoodooI2C/VoodooI2C/VoodooI2CDevice/VoodooI2CDeviceNub.hpp"

#define super IOHIDDevice
OSDefineMetaClassAndStructors(VoodooI2CHIDDevice, IOHIDDevice);

extern volatile AbsoluteTime last_multi_touch_event;

bool VoodooI2CHIDDevice::init(OSDictionary* properties) {
    if (!super::init(properties))
        return false;
    awake = true;
    bool temp = false;
    reset_event = &temp;
    memset(&hid_descriptor, 0, sizeof(VoodooI2CHIDDeviceHIDDescriptor));
    acpi_device = NULL;
    api = NULL;
    command_gate = NULL;
    interrupt_simulator = NULL;
    interrupt_source = NULL;
    ready_for_input = false;

    client_lock = IOLockAlloc();

    clients = OSArray::withCapacity(1);

    if (!client_lock || !clients) {
        OSSafeReleaseNULL(clients);
        return false;
    }

    return true;
}

void VoodooI2CHIDDevice::free() {
    if (client_lock)
        IOLockFree(client_lock);

    super::free();
}

IOReturn VoodooI2CHIDDevice::getHIDDescriptor() {
    VoodooI2CHIDDeviceCommand command;
    command.c.reg = hid_descriptor_register;

    if (api->writeReadI2C(command.data, 2, (UInt8*)&hid_descriptor, (UInt16)sizeof(VoodooI2CHIDDeviceHIDDescriptor)) != kIOReturnSuccess) {
        IOLog("%s::%s Request for HID descriptor failed\n", getName(), name);
        return kIOReturnIOError;
    }

    return parseHIDDescriptor();
}

IOReturn VoodooI2CHIDDevice::parseHIDDescriptor() {
    if (hid_descriptor.bcdVersion != 0x0100) {
        IOLog("%s::%s Incorrect BCD version %d\n", getName(), name, hid_descriptor.bcdVersion);
        return kIOReturnInvalid;
    }

    if (hid_descriptor.wHIDDescLength != sizeof(VoodooI2CHIDDeviceHIDDescriptor)) {
        IOLog("%s::%s Unexpected size of HID descriptor\n", getName(), name);
        return kIOReturnInvalid;
    }

    OSDictionary* property_array = OSDictionary::withCapacity(1);
    if (!property_array)
        return kIOReturnNoMemory;

    setOSDictionaryNumber(property_array, "HIDDescLength",      hid_descriptor.wHIDDescLength);
    setOSDictionaryNumber(property_array, "BCDVersion",         hid_descriptor.bcdVersion);
    setOSDictionaryNumber(property_array, "ReportDescLength",   hid_descriptor.wReportDescLength);
    setOSDictionaryNumber(property_array, "ReportDescRegister", hid_descriptor.wReportDescRegister);
    setOSDictionaryNumber(property_array, "MaxInputLength",     hid_descriptor.wMaxInputLength);
    setOSDictionaryNumber(property_array, "InputRegister",      hid_descriptor.wInputRegister);
    setOSDictionaryNumber(property_array, "MaxOutputLength",    hid_descriptor.wMaxOutputLength);
    setOSDictionaryNumber(property_array, "OutputRegister",     hid_descriptor.wOutputRegister);
    setOSDictionaryNumber(property_array, "CommandRegister",    hid_descriptor.wCommandRegister);
    setOSDictionaryNumber(property_array, "DataRegister",       hid_descriptor.wDataRegister);
    setOSDictionaryNumber(property_array, "VendorID",           hid_descriptor.wVendorID);
    setOSDictionaryNumber(property_array, "ProductID",          hid_descriptor.wProductID);
    setOSDictionaryNumber(property_array, "VersionID",          hid_descriptor.wVersionID);

    setProperty("HIDDescriptor", property_array);

    OSSafeReleaseNULL(property_array);

    return kIOReturnSuccess;
}

IOReturn VoodooI2CHIDDevice::getHIDDescriptorAddress() {
    IOReturn ret;
    OSObject *obj = nullptr;

    ret = api->evaluateDSM(I2C_DSM_HIDG, HIDG_DESC_INDEX, &obj);
    if (ret == kIOReturnSuccess) {
        OSNumber *number = OSDynamicCast(OSNumber, obj);
        if (number != nullptr) {
            hid_descriptor_register = number->unsigned16BitValue();
            setProperty("HIDDescriptorAddress", hid_descriptor_register, 16);
        } else {
            IOLog("%s::%s HID descriptor address invalid\n", getName(), name);
            ret = kIOReturnInvalid;
        }
    } else {
        IOLog("%s::%s unable to parse HID descriptor address\n", getName(), name);
        ret = kIOReturnNotFound;
    }
    if (obj) obj->release();
    return ret;
}

void VoodooI2CHIDDevice::getInputReport() {
    // I2C-HID length includes its two-byte header. Never parse a failed read.
    const UInt16 capacity = hid_descriptor.wMaxInputLength;
    if (capacity < 2) { latitude_input_malformed++; return; }
    unsigned char* report = (unsigned char*)IOMalloc(capacity);
    if (!report) { latitude_input_failures++; return; }
    bzero(report, capacity);
    AbsoluteTime started, completed;
    clock_get_uptime(&started);
    const IOReturn result = api->readI2C(report, capacity);
    clock_get_uptime(&completed);
    SUB_ABSOLUTETIME(&completed, &started);
    absolutetime_to_nanoseconds(completed, &latitude_input_last_read_ns);
    if (latitude_input_last_read_ns > latitude_input_max_read_ns)
        latitude_input_max_read_ns = latitude_input_last_read_ns;
    latitude_input_reads++;
    if (result != kIOReturnSuccess) {
        latitude_input_failures++;
        latitude_transport_failures++;
        if (result == kIOReturnTimeout) latitude_input_timeouts++;
        setProperty("LatitudeHIDTransportFailures", latitude_transport_failures, 64);
        setProperty("LatitudeHIDTimeoutReads", latitude_input_timeouts, 64);
        setProperty("LatitudeHIDLastReadNS", latitude_input_last_read_ns, 64);
        setProperty("LatitudeHIDMaxReadNS", latitude_input_max_read_ns, 64);
        if (latitude_input_failure_streak < 16) latitude_input_failure_streak++;
        setProperty("LatitudeHIDLastReadResult", (uint64_t)(uint32_t)result, 32);
        setProperty("LatitudeHIDReadFailures", latitude_input_failures, 64);
        IOFree(report, capacity);
        return;
    }
    if (latitude_input_failure_streak) setProperty("LatitudeHIDLastReadResult", (uint64_t)0, 32);
    latitude_input_failure_streak = 0;
    const unsigned length = (unsigned)report[0] | ((unsigned)report[1] << 8);
    latitude_input_last_length = length;
    // Linux I2C_HID_QUIRK_BOGUS_IRQ: an ELAN0xffff reply means no queued data.
    // Limit this imported behaviour to the captured Dell04f3:328e device.
    const bool elan = hid_descriptor.wVendorID == 0x04f3 && hid_descriptor.wProductID == 0x328e;
    if (length == 0xffff && elan) {
        latitude_input_no_data++;
    } else if (length == 0) {
        command_gate->commandWakeup(&reset_event);
    } else if (length < 2 || length > capacity) {
        latitude_input_malformed++;
        setProperty("LatitudeHIDLastMalformedPrefix", report, capacity < 16 ? capacity : 16);
    } else if (ready_for_input && length > 2) {
    // Passive pre-IOHID counters. No packet edits, extra I2C transactions or timers.
    if(hid_descriptor.wVendorID==0x056a && hid_descriptor.wProductID==0x4a19 &&
       length>=10 && (report[2]==0x14 || report[2]==0x0b)) {
        ++latitude_wire_pen_reports;
        if(report[3]&1) ++latitude_wire_pen_tips;
        latitude_wire_flags_or |= report[3];
        UInt32 pressure=static_cast<UInt32>(report[8]) | (static_cast<UInt32>(report[9])<<8);
        if(pressure>latitude_wire_pressure_max)latitude_wire_pressure_max=pressure;
        if(latitude_wire_pen_reports<=8 || !(latitude_wire_pen_reports&63) || !(report[3]&0x20)) {
            setProperty("LatitudeWirePenReports",latitude_wire_pen_reports,64);
            setProperty("LatitudeWirePenTipReports",latitude_wire_pen_tips,64);
            setProperty("LatitudeWirePenFlagsSeen",latitude_wire_flags_or,32);
            setProperty("LatitudeWirePenMaxPressure",latitude_wire_pressure_max,32);
        }
    }


        const unsigned payload = length - 2;
        IOBufferMemoryDescriptor* buffer = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task, 0, payload);
        if (buffer) {
            // Expose exactly the initialized payload; no trailing header-sized garbage.
            if (buffer->writeBytes(0, report + 2, payload) == payload) {
                IOReturn handled = handleReport(buffer, kIOHIDReportTypeInput);
                if (handled == kIOReturnSuccess) latitude_input_delivered++;
                else latitude_input_failures++;
            } else latitude_input_failures++;
            buffer->release();
        } else latitude_input_failures++;
    }
    IOFree(report, capacity);
    // ELAN empty/reset/0xffff replies must not publish a dictionary every poll.
    // Keep the prior non-ELAN diagnostic cadence unchanged.
    if ((latitude_input_reads & 63) == 0 ||
        (!elan && (length < 2 || length > capacity)) ||
        (elan && length != 0 && length != 0xffff && (length < 2 || length > capacity))) {
        setProperty("LatitudeHIDInputReads", latitude_input_reads, 64);
        setProperty("LatitudeHIDNoDataReplies", latitude_input_no_data, 64);
        setProperty("LatitudeHIDLastWireLength", latitude_input_last_length, 32);
        setProperty("LatitudeHIDTransportFailures", latitude_transport_failures, 64);
        setProperty("LatitudeHIDTimeoutReads", latitude_input_timeouts, 64);
        setProperty("LatitudeHIDLastReadNS", latitude_input_last_read_ns, 64);
        setProperty("LatitudeHIDMaxReadNS", latitude_input_max_read_ns, 64);
        setProperty("LatitudeHIDReadFailures", latitude_input_failures, 64);
        setProperty("LatitudeHIDMalformedReports", latitude_input_malformed, 64);
        setProperty("LatitudeHIDDeliveredReports", latitude_input_delivered, 64);
    }
}

IOReturn VoodooI2CHIDDevice::getReport(IOMemoryDescriptor* report, IOHIDReportType reportType, IOOptionBits options) {
    if (!report || (reportType != kIOHIDReportTypeFeature && reportType != kIOHIDReportTypeInput))
        return kIOReturnBadArgument;
    const UInt64 capacity = report->getLength();
    // The transport length is UInt16 and includes the two-byte wire header.
    if (!capacity || capacity > 0xffffU - 2U)
        return kIOReturnBadArgument;
    const UInt16 total = static_cast<UInt16>(capacity + 2);
    UInt8* buffer = static_cast<UInt8*>(IOMalloc(total));
    if (!buffer) return kIOReturnNoMemory;
    bzero(buffer, total);
    const UInt8 reportID = options & 0xff;
    const UInt8 wireType = reportType == kIOHIDReportTypeFeature ? 3 : 1;
    UInt8 command[7] = {
        static_cast<UInt8>(hid_descriptor.wCommandRegister & 0xff),
        static_cast<UInt8>(hid_descriptor.wCommandRegister >> 8),
        static_cast<UInt8>((wireType << 4) | (reportID >= 15 ? 15 : reportID)), 2
    };
    UInt16 length = 4;
    if (reportID >= 15) command[length++] = reportID;
    command[length++] = hid_descriptor.wDataRegister & 0xff;
    command[length++] = hid_descriptor.wDataRegister >> 8;
    IOReturn result = api->writeReadI2C(command, length, buffer, total);
    // Failed or incomplete feature queries must not publish uninitialized data.
    if (result == kIOReturnSuccess) {
        const UInt16 received = static_cast<UInt16>(buffer[0] | (buffer[1] << 8));
        const bool dellMode = reportType == kIOHIDReportTypeFeature && reportID == 3 &&
            hid_descriptor.wVendorID == 0x04f3 && hid_descriptor.wProductID == 0x328e;
        // Captured Dell report 3 contains a 16-bit mode after its report ID.
        const UInt16 minimum = dellMode ? 5 : (reportID ? 4 : 3);
        if (received < minimum || received > total)
            result = kIOReturnUnderrun;
        else if (reportID && buffer[2] != reportID)
            result = kIOReturnError;
        else {
            // IOHID may request the largest feature buffer for a shorter report.
            // Preserve that contract, but never copy bytes beyond the wire length.
            bzero(buffer + received, total - received);
            if (report->writeBytes(0, buffer + 2, capacity) != capacity)
                result = kIOReturnUnderrun;
        }
    }
    IOFree(buffer, total);
    return result;
}

void VoodooI2CHIDDevice::interruptOccured(OSObject* owner, IOInterruptEventSource* src, int intCount) {
    if (!awake)
        return;

    command_gate->attemptAction(OSMemberFunctionCast(IOCommandGate::Action, this, &VoodooI2CHIDDevice::getInputReport));
}

VoodooI2CHIDDevice* VoodooI2CHIDDevice::probe(IOService* provider, SInt32* score) {
    if (!super::probe(provider, score))
        return NULL;

    name = getMatchedName(provider);

    acpi_device = OSDynamicCast(IOACPIPlatformDevice, provider->getProperty("acpi-device"));
    //acpi_device->retain();

    if (!acpi_device) {
        IOLog("%s::%s Could not get ACPI device\n", getName(), name);
        return NULL;
    }

    // Sometimes an I2C HID will have power state methods, lets turn it on in case

    acpi_device->evaluateObject("_PS0");

    api = OSDynamicCast(VoodooI2CDeviceNub, provider);
    //api->retain();

    if (!api) {
        IOLog("%s::%s Could not get VoodooI2C API access\n", getName(), name);
        return NULL;
    }

    if (getHIDDescriptorAddress() != kIOReturnSuccess) {
        IOLog("%s::%s Could not get HID descriptor\n", getName(), name);
        return NULL;
    }

    if (getHIDDescriptor() != kIOReturnSuccess) {
        IOLog("%s::%s Could not get HID descriptor\n", getName(), name);
        return NULL;
    }

    return this;
}

void VoodooI2CHIDDevice::releaseResources() {
    if (command_gate) {
        command_gate->disable();
        work_loop->removeEventSource(command_gate);
        OSSafeReleaseNULL(command_gate);
    }

    stopInterrupt();

    if (interrupt_simulator) {
        work_loop->removeEventSource(interrupt_simulator);
        OSSafeReleaseNULL(interrupt_simulator);
    }

    if (interrupt_source) {
        work_loop->removeEventSource(interrupt_source);
        OSSafeReleaseNULL(interrupt_source);
    }

    if (work_loop) {
        OSSafeReleaseNULL(work_loop);
    }

    if (acpi_device) {
        OSSafeReleaseNULL(acpi_device);
    }

    if (api) {
        if (api->isOpen(this))
            api->close(this);
        OSSafeReleaseNULL(api);
    }
}

IOReturn VoodooI2CHIDDevice::resetHIDDevice() {
    return command_gate->runAction(OSMemberFunctionCast(IOCommandGate::Action, this, &VoodooI2CHIDDevice::resetHIDDeviceGated));
}

IOReturn VoodooI2CHIDDevice::resetHIDDeviceGated() {
    setHIDPowerState(kVoodooI2CStateOn);

    VoodooI2CHIDDeviceCommand command;
    command.c.reg = hid_descriptor.wCommandRegister;
    command.c.opcode = 0x01;
    command.c.report_type_id = 0;

    api->writeI2C(command.data, 4);

    if (quirks & I2C_HID_QUIRK_NO_IRQ_AFTER_RESET) {
        IOSleep(100);
    } else {
        // Device is required to complete a host-initiated reset in at most 5 seconds. We give it 12 as Linux quirks don't handle some devices.

        AbsoluteTime absolute_time, deadline;
        nanoseconds_to_absolutetime(12000000000, &absolute_time);
        clock_absolutetime_interval_to_deadline(absolute_time, &deadline);

        IOReturn sleep = command_gate->commandSleep(&reset_event, deadline, THREAD_UNINT);

        if (sleep == THREAD_TIMED_OUT) {
            IOLog("%s::%s Timeout waiting for device to complete host initiated reset\n", getName(), name);
            return kIOReturnTimeout;
        } else if (sleep == THREAD_AWAKENED) {
            IOLog("%s::%s Device initiated reset accomplished\n", getName(), name);
        }
    }

    return kIOReturnSuccess;
}

IOReturn VoodooI2CHIDDevice::setHIDPowerState(VoodooI2CState state) {
    VoodooI2CHIDDeviceCommand command;
    IOReturn ret = kIOReturnSuccess;
    int attempts = 5;
    do {
        command.c.reg = hid_descriptor.wCommandRegister;
        command.c.opcode = 0x08;
        command.c.report_type_id = state ? I2C_HID_PWR_ON : I2C_HID_PWR_SLEEP;

        ret = api->writeI2C(command.data, 4);
        IOSleep(100);
    } while (ret != kIOReturnSuccess && --attempts >= 0);
    return ret;
}

IOReturn VoodooI2CHIDDevice::setReport(IOMemoryDescriptor* report, IOHIDReportType reportType, IOOptionBits options) {
    if (!report || (reportType != kIOHIDReportTypeFeature && reportType != kIOHIDReportTypeOutput))
        return kIOReturnBadArgument;
    const UInt64 capacity = report->getLength();
    if (!capacity || capacity > 0xffffU) return kIOReturnBadArgument;
    UInt8* data = static_cast<UInt8*>(IOMalloc(capacity));
    if (!data) return kIOReturnNoMemory;
    if (report->readBytes(0, data, capacity) != capacity) {
        IOFree(data, capacity);
        return kIOReturnUnderrun;
    }
    const UInt8 reportID = options & 0xff;
    const bool prefixed = reportID != 0 && data[0] == reportID;
    const UInt32 payloadLength = static_cast<UInt32>(capacity) - (prefixed ? 1U : 0U);
    const UInt32 wireSize = 2U + (reportID ? 1U : 0U) + payloadLength;
    const UInt32 length = 4U + (reportID >= 15 ? 1U : 0U) + 2U + wireSize;
    if (length > 0xffffU) {
        IOFree(data, capacity);
        return kIOReturnBadArgument;
    }
    UInt8* command = static_cast<UInt8*>(IOMalloc(length));
    if (!command) {
        IOFree(data, capacity);
        return kIOReturnNoMemory;
    }
    UInt16 pos = 0;
    command[pos++] = hid_descriptor.wCommandRegister & 0xff;
    command[pos++] = hid_descriptor.wCommandRegister >> 8;
    const UInt8 wireType = reportType == kIOHIDReportTypeFeature ? 3 : 2;
    command[pos++] = (wireType << 4) | (reportID >= 15 ? 15 : reportID);
    command[pos++] = 3;
    if (reportID >= 15) command[pos++] = reportID;
    command[pos++] = hid_descriptor.wDataRegister & 0xff;
    command[pos++] = hid_descriptor.wDataRegister >> 8;
    command[pos++] = wireSize & 0xff;
    command[pos++] = wireSize >> 8;
    if (reportID) command[pos++] = reportID;
    memcpy(command + pos, data + (prefixed ? 1 : 0), payloadLength);
    IOReturn result = api->writeI2C(command, static_cast<UInt16>(length));
    IOSleep(10);
    IOFree(command, length);
    IOFree(data, capacity);
    return result;
}

void VoodooI2CHIDDevice::lookupQuirks() {
    quirks = 0;
    for (size_t n = 0; i2c_hid_quirks[n].idVendor; n++) {
        if (i2c_hid_quirks[n].idVendor == hid_descriptor.wVendorID &&
            (i2c_hid_quirks[n].idProduct == HID_ANY_ID ||
             i2c_hid_quirks[n].idProduct == hid_descriptor.wProductID)) {
            quirks = i2c_hid_quirks[n].quirks;
        }
    }
}

IOReturn VoodooI2CHIDDevice::setPowerState(unsigned long whichState, IOService* whatDevice) {
    if (whatDevice != this)
        return kIOReturnInvalid;
    if (whichState == 0) {
        if (awake) {
            setHIDPowerState(kVoodooI2CStateOff);

            stopInterrupt();

            IOLog("%s::%s Going to sleep\n", getName(), name);
            awake = false;
        }
    } else {
        if (!awake) {
            awake = true;

            startInterrupt();

            if (quirks & I2C_HID_QUIRK_RESET_ON_RESUME) {
                resetHIDDevice();
            } else {
                setHIDPowerState(kVoodooI2CStateOn);
            }

            IOLog("%s::%s Woke up\n", getName(), name);
        }
    }
    return kIOPMAckImplied;
}

bool VoodooI2CHIDDevice::handleStart(IOService* provider) {
    if (!IOHIDDevice::handleStart(provider)) {
        return false;
    }

    work_loop = getWorkLoop();

    if (!work_loop) {
        IOLog("%s::%s Could not get work loop\n", getName(), name);
        goto exit;
    }

    work_loop->retain();

    command_gate = IOCommandGate::commandGate(this);
    if (!command_gate || (work_loop->addEventSource(command_gate) != kIOReturnSuccess)) {
        IOLog("%s::%s Could not open command gate\n", getName(), name);
        goto exit;
    }

    acpi_device->retain();
    api->retain();

    if (!api->open(this)) {
        IOLog("%s::%s Could not open API\n", getName(), name);
        goto exit;
    }

    interrupt_source = IOInterruptEventSource::interruptEventSource(this, OSMemberFunctionCast(IOInterruptEventAction, this, &VoodooI2CHIDDevice::interruptOccured), api, 0);
    if (interrupt_source) {
        work_loop->addEventSource(interrupt_source);
    } else {
        IOLog("%s::%s Warning: Could not get interrupt event source, using polling instead\n", getName(), name);
        interrupt_simulator = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &VoodooI2CHIDDevice::simulateInterrupt));

        if (!interrupt_simulator) {
            IOLog("%s::%s Could not get timer event source\n", getName(), name);
            goto exit;
        }

        work_loop->addEventSource(interrupt_simulator);
    }
    startInterrupt();

    lookupQuirks();
    resetHIDDevice();


    PMinit();
    api->joinPMtree(this);
    registerPowerDriver(this, VoodooI2CIOPMPowerStates, kVoodooI2CIOPMNumberPowerStates);

    // Give the reset a bit of time so that IOHIDDevice doesnt happen to start requesting the report
    // descriptor before the driver is ready

    IOSleep(100);

    return true;
exit:
    releaseResources();
    return false;
}

bool VoodooI2CHIDDevice::start(IOService* provider) {
    if (!super::start(provider))
        return false;

    ready_for_input = true;

    setProperty("VoodooI2CServices Supported", kOSBooleanTrue);

    return true;
}

void VoodooI2CHIDDevice::stop(IOService* provider) {
    IOLockLock(client_lock);
    for(;;) {
        if (!clients->getCount()) {
            break;
        }

        IOLockSleep(client_lock, &client_lock, THREAD_UNINT);
    }
    IOLockUnlock(client_lock);

    releaseResources();
    OSSafeReleaseNULL(clients);
    PMstop();
    super::stop(provider);
}

IOReturn VoodooI2CHIDDevice::newReportDescriptor(IOMemoryDescriptor** descriptor) const {
    if (!hid_descriptor.wReportDescLength) {
        IOLog("%s::%s Invalid report descriptor size\n", getName(), name);
        return kIOReturnDeviceError;
    }

    VoodooI2CHIDDeviceCommand command;
    command.c.reg = hid_descriptor.wReportDescRegister;

    UInt8* buffer = reinterpret_cast<UInt8*>(IOMalloc(hid_descriptor.wReportDescLength));
    memset(buffer, 0, hid_descriptor.wReportDescLength);

    if (api->writeReadI2C(command.data, 2, buffer, hid_descriptor.wReportDescLength) != kIOReturnSuccess) {
        IOLog("%s::%s Could not get report descriptor\n", getName(), name);
        IOFree(buffer, hid_descriptor.wReportDescLength);
        return kIOReturnIOError;
    }

    IOBufferMemoryDescriptor* report_descriptor = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task, 0, hid_descriptor.wReportDescLength);

    if (!report_descriptor) {
        IOLog("%s::%s Could not allocated buffer for report descriptor\n", getName(), name);
        return kIOReturnNoResources;
    }

    report_descriptor->writeBytes(0, buffer, hid_descriptor.wReportDescLength);
    *descriptor = report_descriptor;

    IOFree(buffer, hid_descriptor.wReportDescLength);

    return kIOReturnSuccess;
}

OSNumber* VoodooI2CHIDDevice::newVendorIDNumber() const {
    return OSNumber::withNumber(hid_descriptor.wVendorID, 16);
}

OSNumber* VoodooI2CHIDDevice::newProductIDNumber() const {
    return OSNumber::withNumber(hid_descriptor.wProductID, 16);
}

OSNumber* VoodooI2CHIDDevice::newVersionNumber() const {
    return OSNumber::withNumber(hid_descriptor.wVersionID, 16);
}

OSString* VoodooI2CHIDDevice::newTransportString() const {
    return OSString::withCString("I2C");
}

OSString* VoodooI2CHIDDevice::newManufacturerString() const {
    return OSString::withCString("Apple");
}

void VoodooI2CHIDDevice::simulateInterrupt(OSObject* owner, IOTimerEventSource* timer) {
    AbsoluteTime prev_time = last_multi_touch_event;
    if (awake) {
        VoodooI2CHIDDevice::getInputReport();
    }

    // This Dell ELAN uses polling. Back off failed reads without spinning,
    // replaying an old report, issuing firmware resets or blocking another bus.
    if (hid_descriptor.wVendorID == 0x04f3 && hid_descriptor.wProductID == 0x328e && latitude_input_failure_streak) {
        interrupt_simulator->setTimeoutMS(latitude_input_failure_streak < 3 ? 50 : 250);
        return;
    }

    if (last_multi_touch_event == 0) {
        interrupt_simulator->setTimeoutMS(INTERRUPT_SIMULATOR_TIMEOUT);
        return;
    }

    IOSleep(1);
    if (last_multi_touch_event != prev_time) {
        interrupt_simulator->setTimeoutMS(INTERRUPT_SIMULATOR_TIMEOUT_BUSY);
        return;
    }

    uint64_t        nsecs;
    AbsoluteTime    cur_time;
    clock_get_uptime(&cur_time);
    SUB_ABSOLUTETIME(&cur_time, &last_multi_touch_event);
    absolutetime_to_nanoseconds(cur_time, &nsecs);
    interrupt_simulator->setTimeoutMS((nsecs > 1500000000) ? INTERRUPT_SIMULATOR_TIMEOUT_IDLE : INTERRUPT_SIMULATOR_TIMEOUT_BUSY);
}

bool VoodooI2CHIDDevice::open(IOService *forClient, IOOptionBits options, void *arg) {
    IOLockLock(client_lock);
    clients->setObject(forClient);
    IOUnlock(client_lock);

    return super::open(forClient, options, arg);
}

void VoodooI2CHIDDevice::close(IOService *forClient, IOOptionBits options) {
    IOLockLock(client_lock);

    for(int i = 0; i < clients->getCount(); i++) {
        OSObject* service = clients->getObject(i);

        if (service == forClient) {
            clients->removeObject(i);
            break;
        }
    }

    IOUnlock(client_lock);

    IOLockWakeup(client_lock, &client_lock, true);

    super::close(forClient, options);
}

void VoodooI2CHIDDevice::startInterrupt() {
    if (is_interrupt_started) {
        return;
    }

    if (interrupt_simulator) {
        interrupt_simulator->setTimeoutMS(200);
        interrupt_simulator->enable();
    } else if (interrupt_source) {
        interrupt_source->enable();
    }
    is_interrupt_started = true;
}

void VoodooI2CHIDDevice::stopInterrupt() {
    if (!is_interrupt_started) {
        return;
    }

    if (interrupt_simulator) {
        interrupt_simulator->disable();
    } else if (interrupt_source) {
        interrupt_source->disable();
    }
    is_interrupt_started = false;
}
