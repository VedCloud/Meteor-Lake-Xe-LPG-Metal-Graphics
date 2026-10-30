//
//  VoodooI2CTouchscreenHIDEventDriver.cpp
//  VoodooI2CHID
//
//  Created by blankmac on 9/30/17.
//  Copyright © 2017 Alexandre Daoud. All rights reserved.
//

#include "VoodooI2CTouchscreenHIDEventDriver.hpp"
#include "LatitudeWacomPen.hpp"
#include <IOKit/IOBufferMemoryDescriptor.h>

extern AbsoluteTime last_multi_touch_event;

#define super VoodooI2CMultitouchHIDEventDriver

/* Check if the first set of coordinates is within fat finger distance of the second set
 */
static bool isCloseTo(IOFixed x, IOFixed y, IOFixed other_x, IOFixed other_y) {
   IOFixed diff_x = x - other_x;
   IOFixed diff_y = y - other_y;
   return  (diff_x * diff_x) +
           (diff_y * diff_y) <
           FAT_FINGER_ZONE;
}

OSDefineMetaClassAndStructors(VoodooI2CTouchscreenHIDEventDriver, VoodooI2CMultitouchHIDEventDriver);

// Override of VoodooI2CMultitouchHIDEventDriver

bool VoodooI2CTouchscreenHIDEventDriver::checkFingerTouch(AbsoluteTime timestamp, VoodooI2CMultitouchEvent event) {
    absolutetime_to_nanoseconds(timestamp, &last_interaction_time);
    
    // Avoid phantom clicks happening after scrolling
    if ((last_interaction_time - last_multitouch_interaction_time) < MULTITOUCH_TIMEOUT) {
        return false;
    }

    // The first transducer is fixed to stylus on some touchscreens
    for (int index = 0; index < digitiser.contact_count->getValue() + 1; index++) {
        VoodooI2CDigitiserTransducer* transducer = OSDynamicCast(VoodooI2CDigitiserTransducer, event.transducers->getObject(index));

        if (transducer && transducer->type == kDigitiserTransducerFinger && transducer->tip_switch.value()) {
            IOFixed x;
            IOFixed y;
            getCoordinates(transducer, &x, &y);

            if (
                isCloseTo(x, y, last_click_x, last_click_y) &&
                (last_interaction_time - last_click_time) <= DOUBLE_CLICK_TIME
            ) {
                // Double click start
                // If we're clicking again where we just clicked, precisely position the pointer where it was before
                x = last_click_x;
                y = last_click_y;
            }

            if (!is_finger_down) {
                is_finger_down = true;
                touch_start_time = last_interaction_time;
                touch_start_x = x;
                touch_start_y = y;
            } else if (!is_dragging) {
                bool interaction_is_close_to_start = isCloseTo(x, y, touch_start_x, touch_start_y);

                if (
                    !is_right_clicking &&
                     interaction_is_close_to_start &&
                    (last_interaction_time - touch_start_time) >= RIGHT_CLICK_TIME
                ) {
                    // Right click start
                    dispatchDigitizerEventWithTiltOrientation(timestamp, transducer->secondary_id, transducer->type, 0x1, RIGHT_CLICK, x, y);
                    dispatchDigitizerEventWithTiltOrientation(timestamp, transducer->secondary_id, transducer->type, 0x1, HOVER, x, y);
                    is_right_clicking = true;
                }

                if (is_right_clicking) {
                    // Continue right click
                    if (interaction_is_close_to_start) {
                        // Adopt the location of the touch start so we don't stray and close the right click menu
                        x = touch_start_x;
                        y = touch_start_y;
                    }
                    else {
                        // Track if we moved after the right click so we know if we need to click again on release
                        moved_during_right_click = true;
                    }
                } else {
                    // Check for drag start
                    if (!is_dragging && !drag_start_requested && !interaction_is_close_to_start) {
                        scheduleDragStart();
                        drag_start_requested = true;
                    }
                }
            }

            finger_interaction = HOVER;
            if (is_dragging) {
                finger_interaction = LEFT_CLICK;
            }

            dispatchDigitizerEventWithTiltOrientation(timestamp, transducer->secondary_id, transducer->type, 0x1, finger_interaction, x, y);

            // Track last ID and coordinates so that we can send the finger lift event after our watch dog timeout.
            last_x = x;
            last_y = y;
            last_id = transducer->secondary_id;

            scheduleClick();

            return true;
        }
    }
    return false;
}

void VoodooI2CTouchscreenHIDEventDriver::getCoordinates(VoodooI2CDigitiserTransducer* transducer, IOFixed* x, IOFixed* y) {
    // Convert logical coordinates to IOFixed and Scaled;
    *x = ((transducer->coordinates.x.value() * 1.0f) / transducer->logical_max_x) * 65535;
    *y = ((transducer->coordinates.y.value() * 1.0f) / transducer->logical_max_y) * 65535;

    checkRotation(x, y);
}

void VoodooI2CTouchscreenHIDEventDriver::checkRotation(IOFixed* x, IOFixed* y) {
    if (active_framebuffer) {
        if (current_rotation & kIOFBSwapAxes) {
            IOFixed old_x = *x;
            *x = *y;
            *y = old_x;
        }
        if (current_rotation & kIOFBInvertX)
            *x = 65535 - *x;
        if (current_rotation & kIOFBInvertY)
            *y = 65535 - *y;
    }
}

IOReturn VoodooI2CTouchscreenHIDEventDriver::parseElements(UInt32) {
    return super::parseElements(kHIDUsage_Dig_TouchScreen);
}

bool VoodooI2CTouchscreenHIDEventDriver::checkStylus(AbsoluteTime timestamp, VoodooI2CMultitouchEvent event) {
    //  Check the current transducers for stylus operation, dispatch the pointer events and return true.
    //  At this time, Apple has removed all methods of handling additional information from the event driver.  Only x, y, buttonstate, and
    //  inrange are valid for macOS Sierra +.  10.11 still makes use of extended functions.
    
    for (int index = 0, count = event.transducers->getCount(); index < count; index++) {
        VoodooI2CDigitiserTransducer* transducer = OSDynamicCast(VoodooI2CDigitiserTransducer, event.transducers->getObject(index));

        if (transducer->type == kDigitiserTransducerStylus && transducer->in_range) {
            VoodooI2CDigitiserStylus* stylus = (VoodooI2CDigitiserStylus*)transducer;
            IOFixed x = ((stylus->coordinates.x.value() * 1.0f) / stylus->logical_max_x) * 65535;
            IOFixed y = ((stylus->coordinates.y.value() * 1.0f) / stylus->logical_max_y) * 65535;
            IOFixed z = ((stylus->coordinates.z.value() * 1.0f) / stylus->logical_max_z) * 65535;
            IOFixed stylus_pressure = ((stylus->tip_pressure.value() * 1.0f) /stylus->pressure_physical_max) * 65535;
            
            checkRotation(&x, &y);
            
            if (stylus->barrel_switch.value() != HOVER && stylus->barrel_switch.value() != RIGHT_CLICK && (stylus->barrel_switch.value()-barrel_switch_offset) != RIGHT_CLICK)
                barrel_switch_offset = stylus->barrel_switch.value();
            if (stylus->eraser.value() != HOVER && stylus->eraser.value() != RIGHT_CLICK && (stylus->eraser.value()-eraser_switch_offset) != ERASE)
                eraser_switch_offset = stylus->eraser.value();
            
            stylus_buttons = stylus->tip_switch.value();
            
            if (stylus->barrel_switch.value() == RIGHT_CLICK || (stylus->barrel_switch.value() - barrel_switch_offset) == RIGHT_CLICK) {
                stylus_buttons = RIGHT_CLICK;
            }
            
            if (stylus->eraser.value() == ERASE || (stylus->eraser.value() - eraser_switch_offset) == ERASE) {
                stylus_buttons = ERASE;
            }
            
            dispatchDigitizerEventWithTiltOrientation(timestamp, stylus->secondary_id, stylus->type, stylus->in_range, stylus_buttons, x, y, z, stylus_pressure, stylus->barrel_pressure.value(), stylus->azi_alti_orientation.twist.value(), stylus->tilt_orientation.x_tilt.value(), stylus->tilt_orientation.y_tilt.value());

            return true;
        }
    }
    
    return false;
}

void VoodooI2CTouchscreenHIDEventDriver::fingerLift() {
    //  Here we execute a single touch pointer lift event.  Finger based digitizer events have no in_range
    // component.  Greater accuracy / error rejection is achieved by filtering by tip_switch vs transducer id in the
    //  checkFingerTouch function, however, this has the side effect of not releasing the pointer.  This watchdog
    //  timer is needed regardless so the pointer release is best done here.
    
    uint64_t now_abs;
    clock_get_uptime(&now_abs);

    if (!is_dragging && (!is_right_clicking || moved_during_right_click)) {
        dispatchDigitizerEventWithTiltOrientation(now_abs, last_id, kDigitiserTransducerFinger, 0x1, LEFT_CLICK, last_x, last_y);
    }
    dispatchDigitizerEventWithTiltOrientation(now_abs, last_id, kDigitiserTransducerFinger, 0x1, HOVER, last_x, last_y);
    
    last_click_x = last_x;
    last_click_y = last_y;
    absolutetime_to_nanoseconds(now_abs, &last_click_time);

    resetTouch();
    resetScroll();
}

void VoodooI2CTouchscreenHIDEventDriver::resetTouch() {
    is_finger_down = false;
    is_right_clicking = false;
    moved_during_right_click = false;
    is_dragging = false;
    drag_start_requested = false;
}

IOFramebuffer* VoodooI2CTouchscreenHIDEventDriver::getFramebuffer() {
    IORegistryEntry* display = NULL;
    IOFramebuffer* framebuffer = NULL;
    
    OSDictionary *match = serviceMatching("IODisplay");
    OSIterator *iterator = getMatchingServices(match);

    if (iterator) {
        display = OSDynamicCast(IORegistryEntry, iterator->getNextObject());
        
        if (display) {
            IORegistryEntry *entry = display->getParentEntry(gIOServicePlane)->getParentEntry(gIOServicePlane);
            if (entry)
                framebuffer = reinterpret_cast<IOFramebuffer*>(entry->metaCast("IOFramebuffer"));
        }
        
        iterator->release();
    }
    
    OSSafeReleaseNULL(match);

    return framebuffer;
}

void VoodooI2CTouchscreenHIDEventDriver::forwardReport(VoodooI2CMultitouchEvent event, AbsoluteTime timestamp) {
    // Palm rejection. Recover after a lost out-of-range packet / sleep reset.
    if (latitude_pen_qualified && latitude_pen_in_range) {
        UInt64 now_ns = 0; absolutetime_to_nanoseconds(timestamp, &now_ns);
        if (now_ns >= latitude_pen_timestamp_ns && now_ns - latitude_pen_timestamp_ns > 1000000000ULL)
            latitudeReleasePen(timestamp);
        else return;
    }
    if (!active_framebuffer) {
        active_framebuffer = getFramebuffer();
        
        if (active_framebuffer) {
            active_framebuffer->retain();
        }
    }

    if (active_framebuffer) {
        OSNumber* number = OSDynamicCast(OSNumber, active_framebuffer->getProperty(kIOFBTransformKey));
        // IOFramebuffer subclasses need not publish IOFBTransform. Preserve
        // the prior rotation when the property is absent or is not a number.
        if (number) {
            current_rotation = number->unsigned8BitValue() / 0x10;
            multitouch_interface->setProperty(kIOFBTransformKey, current_rotation, 8);
        }
    }
    
    if (event.contact_count) {
        // Send multitouch information to the multitouch interface

        if (event.contact_count >= 2) {
            // Immediately cancel any outstanding click
            timer_source->cancelTimeout();
            drag_timer_source->cancelTimeout();
            resetTouch();

            absolutetime_to_nanoseconds(timestamp, &last_multitouch_interaction_time);

            if (event.contact_count == 2) {
                if (!is_scrolling) {
                    scrollPosition(timestamp, event);
                }
                scheduleScrollEnd();
            }

            multitouch_interface->handleInterruptReport(event, timestamp);
        } else {
            // Process single touch data
            if (!checkStylus(timestamp, event)) {
                if (!checkFingerTouch(timestamp, event))
                    multitouch_interface->handleInterruptReport(event, timestamp);
            }
        }
    }
}

// The captured DeviceConfiguration/DeviceSettings feature has InputMode at
// report 0x0e byte 1 and DeviceIndex at byte 2. Linux wacom_hid_set_device_mode
// explicitly commits InputMode=2, even when it is already the cached value.
void VoodooI2CTouchscreenHIDEventDriver::latitudeInitPenMode() {
    if(!latitude_pen_qualified || !hid_device) return;
    IOBufferMemoryDescriptor* feature = IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,0,3);
    if(!feature){setProperty("LatitudePenInputModeStatus","allocation-failed");return;}
    unsigned char before[3]={0xff,0xff,0xff}, after[3]={0xff,0xff,0xff};
    feature->writeBytes(0,before,sizeof(before));
    IOReturn ret=hid_device->getReport(feature,kIOHIDReportTypeFeature,0x0e);
    setProperty("LatitudePenInputModeReadResult",static_cast<UInt32>(ret),32);
    if(ret!=kIOReturnSuccess){setProperty("LatitudePenInputModeStatus","read-failed");feature->release();return;}
    feature->readBytes(0,before,sizeof(before));
    setProperty("LatitudePenInputModeReply",before,sizeof(before));
    // Linux hid-multitouch sets descriptor-qualified input mode without GET_REPORT
    // unless a device explicitly needs FORCE_GET_FEATURE. This exact Dell Wacom
    // returns FF FF FF to GET, so requiring readback prevented every SET.
    if(before[0]==0xff && before[1]==0xff && before[2]==0xff){
        const unsigned char requested[3]={0x0e,2,0};
        setProperty("LatitudePenInputModeStrategy","set-only-known-device");
        feature->writeBytes(0,requested,sizeof(requested));
        ret=hid_device->setReport(feature,kIOHIDReportTypeFeature,0x0e);
        setProperty("LatitudePenInputModeWriteResult",static_cast<UInt32>(ret),32);
        setProperty("LatitudePenInputModeStatus",ret==kIOReturnSuccess?
                    "native-mode-written-no-readback":"set-only-write-failed");
        feature->release();return;
    }
    if(feature->readBytes(0,before,sizeof(before))!=sizeof(before) ||
       before[0]!=0x0e || before[1]>10 || before[2]>10){
        setProperty("LatitudePenInputModeStatus","unexpected-feature-reply");feature->release();return;
    }
    setProperty("LatitudePenInputModeBefore",before[1],32);
    setProperty("LatitudePenInputDeviceIndex",before[2],32);
    unsigned char requested[3]={0x0e,2,before[2]};
    feature->writeBytes(0,requested,sizeof(requested));
    ret=hid_device->setReport(feature,kIOHIDReportTypeFeature,0x0e);
    setProperty("LatitudePenInputModeWriteResult",static_cast<UInt32>(ret),32);
    if(ret!=kIOReturnSuccess){
        feature->writeBytes(0,before,sizeof(before));
        IOReturn restored=hid_device->setReport(feature,kIOHIDReportTypeFeature,0x0e);
        setProperty("LatitudePenInputModeRestoreResult",static_cast<UInt32>(restored),32);
        setProperty("LatitudePenInputModeStatus",restored==kIOReturnSuccess?
                    "write-failed-restore-written":"write-failed-restore-failed");
        feature->release();return;
    }
    feature->writeBytes(0,after,sizeof(after));
    ret=hid_device->getReport(feature,kIOHIDReportTypeFeature,0x0e);
    if(ret==kIOReturnSuccess && feature->readBytes(0,after,sizeof(after))==sizeof(after) &&
       after[0]==0x0e && after[1]==2 && after[2]==before[2]){
        setProperty("LatitudePenInputModeAfter",after[1],32);
        setProperty("LatitudePenInputModeStatus","native-mode-verified");
    }else{
        // A failed verification must not silently leave a changed mode/index.
        feature->writeBytes(0,before,sizeof(before));
        IOReturn restored=hid_device->setReport(feature,kIOHIDReportTypeFeature,0x0e);
        setProperty("LatitudePenInputModeRestoreResult",static_cast<UInt32>(restored),32);
        setProperty("LatitudePenInputModeStatus",restored==kIOReturnSuccess?
                    "verify-failed-restore-written":"verify-failed-restore-failed");
    }
    feature->release();
}

void VoodooI2CTouchscreenHIDEventDriver::latitudeReleasePen(AbsoluteTime timestamp) {
    if (!latitude_pen_in_range) return;
    // Legacy macOS digitizer dispatch forwards pointer buttons only in range.
    if(latitude_pen_last_buttons)
        dispatchDigitizerEventWithTiltOrientation(timestamp,0,kDigitiserTransducerStylus,
            true,0,latitude_pen_x,latitude_pen_y);
    latitude_pen_last_buttons=0;
    dispatchDigitizerEventWithTiltOrientation(timestamp, 0,
        kDigitiserTransducerStylus, false, 0, latitude_pen_x, latitude_pen_y);
    latitude_pen_in_range = false;
    setProperty("LatitudePenInRange", false);
}

// Mixed Wacom touchscreen reports have no finger contact count. Do not feed
// pen packets through the finger packet aggregator or stale IOHIDElement values.
void VoodooI2CTouchscreenHIDEventDriver::handleInterruptReport(
        AbsoluteTime timestamp, IOMemoryDescriptor* report,
        IOHIDReportType report_type, UInt32 report_id) {
    // Observe report IDs without changing routing of finger/vendor reports.
    if (latitude_pen_qualified && report_type == kIOHIDReportTypeInput && report_id < 32) {
        bool first = latitude_hid_report_counts[report_id]++ == 0;
        UInt64 now_ns = 0; absolutetime_to_nanoseconds(timestamp, &now_ns);
        if (first || now_ns - latitude_hid_publish_ns > 1000000000ULL) {
            setProperty("LatitudeHIDReportCountsLE64", latitude_hid_report_counts,
                        sizeof(latitude_hid_report_counts));
            latitude_hid_publish_ns = now_ns;
        }
    }
    if (!latitude_pen_qualified || report_type != kIOHIDReportTypeInput ||
        (report_id != 0x14 && report_id != 0x0b)) {
        super::handleInterruptReport(timestamp, report, report_type, report_id);
        return;
    }
    if (!latitudePenAllowed()) { latitudeReleasePen(timestamp); return; }
    unsigned char bytes[22] = {};
    LatitudeWacomPen pen;
    if (!report || report->getLength() < sizeof(bytes) ||
        report->readBytes(0, bytes, sizeof(bytes)) != sizeof(bytes) ||
        !latitudeDecodePen(bytes, sizeof(bytes), report_id, pen)) {
        ++latitude_pen_rejected;
        if(latitude_pen_rejected < 8 || !(latitude_pen_rejected & 63))
            setProperty("LatitudePenRejected", latitude_pen_rejected, 64);
        return;
    }
    // The unchanged HID transport's adaptive poll timer watches this global.
    // Omitting this update made pen-only use appear idle (50 ms polling).
    clock_get_uptime(&last_multi_touch_event);
    UInt64 now_ns = 0; absolutetime_to_nanoseconds(timestamp, &now_ns);
    if (pen.inRange && latitude_pen_in_range && now_ns >= latitude_pen_timestamp_ns) {
        UInt64 delta = now_ns - latitude_pen_timestamp_ns;
        // Separate normal active intervals from gaps between pen sessions.
        if (delta <= 250000000ULL) {
            ++latitude_pen_interval_samples;
            latitude_pen_interval_ns += delta;
            if (delta > latitude_pen_interval_max_ns) latitude_pen_interval_max_ns = delta;
            if (delta > 32000000ULL) ++latitude_pen_gaps_over32ms;
        }
    }
    latitude_pen_last_flags = pen.rawFlags;
    latitude_pen_last_pressure = pen.rawPressure;
    latitude_pen_flags_or |= pen.rawFlags;
    if (pen.rawFlags & 1) ++latitude_pen_raw_tips;
    if (pen.rawFlags & 32) ++latitude_pen_raw_range;
    if (pen.rawPressure) ++latitude_pen_pressure_reports;
    if (pen.rawPressure > latitude_pen_pressure_max) latitude_pen_pressure_max = pen.rawPressure;
    // Refresh the same framebuffer transform used by finger touch, safely.
    if (!active_framebuffer) {
        active_framebuffer = getFramebuffer();
        if (active_framebuffer) active_framebuffer->retain();
    }
    if (active_framebuffer) {
        OSNumber* rotation = OSDynamicCast(OSNumber, active_framebuffer->getProperty(kIOFBTransformKey));
        if (rotation) current_rotation = rotation->unsigned8BitValue() / 0x10;
    }
    IOFixed x = pen.x, y = pen.y;
    checkRotation(&x, &y);
    if (pen.inRange) {
        if (!latitude_pen_in_range) {
            timer_source->cancelTimeout();
            drag_timer_source->cancelTimeout();
            scroll_timer_source->cancelTimeout();
            // Release a prior finger drag before starting a pen interaction.
            if(is_dragging || is_right_clicking)
                dispatchDigitizerEventWithTiltOrientation(timestamp, last_id,
                    kDigitiserTransducerFinger, true, 0, last_x, last_y);
            resetTouch();
            resetScroll();
        }
        latitude_pen_x = x; latitude_pen_y = y;
    }
    // Send button-up before proximity-out so the legacy pointing shim sees it.
    if(!pen.inRange && latitude_pen_in_range && latitude_pen_last_buttons)
        dispatchDigitizerEventWithTiltOrientation(timestamp,0,kDigitiserTransducerStylus,
            true,0,latitude_pen_x,latitude_pen_y);
    latitude_pen_last_buttons=pen.buttons;
    // Emit explicit out-of-range/button release, retaining last valid coordinates.
    if (pen.inRange || latitude_pen_in_range)
        dispatchDigitizerEventWithTiltOrientation(timestamp, 0,
            kDigitiserTransducerStylus, pen.inRange, pen.buttons,
            latitude_pen_x, latitude_pen_y, 0, pen.pressure, 0, 0,
            pen.tiltX, pen.tiltY);
    absolutetime_to_nanoseconds(timestamp, &latitude_pen_timestamp_ns);
    bool changed = latitude_pen_in_range != pen.inRange;
    latitude_pen_in_range = pen.inRange;
    ++latitude_pen_reports;
    if(pen.tip || pen.eraser) ++latitude_pen_tips;
    // Bounded registry publication, no per-frame logging or raw coordinates.
    if(changed || latitude_pen_reports <= 8 || !(latitude_pen_reports & 63)) {
        setProperty("LatitudePenReports", latitude_pen_reports, 64);
        setProperty("LatitudePenTipReports", latitude_pen_tips, 64);
        setProperty("LatitudePenInRange", pen.inRange);
        setProperty("LatitudePenLastReportID", report_id, 32);
        setProperty("LatitudePenRawTipReports", latitude_pen_raw_tips, 64);
        setProperty("LatitudePenRawInRangeReports", latitude_pen_raw_range, 64);
        setProperty("LatitudePenLastRawFlags", latitude_pen_last_flags, 32);
        setProperty("LatitudePenFlagsSeen", latitude_pen_flags_or, 32);
        setProperty("LatitudePenLastRawPressure", latitude_pen_last_pressure, 32);
        setProperty("LatitudePenMaxRawPressure", latitude_pen_pressure_max, 32);
        setProperty("LatitudePenPressureReports", latitude_pen_pressure_reports, 64);
        setProperty("LatitudePenIntervalSamples", latitude_pen_interval_samples, 64);
        setProperty("LatitudePenMeanPacketIntervalUS", latitude_pen_interval_samples ?
                    latitude_pen_interval_ns / latitude_pen_interval_samples / 1000 : 0ULL, 64);
        setProperty("LatitudePenMaxPacketIntervalUS", latitude_pen_interval_max_ns / 1000, 64);
        setProperty("LatitudePenPacketGapsOver32ms", latitude_pen_gaps_over32ms, 64);
        setProperty("LatitudeHIDReportCountsLE64", latitude_hid_report_counts,
                    sizeof(latitude_hid_report_counts));
    }
}

bool VoodooI2CTouchscreenHIDEventDriver::handleStart(IOService* provider) {
    if (!super::handleStart(provider))
        return false;
    
    work_loop = getWorkLoop();
    if (!work_loop) {
        IOLog("%s::Unable to get workloop\n", getName());
        stop(provider);
        return false;
    }
    
    work_loop->retain();
    
    timer_source = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &VoodooI2CTouchscreenHIDEventDriver::fingerLift));
    
    if (!timer_source || work_loop->addEventSource(timer_source) != kIOReturnSuccess) {
        IOLog("%s::Could not add timer source to work loop\n", getName());
        return false;
    }
    
    drag_timer_source = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &VoodooI2CTouchscreenHIDEventDriver::dragStart));

    if (!drag_timer_source || work_loop->addEventSource(drag_timer_source) != kIOReturnSuccess) {
        IOLog("%s::Could not add timer source to work loop\n", getName());
        return false;
    }
    
    scroll_timer_source = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &VoodooI2CTouchscreenHIDEventDriver::resetScroll));

    if (!scroll_timer_source || work_loop->addEventSource(scroll_timer_source) != kIOReturnSuccess) {
        IOLog("%s::Could not add timer source to work loop\n", getName());
        return false;
    }

    active_framebuffer = getFramebuffer();
    if (active_framebuffer) {
        active_framebuffer->retain();
    }

    // Exact saved descriptor + hardware identity: no speculative layouts on other panels.
    OSData* descriptor = OSDynamicCast(OSData, hid_device->getProperty("ReportDescriptor"));
    latitude_pen_qualified = getVendorID() == 0x056a && getProductID() == 0x4a19 &&
        descriptor && latitudePenDescriptor(static_cast<const unsigned char*>(descriptor->getBytesNoCopy()), descriptor->getLength());
    setProperty("LatitudePenQualified", latitude_pen_qualified);
    setProperty("LatitudePenDriverVersion", "1.0.6");
    setProperty("LatitudePenActivityRefresh", true);
    setProperty("LatitudePenReports", 0ULL, 64);
    setProperty("LatitudePenTipReports", 0ULL, 64);
    setProperty("LatitudePenRejected", 0ULL, 64);
    latitudeInitPenMode();

    return true;
}

void VoodooI2CTouchscreenHIDEventDriver::handleStop(IOService* provider) {
    UInt64 now_abs = 0; clock_get_uptime(&now_abs);
    latitudeReleasePen(now_abs);
    latitude_pen_qualified = false;
    if (timer_source) {
        work_loop->removeEventSource(timer_source);
        OSSafeReleaseNULL(timer_source);
    }
    
    OSSafeReleaseNULL(work_loop);
    
    if (active_framebuffer) {
        OSSafeReleaseNULL(active_framebuffer);
    }
    
    super::handleStop(provider);
}

void VoodooI2CTouchscreenHIDEventDriver::scrollPosition(AbsoluteTime timestamp, VoodooI2CMultitouchEvent event) {
    int index = 0;
    VoodooI2CDigitiserTransducer* transducer = OSDynamicCast(VoodooI2CDigitiserTransducer, event.transducers->getObject(index));
    if (transducer->type == kDigitiserTransducerStylus) {
        index = 1;
        transducer = OSDynamicCast(VoodooI2CDigitiserTransducer, event.transducers->getObject(index));
    }
    
    IOFixed x = ((transducer->coordinates.x.value() * 1.0f) / transducer->logical_max_x) * 65535;
    IOFixed y = ((transducer->coordinates.y.value() * 1.0f) / transducer->logical_max_y) * 65535;
    
    index++;
    transducer = OSDynamicCast(VoodooI2CDigitiserTransducer, event.transducers->getObject(index));
    
    IOFixed x2 = ((transducer->coordinates.x.value() * 1.0f) / transducer->logical_max_x) * 65535;
    IOFixed y2 = ((transducer->coordinates.y.value() * 1.0f) / transducer->logical_max_y) * 65535;
    
    IOFixed cursor_x = (x+x2)/2;
    IOFixed cursor_y = (y+y2)/2;
    
    checkRotation(&cursor_x, &cursor_y);
    
    dispatchDigitizerEventWithTiltOrientation(timestamp, transducer->secondary_id, transducer->type, 0x1, HOVER, cursor_x, cursor_y);

    last_x = cursor_x;
    last_y = cursor_y;
    last_id = transducer->secondary_id;
    
    is_scrolling = true;
}

void VoodooI2CTouchscreenHIDEventDriver::dragStart() {
    // Start dragging where the interaction began
    uint64_t now_abs;
    clock_get_uptime(&now_abs);
    dispatchDigitizerEventWithTiltOrientation(now_abs, last_id, kDigitiserTransducerFinger, 0x1, LEFT_CLICK, touch_start_x, touch_start_y);
    is_dragging = true;
}

void VoodooI2CTouchscreenHIDEventDriver::resetScroll() {
    is_scrolling = false;
}

void VoodooI2CTouchscreenHIDEventDriver::scheduleClick() {
    timer_source->setTimeoutMS(FINGER_LIFT_DELAY);
}

void VoodooI2CTouchscreenHIDEventDriver::scheduleDragStart() {
    drag_timer_source->setTimeoutMS(DRAG_START_DELAY);
}

void VoodooI2CTouchscreenHIDEventDriver::scheduleScrollEnd() {
    scroll_timer_source->setTimeoutMS(FINGER_LIFT_DELAY);
}
