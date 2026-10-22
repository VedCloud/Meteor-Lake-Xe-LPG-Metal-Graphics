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

#include <IOKit/assert.h>
#include <IOKit/IOLib.h>
#include <libkern/c++/OSString.h>
#include <IOKit/hidsystem/IOHIDParameter.h>

#include "ApplePS2SmartTouchpad.h"
#include "FocalTech.h"
#include "Synaptics.h"
#include "ELAN.h"

/*  Plist Config keys */
#define SynapticsTPadMinMax   "SynapticsTPadMinMax"
#define SynapticsTPadXmin     "Xmin"
#define SynapticsTPadXmax     "Xmax"
#define SynapticsTPadYmin     "Ymin"
#define SynapticsTPadYmax     "Ymax"

#define PointerPreferencesKey       "Pointing"
#define HIDTrackpadAccelLevel       "HIDTrackpadAccelLevel"
#define PointerResolution           "PointerResolution"
#define PointerCustomAcceleration   "PointerCustomAcceleration"
#define EnhancedAccelerationLevel   "EnhancedAccelerationLevel"
#define EnhancedAccelerationMinDelta    "EnhancedAccelerationMinDelta"
#define PointerSmoothSamples        "PointerSmoothSamples"
#define PointerSmoothLevel          "PointerSmoothLevel"
#define Sensitivity                 "Sensitivity"
#define AccelerationPrefValue       "AccelerationPrefValue"
#define SlowMoveNoiseMaxDelta       "SlowMoveNoiseMaxDelta"

#define TouchpadEdgesPreferencesKey   "TouchpadEdges"
#define DisableLeftEdgeArea     "DisableLeftEdgeActiveArea"
#define DisableRightEdgeArea    "DisableRightEdgeActiveArea"
#define DisableTopEdgeArea      "DisableTopEdgeActiveArea"
#define DisableBottomEdgeArea   "DisableBottomEdgeActiveArea"

#define Clickpad2FingersMove    "Clickpad2FingersMove"
#define DisableTrackPointButton "DisableTrackPointButton"

#define ScrollPreferencesKey    "Scrolling"
#define ScrollResolution            "ScrollResolution"
#define ScrollAccelLevelAtBoot       "ScrollLevelAtBoot"
#define InertialScrollDepth         "InertialScrollDepth"
#define InertialScrollDelay         "InertialScrollDelay"
#define NoInertiaForHScroll         "NoInertiaForHScroll"
#define ScrollAccelDelay            "ScrollAccelDelay"
#define ScrollLevelFactor           "ScrollLevelFactor"
#define ScrollLevelGranularity      "ScrollLevelGranularity"
#define ScrollSmoothSamples         "ScrollSmoothSamples"
#define ScrollMaxIdleStateDelta     "ScrollMaxIdleStateDelta"
#define ScrollMaxIdleDetectTimeout  "ScrollMaxIdleDetectTimeout"
#define ScrollStopDetectSamples     "ScrollStopDetectSamples"
#define ScrollDirectionSamples      "ScrollDirectionSamples"
#define EContScrollMaxDelta         "EContScrollMaxDelta"
#define Cont2FScrollMaxDelta        "2FContScrollMaxDelta"

#define SlingScrollStartTimeOut     "SlingScrollStartTimeOut"
#define SlingTriggerArea            "SlingTriggerArea"

#define EdgeScrollPreferencesKey  "EdgeScroll"
#define EdgeVScrollArea         "EdgeVScrollArea"
#define EdgeHScrollArea         "EdgeHScrollArea"
#define EdgeHScrollAdaptArea    "EdgeHScrollAdaptArea"
#define EdgeVScrollAdaptArea    "EdgeVScrollAdaptArea"
#define EdgeCircularScrollDelta "EdgeCircularScrollDelta"
#define ContinuousEdgeScroll    "ContinuousEdgeScroll"
#define AutoEdgeScroll          "AutoEdgeScroll"
#define SlingEdgeScroll         "SlingEdgeScroll"
#define EdgeCircularScroll      "EdgeCircularScroll"
#define EdgeScrolling           "EdgeScrolling"
#define EScrolSpeedFactor       "EScrolSpeedFactor"
#define EdgeScrollAccelMode     "EdgeScrollAccelMode"
#define InertialEdgeScroll      "InertialEdgeScroll"

#define Scroll2FPreferencesKey    "2FScroll"
#define Scroll2FVertical         "2FVerticalScroll"
#define Scroll2FHorizontal       "2FHorizontalScroll"
#define TwoFingScrollAcceMode   "2FScrollAccelMode"
#define Inertial2FingScroll     "Inertial2FingScroll"
#define Continuous2FingScroll   "Continuous2FingScroll"
#define Auto2FingScroll         "Auto2FingScroll"
#define Sling2FingScroll        "Sling2FingScroll"

#define ChangeScrollFlowlDirection "ChangeScrollFlowlDirection"

#define SwipesPreferencesKey    "Swiping"
#define EdgeSwipes              "EdgeSwipes"
#define EdgeSwipes2Ways         "EdgeSwipes2Ways"
#define Swipes3F                "3FingerSwipes"
#define Swipes4F                "4FingerSwipes"
#define EdgeSwipesResetPointer  "EdgeSwipesResetPointer"

#define EdgeSwipeLeft           "EdgeSwipeLeft"
#define EdgeSwipeRight          "EdgeSwipeRight"
#define EdgeSwipeTop            "EdgeSwipeTop"
#define EdgeSwipeBottom         "EdgeSwipeBottom"

#define MultiFingerSwipesDeltaX "MultiFingerSwipesDeltaX"
#define MultiFingerSwipesDeltaY "MultiFingerSwipesDeltaY"
#define EdgeSwipeDeltaX         "EdgeSwipeDeltaX"
#define EdgeSwipeDeltaY         "EdgeSwipeDeltaY"
#define EdgeSwipeFingers        "EdgeSwipeFingers"
#define EdgeSwipeSenseHArea    "EdgeSwipeSenseHArea"
#define EdgeSwipeSenseVArea    "EdgeSwipeSenseVArea"

#define SwipeUpAction3F    "3FingerSwipeUpAction"
#define SwipeLeftAction3F  "3FingerSwipeLeftAction"
#define SwipeRightAction3F "3FingerSwipeRightAction"
#define SwipeDownAction3F  "3FingerSwipeDownAction"

#define SwipeUpAction4F    "4FingerSwipeUpAction"
#define SwipeLeftAction4F  "4FingerSwipeLeftAction"
#define SwipeRightAction4F "4FingerSwipeRightAction"
#define SwipeDownAction4F  "4FingerSwipeDownAction"

#define FingerPressPreferencesKey   "FingersPress"
#define FingersPressAction2F        "2FingersPressAction"
#define FingersPressAction3F        "3FingersPressAction"
#define FingersPressAction4F        "4FingersPressAction"
#define FingersPressAction5F        "5FingersPressAction"
#define FingersPressTimeOut         "FingersPressTimeOut"

#define TapPreferencesKey       "Tapping"
#define EnableTaps              "Enable"
#define TrackpadTappingCorners  "TrackpadTappingCorners"
#define DisableBottomCornersTap "DisableBottomCornersTap"
#define CornerTapStartABS       "CornerTapStartABS"
#define CornerTapAreaLength     "CornerTapAreaLength"
#define CornerTapAreaHeight     "CornerTapAreaHeight"
#define MulFingDoubleTaps       "MulFingDoubleTaps"
#define TapAction1F             "1FingerTapAction"
#define TapAction2F             "2FingersTapAction"
#define TapAction3F             "3FingersTapAction"
#define TapAction4F             "4FingersTapAction"
#define TapAction5F             "5FingersTapAction"
#define MulFingTapTimeOut       "MulFingTapTimeOut"
#define Click1FTimeOut          "1FClickTimeOut"
#define Tap12FTimeout           "12TapTimeOut"
#define TapPressure1F           "1FTapPressure"
#define TapMaxDelta             "TapMaxDelta"
#define MulFingTapMaxDelta      "MulFingTapMaxDelta"

#define DragPreferencesKey          "Dragging"
#define EnableDrag                  "Enable"
#define DragLock                    "DragLock"
#define FLiftDragReleaseTimeOut     "FLiftDragReleaseTimeOut"
#define FingerPressDrag             "FingerPressDrag"
#define DragFingersNum              "DragFingersNum"
#define DragFingerIDToUseForData    "DragFingerIDToUseForData"
#define FingerPressDragStartTimeOut "FingerPressDragStartTimeOut"
#define FingerPressDragPressure     "FingerPressDragPressure"

#define MousePreferencesKey     "Mouse"
#define MouseResolution         "Resolution"
#define MouseSampleRate         "SampleRate"
#define MouseScaling            "Scaling"

#define KeyboardPreferencesKey              "Keyboard"
#define IgnoreAccidentalTouchpadInput       "IgnoreAccidentalTouchpadInput"
#define TriggerAccInputForKeyPressCount     "TriggerAccInputForKeyPressCount"
#define TapEnablesTPadFromAccInput          "TapEnablesTPadFromAccInput"
#define TimeOutEnablesTPadAfterAccInput     "TimeOutEnablesTPadAfterAccInput"
#define EnableAsusKBacklightSupport         "EnableAsusKBacklightSupport"
#define EnableAsusKBacklightAutoOff         "EnableAsusKBacklightAutoOff"
#define AsusKBacklightAutoOffTimeOut        "AsusKBacklightAutoOffTimeOut"
#define AsusKBacklightHas16Levels           "AsusKBacklightHas16Levels"

#define OtherPreferencesKey "Others"
#define PinchAction5F       "5FingersPinchAction"
#define PinchAction4F       "4FingersPinchAction"
#define PinchZoom           "PinchZoom"
#define PinchZoomDelta      "PinchZoomDelta"
#define PinchDeltaScale     "PinchDeltaScale"
#define Rotation            "Rotation"
#define RotationDelta       "RotationDelta"
#define RotationMaxCFingDelta   "RotationMaxCFingDelta"
#define RotationMinCFingTime    "RotationMinCFingIdleTime"
#define CircularRotate      "CircularRotate"
#define PinchRotateMinSamples    "PinchRotateMinSamples"
#define DisableTPadOnLIDClose    "DisableTPadOnLIDClose"
#define TPadStatusCheckPollRate  "TPadStatusCheckPollRate"

#define MaxTouchEndPressure      "MaxTouchEndPressure"
#define MaxTouchEndDelta         "MaxTouchEndDelta"

#define PalmDetection            "PalmDetection"
#define PalmCheckMinWidth        "PalmCheckMinWidth"
#define PalmCheckMinPressure     "PalmCheckMinPressure"

#define UseSynapticsGenericBounds   "UseSynapticsGenericBounds"

#define ButtonsKey                  "Buttons"
#define ClickpadVirtualButtons      "ClickpadVirtualButtons"
#define ClickpadVirtualButtonsArea  "ClickpadVirtualButtonsArea"
#define DisablePhysicalClicks       "DisablePhysicalClicks"
#define VirtualMiddleButton         "VirtualMiddleButton"
#define FingersClickAction2F        "2FingersClickAction"
#define PointingOnVirtualButtonArea "EnablePointingOnVirtualButtonArea"

#define SleepInitDelay              "SleepInitDelay"

#define super IOHIPointing

OSDefineMetaClassAndStructors(ApplePS2SmartTouchpad, IOHIPointing);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// IOHIPointing Overdides which initializes and updates the Trackpad
// properties from prefpane.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

UInt32  ApplePS2SmartTouchpad::deviceType()  { return NX_EVS_DEVICE_TYPE_MOUSE; };

UInt32  ApplePS2SmartTouchpad::interfaceID() { return NX_EVS_DEVICE_INTERFACE_BUS_ACE; };

IOFixed ApplePS2SmartTouchpad::resolution()  { return (_pointerResolution << 16); };

IOReturn ApplePS2SmartTouchpad::setParamProperties( OSDictionary * config )
{
    const struct { const char* name; bool* var; } boolVars[] = {
        { "Clicking", &_isClickingEnabled},
        { "Dragging", &_isDragEnabled},
        { "DragLock", &_isDragLockEnabled},
        
        { "TrackpadRightClick", &_isTapSwapEnabled},
        { "TrackpadThreeFingerDrag", &_3FDrag},
        { "TrackpadThreeFingerTapGesture", &_3FTapLookUp},

        { "TrackpadScroll", &_isVScrollEnabled},
        { "TrackpadHorizScroll", &_isHScrollEnabled},
        
        { "TrackpadPinch", &_zoom.enabled},
        { "TrackpadRotate", &_rotate.enabled},

        { "TrackpadTwoFingerDoubleTapGesture", &_2FDoubleTapSZoom},

        { "JitterNoMove", &_isTypeModeEnabled},
        
        { "USBMouseStopsTrackpad", &_usbMouseDisablesTrackpad},
        
    };
    
    const struct { const char* name; unsigned char* var; } intVars[] = {
        { "TrackpadCornerSecondaryClick", &_secondarayClickInput},
    
    };
    
    /*
     * Setup Apple trackpad prefpane keys
     */
    
    OSIterator *iter = 0;
    const OSSymbol *dictKey = 0;
    OSNumber  *dictKeyNumber = 0;
    OSBoolean *dictKeyBool = kOSBooleanFalse;
    
    iter = OSCollectionIterator::withCollection(config);
    if (iter) {
        while ((dictKey = (const OSSymbol *)iter->getNextObject())) {
            
            const char *keyStrValue = dictKey->getCStringNoCopy();
            
            // IOLog("STD :: KeyString %s\n", keyStrValue);
            
            
            dictKeyNumber = OSDynamicCast(OSNumber, config->getObject(dictKey));
            if (dictKeyNumber) {
               // IOLog("STD :: Key %s, Num value %llu.\n", keyStrValue, dictKeyNumber->unsigned64BitValue());
               setProperty(keyStrValue, dictKeyNumber);
            }
            
            dictKeyBool = OSDynamicCast(OSBoolean, config->getObject(dictKey));
            if (dictKeyBool) {
               // IOLog("STD :: Key %s, Bool value %d.\n", keyStrValue, dictKeyBool->getValue());
               setProperty(keyStrValue, dictKeyBool->getValue());
            }
            
           setProperty(keyStrValue, dictKeyBool);
        }
    }
    
    /* Set Apple trackpad prefpane identifier for sierra */
    if (_osxVersion > 11)
    {
        setProperty("ApplePreferenceIdentifier", "com.apple.AppleMultitouchTrackpad");
        setProperty("MTHIDDevice", true);
        setProperty("MT Built-in", true);
        setProperty("ApplePreferenceCapability", true);
        setProperty("TrackpadEmbedded", true);
        
        /* Enable apple features */
        setProperty("SupportsGestureScrolling", true);
        setProperty("TrackpadThreeFingerDrag", true);
        
        setProperty("TrackpadFourFingerGestures", false);
        
        /* Setup apple features */
        /*if (_button.isClickPad)
         {
         setProperty("TrackpadCornerSecondaryClick", true);
         setProperty("TrackpadSecondaryClickCorners", true);
         }*/
        setProperty("TrackpadTwoFingerDoubleTapGesture", true);
        setProperty("TrackpadTwoFingerFromRightEdgeSwipeGesture", true);
        
        setProperty("TrackpadThreeFingerVertSwipeGesture", false);
        setProperty("TrackpadThreeFingerHorizSwipeGesture", false);
    }
   

    OSBoolean* boolVar;
    OSNumber* numVar;
    // Setup boolean config
    for (int i = 0; i < elementSizeOfArray(boolVars); i++)
     {
      //   IOLog("STD :: Key %s\n", boolVars[i].name);

         if ((boolVar = OSDynamicCast(OSBoolean, getProperty(boolVars[i].name))))
         {
             *boolVars[i].var = boolVar->isTrue();
            // IOLog("STD :: Boolean Value %d\n", boolVar->isTrue());
         }
         
         else if ((numVar = OSDynamicCast(OSNumber, getProperty(boolVars[i].name))))
         {
             *boolVars[i].var = numVar->unsigned8BitValue();
            // IOLog("STD :: Integer Value %d\n", numVar->unsigned8BitValue());
         }
     }
    
    // Setup integer config
    for (int i = 0; i < elementSizeOfArray(intVars); i++)
    {
       // IOLog("STD :: Key %s\n", boolVars[i].name);
        
        if ((numVar = OSDynamicCast(OSNumber, getProperty(boolVars[i].name))))
        {
            *boolVars[i].var = numVar->unsigned32BitValue();
          //  IOLog("STD :: Integer Value %d\n", numVar->unsigned8BitValue());
        }
    }
    

    /* Read prefpane values to setup driver */
    OSNumber * paccell      = OSDynamicCast( OSNumber, config->getObject(kIOHIDTrackpadAccelerationType) );
    OSNumber * saccell      = OSDynamicCast( OSNumber, config->getObject(kIOHIDTrackpadScrollAccelerationKey) );
    OSNumber * clicktime    = OSDynamicCast( OSNumber, config->getObject(kIOHIDClickTimeKey) );
    
    // Initilize
    setAccelAndResolution();

    if (paccell)
    {
        _pointerAccell = paccell->unsigned64BitValue();
        setProperty(kIOHIDTrackpadAccelerationType, _pointerAccell, 64);
       // IOLog("STD :: Updated Pointer accelleration %lld\n", _pointerAccell);
    }
    
    if (saccell)
    {
        _scrollAccell = saccell->unsigned64BitValue();
        
       // IOLog("STD ::  Scroll Pref Accel %lld, \n", _scrollAccell);

        UInt8 prefscrollFactor = 0;

        // Fix the broken value for level 8 in OSX 10.10
        if (_scrollAccell == 655)
        {
            _scrollAccell = 24 * 4096;
        }
        
        //
        // Trackpad prefpane scroll acceleration values
        // using below expression are :
        // OS X 10.9 (or) below has 10 levels - 0 2 8 11 14 16 24 32 40 48
        // OS X >= 10.10 has 8 levels - 0 2 3 5 8 12 16 24
        //
        
        //
        // Temp fix for the OSX prefpane bug,
        // which is not saving scroll level above 6.
        //
        
        if (_scroll.levelAtBoot != 0) {
            
            switch (_scroll.levelAtBoot) {
                case 1:
                    _scrollAccell = 0;
                    _scrollFactor = 1;
                    break;
                    
                case 2:
                    _scrollAccell = 2 * 4096;
                    _scrollFactor = 2;
                    break;
                    
                case 3:
                    if (_osxVersion >= 10) {
                        _scrollAccell = 3 * 4096;
                    }
                    else {
                        _scrollAccell = 8 * 4096;
                    }
                    _scrollFactor = 3;
                    break;
                    
                case 4:
                    if (_osxVersion >= 10) {
                        _scrollAccell = 5 * 4096;
                    }
                    else {
                        _scrollAccell = 11 * 4096;
                    }
                    _scrollFactor = 4;
                    break;
                    
                case 5:
                    if (_osxVersion >= 10) {
                        _scrollAccell = 8 * 4096;
                    }
                    else {
                        _scrollAccell = 14 * 4096;
                    }
                    _scrollFactor = 5;
                    break;
                    
                case 6:
                    if (_osxVersion >= 10) {
                        _scrollAccell = 12 * 4096;
                    }
                    else {
                        _scrollAccell = 16 * 4096;
                    }
                    _scrollFactor = 6;
                    break;
                    
                case 7:
                    if (_osxVersion >= 10) {
                        _scrollAccell = 16 * 4096;
                    }
                    else {
                        _scrollAccell = 24 * 4096;
                    }
                    _scrollFactor = 7;
                    break;
                    
                case 8:
                    if (_osxVersion >= 10) {
                        _scrollAccell = 24 * 4096;
                    }
                    else {
                        _scrollAccell = 32 * 4096;
                    }
                    _scrollFactor = 8;
                    break;
                    
                case 9:
                    _scrollAccell = 40 * 4096;
                    _scrollFactor = 9;
                    break;
                    
                case 10:
                    _scrollAccell = 48 * 4096;
                    _scrollFactor = 10;
                    break;
                    
                default:
                    break;
            }
            
            _scroll.levelAtBoot = 0; // Clear so next read can be from the ioreg
        }
        else
        {
            prefscrollFactor =  _scrollAccell/4096;

           // IOLog("STD ::  Scroll Pref Level %d Accel %lld, \n", prefscrollFactor, _scrollAccell);
            //
            // Mapping Trackpad scroll levels to custom levels for smoothing
            //
            
            switch (prefscrollFactor) {
                    
                case 0: // Level 1
                    _scrollFactor = 1;
                    break;
                    
                case 2: // Level 2
                    _scrollFactor = 2;
                    break;
                    
                case 3: // Level 3, 10.10
                    _scrollFactor = 3;
                    break;
                    
                case 8: // Level 3 (Level 5 in 10.10)
                    if (_osxVersion >= 10) {
                        _scrollFactor = 5;
                    }
                    else {
                        _scrollFactor = 3;
                    }
                    break;
                    
                case 5:  // Level 4, 10.10
                case 11: // Level 4
                    _scrollFactor = 4;
                    break;
                    
                case 14: // Level 5
                    _scrollFactor = 5;
                    break;
                    
                case 12: // Level 6, 10.10
                    _scrollFactor = 6;
                    break;
                    
                case 16: // Level 6 (Level 7 in 10.10)
                    if (_osxVersion >= 10) {
                        _scrollFactor = 7;
                    }
                    else {
                        _scrollFactor = 6;
                    }
                    break;
                    
                case 24: // Level 7 (Level 8 in 10.10)
                    if (_osxVersion >= 10) {
                        _scrollFactor = 8;
                    }
                    else {
                        _scrollFactor = 7;
                    }
                    break;
                    
                case 32: // Level 8
                    _scrollFactor = 8;
                    break;
                    
                case 40: // Level 9
                    _scrollFactor = 9;
                    break;
                    
                case 48: // Level 10
                    _scrollFactor = 10;
                    break;
                    
                default:
                    _scrollFactor = 0;
                    break;
            }
        }
        
        //
        // Calculate scroll factor based on plist values granularity and level factor.
        //
        
        if (_scroll.levelFactor > 0)
            _scrollFactor = _scrollFactor * _scroll.levelFactor;
        
        if (_scroll.levelGranularity > 0)
            _scrollFactor = _scrollFactor * _scroll.levelGranularity/100;
        
        //
        // Control scroll factor for scroll modes > 2
        //
        
        if (_scroll.accelMode2F > 2) {
            
            if (_osxVersion >= 10)
                _scrollFactor = _scrollFactor/8;
            else
                _scrollFactor = _scrollFactor/10;
        }
        
        
        // IOLog("STD :: Update Scroll Pref Level %d Accel %lld, Boot Level %d, ScrollFactor %d (Plist factor %d, Granularity %d Percent)\n", prefscrollFactor, _scrollAccell, _scroll.levelAtBoot, (int)_scrollFactor, _scroll.levelFactor, _scroll.levelGranularity);
        
        saccell->setValue(_scrollAccell);
        setProperty(kIOHIDTrackpadScrollAccelerationKey, _scrollAccell, 64);
    }
    else {
        if (_osxVersion >= 10) {
            _scrollAccell = 8 * 4096;
        }
        else {
            _scrollAccell = 11 * 4096;
        }
        _scrollFactor = 4;

        //
        // Calculate scroll factor based on plist values granularity and level factor.
        //
        
        if (_scroll.levelFactor > 0)
            _scrollFactor = _scrollFactor * _scroll.levelFactor;
        
        if (_scroll.levelGranularity > 0)
            _scrollFactor = _scrollFactor * _scroll.levelGranularity/100;
        
        //
        // Control scroll factor for scroll modes > 2
        //
        
        if (_scroll.accelMode2F > 2) {
            
            if (_osxVersion >= 10)
                _scrollFactor = _scrollFactor/8;
            else
                _scrollFactor = _scrollFactor/10;
        }
        
        
       // IOLog("STD :: Default Scroll Accel %lld, Boot Level %d, ScrollFactor %d (Plist factor %d, Granularity %d Percent)\n", _scrollAccell, _scroll.levelAtBoot, (int)_scrollFactor, _scroll.levelFactor, _scroll.levelGranularity);
        
        setProperty(kIOHIDTrackpadScrollAccelerationKey, _scrollAccell, 64);
    }
   
    if (clicktime)
    {
        _maxDoubleClickTime = clicktime->unsigned64BitValue();
        setProperty (kIOHIDClickTimeKey, _maxDoubleClickTime, 64);
    }

    // IOLog("STD :: Set Properties.\n");
    
    return super::setParamProperties(config);
}

void ApplePS2SmartTouchpad::setAccelAndResolution()
{
    //
    // Must add this property to let our superclass know that it should handle
    // trackpad acceleration settings from user space.  Without this, tracking
    // speed adjustments from the mouse prefs panel have no effect.
    //
    
    setProperty(kIOHIDPointerResolutionKey, (_pointerResolution << 16), 32);
    setProperty(kIOHIDScrollResolutionKey, (_scroll.resoultion << 16), 32);

    // For Sierra precise scrolling (credit usr-sse2)
    setProperty("HIDScrollResolutionX", (_scroll.resoultion << 16), 32);
    setProperty("HIDScrollResolutionX", (_scroll.resoultion << 16), 32);

    setProperty(kIOHIDPointerButtonCountKey, 3, 32);
    
    setProperty(kIOHIDPointerAccelerationTypeKey, kIOHIDTrackpadAccelerationType);
    setProperty(kIOHIDScrollAccelerationTypeKey, kIOHIDTrackpadScrollAccelerationKey);
    
    IOLog("STD :: Pointer resolution %d, Scroll resolution %d.\n", _pointerResolution, _scroll.resoultion);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// IOKit Overdides which initializes, starts and stops the driver.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

bool ApplePS2SmartTouchpad::init( OSDictionary * properties )
{
    //
    // Initialize this object's minimal state. This is invoked right after this
    // object is instantiated.
    //
    
    if (!super::init(properties))  return false;
    
    IOLog("STD :: Smart PS2 Touchpad v4.7 beta 6 Copyright (c) EMlyDinEsH <www.osxlatitude.com> 2012-2016.\n");
    
    BASIC_DBG_LOG("STD :: Initializing driver...\n");
    
    _touchmode    =   MODE_IDLE;
    _tSenseLvl    =   0;
    
    _gestureTime    = 0;
    _lastDataTime   = 0;
    
    // Set usable Trackpad prefpane values as these are not loaded until login
    
    _pointerAccell      = 0;
    _scrollAccell       = 0;
    _maxDoubleClickTime = 0;
    
    _isDragEnabled      = true;
    _isClickingEnabled  = true;
    _isVScrollEnabled   = true;

    _isDragLockEnabled  = false;
    _isHScrollEnabled   = false;
    _isTapSwapEnabled   = false;
    _isTypeModeEnabled  = false;
    
    //
    
    _lastExtMouseStatusCheckTime    = 0;
    _extMouseCheckPollRate          = 1500; // 1.5 sec
    _externalMouseFound        = false;
    _usbMouseDisablesTrackpad  = false;

    _interruptHandlerInstalled = false;
    
    _debugLvl = 16;

    _device                 = 0;
    _now                    = 0;
    _stdWorkLoop            = 0;

    _pointerTimer           = 0;
    _scrollTimer            = 0;
    _buttonDragTimer        = 0;
    _featureTimer           = 0;
    _kBackLightTimer        = 0;
    
    _doTimerJob             = false;

    _packetByteCount        = 0;
    _endPacketsCount        = 0;
    _packetsCount           = 0;

    _modeSetTries           = 3;
    _pointerResolution      = 1200;
    _pSmoothSamples         = 5;
    _pSmoothLevel           = 2;
    _pAccelCustom           = 0;
    _pAccelPreference       = 0;
    
    _pExtAccelMinDelta      = 0;
    _pExtAccelLevel         = 0;
    _pSlowMoveNoiseDelta    = 0;
    
    _pContinuousMove        = false;

    _pDx        = 0;
    _pDy        = 0;
    _pLastDx    = 0;
    _pLastDy    = 0;
    _pSmoothX   = 0;
    _pSmoothY   = 0;
    
    _xmin       = 0;
    _ymin       = 0;
    _xmax       = 0;
    _ymax       = 0;
    _xcenter    = 0;
    _ycenter    = 0;

    _lastSample[SAMPLES_COUNT] = {0};
        
    _cSample        = 0;
    _skipSample     = 0;
    _scrollSample   = 0;
    
    _ps2Mouse               = {0};
    _ps2Mouse.mScaling      = kDP_SetMouseScaling1To1;
    _ps2Mouse.mSampleRate   = 80;
    _ps2Mouse.mResolution   = 3;
    
    _hasAsusKBackLight      = false;
    _has16LvlKBLight        = false;
    _hasKBLightAutoOff      = false;
    _isKBLightOff           = true;
    _kBLightAutoOffTimeout  = 3000; // 3 sec
    
    clock_get_uptime(TIME_NOW);
    _lastKBLightTriggerTime = absoluteToUINT64(_now);
    
    _acpiPollRate           = 1500000000; // 1500 ms
    _enableLidPolling       = true;
    _lidClosed              = false;

    _palm.detectionEnabled  = false;
    _palm.detected          = false;
    _palm.minPressure       = 255;
    _palm.minWidth          = 15;
    
    _pid                    = -1;
    _sid                    = -1;
    _cFingers               = 0;
    _lastFingers            = 0;
    _fing[STD_MAX_FINGERS]  = {0};

    _drag                   = {0};
    _drag.releaseTime       = 500; // 500 ms

    _scroll                     = {0};
    _scroll.resoultion          = 400;
    _scroll.slingEffectRunTime  = 4000000000; // 4 sec
    _scroll.directionSamples    = 5;

    _cornerTap              = {0};
    _zoom                   = {0};
    _rotate                 = {0};
    _swipe                  = {0};
    _fingPress              = {0};
    
    _tap.mulFingsTimeout    = MAX_MULF_TAP_TIME;
    _tap.clickTimeout       = MAX_CLICK_TIME;
    _tap.timeout            = MAX_TAP_TIME;
    _tap                    = {0};

    _fings4PinchAction      = 0;
    _fings5PinchAction      = 0;
    
    _button                 = {0};
    _kbEvent                = {0};
    
    _isAsusNotebook             = false,
    _isFnKeyDisabledTouchpad    = false;
    _isKBLightOff               = false;

    _hasAsusKBackLight          = false;
    _has16LvlKBLight            = false;
    _hasKBLightAutoOff          = false;
    
    _kBLightAutoOffTimeout      = 10000; // 10 sec;
    _lastKBLightTriggerTime     = 0;
    _lastFnKeyStateChkTime      = 0;
    
    
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

ApplePS2SmartTouchpad * ApplePS2SmartTouchpad::probe( IOService * provider, SInt32 * score )
{
    
    if (!super::probe(provider, score)) return 0;
    
    
    BASIC_DBG_LOG("STD :: Probing hardware...\n");
    
    //
    // The driver has been instructed to verify the presence of the actual
    // hardware we represent. We are guaranteed by the controller that the
    // mouse clock is enabled and the mouse itself is disabled (thus it
    // won't send any asynchronous mouse data that may mess up the
    // responses expected by the commands we send it).
    //
    
    _device  = (ApplePS2MouseDevice *) provider;
    
    //
    // Detect the presence of ELAN Touchpad
    //
    
    IOLog("STD :: Detecting the touchpad...\n");
    
    detectTouchpadVendor();
    
    if (Detect_touchpad()) {
        IOLog("STD :: Failed not detect touchpad.\n");
        return 0;
    }
    
    return this;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2SmartTouchpad::start( IOService * provider )
{
    //
    // Read and Set the debug Level from plist
    //
    
    OSNumber *dbgLvl = OSDynamicCast(OSNumber, getProperty("Debug Level"));
    _debugLvl = dbgLvl->unsigned32BitValue();
    
    BASIC_DBG_LOG("STD :: Debug Level value %d\n", _debugLvl);
    
    //
    // The driver has been instructed to start. This is called after a
    // successful probe and match.
    //
    
    if (!super::start(provider)) return false;
    
    BASIC_DBG_LOG("STD :: Starting driver...\n");
    
    //
    // Maintain a pointer to and retain the provider object.
    //
    
    _device = (ApplePS2MouseDevice *) provider;
    _device->retain();
    
    
    //
    // Initialize mouse with sampleRate, resolution and scaling.
    //
    
    if (initializeMouse(_device, _ps2Mouse.mSampleRate, _ps2Mouse.mResolution, _ps2Mouse.mScaling))
        return false;
    
    
    //
    //  Setup touchpad
    //
    
    switch (_touchpadVendor) {
        case STD_FOCAL_TECH:
            
            if (!Focaltech_setup_touchpad())
                return false;
            
            _pktsize = 6;
            _hw_version = STD_FOCAL_TECH_HW;
            _button.isClickPad = true;
            break;
            
        case STD_ELAN_TECH:
            
            if (!Elantech_setup_touchpad())
                return false;
            
            _pktsize = _hw_version > 1 ? 6 : 4;
            
            // Initializing parity check for v1 and v2 Elan hardware
            if (_hw_version < 3)
            {
                _elanTPad.parityBit[0] = 1;
                for (int i = 1; i < 256; i++)
                    _elanTPad.parityBit[i] = _elanTPad.parityBit[i & (i - 1)] ^ 1;
            }
            break;
            
        case STD_SYNAPTICS:
            
            if (!Synaptics_setup_touchpad())
                return false;
            
            _pktsize = 6;
            _hw_version = STD_SYNAPTICS_HW;
            break;
            
        case STD_MOUSE:
        default:
            _button.isClickPad = false;
            _pktsize = 3;
            _pointerResolution = 200;
            _hw_version = STD_MOUSE_HW;
            break;
    }
    
    BASIC_DBG_LOG("STD :: Packet Size %d.\n", _pktsize);

    //
    // Read configurations from plist
    //
    
    ReadSTDPlistConfig();
    ReadAccelPlistConfig();
    
    
    //
    // Re-Initialize mouse with sampleRate, resolution and scaling.
    //
    
    if (initializeMouse(_device, _ps2Mouse.mSampleRate, _ps2Mouse.mResolution, _ps2Mouse.mScaling))
        return false;
    
    //
    // Adapt features on some touchpads
    //
    
    switch (_touchpadVendor) {
        case STD_FOCAL_TECH:
            break;
            
        case STD_SYNAPTICS:
            
            //
            // They have only 3 fingers support and both fingers Y delta varies in 2F
            //
            
            if (SYN_ID_MAJOR(_synTPad.identity) < 8)
            {
                _rotate.maxCFDeltaY = _rotate.delta/3;
            }
            
            //
            // Setupt touchpad ranges
            //
            
            if (_xmax == 0 || _ymax == 0 || _synTPad.useGenericBounds)
            {
                _xmin = XMIN_NOMINAL;
                _ymin = YMIN_NOMINAL;
                _ymax = YMAX_NOMINAL;
                _xmax = XMAX_NOMINAL;
                
                IOLog("STD :: Using generic bounds for max and min coordinates\n");
            }
            
            _xcenter = (_xmax + _xmin)/2;
            _ycenter = (_ymax + _ymin)/2;
            
            
            IOLog("STD :: Touchpad ranges are X_min %d Y_min %d X_max %d Y_max %d, CenterX %d, CenterY %d.\n", _xmin, _ymin, _xmax, _ymax, _xcenter, _ycenter);
            
            //
            // Already touchpad has some area dedicated for edges
            // so reduce the area of plist.
            //
            
            _swipe.nonEdgeHArea = _swipe.nonEdgeHArea/3;
            _swipe.edgeHArea = _swipe.edgeHArea/2;
            _swipe.nonEdgeVArea = _swipe.nonEdgeVArea/3;
            _swipe.edgeVArea = _swipe.edgeVArea/2;
            
            _scroll.edgeVArea = _scroll.edgeVArea/2;
            _scroll.edgeHArea = _scroll.edgeHArea/2;
            break;
            
        case STD_ELAN_TECH:
            
            //
            // Disable 2 fingers move for touchpad with physical buttons
            //
            
            if (!_button.isClickPad)
                _button.clickPad2FMove = false;
            
            //
            // Note:
            // ELAN v2 and v3 (Legacy mode) hardware data
            // has only half of real hardare x and y resolutions.
            // So decrease resolution for these touchpads.
            //
            
            switch (_hw_version) {
                case 2:
                    
                    _swipe.mulFingDeltaX = _swipe.mulFingDeltaX/3;
                    _swipe.mulFingDeltaY = _swipe.mulFingDeltaY/3;
                    
                    _tap.noiseDelta = _tap.noiseDelta/2;
                    _tap.noiseMultiTapDelta = _tap.noiseMultiTapDelta/2;
                    
                    _zoom.delta = _zoom.delta * 1/2;
                    _rotate.delta = _rotate.delta * 3/4;
                    
                    _swipe.edgeHArea = _swipe.edgeHArea * 3/4;
                    _swipe.edgeVArea = _swipe.edgeVArea * 3/4;
                    
                    _scroll.edgeHArea = _scroll.edgeHArea * 3/4;
                    _scroll.edgeVArea = _scroll.edgeVArea * 3/4;
                    
                    // IOLog("STD :: MDX %d, MDY %d, TD %d, RD %d, PD %d, EH %d, EV %d, ESV %d, ESH %d\n", _swipe.mulFingDeltaX, _swipe.mulFingDeltaY, _tap.noiseDelta, _rotate.delta, _zoom.delta, _swipe.edgeHArea, _swipe.edgeVArea, _scroll.edgeVArea, _scroll.edgeHArea);
                    
                    //
                    // Both fingers delta Y changes in 2F rotate gesture even if a finger is constant,
                    // so increase max Y delta threshold for constant finger to overcome this.
                    //
                    
                    _rotate.maxCFDeltaY = _rotate.delta/3;
                    break;
                    
                case 3:
                    if (_elanTPad.v3ElanLegacyMode && _pointerResolution >= 500) {
                        _pointerResolution = _pointerResolution - 400;
                    }
                    
                    // Decrease swipe delta as its got less resolution
                    _swipe.mulFingDeltaX = _swipe.mulFingDeltaX - (_swipe.mulFingDeltaX/4);
                    _swipe.mulFingDeltaY = _swipe.mulFingDeltaY - (_swipe.mulFingDeltaY/4);
                    
                    //
                    // Both fingers delta Y changes in 2F rotate gesture even if a finger is constant,
                    // so increase max Y delta threshold for constant finger to overcome this.
                    //
                    
                    _rotate.maxCFDeltaY = _rotate.delta/3;
                    break;
                    
                default:
                case 4:
                    break;
            }
            break;
            
        case STD_MOUSE:
        default:
            break;
    }
    
    //
    // Update if plist asked
    //
    
    setAccelAndResolution();
    
    //
    // Set Trackpad name and driver version (optional)
    //
    
    setProperty("Product", "PS/2 Smart-Pad");
    
    setProperty ("VersionNumber", 4, 32);
    setProperty ("VersionString", "4.7");
    
    //
    // Setup workloop for scroll timer thread
    //
    
    _stdWorkLoop = getWorkLoop();
    
    if (!_stdWorkLoop)
    {
        _device->release();
        return false;
    }
    
    //
    // Setup Pointer timer event source for pointer enhancement
    //
    
    _pointerTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &ApplePS2SmartTouchpad::EnhancePointerAcceleration));
    
    if (_pointerTimer)
        _stdWorkLoop->addEventSource(_pointerTimer);
    
    //
    // Setup scroll timer event source for scroll enhancement
    //
    
    _scrollTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &ApplePS2SmartTouchpad::EnhanceScrollAcceleration));
    
    if (_scrollTimer)
        _stdWorkLoop->addEventSource(_scrollTimer);
    
    //
    // Setup button and Drag Release timer event source
    //
    
    _buttonDragTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &ApplePS2SmartTouchpad::Release_ButtonsAndDrag));
    
    if (_buttonDragTimer)
        _stdWorkLoop->addEventSource(_buttonDragTimer);
    
    //
    // Setup Features Toggle timer event source
    //
    
    _featureTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &ApplePS2SmartTouchpad::ProcessFeaturesToggle));
    
    if (_featureTimer)
        _stdWorkLoop->addEventSource(_featureTimer);
    
    //
    // Setup Keyboard backlight off timer event source
    //
    
    if (_hasAsusKBackLight && _hasKBLightAutoOff) {
        _kBackLightTimer = IOTimerEventSource::timerEventSource(this, OSMemberFunctionCast(IOTimerEventSource::Action, this, &ApplePS2SmartTouchpad::ControlfKeybrdBackLight));
        
        if (_kBackLightTimer)
            _stdWorkLoop->addEventSource(_kBackLightTimer);
    }
    
    //
    // Install our driver's interrupt handler, for asynchronous data delivery.
    //
    
    _device->installInterruptAction(this,OSMemberFunctionCast(PS2InterruptAction, this, &ApplePS2SmartTouchpad::interruptOccurred));
    
    _interruptHandlerInstalled = true;
    
    
    // Install our power control handler.
    //
    
    _device->installPowerControlAction( this, OSMemberFunctionCast(PS2PowerControlAction, this, &ApplePS2SmartTouchpad::setDevicePowerState) );
    
    _powerControlHandlerInstalled = true;
    
    //
    // Install our Keyboard Notification Handler
    //
    
    _device->installPS2NotificationAction(this, OSMemberFunctionCast(PS2NotificationAction, this, &ApplePS2SmartTouchpad::receiveKeyboardNotifications));
    
    _keyboardNotificationHandlerInstalled = true;
    
    
    //
    // Detect OSX version
    //
    
    detectOSXVersion();
    
    //
    // Check for the ATKD entry to detect Asus Notebook
    //
    
    IORegistryEntry * fnDeviceEntry = IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/ATKD");
    if (fnDeviceEntry != NULL) {
        _isAsusNotebook = true;
        fnDeviceEntry->release();
    }
    else
        _isAsusNotebook = false;
    
    //
    // Initialize keyboard and enable Asus Keyboard backlight support
    //
    
    if (_hasAsusKBackLight)
        _device->dispatchPS2Notification(kPS2C_EnableAsusKBacklight);
    
    if (_has16LvlKBLight)
        _device->dispatchPS2Notification(kPS2C_EnableAsusKBacklight16Levels);
    
    _device->dispatchPS2Notification(kPS2C_InitKeyboard);
    
    //
    // Inform OS X version data to keyboard
    //
    
    _device->dispatchPS2Notification(kPS2C_OSXVersionNotify);
    // Send OS X version data
    _device->dispatchPS2Notification(_osxVersion);
    
    //
    // Finally, we enable the trackpad itself, so that it may start reporting
    // asynchronous events.
    //
    
    Enable_touchpad (true);
    
    
    return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SmartTouchpad::stop( IOService * provider )
{
    //
    // The driver has been instructed to stop.  Note that we must break all
    // connections to other service objects now (ie. no registered actions,
    // no pointers and retains to objects, etc), if any.
    //
    
    BASIC_DBG_LOG("STD :: Stopping driver...\n");
    
    assert(_device == provider);
    
    //
    // Reset mode for Synaptics
    //
    
    if (_touchpadVendor == STD_SYNAPTICS)
        Synaptics_reset();
    
    //
    // Release workLoop
    //
    
    OSSafeReleaseNULL(_stdWorkLoop);
    
    //
    // Disable the mouse itself, so that it may stop reporting mouse events.
    //
    
    Enable_touchpad (false);
    
    //
    // Disable the mouse clock and the mouse IRQ line.
    //
    
    setCommandByte( _device, kCB_DisableMouseClock, kCB_EnableMouseIRQ );
    
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
    // Uninstall the Keyboard Notification handler
    //
    
    if (_keyboardNotificationHandlerInstalled) _device->unistallPS2NotificationAction();
    _keyboardNotificationHandlerInstalled = false;
    
    super::stop(provider);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SmartTouchpad::free()
{
    //
    // Release the pointer to the provider object.
    //
    
    if (_device)
    {
        _device->release();
        _device = 0;
    }
    
    //
    // Release timers
    //
    
    if (_pointerTimer)
    {
        _pointerTimer->cancelTimeout();
        _stdWorkLoop->removeEventSource(_pointerTimer);
        _pointerTimer->release();
        _pointerTimer = 0;
    }
    
    if (_scrollTimer)
    {
        _scrollTimer->cancelTimeout();
        _stdWorkLoop->removeEventSource(_scrollTimer);
        _scrollTimer->release();
        _scrollTimer = 0;
    }
    if (_buttonDragTimer)
    {
        _buttonDragTimer->cancelTimeout();
        _stdWorkLoop->removeEventSource(_buttonDragTimer);
        _buttonDragTimer->release();
        _buttonDragTimer = 0;
    }
    if (_featureTimer)
    {
        _featureTimer->cancelTimeout();
        _stdWorkLoop->removeEventSource(_featureTimer);
        _featureTimer->release();
        _featureTimer = 0;
    }
    if (_kBackLightTimer)
    {
        _kBackLightTimer->cancelTimeout();
        _stdWorkLoop->removeEventSource(_kBackLightTimer);
        _kBackLightTimer->release();
        _kBackLightTimer = 0;
    }
    
    super::free();
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Function that processs device power state transitions.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::setDevicePowerState( UInt32 whatToDo )
{
    switch ( whatToDo )
    {
        case kPS2C_DisableDevice:
            
            //
            // Disable touchpad (synchronous).
            //
            
            Enable_touchpad(false);
            
            break;
            
        case kPS2C_EnableDevice:
            
            //
            // Must not issue any commands before the device has
            // completed its power-on self-test and calibration.
            //
            
            IOSleep(_sleepInitDelay);
            
            //
            // Clear the Accidental input, touchpad, gesture and reset mode try status
            //
            
            _kbEvent.accidentalInput = false;
            _kbEvent.disableTouchpad = false;
            _kbEvent.showPressWidth  = false;
            _modeSetTries = 3;
            
            //
            // Re-connect touchpad
            //
            
            Reconnect_touchpad();
            
            break;
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Methods that reads config from the plist.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::ReadSTDPlistConfig()
{
    
    if (OSDictionary *prefDict = OSDynamicCast(OSDictionary, getProperty("Preferences")))
    {
        UInt32 plistTimeout = 0;

        readPlistKey<UInt16>(prefDict, SleepInitDelay, INT16_TYPE, _sleepInitDelay);

        //
        // Buttons Preferences
        //
        
        if (OSDictionary *btnDict = OSDynamicCast(OSDictionary, prefDict->getObject(ButtonsKey)))
        {
            readPlistKey<bool>(btnDict, DisablePhysicalClicks, BOOL_TYPE, _button.disableClicks);
            readPlistKey<bool>(btnDict, ClickpadVirtualButtons, BOOL_TYPE, _button.clickPadHasVButtons);
            readPlistKey<bool>(btnDict, VirtualMiddleButton, BOOL_TYPE, _button.enableMiddle);
            readPlistKey<bool>(btnDict, PointingOnVirtualButtonArea, BOOL_TYPE, _button.enablePointingOnBtnArea);

            readPlistKey<UInt16>(btnDict, ClickpadVirtualButtonsArea, INT16_TYPE, _button.clickPadVBtnArea);
            _button.clickPadVBtnArea =
            (_button.clickPadVBtnArea > 0)? ((_ymax - _ymin) * _button.clickPadVBtnArea/100):0;

            readPlistKey<UInt8>(btnDict, FingersClickAction2F, INT8_TYPE, _fingPress.fings2Click);
            if (_fingPress.fings2Click > MAX_GESTURE_ACTIONS)
                _fingPress.fings2Click = 0; // Disable
        }
        
        //
        // Pointing Preferences
        //
        
        if (OSDictionary *pDict = OSDynamicCast(OSDictionary, prefDict->getObject(PointerPreferencesKey)))
        {
            UInt8 aLevel = 0;
            readPlistKey<UInt8>(pDict, HIDTrackpadAccelLevel, INT8_TYPE, aLevel);
            _pointerAccell = aLevel;
            
            readPlistKey<UInt8>(pDict, AccelerationPrefValue, INT8_TYPE, _pAccelPreference);
            
            readPlistKey<UInt8>(pDict, PointerSmoothSamples, INT8_TYPE, _pSmoothSamples);
            if (_pSmoothSamples > 10) {
                _pSmoothSamples = 10; // Reset to max
            }
            
            readPlistKey<UInt8>(pDict, PointerSmoothLevel, INT8_TYPE, _pSmoothLevel);
            if (_pSmoothLevel > 20) {
                _pSmoothLevel = 20; // Reset to max
            }

            if (OSDictionary *smoveFilterDict =
                OSDynamicCast(OSDictionary, pDict->getObject(SlowMoveNoiseMaxDelta)))
            {
                _pSlowMoveNoiseDelta = ReadPlistDictionaryKey(smoveFilterDict);
            }
            
            // Disabled Touchpad edges area
            if (OSDictionary *tpEdgeDict
                = OSDynamicCast(OSDictionary, pDict->getObject(TouchpadEdgesPreferencesKey)))
            {
                readPlistKey<UInt16>(tpEdgeDict, DisableLeftEdgeArea, INT16_TYPE, _tpLeftEdgeDArea);
                _tpLeftEdgeDArea = (_tpLeftEdgeDArea > 0)? ((_xmax - _xmin) * _tpLeftEdgeDArea/100):0;

                readPlistKey<UInt16>(tpEdgeDict, DisableRightEdgeArea, INT16_TYPE, _tpRightEdgeDArea);
                _tpRightEdgeDArea = (_tpRightEdgeDArea > 0)? ((_xmax - _xmin) * _tpRightEdgeDArea/100):0;
                
                readPlistKey<UInt16>(tpEdgeDict, DisableTopEdgeArea, INT16_TYPE, _tpTopEdgeDArea);
                _tpTopEdgeDArea = (_tpTopEdgeDArea > 0)? ((_ymax - _ymin) * _tpTopEdgeDArea/100):0;
                
                readPlistKey<UInt16>(tpEdgeDict, DisableBottomEdgeArea, INT16_TYPE, _tpBottomEdgeDArea);
                _tpBottomEdgeDArea = (_tpBottomEdgeDArea > 0)? ((_ymax - _ymin) * _tpBottomEdgeDArea/100):0;
                
                if (_tpLeftEdgeDArea || _tpRightEdgeDArea || _tpTopEdgeDArea || _tpBottomEdgeDArea) {
                    IOLog("STD :: Disabled Touchpad edges area for Left %d, Right %d, Top %d, Bottom %d\n", _tpLeftEdgeDArea, _tpRightEdgeDArea, _tpTopEdgeDArea, _tpBottomEdgeDArea);
                }
            }
            
            if (OSDictionary *pResolutionDict = OSDynamicCast(OSDictionary, pDict->getObject(PointerResolution)))
            {
                switch (_hw_version) {
                    case 2:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pResolutionDict->getObject("ELANv2"))) {
                            _pointerResolution = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 3:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pResolutionDict->getObject("ELANv3"))) {
                            _pointerResolution = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 4:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pResolutionDict->getObject("ELANv4"))) {
                            _pointerResolution = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 5:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pResolutionDict->getObject("FocalTech"))) {
                            _pointerResolution = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 6:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pResolutionDict->getObject("Synaptics"))) {
                            _pointerResolution = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    default:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pResolutionDict->getObject("MouseMode"))) {
                            _pointerResolution = numKey->unsigned32BitValue();
                        }
                        break;
                }
            }
            
            if (OSDictionary *pCustAccelDict = OSDynamicCast(OSDictionary, pDict->getObject(PointerCustomAcceleration)))
            {
                switch (_hw_version) {
                    case 2:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pCustAccelDict->getObject("ELANv2"))) {
                            _pAccelCustom = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 3:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pCustAccelDict->getObject("ELANv3"))) {
                            _pAccelCustom = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 4:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pCustAccelDict->getObject("ELANv4"))) {
                            _pAccelCustom = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 5:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pCustAccelDict->getObject("FocalTech"))) {
                            _pAccelCustom = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 6:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pCustAccelDict->getObject("Synaptics"))) {
                            _pAccelCustom = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    default:
                        _pAccelCustom = 16;
                        break;
                }
            }
            
            readPlistKey<UInt8>(pDict, EnhancedAccelerationLevel, INT8_TYPE, _pExtAccelLevel);
            readPlistKey<UInt8>(pDict, EnhancedAccelerationMinDelta, INT8_TYPE, _pExtAccelMinDelta);
            
            if (OSDictionary *pSenseDict = OSDynamicCast(OSDictionary, pDict->getObject(Sensitivity)))
            {
                _tSenseLvl = ReadPlistDictionaryKey(pSenseDict);
                if (_tSenseLvl < 0 && _tSenseLvl > 255)
                    _tSenseLvl = 0;
            }
            
            readPlistKey<bool>(pDict, Clickpad2FingersMove, BOOL_TYPE, _button.clickPad2FMove);
            readPlistKey<bool>(pDict, DisableTrackPointButton, BOOL_TYPE, _button.disbleTrackPoint);
        }
        
        //
        // Scrolling Preferences
        //
        
        if (OSDictionary *scrollDict = OSDynamicCast(OSDictionary, prefDict->getObject(ScrollPreferencesKey)))
        {
            if (OSDictionary *pScrollResolution = OSDynamicCast(OSDictionary, scrollDict->getObject(ScrollResolution)))
            {
                switch (_hw_version) {
                    case 2:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pScrollResolution->getObject("ELANv2"))) {
                            _scroll.resoultion = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 3:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pScrollResolution->getObject("ELANv3"))) {
                            _scroll.resoultion = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 4:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pScrollResolution->getObject("ELANv4"))) {
                            _scroll.resoultion = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 5:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pScrollResolution->getObject("FocalTech"))) {
                            _scroll.resoultion = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    case 6:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pScrollResolution->getObject("Synaptics"))) {
                            _scroll.resoultion = numKey->unsigned32BitValue();
                        }
                        break;
                        
                    default:
                        if (OSNumber *numKey = OSDynamicCast(OSNumber, pScrollResolution->getObject("MouseMode"))) {
                            _scroll.resoultion = numKey->unsigned32BitValue();
                        }
                        break;
                }
            }

            
            if (OSDictionary *edgeScrollDict =
                OSDynamicCast(OSDictionary, scrollDict->getObject(EdgeScrollPreferencesKey)))
            {
                readPlistKey<UInt8>(edgeScrollDict, EdgeScrollAccelMode, INT8_TYPE, _scroll.accelModeEdge);
                readPlistKey<UInt8>(edgeScrollDict, EdgeCircularScrollDelta, INT8_TYPE, _scroll.edgeCirDelta);

                readPlistKey<UInt16>(edgeScrollDict, EdgeHScrollAdaptArea, INT16_TYPE, _scroll.edgeHAdaptArea);
                _scroll.edgeHAdaptArea =
                (_scroll.edgeHAdaptArea > 100)? 0 : (_ymax - _ymin) * _scroll.edgeHAdaptArea/100;

                readPlistKey<UInt16>(edgeScrollDict, EdgeVScrollAdaptArea, INT16_TYPE, _scroll.edgeVAdaptArea);
                _scroll.edgeVAdaptArea = (_scroll.edgeVAdaptArea > 100)? 0 : (_xmax - _xmin) * _scroll.edgeVAdaptArea/100;

                readPlistKey<UInt16>(edgeScrollDict, EdgeHScrollArea, INT16_TYPE, _scroll.edgeHArea);
                _scroll.edgeHArea = (_scroll.edgeHArea > 100)? 0 : (_ymax - _ymin) * _scroll.edgeHArea/100;
                
                readPlistKey<UInt16>(edgeScrollDict, EdgeVScrollArea, INT16_TYPE, _scroll.edgeVArea);
                _scroll.edgeVArea = (_scroll.edgeVArea > 100)? 0 : (_xmax - _xmin) * _scroll.edgeVArea/100 ;

                
                readPlistKey<UInt8>(edgeScrollDict, EScrolSpeedFactor, INT8_TYPE, _scroll.edgeSpeedFactor);
                
                if (OSDictionary *eScrollContDict =
                    OSDynamicCast(OSDictionary, edgeScrollDict->getObject(EContScrollMaxDelta)))
                {
                    _scroll.contMaxDeltaEdge = ReadPlistDictionaryKey(eScrollContDict);
                }
                
                readPlistKey<bool>(edgeScrollDict, EdgeScrolling, BOOL_TYPE, _scroll.hasEdgeScroll);
                readPlistKey<bool>(edgeScrollDict, InertialEdgeScroll, BOOL_TYPE, _scroll.hasEdgeInertia);
                readPlistKey<bool>(edgeScrollDict, EdgeCircularScroll, BOOL_TYPE, _scroll.hasEdgeCScroll);
                readPlistKey<bool>(edgeScrollDict, ContinuousEdgeScroll, BOOL_TYPE, _scroll.hasEdgeCont);
                readPlistKey<bool>(edgeScrollDict, AutoEdgeScroll, BOOL_TYPE, _scroll.hasEdgeAuto);
                readPlistKey<bool>(edgeScrollDict, SlingEdgeScroll, BOOL_TYPE, _scroll.hasEdgeSling);
            }
            
            if (OSDictionary *twoFScrollDict = OSDynamicCast(OSDictionary, scrollDict->getObject(Scroll2FPreferencesKey)))
            {
                bool changeFlow = false;
                readPlistKey<bool>(twoFScrollDict, ChangeScrollFlowlDirection, BOOL_TYPE, changeFlow);
                if (changeFlow) { _scrollFactor = _scrollFactor * -1; }

                readPlistKey<bool>(twoFScrollDict, Scroll2FVertical, BOOL_TYPE, _isVScrollEnabled);
                readPlistKey<bool>(twoFScrollDict, Scroll2FHorizontal, BOOL_TYPE, _isHScrollEnabled);

                readPlistKey<UInt8>(twoFScrollDict, TwoFingScrollAcceMode, INT8_TYPE, _scroll.accelMode2F);
                if (_scroll.accelMode2F > 3) { // Restrict to max 3
                    _scroll.accelMode2F = 3;
                }
                
                readPlistKey<bool>(twoFScrollDict, Inertial2FingScroll, BOOL_TYPE, _scroll.has2FInertia);
                readPlistKey<bool>(twoFScrollDict, Continuous2FingScroll, BOOL_TYPE, _scroll.has2FCont);
                readPlistKey<bool>(twoFScrollDict, Auto2FingScroll, BOOL_TYPE, _scroll.has2FAuto);
                readPlistKey<bool>(twoFScrollDict, Sling2FingScroll, BOOL_TYPE, _scroll.has2FSling);
                
                readPlistKey<UInt8>(twoFScrollDict, ScrollSmoothSamples, INT8_TYPE, _scroll.smoothSamples);
                if (_scroll.smoothSamples > 10) { // Restrict to max 10
                    _scroll.smoothSamples = 10;
                }
                
                if (OSDictionary *cScrollDict =
                    OSDynamicCast(OSDictionary, twoFScrollDict->getObject(Cont2FScrollMaxDelta)))
                {
                    _scroll.contMaxDelta2F = ReadPlistDictionaryKey(cScrollDict);
                }
            }
            
            readPlistKey<UInt8>(scrollDict, InertialScrollDepth, INT8_TYPE, _scroll.inertiaDepth);
            readPlistKey<UInt8>(scrollDict, InertialScrollDelay, INT8_TYPE, _scroll.inertiaDelay);
            readPlistKey<UInt8>(scrollDict, ScrollAccelLevelAtBoot, INT8_TYPE, _scroll.levelAtBoot);
            readPlistKey<UInt8>(scrollDict, ScrollAccelDelay, INT8_TYPE, _scroll.accelDelay);
            readPlistKey<UInt8>(scrollDict, ScrollLevelFactor, INT8_TYPE, _scroll.levelFactor);
            readPlistKey<UInt8>(scrollDict, ScrollLevelGranularity, INT8_TYPE, _scroll.levelGranularity);
            
            if (OSDictionary *sDirDict = OSDynamicCast(OSDictionary, scrollDict->getObject(ScrollDirectionSamples)))
            {
                _scroll.directionSamples = ReadPlistDictionaryKey(sDirDict);
                if (_scroll.directionSamples > 9) { // Restrict to max 9
                    _scroll.directionSamples = 9;
                }
            }
            
            readPlistKey<UInt8>(scrollDict, ScrollStopDetectSamples, INT8_TYPE, _scroll.stopSamples);
            readPlistKey<UInt8>(scrollDict, ScrollMaxIdleStateDelta, INT8_TYPE, _scroll.idleFingMaxDelta);

            readPlistKey<UInt32>(scrollDict, ScrollMaxIdleDetectTimeout, INT32_TYPE, plistTimeout);
             _scroll.idleFingDetectTimeout = plistTimeout * 1000000; // Convert from milli sec to nano sec

            plistTimeout = 0;
            readPlistKey<UInt32>(scrollDict, SlingScrollStartTimeOut, INT32_TYPE, plistTimeout);
            _scroll.slingStartTimeout = plistTimeout * 1000000; // Convert from milli sec to nano sec

            readPlistKey<UInt16>(scrollDict, SlingTriggerArea, INT16_TYPE, _scroll.slingTriggerArea);
            _scroll.slingTriggerArea = (_scroll.slingTriggerArea > 100)? 0 : (_ymax - _ymin) *  _scroll.slingTriggerArea/100 ;
            
            readPlistKey<bool>(scrollDict, NoInertiaForHScroll, BOOL_TYPE, _scroll.noHSInertia);
        }
        
        //
        // Swiping Preferences
        //
        
        if (OSDictionary *swipeDict = OSDynamicCast(OSDictionary, prefDict->getObject(SwipesPreferencesKey)))
        {
            readPlistKey<bool>(swipeDict, EdgeSwipes, BOOL_TYPE, _swipe.hasEdgeSwipes);
            readPlistKey<bool>(swipeDict, EdgeSwipes2Ways, BOOL_TYPE, _swipe.has2WayEdgeSwipes);
            readPlistKey<bool>(swipeDict, EdgeSwipesResetPointer, BOOL_TYPE, _swipe.resetPointerOnEdge);

            readPlistKey<bool>(swipeDict, Swipes3F, BOOL_TYPE, _swipe.has3FSwipes);
            readPlistKey<bool>(swipeDict, Swipes4F, BOOL_TYPE, _swipe.has4FSwipes);

            readPlistKey<UInt8>(swipeDict, EdgeSwipeLeft, INT8_TYPE, _swipe.leftEdge);
            readPlistKey<UInt8>(swipeDict, EdgeSwipeRight, INT8_TYPE, _swipe.rightEdge);
            readPlistKey<UInt8>(swipeDict, EdgeSwipeTop, INT8_TYPE, _swipe.topEdge);
            readPlistKey<UInt8>(swipeDict, EdgeSwipeBottom, INT8_TYPE, _swipe.bottomEdge);

            readPlistKey<UInt16>(swipeDict, EdgeSwipeSenseHArea, INT16_TYPE, _swipe.edgeHArea);
            _swipe.edgeHArea = (_swipe.edgeHArea > 100)? 0 : (_xmax - _xmin) * _swipe.edgeHArea/100;
            _swipe.nonEdgeHArea = (_xmax - _xmin)/5;
            
            readPlistKey<UInt16>(swipeDict, EdgeSwipeSenseVArea, INT16_TYPE, _swipe.edgeVArea);
            _swipe.edgeVArea = (_swipe.edgeVArea > 100)? 0 : (_ymax - _ymin) * _swipe.edgeVArea/100;
            _swipe.nonEdgeVArea = (_ymax - _ymin)/5;
            
            readPlistKey<UInt8>(swipeDict, EdgeSwipeFingers, INT8_TYPE, _swipe.edgeFingers);

            readPlistKey<UInt16>(swipeDict, EdgeSwipeDeltaX, INT8_TYPE, _swipe.edgeHDistance);
            _swipe.edgeHDistance = (_swipe.edgeHDistance > 100)? 0 : (_xmax - _xmin) * _swipe.edgeHDistance/100;

            readPlistKey<UInt16>(swipeDict, EdgeSwipeDeltaY, INT8_TYPE, _swipe.edgeVDistance);
            _swipe.edgeVDistance = (_swipe.edgeVDistance > 100)? 0 : (_ymax - _ymin) * _swipe.edgeVDistance/100;
            
            readPlistKey<UInt8>(swipeDict, SwipeLeftAction3F, INT8_TYPE, _swipe.left3F);
            readPlistKey<UInt8>(swipeDict, SwipeRightAction3F, INT8_TYPE, _swipe.right3F);
            readPlistKey<UInt8>(swipeDict, SwipeUpAction3F, INT8_TYPE, _swipe.up3F);
            readPlistKey<UInt8>(swipeDict, SwipeDownAction3F, INT8_TYPE, _swipe.down3F);

            readPlistKey<UInt8>(swipeDict, SwipeLeftAction4F, INT8_TYPE, _swipe.left4F);
            readPlistKey<UInt8>(swipeDict, SwipeRightAction4F, INT8_TYPE, _swipe.right4F);
            readPlistKey<UInt8>(swipeDict, SwipeUpAction4F, INT8_TYPE, _swipe.up4F);
            readPlistKey<UInt8>(swipeDict, SwipeDownAction4F, INT8_TYPE, _swipe.down4F);
        
            readPlistKey<UInt16>(swipeDict, MultiFingerSwipesDeltaX, INT16_TYPE, _swipe.mulFingDeltaX);
            readPlistKey<UInt16>(swipeDict, MultiFingerSwipesDeltaY, INT16_TYPE, _swipe.mulFingDeltaY);
        }
        
        //
        // Fingers press Preferences
        //
        
        if (OSDictionary *fpressDict = OSDynamicCast(OSDictionary, prefDict->getObject(FingerPressPreferencesKey)))
        {
            readPlistKey<SInt8>(fpressDict, FingersPressAction2F, INT8_TYPE, _fingPress.fings2Press);
            readPlistKey<SInt8>(fpressDict, FingersPressAction3F, INT8_TYPE, _fingPress.fings3Press);
            readPlistKey<SInt8>(fpressDict, FingersPressAction4F, INT8_TYPE, _fingPress.fings4Press);
            readPlistKey<SInt8>(fpressDict, FingersPressAction5F, INT8_TYPE, _fingPress.fings5Press);

            plistTimeout = 0;
            readPlistKey<UInt32>(fpressDict, FingersPressTimeOut, INT32_TYPE, plistTimeout);
            _fingPress.timeout = plistTimeout * 1000000; // Convert from milli sec to nano sec
        }
        
        //
        // Tapping Preferences
        //
        
        if (OSDictionary *tapDict = OSDynamicCast(OSDictionary, prefDict->getObject(TapPreferencesKey)))
        {
            readPlistKey<bool>(tapDict, EnableTaps, BOOL_TYPE, _isClickingEnabled);

            readPlistKey<bool>(tapDict, TrackpadTappingCorners, BOOL_TYPE, _cornerTap.enabled);
            readPlistKey<bool>(tapDict, DisableBottomCornersTap, BOOL_TYPE, _cornerTap.bottomCornersDisabled);
            readPlistKey<bool>(tapDict, CornerTapStartABS, BOOL_TYPE, _cornerTap.releaseStartABS);
            readPlistKey<bool>(tapDict, MulFingDoubleTaps, BOOL_TYPE, _tap.hasMulFDoubleTaps);

            readPlistKey<UInt16>(tapDict, CornerTapAreaLength, INT8_TYPE, _cornerTap.tapAreaLength);
            _cornerTap.tapAreaLength = (_cornerTap.tapAreaLength > 100)? 0 : _xmax * _cornerTap.tapAreaLength/100;

            readPlistKey<UInt16>(tapDict, CornerTapAreaHeight, INT8_TYPE, _cornerTap.tapAreaHeight);
            _cornerTap.tapAreaHeight = (_cornerTap.tapAreaHeight > 100)? 0 : _ymax * _cornerTap.tapAreaHeight/100;

            readPlistKey<SInt8>(tapDict, TapAction1F, INT8_TYPE, _tap.fingers1);
            readPlistKey<SInt8>(tapDict, TapAction2F, INT8_TYPE, _tap.fingers2);
            readPlistKey<SInt8>(tapDict, TapAction3F, INT8_TYPE, _tap.fingers3);
            readPlistKey<SInt8>(tapDict, TapAction4F, INT8_TYPE, _tap.fingers4);
            readPlistKey<SInt8>(tapDict, TapAction5F, INT8_TYPE, _tap.fingers5);

            readPlistKey<UInt8>(tapDict, TapMaxDelta, INT8_TYPE, _tap.noiseDelta);
            readPlistKey<UInt8>(tapDict, MulFingTapMaxDelta, INT8_TYPE, _tap.noiseMultiTapDelta);

            plistTimeout = 0;
            readPlistKey<UInt32>(tapDict, MulFingTapTimeOut, INT32_TYPE, plistTimeout);
            _tap.mulFingsTimeout = plistTimeout * 1000000;

            plistTimeout = 0;
            readPlistKey<UInt32>(tapDict, Click1FTimeOut, INT32_TYPE, plistTimeout);
            _tap.clickTimeout = plistTimeout * 1000000;
            
            plistTimeout = 0;
            readPlistKey<UInt32>(tapDict, Tap12FTimeout, INT32_TYPE, plistTimeout);
            _tap.timeout = plistTimeout * 1000000;
            
            if (OSDictionary *tapPressDict = OSDynamicCast(OSDictionary, tapDict->getObject(TapPressure1F)))
            {
                _tap.pressure = ReadPlistDictionaryKey(tapPressDict);
            }
        }
        
        //
        // Dragging Preferences
        //
        
        if (OSDictionary *dragDict = OSDynamicCast(OSDictionary, prefDict->getObject(DragPreferencesKey)))
        {
            readPlistKey<bool>(dragDict, EnableDrag, BOOL_TYPE, _isDragEnabled);
            readPlistKey<bool>(dragDict, DragLock, BOOL_TYPE, _isDragLockEnabled);
            
            readPlistKey<UInt8>(dragDict, DragFingerIDToUseForData, INT8_TYPE, _drag.fPressDragFingersID);

            readPlistKey<UInt8>(dragDict, DragFingersNum, INT8_TYPE, _drag.fPressDragFingersNum);
            readPlistKey<UInt32>(dragDict, FLiftDragReleaseTimeOut, INT32_TYPE, _drag.releaseTime);
            readPlistKey<bool>(dragDict, FingerPressDrag, BOOL_TYPE, _drag.hasFPresDrag);
            
            plistTimeout = 0;
            readPlistKey<UInt32>(dragDict, FingerPressDragStartTimeOut, INT32_TYPE, plistTimeout);
            _drag.fPressDragStartTimeOut = plistTimeout * 1000000;
            
            if (OSDictionary *fPressDragDict = OSDynamicCast(OSDictionary, dragDict->getObject(FingerPressDragPressure)))
            {
                _drag.fPressDragPressure = ReadPlistDictionaryKey(fPressDragDict);
            }
            
        }
        
        //
        // Other gestures Preferences
        //
        
        if (OSDictionary *otherGestDict = OSDynamicCast(OSDictionary, prefDict->getObject(OtherPreferencesKey)))
        {
            readPlistKey<bool>(otherGestDict, UseSynapticsGenericBounds, BOOL_TYPE, _synTPad.useGenericBounds);

            readPlistKey<bool>(otherGestDict, PinchZoom, BOOL_TYPE, _zoom.enabled);
            readPlistKey<bool>(otherGestDict, Rotation, BOOL_TYPE, _rotate.enabled);
            readPlistKey<bool>(otherGestDict, CircularRotate, BOOL_TYPE, _rotate.cirEnabled);
            readPlistKey<bool>(otherGestDict, DisableTPadOnLIDClose, BOOL_TYPE, _enableLidPolling);

            readPlistKey<UInt8>(otherGestDict, PinchDeltaScale, INT8_TYPE, _zoom.pinchDeltaScale);
            readPlistKey<UInt8>(otherGestDict, PinchRotateMinSamples, INT8_TYPE, _zoom.minSamples);

            readPlistKey<UInt16>(otherGestDict, PinchZoomDelta, INT16_TYPE, _zoom.delta);
            readPlistKey<UInt16>(otherGestDict, RotationDelta, INT16_TYPE, _rotate.delta);
            
            readPlistKey<UInt16>(otherGestDict, RotationMaxCFingDelta, INT16_TYPE, _rotate.maxCFDelta);
            _rotate.maxCFDeltaY   = _rotate.maxCFDelta;
            
            UInt16 cfTime = 0;
            readPlistKey<UInt16>(otherGestDict, RotationMinCFingTime, INT16_TYPE, cfTime);
            _rotate.minCFingIdleTime   = cfTime * 1000000;
            
            readPlistKey<SInt8>(otherGestDict, PinchAction4F, INT8_TYPE, _fings4PinchAction);
            readPlistKey<SInt8>(otherGestDict, PinchAction5F, INT8_TYPE, _fings5PinchAction);
            
            readPlistKey<bool>(otherGestDict, PalmDetection, BOOL_TYPE, _palm.detectionEnabled);
            
            if (OSDictionary *maxTEPressDict = OSDynamicCast(OSDictionary, otherGestDict->getObject(MaxTouchEndPressure)))
            {
                _tap.maxTEndPressure = ReadPlistDictionaryKey(maxTEPressDict);
            }
            
            if (OSDictionary *maxTEDeltaDict = OSDynamicCast(OSDictionary, otherGestDict->getObject(MaxTouchEndDelta)))
            {
                _tap.maxTEndDelta = ReadPlistDictionaryKey(maxTEDeltaDict);
            }
            
            if (OSDictionary *palmPressDict = OSDynamicCast(OSDictionary, otherGestDict->getObject(PalmCheckMinPressure)))
            {
                _palm.minPressure = ReadPlistDictionaryKey(palmPressDict);
            }
            
            if (OSDictionary *palmWidthDict = OSDynamicCast(OSDictionary, otherGestDict->getObject(PalmCheckMinWidth)))
            {
                _palm.minWidth = ReadPlistDictionaryKey(palmWidthDict);
            }
            
            if (OSNumber *numKey = OSDynamicCast(OSNumber, otherGestDict->getObject(TPadStatusCheckPollRate)))
            {
                _acpiPollRate = numKey->unsigned32BitValue() * 1000000; // 1500000000
                if (_acpiPollRate < 500000000)
                {
                    _acpiPollRate = 500000000; // 500 ms, dont let this below 0.5 sec
                }
            }
        }
        
        //
        // Mouse Preferences
        //
        if (OSDictionary *mouseDict = OSDynamicCast(OSDictionary, prefDict->getObject(MousePreferencesKey)))
        {
            readPlistKey<UInt8>(mouseDict, MouseResolution, INT8_TYPE, _ps2Mouse.mResolution);
            readPlistKey<UInt8>(mouseDict, MouseSampleRate, INT8_TYPE, _ps2Mouse.mSampleRate);
            readPlistKey<UInt8>(mouseDict, MouseScaling, INT8_TYPE, _ps2Mouse.mScaling);

            if (_ps2Mouse.mScaling > 1)
                _ps2Mouse.mScaling = kDP_SetMouseScaling2To1;
            else
                _ps2Mouse.mScaling = kDP_SetMouseScaling1To1;
        }
        
        //
        // Keyboard Preferences
        //
        
        if (OSDictionary *kboardDict = OSDynamicCast(OSDictionary, prefDict->getObject(KeyboardPreferencesKey)))
        {
            readPlistKey<bool>(kboardDict, IgnoreAccidentalTouchpadInput, BOOL_TYPE, _isTypeModeEnabled);
            readPlistKey<bool>(kboardDict, EnableAsusKBacklightSupport, BOOL_TYPE, _hasAsusKBackLight);
            readPlistKey<bool>(kboardDict, EnableAsusKBacklightAutoOff, BOOL_TYPE, _hasKBLightAutoOff);
            readPlistKey<bool>(kboardDict, AsusKBacklightHas16Levels, BOOL_TYPE, _has16LvlKBLight);
            readPlistKey<bool>(kboardDict, TapEnablesTPadFromAccInput, BOOL_TYPE, _kbEvent.tapEnableTPad);

            readPlistKey<UInt32>(kboardDict, AsusKBacklightAutoOffTimeOut, INT32_TYPE, _kBLightAutoOffTimeout);

            readPlistKey<UInt8>(kboardDict, TriggerAccInputForKeyPressCount, INT8_TYPE, _kbEvent.accInputKeysCount);
            
            plistTimeout = 0;
            readPlistKey<UInt32>(kboardDict, TimeOutEnablesTPadAfterAccInput, INT32_TYPE, plistTimeout);
            _kbEvent.accInputTimeOut = plistTimeout * 1000000;
        }
        
    }
}

template <class pType> void ApplePS2SmartTouchpad::readPlistKey(const void* keyDict,
                                                                const char* keyStr,
                                                                const UInt8 keyType, pType& value)
{
    value = 0; // Init bool = false, int = 0 as default

    if (keyDict == NULL) {
        return;
    }
    
    OSNumber* nKey = NULL;
    OSBoolean* bKey = NULL;
    
    switch (keyType) {
        case INT8_TYPE:
            if ((nKey = OSDynamicCast(OSNumber, ((OSDictionary*)keyDict)->getObject(keyStr)))) {
                value = nKey->unsigned8BitValue();
                
                // Reset
                if (value > 255)
                    value = 0;
            }
            break;
            
        case INT16_TYPE:
            if ((nKey = OSDynamicCast(OSNumber, ((OSDictionary*)keyDict)->getObject(keyStr)))) {
                value = nKey->unsigned16BitValue();
            }
            break;
            
        case INT32_TYPE:
            if ((nKey = OSDynamicCast(OSNumber, ((OSDictionary*)keyDict)->getObject(keyStr)))) {
                value = nKey->unsigned32BitValue();
            }
            break;
            
        case BOOL_TYPE:
            if ((bKey = OSDynamicCast(OSBoolean, ((OSDictionary*)keyDict)->getObject(keyStr)))) {
                value = bKey->getValue();
            }
            break;
            
        default:
            value = 0; // bool = false, int = 0 as default
            break;
    }
    
    DEBUG_LOG("STD :: %s Key -> value %d\n", keyStr, value);
    
    // Reset to default if its negative
    if (value < 0)
        value = 0;
}

UInt8 ApplePS2SmartTouchpad::ReadPlistDictionaryKey(OSDictionary *dict)
{
    UInt8 value = 0;
    
    if (dict)
    {
        switch (_hw_version) {
            case 2:
                if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("ELANv2"))) {
                    value = numKey->unsigned8BitValue();
                }
                break;
                
            case 3:
                if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("ELANv3"))) {
                    value = numKey->unsigned8BitValue();
                }
                break;
                
            case 4:
                if (_button.isClickPad)
                {
                    if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("ELANv4ClickPad"))) {
                        value = numKey->unsigned8BitValue();
                    }
                }
                else {
                    if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("ELANv4"))) {
                        value = numKey->unsigned8BitValue();
                    }
                }
                break;
                
            case 5:
                if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("FocalTech"))) {
                    value = numKey->unsigned8BitValue();
                }
                break;
                
            case 6:
                if (_button.isClickPad)
                {
                    if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("SynapticsClickPad"))) {
                        value = numKey->unsigned8BitValue();
                    }
                }
                else {
                    if (OSNumber *numKey = OSDynamicCast(OSNumber, dict->getObject("Synaptics"))) {
                        value = numKey->unsigned8BitValue();
                    }
                }
                break;
                
            default:
                break;
        }
    }
    
    // IOLog("STD :: Dictionary %d\n", value);
    
    return value;
}

void ApplePS2SmartTouchpad::ReadAccelPlistConfig()
{
    if (_pAccelPreference != 0)
    {
        OSDictionary *accelprefDict;
        accelprefDict = OSDynamicCast(OSDictionary, getProperty("AccelerationPreferences"));
        if (accelprefDict){
            OSIterator *iter = 0;
            
            const OSSymbol *dictKey = 0;
            OSData   *dictKeyData   = 0;
            OSArray  *arrayKey      = 0;
            
            iter = OSCollectionIterator::withCollection(accelprefDict);
            if (iter) {
                while ((dictKey = (const OSSymbol *)iter->getNextObject())) {
                    dictKeyData = 0;
                    arrayKey    = 0;
                    
                    const char *keyStrValue = dictKey->getCStringNoCopy();
                    
                    arrayKey = OSDynamicCast(OSArray, accelprefDict->getObject(dictKey));
                    if (arrayKey) {
                        if (!strncmp(keyStrValue,"HIDAccelCurvesMT", strlen(keyStrValue)) && _pAccelPreference == 1)
                        {
                            setProperty("HIDAccelCurves", arrayKey);
                        }
                        else
                            if (!strncmp(keyStrValue,"HIDAccelCurvesBT", strlen(keyStrValue)) && _pAccelPreference == 0)
                            {
                                setProperty("HIDAccelCurves", arrayKey);
                            }
                    }
                    
                    dictKeyData = OSDynamicCast(OSData, accelprefDict->getObject(dictKey));
                    if (dictKeyData) {
                        
                        if (!strncmp(keyStrValue,"AppleMultiTouch", strlen(keyStrValue)) && _pAccelPreference == 1)
                        {
                            setProperty("HIDPointerAccelerationTable", dictKeyData);
                            setProperty("HIDDefaultBehavior", "Trackpad");
                        }
                        
                        else if (!strncmp(keyStrValue,"AppleUSBMouse", strlen(keyStrValue)) && _pAccelPreference == 2)
                        {
                            setProperty("HIDPointerAccelerationTable", dictKeyData);
                            setProperty("HIDDefaultBehavior", "Mouse");
                            removeProperty("HIDAccelCurves");
                        }
                        else if (!strncmp(keyStrValue,"AppleWirelessMouse2007", strlen(keyStrValue)) && _pAccelPreference == 3)
                        {
                            setProperty("HIDPointerAccelerationTable", dictKeyData);
                            setProperty("HIDDefaultBehavior", "Mouse");
                            removeProperty("HIDAccelCurves");
                        }
                        else if (!strncmp(keyStrValue,"AppleBluetoothMultiTouch", strlen(keyStrValue)) && _pAccelPreference == 4)
                        {
                            setProperty("HIDPointerAccelerationTable", dictKeyData);
                            setProperty("HIDDefaultBehavior", "Trackpad");
                            removeProperty("HIDAccelCurves");
                        }
                        else if (!strncmp(keyStrValue,"AppleBluetoothTrackpad", strlen(keyStrValue)) && _pAccelPreference == 0)
                        {
                            setProperty("HIDPointerAccelerationTable", dictKeyData);
                            setProperty("HIDDefaultBehavior", "Trackpad");
                        }
                        
                    }
                }
                iter->release();
            }
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Find external Mouse (i.e. USB or BT mouse) using IOHIDSystem ioreg.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::detectExternalMouse()
{
    IORegistryEntry* pHIDSystem = IORegistryEntry::fromPath("IOService:/IOResources/IOHIDSystem");
    
    if (pHIDSystem != NULL) {
        DEBUG_LOG("STD :: IOHIDSystem found\n");

        const   OSArray*    pNXInfoArray        = 0;
        const   OSSymbol*   pNXInfoOSSymbol     = 0;
        
        OSIterator* pNXInfoArraryIter   = 0;
        
        pNXInfoArray = OSDynamicCast(OSArray, pHIDSystem->getProperty("NXSystemInfo"));
        if (pNXInfoArray) {
            DEBUG_LOG("STD :: Array Length %d\n", pNXInfoArray->getCount());
            
            pNXInfoArraryIter = OSCollectionIterator::withCollection(pNXInfoArray);
            
            if (pNXInfoArraryIter) {
                UInt8 mouseCount = 0;
                
                while ((pNXInfoOSSymbol = (const OSSymbol*)pNXInfoArraryIter->getNextObject()))
                {
                    // IOLog("STD :: Array Object %s, Length %d\n", pNXInfoOSSymbol->getCStringNoCopy(), pNXInfoOSSymbol->getLength());
                    const OSDictionary* pNXInfoArraryDict = OSDynamicCast(OSDictionary, pNXInfoOSSymbol);
                    if (pNXInfoArraryDict)
                    {
                        DEBUG_LOG("STD :: Array Dictionary found\n");
                            
                        if (const OSNumber* numKey = OSDynamicCast(OSNumber, pNXInfoArraryDict->getObject("HIDKind"))) {
                            UInt8 hidKind = numKey->unsigned8BitValue();
                            DEBUG_LOG("STD :: HIDKind %d\n", hidKind);
                            if (hidKind == 2)
                            {
                                // From iokit/IOKit/hidsystem/IOHIDevice.h
                                // typedef enum {
                                //    kHIUnknownDevice          = 0,
                                //    kHIKeyboardDevice         = 1, // Keyboard
                                //    kHIRelativePointingDevice = 2 // Mouse
                                //} IOHIDKind;
                                
                                mouseCount++;
                                if (mouseCount > 1) {
                                    _externalMouseFound = true;
                                }
                                else {
                                    _externalMouseFound = false;
                                }
                                DEBUG_LOG("STD :: External mouse %s\n", _externalMouseFound?"Found":"Not found");
                            }
                        }
                    }
                }
                pNXInfoArraryIter->release();
            }
        }
        
        pHIDSystem->release();
    }
    else {
        DEBUG_LOG("STD :: IOHIDSystem->NXSystemInfo not found.\n");
    }
    
   clock_get_uptime(TIME_NOW);
   _lastExtMouseStatusCheckTime = absoluteToUINT64(_now);
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// ACPI and IOREG Poller.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_ACPI_IOREG_Polling()
{
    if (_isAsusNotebook) {
       
        IORegistryEntry *fnDeviceEntry = IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/ATKD/AsusNBFnKeys");
        
        if (fnDeviceEntry != NULL) {
            
            if (OSDynamicCast(OSBoolean, fnDeviceEntry->getProperty("TouchpadEnabled")))
                _isFnKeyDisabledTouchpad = false;
            
            else if (OSDynamicCast(OSBoolean, fnDeviceEntry->getProperty("TouchpadDisabled")))
                _isFnKeyDisabledTouchpad = true;
            
            DEBUG_LOG("STD :: Touchpad is %s by Fn key\n", _isFnKeyDisabledTouchpad?"Disbaled":"Enabled");
            
            //
            // Clear enhanced scroll and move if its active
            // on Touchpad disable.
            //
            
            if (_isFnKeyDisabledTouchpad)
            {
                StopScrollEnhanceTimer();
                StopPointerEnhanceTimer();
            }
            
            OSSafeRelease(fnDeviceEntry);
        }
        else {
            _isAsusNotebook = false;
            IOLog("STD :: AsusNBFnKeys kext not found, Fn key touchpad disable support wont work.\n");
        }
    }
    
    //
    // Read the LID state from the DSDT
    //
    
    if (_enableLidPolling) {
        
        IOACPIPlatformDevice * acpiProvider;
        acpiProvider =
        (IOACPIPlatformDevice*)IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/LID0");
        
        if (acpiProvider)
            acpiProvider->retain();
        else {
            acpiProvider =
            (IOACPIPlatformDevice*)IORegistryEntry::fromPath("IOService:/AppleACPIPlatformExpert/LID");
            if (acpiProvider)
                acpiProvider->retain();
        }
        
        if (acpiProvider) {
            UInt32 res = 0;
            
            if (kIOReturnSuccess == acpiProvider->validateObject("_LID")) {
                acpiProvider->evaluateInteger("_LID", &res);
                if (res == _lidClosed) {
                    IOLog("STD :: LID is %s\n",(res == 0)?"Closed":"Open");
                    if (res == 0)
                        _lidClosed = true;
                    else
                        _lidClosed = false;
                    
                    IOLog("STD :: Touchpad is %s\n", _lidClosed?"Disbaled":"Enabled");
                }
            }
            else {
                _enableLidPolling = false;
                IOLog("STD :: _LID method not found, touchpad control on LID Close/Open disabled.\n");
            }
            
            OSSafeRelease(acpiProvider);
        }
    }
    
    clock_get_uptime(TIME_NOW);
    _lastFnKeyStateChkTime = absoluteToUINT64(_now);
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Interrupt Handler for the touchpad data received from the controller.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::interruptOccurred( UInt8 data )
{
    //
    // This will be invoked automatically from our device when asynchronous
    // events need to be delivered. Process the trackpad data. Do NOT issue
    // any BLOCKING commands to our device in this context.
    //
    
    //
    // Add this byte to the packet buffer. If the packet is complete, that is,
    // we have the 6 bytes, dispatch this packet for processing.
    //
    _packetBuffer[_packetByteCount++] = data;
    
    DEBUG_LOG("STD :: Touchpad Data from PS/2 controller %x, BytesCount %d, PacketsCount %d\n", data, _packetByteCount, _packetsCount);
    
    //
    // Ignore all bytes until we see the valid start packet for the packet stream and
    // when "0xAA 0x00" 2-byte packet is sent following a hardware reset
    // which happens during power state transitions at boot, after sleep etc.,
    // Reset the packet buffer, otherwise packets will go out of sequence.
    //
    
    if ((_packetByteCount == 1 && _packetBuffer[0] == 0x00 && _packetsCount == 0 && _cFingers == 0) ||
        (_packetByteCount == 2 && _packetBuffer[0] == kSC_Reset && _packetBuffer[1] == 0x00))
    {
        if (_packetByteCount == 1 && _touchpadVendor == STD_ELAN_TECH && _elanTPad.crcEnabled)
        {
            // Skip for crc enabled packets as they have 0x00 in first byte sometimes
        }
        else {
            if (_packetBuffer[0] == kSC_Reset)
            {
                DEBUG_LOG("STD :: Reset data from PS/2 controller.\n");
                
                /*
                 * Sometimes touchpad may be not in absolute mode for some reason,
                 * which may happen during power state transitions like at boot, after sleep etc.,
                 * So try to reconnect the touchpad again.
                 */
                
                Reconnect_touchpad();
            }
            else {
                DEBUG_LOG("STD :: Invalid data 0x%x (%d) from PS/2 controller\n", _packetBuffer[0], data);
            }
            _packetByteCount = 0;
            _endPacketsCount = 0;
        }
    }
    
    //
    // Process the packet when the buffer has 6 bytes
    //
    
    else if (_packetByteCount == _pktsize)
    {
        _packetByteCount = 0;

        //
        // Look for duplicate packets sometimes received at the end of the stream
        // and discard them.
        //
        
        if (_cFingers == 0 && // No finger
            !(_packetBuffer[0] & 0x03) && _button.data == 0 && // No button
            _packetBuffer[1] == 0 && _packetBuffer[2] == 0 &&
            _packetBuffer[4] == 0 && _packetBuffer[5] == 0)
        {
            _endPacketsCount++;
            DEBUG_LOG("STD :: Duplicate packet detected.\n");
        }
        else
            _endPacketsCount = 0;
        
        if (_endPacketsCount > 1) {
            DEBUG_LOG("STD :: Duplicate end packet discarded.\n");
            return;
        }
        
        // Update up time
        clock_get_uptime(TIME_NOW);
        
        UInt8 *packet = _packetBuffer;
        UInt8 packet_type = 255;
        UInt8 p1, p2, p3;
        
        //
        // Check for external mouse (works only in 10.10 or lower)
        //
        
        if (_usbMouseDisablesTrackpad)
        {
            uint64_t timeSinceExtMouseCheck = absoluteToUINT64(_now) - _lastExtMouseStatusCheckTime;

            DEBUG_LOG("STD :: Time since status check is %lld, PollRate %lld, Enabled %d\n", timeSinceExtMouseCheck, _extMouseCheckPollRate, _usbMouseDisablesTrackpad);

            if (timeSinceExtMouseCheck > _extMouseCheckPollRate) {
                detectExternalMouse();
            }
            
            if (_externalMouseFound)
            {
                DEBUG_LOG("STD :: Touchpad disabled due to external mouse, and user asked to ignore trackpad on external mouse.\n");
                return; // Disable trackpad
            }
        }
        
        //
        // Read Touchpad state from Asus Fn keys driver ioreg entry
        // and LID status from the DSDT device LID if the last read was done before poll rate.
        //

        if (_isAsusNotebook || _enableLidPolling) {
            uint64_t timeSinceACPIRead  = absoluteToUINT64(_now) - _lastFnKeyStateChkTime;

             DEBUG_LOG("STD :: LastCheck is %lld, PollRate %lld, Enabled %d\n", timeSinceACPIRead, _acpiPollRate, (_enableLidPolling | _isAsusNotebook));
            
            if (timeSinceACPIRead > _acpiPollRate) {
                Process_ACPI_IOREG_Polling ();
            }
            
            if (_isFnKeyDisabledTouchpad || _lidClosed) {
                return;
            }
        }
        
        //
        // Accidental Input mode control
        // Enable/Disable the touchpad data processing based on the _kbEvent.lastKeyPressTime time difference
        //
        
        uint64_t timeSinceAccInput      = absoluteToUINT64(_now) - _kbEvent.lastKeyPressTime;

        if (_isTypeModeEnabled && _kbEvent.accidentalInput
            && timeSinceAccInput > _kbEvent.accInputTimeOut
            && (_kbEvent.accInputTimeOut > 0 || !_kbEvent.tapEnableTPad))
        {

            _kbEvent.accidentalInput = false;
            _kbEvent.kbKeysCount = 0;
            DEBUG_LOG("STD :: Touchpad enabled back after timeout for Accidental input\n");
        }
        
        //
        // Stop if touchpad is disabled (or) its an accidental input.
        //
        if ((_kbEvent.accidentalInput && !_kbEvent.tapEnableTPad) || _kbEvent.disableTouchpad)
        {
            DEBUG_LOG("STD ::LastKeyPressTime %lld TimeOut %lld, Diff %lld, LastTouchPadStateCheck Time %lld, TimeFromLastStatusRead %lld, HW %d\n",_kbEvent.lastKeyPressTime, _kbEvent.accInputTimeOut, timeSinceAccInput, _lastFnKeyStateChkTime, lastStatusRead, _hw_version);

            return;
        }
        
        //
        // Process packet
        //
        
        switch (_hw_version) {
                
            case STD_MOUSE_HW: // BASIC MOUSE
                processMousePacket(packet);
                
#if DEBUG_TPD
                if (_debugLvl & 0x1)
                    IOLog("STD :: PS/2 Mouse packet [ 0x%02x , 0x%02x, 0x%02x ]\n", packet[0], packet[1], packet[2]);
#endif
                break;
                
            case STD_SYNAPTICS_HW:
                
                if (_synTPad.pkt_type == SYN_NEWABS) {
                    
                    _synTPad.pkt_type = SYN_NEWABS_STRICT;
                    
                    for (int i = 0; i < 5; i++)
                        
                        if (!Synaptics_validate_byte(packet, i, SYN_NEWABS_STRICT))
                        {
                            DEBUG_LOG("STD :: Using relaxed packet validation\n");
                            _synTPad.pkt_type = SYN_NEWABS_RELAXED;
                        }
                }
                
                Synaptics_report_absolute(packet);
                
                //
                // Tail packet 0x80 0x00 0x00 0xc0 0x00 0x00
                // is received at the end.
                //
                
                if ((packet[0] & 0x80) == 0x80 &&
                    (packet[3] & 0xc0) == 0xc0 &&
                    packet[1] == 0x00 && packet[2] == 0x00 &&
                    packet[4] <= 0x01 && packet[5] <= 0x01)
                {
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_TAIL);
#endif
                }
                
                //
                // Data packet 0x8x xx xx 0xCx xx xxx of touch and phy buttons.
                //
                
                else if ((packet[0] & 0x80) == 0x80 && (packet[3] & 0xc0) == 0xc0)
                {
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_HEAD);
#endif
                }
                else {
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_HEAD);
#endif
                }
                break;
                
            case STD_FOCAL_TECH_HW: // FOCAL TECH
                
                // Trigger keyboard backlight on event
                if (_hasAsusKBackLight && _hasKBLightAutoOff)
                    receiveKeyboardNotifications(kPS2C_KeybrdBackLightOn);
                
                packet_type = packet[0] & 0xf;
                
                switch (packet_type) {
                    case PACKET_FT_STATUS:
                        Process_absolute_events_ELANv4AndFocalTech(PACKET_STATUS, packet);
#if DEBUG_TPD
                        packet_type = PACKET_STATUS;
#endif
                        break;
                        
                    case PACKET_FT_ABSOLUTE:
                        Process_absolute_events_ELANv4AndFocalTech(PACKET_ABSOLUTE, packet);
#if DEBUG_TPD
                        packet_type = PACKET_ABSOLUTE;
#endif
                        break;
                        
                    case PACKET_FT_RELATIVE:
                        Process_absolute_events_ELANv4AndFocalTech(PACKET_RELATIVE, packet);
#if DEBUG_TPD
                        packet_type = PACKET_RELATIVE;
#endif
                        break;
                        
                    default:
#if DEBUG_TPD
                        packet_type = PACKET_UNKNOWN;
#endif
                        
                        /*
                         * Sometimes touchpad may be not in absolute mode for some reason,
                         * which may happen during power state transitions like at boot, after sleep etc.,
                         * So try to reconnect the touchpad again.
                         */
                        
                        if (_modeSetTries > 0) {
                            Reconnect_touchpad();
                            _modeSetTries--;
                        }
                        break;
                }
                
#if DEBUG_TPD
                if (_debugLvl & 0x1)
                    LogPacketData(packet, packet_type);
#endif
                break;
                
            case STD_ELAN_HW_V1: // ELAN v1 hardware
                
                DUMP_LOG("STD :: PS/2 packet [ 0x%02x , 0x%02x, 0x%02x, 0x%02x ]\n",
                         packet[0], packet[1], packet[2], packet[3]);
                
                /* Parity bits are placed differently */
                if (_elanTPad.fw_version < 0x020000) {
                    /* byte 0:  D   U  p1  p2   1  p3   R   L */
                    p1 = (packet[0] & 0x20) >> 5;
                    p2 = (packet[0] & 0x10) >> 4;
                } else {
                    /* byte 0: n1  n0  p2  p1   1  p3   R   L */
                    p1 = (packet[0] & 0x10) >> 4;
                    p2 = (packet[0] & 0x20) >> 5;
                }
                
                p3 = (packet[0] & 0x04) >> 2;
                
                
                if (_elanTPad.paritycheck && !(_elanTPad.parityBit[packet[1]] == p1 &&
                                              _elanTPad.parityBit[packet[2]] == p2 &&
                                              _elanTPad.parityBit[packet[3]] == p3))
                {
                    break; // bad data
                }
                
                Process_absolute_events_ELANv1(packet);
                break;
                
            case STD_ELAN_HW_V2: // ELAN v2 hardware
                
                /*
                 * Ignore debounce packets if found
                 */
                
                if (!memcmp(packet, elan_debounce_packet_v2, sizeof(elan_debounce_packet_v2)))
                {
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_DEBOUNCE);
#endif
                    break;
                }
                
                /*
                 * Note From Linux:
                 * V2 hardware has two flavors. Older ones that do not report pressure,
                 * and newer ones that reports pressure and width. With newer ones, all
                 * packet (1, 2, 3 finger touch) have the same constant bits. With
                 * older ones, 1/3 finger touch packet and 2 finger touch packet
                 * have different constant bits.
                 * With all three cases, if the constant bits are not exactly what I
                 * expected, I con_sider them invalid.
                 */
                
#if DEBUG_TPD
                if (_debugLvl & 0x1)
                    LogPacketData(packet, 0);
#endif
                
                if (_elanTPad.paritycheck)
                {
                    if (_elanTPad.reportsPressure
                        && !((packet[0] & 0x0c) == 0x04 && (packet[3] & 0x0f) == 0x02))
                    {
                        DUMP_LOG("STD :: Invalid packet but reports pressure.\n");
                    }
                    else if (((packet[0] & 0xc0) == 0x80) &&
                             !((packet[0] & 0x0c) == 0x0c && (packet[3] & 0x0e) == 0x08))
                    {
                        DUMP_LOG("STD :: Invalid packet with constant bit in packet[0].\n");
                    }
                    else if (!((packet[0] & 0x3c) == 0x3c && (packet[1] & 0xf0) == 0x00 &&
                               (packet[3] & 0x3e) == 0x38 && (packet[4] & 0xf0) == 0x00))
                    {
                        DUMP_LOG("STD :: Invalid packet.\n");
                    }
                }
                
                // Trigger keyboard backlight on event
                if (_hasAsusKBackLight && _hasKBLightAutoOff)
                    receiveKeyboardNotifications(kPS2C_KeybrdBackLightOn);
                
                Process_absolute_events_ELANv2v3(0, packet);
                break;
                
                /*
                 * We check the constant bits to determine what packet type we get,
                 * so packet checking is mandatory for v3 and later hardware.
                 */
                
            case STD_ELAN_HW_V3: // ELAN v3 hardware
                
                /*
                 * Ignore the PACKET_END packet which is received
                 * at the end of every packet stream after a little delay.
                 */
                
                if (!memcmp(packet, elan_end_packet_v3, sizeof(elan_end_packet_v3)))
                {
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_END);
#endif
                    break;
                }
                
                /*
                 * Ignore deboune packets because it has the same signature as PACKET_HEAD in byte 0
                 * and byte 3, but rest of the bytes are invalid.
                 */
                
                if (!memcmp(packet, elan_debounce_packet_v3, sizeof(elan_debounce_packet_v3)) ||
                    !memcmp(packet, elan_debounce_packet_v3_2, sizeof(elan_debounce_packet_v3_2)))
                {
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_DEBOUNCE);
#endif
                    break;
                }
                
                /*
                 * If the hardware flag '_elanTPad.crcEnabled' is set, then packet have
                 * different signatures.
                 */
                
                if (_elanTPad.crcEnabled) {
                    DUMP_LOG("STD :: CRC enabled packet detected\n");
                    
                    if ((packet[3] & 0x09) == 0x08)
                        packet_type = PACKET_HEAD;
                    
                    if ((packet[3] & 0x09) == 0x09)
                        packet_type = PACKET_TAIL;
                } else {
                    if ((packet[0] & 0x0c) == 0x04 && (packet[3] & 0xcf) == 0x02)
                        packet_type = PACKET_HEAD;
                    
                    if ((packet[0] & 0x0c) == 0x0c && (packet[3] & 0xce) == 0x0c)
                        packet_type = PACKET_TAIL;
                    
                    if ((packet[3] & 0x0f) == 0x06)
                        packet_type = PACKET_TRACKPOINT;
                }
                
                // Trigger keyboard backlight on event
                if (_hasAsusKBackLight && _hasKBLightAutoOff)
                    receiveKeyboardNotifications(kPS2C_KeybrdBackLightOn);
                
                switch (packet_type) {
                    case PACKET_TRACKPOINT:
                    case PACKET_HEAD: // Head packet
                    case PACKET_TAIL: // Tail packet (will be received only in two fingers data)
                        Process_absolute_events_ELANv2v3(packet_type , packet);
                        break;
                        
                    default:
                        if (_elanTPad.crcEnabled) {
                            DUMP_LOG("STD :: CRC enabled packet detected\n");
                            
                            if ((packet[3] & 0x09) == 0x08) {
                                packet_type = PACKET_HEAD;
                            }
                            
                            if ((packet[3] & 0x09) == 0x09) {
                                packet_type = PACKET_TAIL;
                            }
                            Process_absolute_events_ELANv2v3(packet_type , packet);
                        }
                        else {
                            /*
                             * Sometimes touchpad may be not in absolute mode for some reason,
                             * which may happen during power state transitions like at boot, after sleep etc.,
                             * So try to reconnect the touchpad again.
                             */
                            
                            if (_modeSetTries > 0) {
                                _modeSetTries--;
                                Reconnect_touchpad();
                            }

                        }
                        break;
                }
                
#if DEBUG_TPD
                if (_debugLvl & 0x1)
                    LogPacketData(packet, packet_type);
#endif
                break;
                
            case STD_ELAN_HW_V4: // ELAN v4 hardware
                
                if (_elanTPad.supportsTrackPoint && (packet[3] & 0x0f) == 0x06)
                    packet_type = PACKET_TRACKPOINT;
                else
                    packet_type = packet[3] & 0x03;
                
                /*
                 * Sanity check based on the constant bits of a packet.
                 * The constant bits change depending on the value of
                 * the hardware flag '_elanTPad.crcEnabled' but are the same for
                 * every packet, regardless of the type.
                 */
                
                if (_elanTPad.crcEnabled)
                    _elanTPad.sanityCheck = ((packet[3] & 0x08) == 0x00);
                else
                    _elanTPad.sanityCheck = ((packet[0] & 0x0c) == 0x04 &&
                                            (packet[3] & 0x1c) == 0x10);
                
                if (!_elanTPad.sanityCheck || (packet_type == 0x00 && packet[0] == 0x00
                                      && (packet[1] != 0x00 || packet[3] != 0x00 ||
                                          packet[2] != 0x00 || packet[4] != 0x00 ||
                                          packet[5] != 0x00)))
                {
                    /*
                     * Some systems fail to work in absolute mode at boot even though we tried set absolute
                     * mode before, these cases fail at sanity check due not being in absolute mode.
                     * So try to reconnect the touchpad again.
                     */
                    
                    if (!_elanTPad.sanityCheck && _modeSetTries > 0) {
                        _modeSetTries--;
                        Reconnect_touchpad();
                    }
                    
#if DEBUG_TPD
                    if (_debugLvl & 0x1)
                        LogPacketData(packet, PACKET_UNKNOWN);
#endif
                    break;
                }
                
                // Trigger keyboard backlight on event
                if (_hasAsusKBackLight && _hasKBLightAutoOff)
                    receiveKeyboardNotifications(kPS2C_KeybrdBackLightOn);
                
                switch (packet_type) {
                        
                    case PACKET_STATUS:
                        
                        /*
                         * Note: Legacy fix which may no longer needed, because this was seen only in CRC enabled 
                         * firmware where byte 0 is not consistent. So,a check is added to prevent ignoring byte 0 with 0x00.
                         *
                         * Correct status packet for the unexpected packet stream starting with byte 0 as 0x00,
                         * because 0x00 at byte 0 is invalid mostly, but seems like its valid
                         * in new firmware from Lenovo Z51.
                         * We have to correct this packet to work normal.
                         *
                         * Received packet -> 00 xx 00 10 00 00
                         * Edited packet for invalid byte 0 -> xx 00 10 00 00 xx
                         * Corrected packet xx 00 10 00 00 xx -> 34 xx 00 10 00 00
                         * where xx = fingers count bit mask
                         */
                        
                        if (packet[0] != 0x00 && packet[1] == 0x00 &&
                            packet[2] == 0x10 && packet[3] == 0x00 &&
                            packet[4] == 0x00)
                        {
                            DEBUG_LOG("STD :: Invalid packet detected...\n");

                            // Swap bytes to their place
                            packet[1] = packet[0];
                            packet[3] = packet[2];
                            
                            // Correct bytes
                            packet[0] = 0x34;
                            packet[2] = 0x00;
                            packet[5] = 0x00;
                            
                            // Increment by 1 as the packet stream is going to have 0x00 at byte 0
                            _packetByteCount++;
                        }
                        
                    case PACKET_ABSOLUTE:
                    case PACKET_RELATIVE:
                        // Synchronize Input
                        // Elantech_input_sync_v4();
                        
                        Process_absolute_events_ELANv4AndFocalTech(packet_type , packet);
                        
                        // Synchronize Input
                        // Elantech_input_sync_v4();
                        break;
                        
                    case PACKET_TRACKPOINT: // Not finished yet
                        if (!_button.disbleTrackPoint)
                            Process_trackpoint_events_ELAN(packet_type , packet);
                        break;

                    default:
#if DEBUG_TPD
                        packet_type = PACKET_UNKNOWN;
#endif
                        
                        /*
                         * Sometimes touchpad may be not in absolute mode for some reason,
                         * which may happen during power state transitions like at boot, after sleep etc.,
                         * So try to reconnect the touchpad again.
                         */
                        
                        if (_modeSetTries > 0) {
                            Reconnect_touchpad();
                            _modeSetTries--;
                        }
                        break;
                }
                
#if DEBUG_TPD
                if (_debugLvl & 0x1)
                    LogPacketData(packet, packet_type);
#endif
                break;
                
            default:
                break;
        }
    }
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to dump packets.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::LogPacketData(unsigned char *packet, int packetType)
{
    char *packetName;
    
    switch (packetType) {
        
        case PACKET_PASS_THRU:
            packetName = (char *)"Pass through";
            break;
            
        case PACKET_RESET:
            packetName = (char *)"Reset";
            break;
            
        case PACKET_DEBOUNCE:
            packetName = (char *)"Debounce";
            break;
            
        case PACKET_UNKNOWN:
            packetName = (char *)"Unknown";
            break;
            
        case PACKET_HEAD:
            packetName = (char *)"Head";
            break;
            
        case PACKET_TAIL:
            packetName = (char *)"Tail";
            break;
            
        case PACKET_END:
            packetName = (char *)"End";
            break;
            
        case PACKET_ABSOLUTE:
            packetName = (char *)"Absolute";
            break;
            
        case PACKET_RELATIVE:
            packetName = (char *)"Relative";
            break;
            
        case PACKET_STATUS:
            packetName = (char *)"Status";
            break;
            
        default:
            // Synaptics type
            switch (_synTPad.pkt_type) {
                    
                case SYN_NEWABS:
                    packetName = (char *)"Absolute new";
                    break;
                    
                case SYN_NEWABS_STRICT:
                    packetName = (char *)"Absolute strict";
                    break;
                    
                case SYN_NEWABS_RELAXED:
                    packetName = (char *)"Absolute relaxed";
                    break;
                    
                case SYN_OLDABS:
                    packetName = (char *)"Absolute old";
                    break;
                    
                default:
                    packetName = (char *)"Unknown";
                    break;
            }
            break;
    }
    
    IOLog("STD :: PS/2 %s packet [ 0x%02x , 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x ]\n", packetName, packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
}


//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to detect touchpad.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn ApplePS2SmartTouchpad::Detect_touchpad()
{
    unsigned char param[3];
    IOReturn ret = kIOReturnSuccess;
    PS2Request *    request = _device->allocateRequest();
    
    if (!request) return kIOReturnNoMemory;
    
    //
    // Reset the touchpad to its default state and
    // disable stream mode before the command sequence.
    //
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetDefaultsAndDisable;
    request->commandsCount = 1;
    _device->submitRequestAndBlock(request);
    
    if (request->commandsCount == 1) {
        BASIC_DBG_LOG("STD :: Touchpad set to default state.\n");
    }
    else {
        IOLog("STD :: Failed to reset the touchpad to default state.\n");
        _device->freeRequest(request);
         return kIOReturnIOError;
    }
    
    //
    //  Use magic knock to detect ELAN/FocalTech touchpads
    //
    
    // Magic knock
    request->commands[0].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut  = kDP_SetMouseScaling1To1;
    request->commands[1].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut  = kDP_SetMouseScaling1To1;
    request->commands[2].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut  = kDP_SetMouseScaling1To1;
    
    //  Read Data
    request->commands[3].command  = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut  = kDP_GetMouseInformation;
    request->commands[4].command = kPS2C_ReadDataPort;
    request->commands[4].inOrOut = 0;
    request->commands[5].command = kPS2C_ReadDataPort;
    request->commands[5].inOrOut = 0;
    request->commands[6].command = kPS2C_ReadDataPort;
    request->commands[6].inOrOut = 0;
    request->commandsCount = 7;
    
    _device->submitRequestAndBlock(request);
    
    param[0] = request->commands[4].inOrOut;
    param[1] = request->commands[5].inOrOut;
    param[2] = request->commands[6].inOrOut;
    
    if (!(request->commandsCount == 7)) {
        IOLog("STD :: Sending ELAN/FocalTech magic knock failed.\n");
        ret = kIOReturnIOError;
    }
    
    _device->freeRequest(request);
    
    switch (_touchpadVendor) {
            
        case STD_FOCAL_TECH:
            
            //
            //  Report this in case there are FocalTech models that use a different
            //  set of magic numbers (Tested on FTL101 and FTL102).
            //
            
            if (param[0] != 0x3c || param[1] != 0xf0 || (param[2] != 0xc8 && param[2] != 0x00))
            {
                IOLog("STD :: Unexpected magic knock result 0x%02x, 0x%02x, 0x%02x.\n",
                      param[0], param[1], param[2]);
                ret = kIOReturnError;
            }
            break;
            
        default:
        case STD_ELAN_TECH:
            
            //
            //  Report this in case there are Elantech models that use a different
            //  set of magic numbers.
            //
            
            if (param[0] != 0x3c || param[1] != 0x03 || (param[2] != 0xc8 && param[2] != 0x00))
            {
                BASIC_DBG_LOG("STD :: Unexpected magic knock result 0x%02x, 0x%02x, 0x%02x for ELAN.\n",
                              param[0], param[1], param[2]);
                
                //
                //  Check for Synaptics
                //
                
                if (Synaptics_detect())
                {
                    IOLog("STD :: Unsupported touchpad found, will work only in enhanced mouse mode.\n");
                    _touchpadVendor = STD_MOUSE;
                }
                else {
                    _touchpadVendor = STD_SYNAPTICS;
                    IOLog("STD :: Synaptics touchpad found.\n");
                }
            }
            else {
                _touchpadVendor = STD_ELAN_TECH;
            }
            break;
    }
    
    return ret;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Reset and connect the touchpad.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Reconnect_touchpad()
{
    IOLog("STD :: Reconnecting the touchpad ........\n");
    
    //
    // Reset
    //
    
    resetMouse(_device);
    
    //
    //  Detect and place the touchpad back into absolute mode
    //
    
    for (int i = 0; i < 3; i++)
    {
        if (Detect_touchpad()) {
            IOLog("STD :: Could not detect Touchpad, may be controller was not ready. Will try (#%d) after some time...\n", i + 1);
            
            IOSleep(1000);
            
            if (i == 2) {
                IOLog("STD :: Failed to detect Touchpad.\n");
                return;
            }
        }
        else {
            break;
        }
    }
    
    //
    // Initialize mouse with sampleRate, resolution and scaling
    // but skip on ELAN (if needed) as they break sometimes in some systems if
    // keyboard was used first before touchpad.
    //
    
    if (initializeMouse(_device, _ps2Mouse.mSampleRate, _ps2Mouse.mResolution, _ps2Mouse.mScaling))
        return;
    
    switch (_touchpadVendor) {
            
        case STD_FOCAL_TECH:
            
            if (Focaltech_set_absolute_mode()) {
                IOLog("STD :: Failed to place the touchpad into absolute mode.\n");
                return;
            }
            else {
                BASIC_DBG_LOG("STD :: Successfully placed the touchpad into Absolute mode\n");
            }
            break;
            
        case STD_ELAN_TECH:
            
            if (Elantech_set_absolute_mode()) {
                IOLog("STD :: Failed to place the touchpad into absolute mode.\n");
                return;
            }
            else {
                BASIC_DBG_LOG("STD :: Successfully placed the touchpad into Absolute mode\n");
            }
            break;
            
        case STD_SYNAPTICS:
            
            if (Synaptics_set_absolute_mode()) {
                IOLog("STD :: Failed to place the touchpad into absolute mode.\n");
                return;
            }
            else {
                BASIC_DBG_LOG("STD :: Successfully placed the touchpad into Absolute mode\n");
            }
            break;
            
        default:
            break;
    }
    
    //
    // Clear packet buffer pointer to avoid issues caused by
    // stale packet fragments.
    //
    
    _packetByteCount = 0;
    
    //
    // Finally, we enable the trackpad itself, so that it may
    // start reporting asynchronous events.
    //
    
    Enable_touchpad(true);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SmartTouchpad::Enable_touchpad(bool enable)
{
    //
    // Instructs the trackpad to start or stop the reporting of data packet.
    // It is safe to issue this request from the interrupt/completion context.
    //
    
    //
    // Enable the mouse clock (should already be so) and the mouse IRQ line.
    //
    
    if (enable) {
        setCommandByte( _device, kCB_EnableMouseIRQ, kCB_DisableMouseClock );
    }
    
    PS2Request * request = _device->allocateRequest();
    if ( !request ) return;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = (enable)?kDP_Enable:kDP_SetDefaultsAndDisable;
    request->commandsCount = 1;
    
    _device->submitRequestAndBlock(request);
    
    if (request->commandsCount == 1) {
        if (enable)
            IOLog("STD :: Touchpad enabled.\n");
        else
            IOLog("STD :: Touchpad disabled.\n");
    }
    else
        IOLog("STD :: Failed to enable/disable Touchpad.\n");
    
    _device->freeRequest(request);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// FocalTech methods for handling the device.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

bool ApplePS2SmartTouchpad::Focaltech_setup_touchpad()
{
    unsigned char param[3];
    
    BASIC_DBG_LOG("STD :: Reading touchpad size....\n");
    
    //
    // Read touchpad size
    //
    
    if (Focaltech_read_reg(2, param)) {
        IOLog("STD :: Failed to read register.\n");
        return false;
    }
    
    /*
     * Note from linux:
     * not sure whether this is 100% correct
     */
    
    _xmax = (unsigned char)param[1] * 128;
    _ymax = (unsigned char)param[2] * 128;
    _xcenter = _xmax/2;
    _ycenter = _ymax/2;
    
    BASIC_DBG_LOG("STD :: Reading touchpad size....Xmax %d, Ymax %d\n", _xmax, _ymax);
    
    //
    // Set the touchpad into absolute mode device
    //
    
    if (Focaltech_set_absolute_mode()) {
        IOLog("STD :: Failed to set the touchpad into absolute mode.\n");
        return false;
    } else {
        BASIC_DBG_LOG("STD :: Successfully placed the touchpad into Absolute mode\n");
    }
    
    return true;
}

IOReturn ApplePS2SmartTouchpad::Focaltech_read_reg(int reg, unsigned char *param)
{
    IOReturn ret = kIOReturnSuccess;
    PS2Request *    request = _device->allocateRequest();
    
    if (!request) return kIOReturnNoMemory;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetMouseScaling1To1;
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = kDP_SetMouseResolution;
    request->commands[2].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut = 0;
    request->commands[3].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut = kDP_SetMouseResolution;
    request->commands[4].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[4].inOrOut = 0;
    request->commands[5].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[5].inOrOut = kDP_SetMouseResolution;
    request->commands[6].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[6].inOrOut = 0;
    request->commands[7].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[7].inOrOut = kDP_SetMouseResolution;
    request->commands[8].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[8].inOrOut = reg;
    
    request->commandsCount = 9;
    
    _device->submitRequestAndBlock(request);
    
    if (request->commandsCount != 9) {
        ret= kIOReturnIOError;
    }
    
    _device->freeRequest(request);
    
    if (getMouseInformation(_device, param))
    {
        ret= kIOReturnIOError;
    }
    
    return ret;
}

IOReturn ApplePS2SmartTouchpad::Focaltech_set_absolute_mode()
{
    BASIC_DBG_LOG("STD :: Trying to place the touchpad into Absolute mode...\n");
    
    IOReturn ret = kIOReturnSuccess;
    PS2Request *    request = _device->allocateRequest();
    
    if (!request) return kIOReturnNoMemory;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetAllMakeRelease;
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = 0;
    request->commands[2].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut = kDP_SetAllMakeRelease;
    request->commands[3].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut = 0;
    request->commands[4].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[4].inOrOut = kDP_SetAllMakeRelease;
    request->commands[5].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[5].inOrOut = 0;
    request->commands[6].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[6].inOrOut = kDP_SetAllMakeRelease;
    request->commands[7].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[7].inOrOut = 1;
    request->commands[8].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[8].inOrOut = kDP_SetMouseScaling1To1;
    
    request->commandsCount = 9;
    
    _device->submitRequestAndBlock(request);
    
    if (request->commandsCount != 9) {
        ret= kIOReturnIOError;
    }
    
    _device->freeRequest(request);
    
    return ret;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Synaptics methods for handling the device.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn ApplePS2SmartTouchpad::Synaptics_detect()
{
    IOReturn ret = kIOReturnSuccess;
    unsigned char param[3] = {0, 0, 0};
    
    PS2Request *    request = _device->allocateRequest();
    
    if (!request) return kIOReturnNoMemory;
    
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_SetMouseScaling1To1;
    request->commands[1].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[1].inOrOut = kDP_SetMouseResolution;
    request->commands[2].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[2].inOrOut = 0;
    request->commands[3].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[3].inOrOut = kDP_SetMouseResolution;
    request->commands[4].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[4].inOrOut = 0;
    request->commands[5].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[5].inOrOut = kDP_SetMouseResolution;
    request->commands[6].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[6].inOrOut = 0;
    request->commands[7].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[7].inOrOut = kDP_SetMouseResolution;
    request->commands[8].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[8].inOrOut = 0;
    
    request->commandsCount = 9;
    
    _device->submitRequestAndBlock(request);
    
    if (request->commandsCount != 9) {
        IOLog("STD :: Sending synaptics magic knock failed.\n");
        ret= kIOReturnIOError;
    }
    else
        BASIC_DBG_LOG("STD :: Synaptics magic knock result...\n");
    
    _device->freeRequest(request);
    
    if (getMouseInformation(_device, param))
    {
        ret= kIOReturnIOError;
    }
    
    if (param[1] != 0x47)  // Old Fw: 0x00, 0x46, 0x12
    {
        BASIC_DBG_LOG("STD :: Unexpected magic knock result 0x%02x, 0x%02x, 0x%02x for Synaptics, may be not a Synaptics touchpd.\n", param[0], param[1], param[2]);
        ret= kIOReturnIOError;;
    }
    
    return ret;
}

bool ApplePS2SmartTouchpad::Synaptics_setup_touchpad()
{
    UInt8 id[3];
    
    //
    //  Identify
    //
    
    if (sendSynapticsCmd(_device, SYN_QUE_IDENTIFY, id)) {
        IOLog("STD :: Failed to identify Synaptics.\n");
        return false;
    }
    
    _synTPad.identity = (id[0]<<16) | (id[1]<<8) | id[2];
    BASIC_DBG_LOG("STD :: Synaptics identity %ld.\n", _synTPad.identity);
    
    if (!SYN_ID_IS_SYNAPTICS(_synTPad.identity))
        return false;
    
    //
    //  Find model id
    //
    
    if (sendSynapticsCmd(_device, SYN_QUE_MODEL, id)) {
        IOLog("STD :: Failed to find Synaptics model id.\n");
        return false;
    }
    
    _synTPad.model_id = (id[0]<<16) | (id[1]<<8) | id[2];
    BASIC_DBG_LOG("STD :: Synaptics model id %ld.\n", _synTPad.model_id);
    
    //
    //  Find firmware id
    //
    
    if (sendSynapticsCmd(_device, SYN_QUE_FIRMWARE_ID, id)) {
        IOLog("STD :: Failed to find Synaptics firmware id.\n");
        return false;
    }
    
    _synTPad.firmware_id = (id[0] << 16) | (id[1] << 8) | id[2];
    BASIC_DBG_LOG("STD :: Synaptics firmware id %ld.\n", _synTPad.firmware_id);
    
    
    //
    //  Find board id
    //
    
    if (sendSynapticsCmd(_device, SYN_QUE_MODES, id)) {
        IOLog("STD :: Failed to find Synaptics board id.\n");
        return false;
    }
    
    _synTPad.board_id = ((id[0] & 0xfc) << 6) | id[1];
    BASIC_DBG_LOG("STD :: Synaptics board id %ld.\n", _synTPad.board_id);
    
    //
    //  Find capabilities
    //
    
    UInt8 cap[3];
    
    if (sendSynapticsCmd(_device, SYN_QUE_CAPABILITIES, cap)) {
        IOLog("STD :: Failed to find Synaptics capabilities.\n");
        return false;
    }
    
    _synTPad.capabilities = (cap[0] << 16) | (cap[1] << 8) | cap[2];
    _synTPad.ext_cap = _synTPad.ext_cap_0c = _synTPad.ext_cap_10 = 0;
    
    BASIC_DBG_LOG("STD :: Synaptics capabilities are 0x%lx.\n", _synTPad.capabilities);
    
    // Older firmwares had submodel ID fixed to 0x47
    if (SYN_ID_FULL(_synTPad.identity) < 0x705 &&
        SYN_CAP_SUBMODEL_ID(_synTPad.capabilities) != 0x47)
    {
        IOLog("STD :: Failed to find Synaptics old firmware capabilities.\n");
        return false;
    }
    
    // Unless capExtended is set the rest of the flags should be ignored
    if (!SYN_CAP_EXTENDED(_synTPad.capabilities))
        _synTPad.capabilities = 0;
    
    if (SYN_EXT_CAP_REQUESTS(_synTPad.capabilities) >= 1)
    {
        if (sendSynapticsCmd(_device, SYN_QUE_EXT_CAPAB, cap)) {
            BASIC_DBG_LOG("STD :: Device claims to have extended capabilities, but unable to read them\n");
        }
        else {
            _synTPad.ext_cap = (cap[0] << 16) | (cap[1] << 8) | cap[2];
            
            //
            // if nExtBtn is greater than 8 it should be con_sidered
            // invalid and treated as 0
            //
            
            if (SYN_CAP_MULTI_BUTTON_NO(_synTPad.ext_cap) > 8)
                _synTPad.ext_cap &= 0xff0fff;
            
            BASIC_DBG_LOG("STD :: Synaptics extended capabilities are 0x%lx.\n", _synTPad.ext_cap);
            
        }
    }
    
    if (SYN_EXT_CAP_REQUESTS(_synTPad.capabilities) >= 4) {
        if (sendSynapticsCmd(_device, SYN_QUE_EXT_CAPAB_0C, cap)) {
            BASIC_DBG_LOG("STD :: Device claims to have extended capabilities 0x0c, but unable to read them\n");
        } else {
            _synTPad.ext_cap_0c = (cap[0] << 16) | (cap[1] << 8) | cap[2];
            BASIC_DBG_LOG("STD :: Synaptics extended capabilities 0x0c are 0x%lx.\n", _synTPad.ext_cap_0c);
        }
    }
    
    //
    // Find resolution (Resolution is left zero if touchpad does not support the query)
    //
    
    unsigned char resp[3];
    
    if (SYN_ID_MAJOR(_synTPad.identity) < 4)
        ; // do nothing
    
    else {
        
        if (sendSynapticsCmd(_device, SYN_QUE_RESOLUTION, resp) == 0) {
            if (resp[0] != 0 && (resp[1] & 0x80) && resp[2] != 0) {
                _synTPad.x_res = resp[0]; /* x resolution in units/mm */
                _synTPad.y_res = resp[2]; /* y resolution in units/mm */
            }
        }
        
        //
        //  Read Min and Max coordinates
        //
        
        if (SYN_EXT_CAP_REQUESTS(_synTPad.capabilities) >= 5 &&
            SYN_CAP_MAX_DIMENSIONS(_synTPad.ext_cap_0c))
        {
            if (sendSynapticsCmd(_device, SYN_QUE_EXT_MAX_COORDS, resp)) {
                IOLog("STD :: Device claims to have max coordinates query, but unable to read them\n");
            } else {
                _xmax = (resp[0] << 5) | ((resp[1] & 0x0f) << 1);
                _ymax = (resp[2] << 5) | ((resp[1] & 0xf0) >> 3);
                IOLog("STD :: Max coordinates X %d, Y %d.\n", _xmax, _ymax);
            }
        }
        else
            IOLog("STD :: Device have no capabilities to read maximum coordinates.\n");
        
        if (SYN_EXT_CAP_REQUESTS(_synTPad.capabilities) >= 5 && // was 7 but some touchpads have 5
            SYN_CAP_MIN_DIMENSIONS(_synTPad.ext_cap_0c))
        {
            if (sendSynapticsCmd(_device, SYN_QUE_EXT_MIN_COORDS, resp)) {
                IOLog("STD :: Device claims to have min coordinates query, but unable to read them\n");
                
            } else {
                _xmin = (resp[0] << 5) | ((resp[1] & 0x0f) << 1);
                _ymin = (resp[2] << 5) | ((resp[1] & 0xf0) >> 3);
                IOLog("STD :: Min coordinates X %d, Y %d.\n", _xmin, _ymin);
            }
        }
        else
            IOLog("STD :: Device have no capabilities to read minimum coordinates.\n");
    }
    
    //
    //  Disable Gestures supported?
    //
    
    if (SYN_ID_DISGEST_SUPPORTED(_synTPad.identity))
        _synTPad.disable_gesture = true;
    
    BASIC_DBG_LOG("STD :: Gestures disable supported - %s.\n", _synTPad.disable_gesture?"Yes":"No");
    
    //
    //  Set the touchpad into absolute mode and packet type
    //
    
    _synTPad.absolute_mode = true;
    
    if (Synaptics_set_absolute_mode()) {
        IOLog("STD :: Failed to place the touchpad into absolute mode.\n");
        return false;
    }
    
    _synTPad.pkt_type = SYN_MODEL_NEWABS(_synTPad.model_id) ? SYN_NEWABS : SYN_OLDABS;
    
    IOLog("STD :: Touchpad model: %ld, FW version: %ld.%ld, ID: %#lx, Capabilities: %#lx/%#lx/%#lx, Board id: %lu, FW id: %lu.\n",SYN_ID_MODEL(_synTPad.identity),
          SYN_ID_MAJOR(_synTPad.identity), SYN_ID_MINOR(_synTPad.identity),
          _synTPad.model_id,
          _synTPad.capabilities, _synTPad.ext_cap, _synTPad.ext_cap_0c,
          _synTPad.board_id, _synTPad.firmware_id);
    
    //
    // Capable of pass through?
    //
    
    if (SYN_CAP_PASS_THROUGH(_synTPad.capabilities)) {
        IOLog("STD :: Capable of pass through.\n");
        
        if (sendSynapticsCmd(_device, SYN_QUE_MEXT_CAPAB_10, cap)) {
            IOLog("STD :: Failed to find pass through capabilities.\n");
        }
        else
            _synTPad.ext_cap_10 = (cap[0]<<16) | (cap[1]<<8) | cap[2];
    }
    
    //
    // Capable of multi touch?
    //
    
    if(SYN_CAP_ADV_GESTURE(_synTPad.ext_cap_0c)) {
        BASIC_DBG_LOG("STD :: Supports advanced gesture.\n");
    }
    
    if(SYN_CAP_IMAGE_SENSOR(_synTPad.ext_cap_0c)) {
        BASIC_DBG_LOG("STD :: Supports multi fingers gesture.\n");
    }
    
    //
    // is it a clickPad ?
    //
    
    if (SYN_CAP_CLICKPAD(_synTPad.ext_cap_0c))
        _button.isClickPad = true;
    else
        _button.isClickPad = false;
    
    IOLog("STD :: ClickPad? %s\n", _button.isClickPad?"Yes":"No");
    
    return true;
}

IOReturn ApplePS2SmartTouchpad::Synaptics_set_absolute_mode()
{
    _synTPad.mode = 0;
    
    if (_synTPad.absolute_mode)
        _synTPad.mode |= SYN_BIT_ABSOLUTE_MODE;
    
    if (_synTPad.disable_gesture)
        _synTPad.mode |= SYN_BIT_DISABLE_GESTURE;
    
    if (_ps2Mouse.mSampleRate >= 80)
        _synTPad.mode |= SYN_BIT_HIGH_RATE;
    else
        _synTPad.mode &= ~SYN_BIT_HIGH_RATE;
    
    if (SYN_CAP_EXTENDED(_synTPad.capabilities))
        _synTPad.mode |= SYN_BIT_W_MODE;
    
    if (Synaptics_mode_cmd(_synTPad.mode)) {
        IOLog("STD :: Failed to set the mode.\n");
        return kIOReturnError;
    }
    
    if (_synTPad.absolute_mode &&
        Synaptics_set_advanced_gesture_mode())
    {
        IOLog("STD :: Failed to set the advanced gesture mode.\n");
        return kIOReturnError;
    }
    
    return kIOReturnSuccess;
}

IOReturn ApplePS2SmartTouchpad::Synaptics_mode_cmd(UInt8 mode)
{
    if (sendSlicedPS2Cmd(_device, mode))
        return kIOReturnError;
    
    if (sendPS2Cmd(_device, kDP_SetMouseSampleRate))
        return kIOReturnError;
    
    if (sendPS2Cmd(_device, SYN_PS_SET_MODE2))
        return kIOReturnError;
    
    return kIOReturnSuccess;
}

IOReturn ApplePS2SmartTouchpad::Synaptics_set_advanced_gesture_mode()
{
    if (!(SYN_CAP_ADV_GESTURE(_synTPad.ext_cap_0c) ||
          SYN_CAP_IMAGE_SENSOR(_synTPad.ext_cap_0c))) {
        return kIOReturnSuccess;
    }
    
    if (sendSlicedPS2Cmd(_device, SYN_QUE_MODEL))
        return kIOReturnError;
    
    if (sendPS2Cmd(_device, kDP_SetMouseSampleRate))
        return kIOReturnError;
    
    if (sendPS2Cmd(_device, 0xc8))
        return kIOReturnError;
    
    /* Advanced gesture mode also sends multi finger data */
    _synTPad.capabilities |= (1 << 1); // set bit 1 (1 << x)
    
    return kIOReturnSuccess;
}

void ApplePS2SmartTouchpad::Synaptics_reset()
{
    //
    //  Reset touchpad back to relative mode, gestures enabled
    //
    
    if (Synaptics_mode_cmd(0)) {
        IOLog("STD :: Failed to reset synaptics to relative mode.\n");
    }
}


IOReturn ApplePS2SmartTouchpad::Synaptics_validate_byte(unsigned char *packet, int idx, unsigned char pkt_type)
{
    static const unsigned char newabs_mask[]	= { 0xC8, 0x00, 0x00, 0xC8, 0x00 };
    static const unsigned char newabs_rel_mask[]= { 0xC0, 0x00, 0x00, 0xC0, 0x00 };
    static const unsigned char newabs_rslt[]	= { 0x80, 0x00, 0x00, 0xC0, 0x00 };
    static const unsigned char oldabs_mask[]	= { 0xC0, 0x60, 0x00, 0xC0, 0x60 };
    static const unsigned char oldabs_rslt[]	= { 0xC0, 0x00, 0x00, 0x80, 0x00 };
    
    if (idx < 0 || idx > 4)
        return kIOReturnSuccess;
    
    switch (pkt_type) {
            
        case SYN_NEWABS:
        case SYN_NEWABS_RELAXED:
            return (packet[idx] & newabs_rel_mask[idx]) == newabs_rslt[idx];
            
        case SYN_NEWABS_STRICT:
            return (packet[idx] & newabs_mask[idx]) == newabs_rslt[idx];
            
        case SYN_OLDABS:
            return (packet[idx] & oldabs_mask[idx]) == oldabs_rslt[idx];
            
        default:
            IOLog("STD :: Unknown packet type %d.\n", pkt_type);
            return kIOReturnSuccess;
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// ELAN methods for handling the device.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Elantech_input_sync_v4()
{
    /*
     * Need to call this method for synchronizing the input in v4 hw
     * otherwise packets will be out of sync and touchpad will act strange.
     */
    
    if (_drag.isDragging) {
        _button.state |= 0x1;
    }
    
    dispatchRelativePointerPosition(0, 0, _button.state);
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Check the signatures set of magic numbers.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

bool ApplePS2SmartTouchpad:: Elantech_is_signatures_valid(const unsigned char *param)
{
    static const unsigned char rates[] = { 200, 100, 80, 60, 40, 20, 10 };
    int i;
    
    if (param[0] == 0)
        return false;
    
    if (param[1] == 0)
        return true;
    
    /*
     * Some models have a revision higher then 20. Meaning param[2] may
     * be 10 or 20, skip the rates check for these.
     */
    
    if ((param[1] & 0xef) == 0x0f && param[2] < 40)
        return true;
    
    for (i = 0; i < sizeof(rates); i++)
    {
        if (param[2] == rates[i])
            return false;
    }
    
    return true;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Place the Touchpad into Absolute mode.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn ApplePS2SmartTouchpad::Elantech_set_absolute_mode()
{
    unsigned char val = 0;
    int tries = ETD_READ_BACK_TRIES;
    IOReturn ret = kIOReturnSuccess;
    
    BASIC_DBG_LOG("STD :: Trying to place the touchpad into Absolute mode...\n");
    
    //
    // Reset the mouse
    //
    
    PS2Request * request = _device->allocateRequest();
    if ( !request ) return kIOReturnNoMemory;
    request->commands[0].command = kPS2C_SendMouseCommandAndCompareAck;
    request->commands[0].inOrOut = kDP_Reset; // 0xFF
    request->commandsCount = 1;
    _device->submitRequestAndBlock(request);
    _device->freeRequest(request);
    
    //
    // Set the mode to absolute
    //
    
    switch (_hw_version) {
        case 1:
            _elanTPad.reg_10 = 0x16;
            _elanTPad.reg_11 = 0x8f;
            if (Elantech_write_reg(0x10, &_elanTPad.reg_10) ||
                Elantech_write_reg(0x11, &_elanTPad.reg_11)) {
                ret = kIOReturnError;
            }
            break;
            
        case 2:
            /* Windows driver values */
            _elanTPad.reg_10 = 0x54;
            _elanTPad.reg_11 = 0x88;	/* 0x8a */
            _elanTPad.reg_21 = 0x60;	/* 0x00 */
            if (Elantech_write_reg(0x10, &_elanTPad.reg_10) ||
                Elantech_write_reg(0x11, &_elanTPad.reg_11) ||
                Elantech_write_reg(0x21, &_elanTPad.reg_21)) {
                ret = kIOReturnError;
            }
            break;
            
        case 3:
            /*
             * For Gigabyte notebooks (maybe for some others too),
             * we need to use the register value 0x03 (means not setting bit 3 of r10 register
             * which enables "Real H/W Resolution" in Absolute mode), so this
             * only will result in half the x and y resolution we get with that bit set.
             * Without this, touchpad will not work and resolutions are taken
             * care manually later.
             */
            
            _elanTPad.reg_10 = 0x0b;
            if (Elantech_write_reg(0x10, &_elanTPad.reg_10))
            {
                _elanTPad.v3ElanLegacyMode = true;
                /* Try old register value '0x03' */
                _elanTPad.reg_10 = 0x03;
                if (Elantech_write_reg(0x10, &_elanTPad.reg_10))
                    ret = kIOReturnError;
            }
            break;
            
        case 4:
            _elanTPad.reg_07 = 0x01;
            if (Elantech_write_reg(0x07, &_elanTPad.reg_07))
                ret = kIOReturnError;
            
            goto skip_readback_reg_10; /* v4 has no reg 0x10 to read */
    }
    
    if (ret == 0) {
        /*
         * Read back reg 0x10. For hardware version 1 we must make
         * sure the absolute mode bit is set. For hardware version 2
         * the touchpad is probably initializing and not ready until
         * we read back the value we just wrote.
         */
        do {
            ret = Elantech_read_reg(0x10, &val);
            if (ret == 0)
                break;
            tries--;
            IOLog("STD :: retrying read (%d).\n", tries);
            IOSleep(ETD_READ_BACK_DELAY);
        } while (tries > 0);
        
        if (ret) {
            IOLog("STD :: Failed to read back register 0x10.\n");
        }
        else if (_hw_version == 1 && !(val & ETD_R10_ABSOLUTE_MODE)) {
            IOLog("STD :: touchpad refuses to switch to absolute mode.\n");
            ret = kIOReturnError;
        }
    }
    
skip_readback_reg_10:
    if (ret)
        IOLog("STD :: Failed to initialise registers.\n");
    
    return ret;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Determine hardware version and setup some properties according to it.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn ApplePS2SmartTouchpad::Elantech_set_properties()
{
    /* This represents the version of IC body. */
    int ver = (_elanTPad.fw_version & 0x0f0000) >> 16;
    
    /* Early version of Elan touchpads doesn't obey the rule. */
    if (_elanTPad.fw_version < 0x020030 || _elanTPad.fw_version == 0x020600)
        _hw_version = 1;
    
    else {
        switch (ver) {
            case 2:
            case 4:
                _hw_version = 2;
                break;
            case 5:
                _hw_version = 3;
                break;
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 13:
            case 14:
                _hw_version = 4;
                break;
            default:
                if (_hw_version > 14) {
                    IOLog("STD :: Unknown hardware revision %d detected, assuming version hardware version 4.\n", _hw_version);
                    break;
                }
                return kIOReturnNoDevice;
        }
    }
    
    /* Set physical buttons existence */
    if (_hw_version < 4)
        _button.isClickPad = false;
    
    /* Turn on packet checking by default */
    _elanTPad.paritycheck = 1;
    
    /*
     * This firmware suffers from misreporting coordinates when
     * a touch action starts causing the mouse cursor or scrolled page
     * to jump. Enable a workaround.
     */
    _elanTPad.jumpyCursor = (_elanTPad.fw_version == 0x020022 || _elanTPad.fw_version == 0x020600);
    
    if (_hw_version > 1) {
        if (_elanTPad.fw_version >= 0x020800)
            _elanTPad.reportsPressure = true;
    }
    
    /*
     * The signatures of v3 and v4 packet change depending on the
     * value of this hardware flag.
     */
    _elanTPad.crcEnabled = ((_elanTPad.fw_version & 0x4000) == 0x4000);
    
    return kIOReturnSuccess;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Determine and set the touchpad range values.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn ApplePS2SmartTouchpad::Elantech_set_range()
{
    unsigned char param[3];
    unsigned char traces;
    
    BASIC_DBG_LOG("STD :: Setting the touchpad ranges...\n");
    
    switch (_hw_version) {
        case 1:
            _xmin = ETD_XMIN_V1;
            _ymin = ETD_YMIN_V1;
            _xmax = ETD_XMAX_V1;
            _ymax = ETD_YMAX_V1;
            break;
            
        case 2:
            if (_elanTPad.fw_version == 0x020800 ||
                _elanTPad.fw_version == 0x020b00 ||
                _elanTPad.fw_version == 0x020030) {
                _xmin = ETD_XMIN_V2;
                _ymin = ETD_YMIN_V2;
                _xmax = ETD_XMAX_V2;
                _ymax = ETD_YMAX_V2;
            } else {
                int i;
                int fixed_dpi;
                
                i = (_elanTPad.fw_version > 0x020800 &&
                     _elanTPad.fw_version < 0x020900) ? 1 : 2;
                
                if (sendSynapticsCmd(_device, ETD_FW_ID_QUERY, param))
                    return kIOReturnError;
                
                fixed_dpi = param[1] & 0x10;
                
                if (((_elanTPad.fw_version >> 16) == 0x14) && fixed_dpi)
                {
                    if (sendSynapticsCmd(_device, ETD_SAMPLE_QUERY, param))
                        return kIOReturnError;
                    
                    _xmax = (_elanTPad.elanCapabs[1] - i) * param[1] / 2;
                    _ymax = (_elanTPad.elanCapabs[2] - i) * param[2] / 2;
                }
                else if (_elanTPad.fw_version == 0x040216) {
                    _xmax = 819;
                    _ymax = 405;
                }
                else if (_elanTPad.fw_version == 0x040219 || _elanTPad.fw_version == 0x040215) {
                    _xmax = 900;
                    _ymax = 500;
                }
                else {
                    _xmax = (_elanTPad.elanCapabs[1] - i) * 64;
                    _ymax = (_elanTPad.elanCapabs[2] - i) * 64;
                }
            }
            break;
            
        case 3:
            if (sendElantechCmd(_device, ETD_FW_ID_QUERY, param))
                return kIOReturnError;
            
            _xmin = 0;
            _ymin = 0;
            _xmax = (0x0f & param[0]) << 8 | param[1];
            _ymax = (0xf0 & param[0]) << 4 | param[2];
            break;
            
        case 4:
            if (sendElantechCmd(_device, ETD_FW_ID_QUERY, param))
                return kIOReturnError;
            
            _xmin = 0;
            _ymin = 0;
            _xmax = (0x0f & param[0]) << 8 | param[1];
            _ymax = (0xf0 & param[0]) << 4 | param[2];
            traces = _elanTPad.elanCapabs[1];
            
            if ((traces < 2) || (traces > _xmax))
                return kIOReturnError;
            
            _elanTPad.tracesWidth = _xmax / (traces - 1);
            break;
    }
    
    // Setup touchpad centre
    _xcenter = _xmax/2;
    _ycenter = _ymax/2;
    
    IOLog("STD :: Touchpad ranges are X_min %d Y_min %d X_max %d Y_max %d, CenterX %d, CenterY %d.\n", _xmin, _ymin, _xmax, _ymax, _xcenter, _ycenter);
    
    return  kIOReturnSuccess;
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// ELAN style special command to write a value to a register.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

IOReturn ApplePS2SmartTouchpad::Elantech_write_reg(unsigned char reg, unsigned char *val)
{
    IOReturn ret = kIOReturnSuccess;
    
    if (reg < 0x07 || reg > 0x26)
        return -1;
    
    if (reg > 0x11 && reg < 0x20)
        return -1;
    
    switch (_hw_version) {
        case 1:
            if (sendSlicedPS2Cmd(_device, ETD_REGISTER_WRITE) ||
                sendSlicedPS2Cmd(_device, reg) ||
                sendSlicedPS2Cmd(_device, *val) ||
                sendPS2Cmd(_device, kDP_SetMouseScaling1To1)) {
                ret = kIOReturnError;
            }
            break;
            
        case 2:
            if (sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, ETD_REGISTER_WRITE) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, reg) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, *val) ||
                sendElactechPS2Cmd(_device, kDP_SetMouseScaling1To1)) {
                ret = kIOReturnError;
            }
            break;
            
        case 3:
            if (sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, ETD_REGISTER_READWRITE) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, reg) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, *val) ||
                sendElactechPS2Cmd(_device, kDP_SetMouseScaling1To1)) {
                ret = kIOReturnError;
            }
            break;
            
        case 4:
            if (sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, ETD_REGISTER_READWRITE) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, reg) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, ETD_REGISTER_READWRITE) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, *val) ||
                sendElactechPS2Cmd(_device, kDP_SetMouseScaling1To1)) {
                ret = kIOReturnError;
            }
            break;
    }
    
    if (ret)
        IOLog("STD :: Failed to write register 0x%02x with value 0x%02x.\n", reg, *val);
    
    return ret;
    
}

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// ELAN style special command to read a value from a register.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

int ApplePS2SmartTouchpad::Elantech_read_reg(unsigned char reg, unsigned char *val)
{
    unsigned char param[3] = {NULL};
    IOReturn ret = kIOReturnSuccess;
    
    if (reg < 0x07 || reg > 0x26)
        return kIOReturnError;
    
    if (reg > 0x11 && reg < 0x20)
        return kIOReturnError;
    
    switch (_hw_version) {
        case 1:
            if (sendSlicedPS2Cmd(_device, ETD_REGISTER_READ) ||
                sendSlicedPS2Cmd(_device, reg) ||
                getMouseInformation(_device, param)) {
                ret = kIOReturnError;
            }
            break;
            
        case 2:
            if (sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, ETD_REGISTER_READ) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, reg) ||
                getMouseInformation(_device, param)) {
                ret = kIOReturnError;
            }
            break;
            
        case 3 ... 4:
            if (sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, ETD_REGISTER_READWRITE) ||
                sendElactechPS2Cmd(_device, ETD_PS2_CUSTOM_COMMAND) ||
                sendElactechPS2Cmd(_device, reg) ||
                getMouseInformation(_device, param)) {
                ret = kIOReturnError;;
            }
            break;
    }
    
    if (ret)
        IOLog("STD :: Failed to read register 0x%02x.\n", reg);
    else if (_hw_version != 4)
        *val = param[0];
    else
        *val = param[1];
    
    return ret;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

bool ApplePS2SmartTouchpad::Elantech_setup_touchpad()
{
    
    /*
     * Detecting Firmware Version:
     * Query touchpad's firmware version and see if it reports known
     * value to avoid mis-detection. Logitech mice are known to respond
     * to Elantech magic knock and there might be more.
     */
    
    unsigned char param[3];
    
    if (sendSynapticsCmd(_device, ETD_FW_VERSION_QUERY, param)) {
        IOLog("STD :: Failed to query firmware version.\n");
        return false;
    }
    
    BASIC_DBG_LOG("STD :: ELAN version query result 0x%02x, 0x%02x, 0x%02x.\n", param[0], param[1], param[2]);
    
    //
    // check singatures for ELAN touchpad
    //
    
    if (!(Elantech_is_signatures_valid(param))) {
        IOLog("STD :: Probably not a real ELAN touchpad, aborting.\n");
        return false;
    }
    
    _elanTPad.fw_version = (param[0] << 16) | (param[1] << 8) | param[2];
    
    //
    // Set the properties of ELAN hardware
    //
    
    if (Elantech_set_properties()) {
        IOLog("STD :: Unknown hardware version, aborting.\n");
        return false;
    }
    
    IOLog("STD :: ELAN touchpad found, Hardware version %d (Firmware version 0x%x)\n",
          _hw_version, _elanTPad.fw_version);
    
    //
    // Detect clickPad (Experimental, based on linux and synaptics sources)
    //
    
    if (_hw_version > 3 && ELAN_CAP_CLICKPAD(_elanTPad.fw_version))
        _button.isClickPad = true;
    else
        _button.isClickPad = false;
    
    IOLog("STD :: ClickPad? %s\n", _button.isClickPad?"Yes":"No");
    
    //
    // This driver works only for the hardware version > 2 and it doesn't work with hardware v1.
    // Hardware v1 will be supported when beta testers are available.
    //
    
    if (_hw_version < 2)
    {
        IOLog("STD :: ELAN Hardware versions %d is not supported by this driver. Contact me at mg-dinesh@live.com for support.\n", _hw_version);
        return false;
    }
    
    //
    // Query the touchpad capablities
    //
    
    switch (_hw_version) {
        case 1:
        case 2:
            if (sendSynapticsCmd(_device, ETD_CAPABILITIES_QUERY, _elanTPad.elanCapabs)) {
                IOLog("STD :: Failed to query capabilities.\n");
                return false;
            }
            break;
            
        default:
            if (sendElantechCmd(_device, ETD_CAPABILITIES_QUERY, _elanTPad.elanCapabs)) {
                IOLog("STD :: Failed to query capabilities.\n");
                return false;
            }
            break;
    }
    
    IOLog("STD :: Capabilities query result 0x%02x, 0x%02x, 0x%02x.\n", _elanTPad.elanCapabs[0], _elanTPad.elanCapabs[1], _elanTPad.elanCapabs[2]);
    
    //
    // Put the touchpad into absolute mode
    //
    
    if (Elantech_set_absolute_mode()) {
        IOLog("STD :: Failed to put the touchpad into absolute mode.\n");
        return false;
    }
    else {
        BASIC_DBG_LOG("STD :: Successfully placed the touchpad into Absolute mode.\n");
    }
    
    //
    // Setup touchpad ranges
    //
    
    if (Elantech_set_range()) {
        IOLog("STD :: Failed to query touchpad range.\n");
        return false;
    }
    
    //
    // The MSB indicates the presence of the trackpoint
    //
    
    if ((_elanTPad.elanCapabs[0] & 0x80) == 0x80) {
        _elanTPad.supportsTrackPoint = true;
        BASIC_DBG_LOG("STD :: Trackpoint detected.\n");
    }
    else
        _elanTPad.supportsTrackPoint = false;

    return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method to process the mouse packet.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::processMousePacket(unsigned char *packet)
{
    //
    // Process the three byte mouse packet that was retreived from the mouse.
    // The format of the bytes is as follows:
    //
    // 7  6  5  4  3  2  1  0
    // YO XO YS XS 1  M  R  L   BYTE 0
    // X7 X6 X5 X4 X3 X3 X1 X0  BYTE 1
    // Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0  BYTE 2
    //
    
    UInt8       buttons = 0;
    SInt16      dx;
    SInt16      dy;
    
    if ( (packet[0] & 0x1) ) // left button   (bit 0 in packet)
    {
        if(_ps2Mouse.dragPackets > 5 && _drag.isDragging && _ps2Mouse.lastButton == 0)
        {
            _drag.isDragging = false;
            _ps2Mouse.dragPackets = 0;
        }
        else if (_ps2Mouse.dragPackets < 10)
            _ps2Mouse.dragPackets++;
        
        // IOLog("STD :: left button with Last %d, Packets %d, Drag %d\n", _ps2Mouse.lastButton, _ps2Mouse.dragPackets, _drag.isDragging);
        
        buttons |= 0x1;
        
        //
        //  Dispatch context menu on Control + tap
        //
        
        if(_ps2Mouse.isCtrlScrollSwipe)
        {
            
            dispatchRelativePointerPosition(0, 0, 0x2);
            IODelay(100);
            dispatchRelativePointerPosition(0, 0, 0);
            _button.rightClick = true;
        }
        else if(_button.rightClick)
            _button.rightClick = false;
        
    }
    
    if ( (packet[0] & 0x2) ) buttons |= 0x2;  // right button  (bit 1 in packet)
    if ( (packet[0] & 0x4) ) buttons |= 0x4;  // middle button (bit 2 in packet)
    
    dx = ((packet[0] & 0x10) ? 0xffffff00 : 0 ) | packet[1];
    dy = -(((packet[0] & 0x20) ? 0xffffff00 : 0 ) | packet[2]);
    
    // IOLog("STD :: dx %d dy %d Button %x Last %x, _ps2Mouse.dragPackets %d Dragging %d\n", dx, dy, buttons, _ps2Mouse.lastButton, _ps2Mouse.dragPackets, _drag.isDragging);
    
    _ps2Mouse.lastButton = buttons;
    
    if(buttons == 0 && !_drag.isDragging)
        _ps2Mouse.dragPackets = 0;
    
    //
    // Control drag for draglock
    //
    
    if (_isDragEnabled && _isDragLockEnabled && _ps2Mouse.dragPackets > 5)
    {
        if (buttons & 0x1)
        {
            _drag.isDragging = true;
        }
        
        //  Set button to drag in DragLock
        if (_drag.isDragging)
            buttons |= 0x1;
    }
    
    //
    // Disabling dragging
    //
    
    if(!_isDragEnabled && _ps2Mouse.dragPackets > 1)
        buttons = 0x0;
    
    //
    //  Get unsigned delta X and Y for conditional
    //
    
    int tmpDX =0, tmpDY = 0;
    
    if(dx < 0)
        tmpDX = 0 - dx;
    else
        tmpDX = dx;
    
    if(dy < 0)
        tmpDY = 0 - dy;
    else
        tmpDY = dy;
    
    //
    //  Horizontal scroll level
    //
    
    int s_yDelta, s_xDelta;
    
    if (_isHScrollEnabled) {
        
        switch (tmpDX) {
            case 1 ... 5:
                s_xDelta = -2;
                break;
                
            case 6 ... 15:
                s_xDelta = -5;
                break;
                
            case 16 ... 30:
                s_xDelta = -10;
                break;
                
            default:
                s_xDelta = -15;
                break;
        }
        
        // Changing scroll direction if delta -ve
        if(dx < 0)
            s_xDelta = 0 - s_xDelta;
        
    }
    else
        s_xDelta = 0;
    
    //
    //  Vertical scroll level
    //
    
    if (_isVScrollEnabled) {
        
        
        switch (tmpDY) {
            case 1 ... 5:
                s_yDelta = -2;
                break;
                
            case 6 ... 15:
                s_yDelta = -5;
                break;
                
            case 16 ... 30:
                s_yDelta = -10;
                break;
                
            default:
                s_yDelta = -15;
                break;
        }
        
        // Changing scroll direction if delta -ve
        if(dy < 0)
            s_yDelta = 0 - s_yDelta;
    }
    else
        s_yDelta = 0;
    
    
    if(!_ps2Mouse.isSwipeDone)
        _ps2Mouse.lastSwipeTime = absoluteToUINT64(_now);
    
    clock_get_uptime(TIME_NOW);
    
    //
    //  Setting swipeDone to false after timout for next swipe to work without releasing control key
    //
    
    if((absoluteToUINT64(_now) - _ps2Mouse.lastSwipeTime) > 250000000)
        _ps2Mouse.isSwipeDone = false;
    
    //
    // Process Swiping
    //
    
    if(_ps2Mouse.isCtrlScrollSwipe
       && _ps2Mouse.enableLRSwipe
       && !_button.rightClick
       && !_ps2Mouse.isSwipeDone
       && tmpDX > tmpDY && tmpDX > 15)
    {
        if(dx < 0) {
            _device->dispatchPS2Notification(kPS2C_Gesture_Backward);
        }
        
        else if(dx > 0) {
            _device->dispatchPS2Notification(kPS2C_Gesture_Forward);
        }
        
        clock_get_uptime(TIME_NOW);
        _ps2Mouse.lastSwipeTime = absoluteToUINT64(_now);
        _ps2Mouse.isSwipeDone = true;
    }
    
    //
    // Dispatch Scrolling
    //
    
    else if(_ps2Mouse.isCtrlScrollSwipe && !_button.rightClick)
    {
        // Release button
        if (buttons) {
            buttons = 0;
            dispatchRelativePointerPosition(0, 0, buttons);
        }
        
        if(tmpDY > tmpDX && dx < 8 && dx > -8)
            dispatchScrollDifference(((s_yDelta * _scrollFactor)), 0, 0);
        
        else if(tmpDX > tmpDY && dy < 6 && dy > -6 && !_ps2Mouse.enableLRSwipe)
            dispatchScrollDifference(0, (s_xDelta * _scrollFactor), 0);
    }
    
    //
    //  Dispatch pointer movement
    //
    
    else if(!_ps2Mouse.isCtrlScrollSwipe || _button.rightClick)
    {
        if(_button.rightClick)
            buttons = 0x2;
        
        dispatchRelativePointerPosition(dx, dy, buttons);
    }
    
    _lastDataTime = absoluteToUINT64(_now);
    
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Interpret complete data packet and process absolute mode input events
// for Elan hardware version 1. (4 byte packet)
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_absolute_events_ELANv1(unsigned char *packet)
{
    if (_elanTPad.fw_version < 0x020000) {
        /*
         * byte 0:  D   U  p1  p2   1  p3   R   L
         * byte 1:  f   0  th  tw  x9  x8  y9  y8
         */
        _cFingers = ((packet[1] & 0x80) >> 7) +
        ((packet[1] & 0x30) >> 4);
    }
    else {
        /*
         * byte 0: n1  n0  p2  p1   1  p3   R   L
         * byte 1:  0   0   0   0  x9  x8  y9  y8
         */
        _cFingers = (packet[0] & 0xc0) >> 6;
    }
    
    if (_elanTPad.jumpyCursor) {
        if (_cFingers != 1) {
            _elanTPad.singleFingerReports = 0;
        }
        else if (_elanTPad.singleFingerReports < 2) {
            /* Discard first 2 reports of one finger, bogus */
            _elanTPad.singleFingerReports++;
            IOLog("STD :: Discarding packet\n");
            return;
        }
    }
    
    // Process buttons
    if (!_button.disableClicks) {
        Process_Physical_Buttons(packet);
    }
    
    /*
     * byte 2: x7  x6  x5  x4  x3  x2  x1  x0
     * byte 3: y7  y6  y5  y4  y3  y2  y1  y0
     */
    if (_cFingers) {
        _fing[0].x = ((packet[1] & 0x0c) << 6) | packet[2];
        _fing[0].y = _ymax - (((packet[1] & 0x03) << 8) | packet[3]);
    }
    
    // Did not finish this because of lack of hardware to debug
    switch (_cFingers) {
        case 1:
            
            break;
            
        case 2:
            
            break;
            
        case 3:
            
            break;
            
        default:
            break;
    }
    
    if (_elanTPad.fw_version < 0x020000 && (_elanTPad.elanCapabs[0] & ETD_CAP_HAS_ROCKER))
    {
        /* rocker up */
        //input_report_key(dev, BTN_FORWARD, packet[0] & 0x40);
        /* rocker down */
        //input_report_key(dev, BTN_BACK, packet[0] & 0x80);
    }
    
    _fing[0].last_x = _fing[0].x;
    _fing[0].last_y = _fing[0].y;
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes the checks for Elan hardware version 2 and 3..
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_ELANv2v3_Checks(UInt8 *packet)
{
    /*
     * Get pressure, width and fingers count from the packet
     */
    
    _fing[0].pressure = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
    _fing[0].width = ((packet[0] & 0x30) >> 2) | ((packet[3] & 0x30) >> 4);
    
    /*
     * Get fingers count on the touchpad
     * byte 0: n1  n0   .   .   .   .   R   L
     *
     * And reset data on finger change.
     */
    
    if (_cFingers != ((packet[0] & 0xc0) >> 6))
    {
        // Set last fingers count
        _lastFingers = _cFingers;
        
        _cFingers = (packet[0] & 0xc0) >> 6;
        
        if (_cFingers > 0)
            Process_fingers_change();
    }
    else {
        _cFingers = (packet[0] & 0xc0) >> 6;
    }
    
    clock_get_uptime(TIME_NOW);
    
    //
    // Calculating time taken by the gesture
    //
    
    if (_packetsCount > 0)
        _gestureTime += (*(uint64_t*)&_now) - _lastDataTime;
    
    _lastDataTime = *(uint64_t*)&_now;
    
    //
    // Process buttons
    //
    
    if (!_button.disableClicks)
    {
        if (_cFingers == 0)
            _packetsCount++;
        
        Process_Physical_Buttons(packet);
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Interpret complete data packet and process absolute mode input events
// for Elan hardware version 2 and 3.
// (12 byte packet for multi fingers and 6 byte packet for single finger)
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_absolute_events_ELANv2v3(UInt8 packet_type, UInt8 *packet)
{
    //
    // Process checks
    //
    
    Process_ELANv2v3_Checks(packet);
    
    //
    // Palm detection
    //
    
    if ((_kbEvent.showPressWidth) ||
        
        (_palm.detected
         && ((_fing[0].pressure == 0 && _fing[0].width > _palm.minWidth) || _fing[0].pressure != 0)) ||

        (_palm.detectionEnabled && _cFingers == 1
         && _fing[0].width > _palm.minWidth  && (_fing[0].pressure == 0 || _fing[0].pressure > _palm.minPressure)))
    {
        //
        // Show Fingers, pressure and width of the touch
        //
        
        if (_kbEvent.showPressWidth) {
            IOLog("STD :: Touch Pressure %d, Width %d, Finger(s) count %d\n",
                  _fing[0].pressure, _fing[0].width, _cFingers);
        }
        else {
            if (!_palm.detected) {
                IOLog("STD :: Palm found for the values Pressure %d and Width %d (Detected Finger(s) %d, Pressure %d, Width %d)\n", _palm.minPressure, _palm.minWidth, _cFingers, _fing[0].pressure, _fing[0].width);
            }
            _palm.detected = true;
            _touchmode = MODE_PALM;
            return;
        }
    }
    
    //
    //
    //  In every packet stream at the end packet
    //  first byte packet[0] = 0x04/ 0x14/ 0x24 is received in v2 hw,
    //  pressure 0 is receievd for v3 hw.
    //  I'm using this packet for processing some gestures thats needs
    //  attention at the end.
    //
    
    switch (_hw_version) {
        case 2:
            if ((packet[0] == 0x04 || packet[0] == 0x14 || packet[0] == 0x24)
                && _cFingers == 0 && _button.data == 0)
            {
                Process_touch_end();
                return;
            }
            break;
            
        case 3:
            
            //
            // Process trackpoint event
            //
            
            if (_cFingers == 0 && _lastFingers == 0 && _fing[0].pressure > 0 && !_button.disbleTrackPoint
                && (packet_type == PACKET_TRACKPOINT || packet_type == PACKET_HEAD))
            {
                Process_trackpoint_events_ELAN(packet_type , packet);
                return;
            }
            
            if (_fing[0].pressure == 0 && _cFingers == 0 && _button.data == 0) // end of touch
            {
                Process_touch_end();
                return;
            }
            break;
    }
    
    //
    // Stop processing on accidental input
    //
    
    if (_kbEvent.accidentalInput && _kbEvent.tapEnableTPad) {
        
        if (_gestureTime < _tap.clickTimeout)
            _touchmode = MODE_1_FING_TAP;
        else
            _touchmode = MODE_IDLE;
        
        return;
    }
    
    int dx1 = 0, dx2 = 0, dy1 = 0, dy2 = 0;
    
    switch (_cFingers) {
        case 3:
            /*
             * Same as one finger, except report of more than 3 fingers:
             * byte 3:  n4  .   w1  w0   .   .   .   .
             */
            if (_hw_version == 2 && (packet[3] & 0x80)) {
                _cFingers = 4;
                DEBUG_LOG("STD :: Four fingers detected..\n");
            }
            /* pass through... */
            
        case 1:
            /*
             * byte 1:  .   .   .   .  x11 x10 x9  x8
             * byte 2: x7  x6  x5  x4  x4  x2  x1  x0
             */
            
            _fing[0].x = ((packet[1] & 0x0f) << 8) | packet[2];
            
            /*
             * byte 4:  .   .   .   .  y11 y10 y9  y8
             * byte 5: y7  y6  y5  y4  y3  y2  y1  y0
             */
            
            _fing[0].y = _ymax - (((packet[4] & 0x0f) << 8) | packet[5]);
            
            //
            // Initialize x and y coordinates
            //
            
            if (_packetsCount <= 1) {
                
                _fing[0].tdx = _fing[0].tdy = 0;
                _fing[0].start_x = _fing[0].x;
                _fing[0].start_y = _fing[0].y;
                _fing[0].last_x = _fing[0].x;
                _fing[0].last_y = _fing[0].y;
            }
            // Calculate delta
            else  {
                dx1 = _fing[0].x - _fing[0].last_x;
                dy1 = _fing[0].y - _fing[0].last_y;
                _fing[0].tdx += dx1;
                _fing[0].tdy += dy1;
            }
            
            //
            // Process the touch
            //
            
            if (_cFingers == 1)
                Process_singlefinger_touch(_fing[0].x, _fing[0].y, 0);
            
            else if (_cFingers == 3 && !_drag.isDragging)
                Process_multiFingers_touch(dx1, dy1, 0);
            
            // Set last x and y coordinates
            _fing[0].last_x = _fing[0].x;
            _fing[0].last_y = _fing[0].y;
            break;
            
        case 2:
            
            //
            //  ELAN v2 hardware ?
            //
            
            if (_hw_version == 2) {
                /*
                 * The coordinate of each finger is reported separately
                 * with a lower resolution for two finger touches:
                 * byte 0:  .   .  ay8 ax8  .   .   .   .
                 * byte 1: ax7 ax6 ax5 ax4 ax3 ax2 ax1 ax0
                 */
                
                _fing[0].x = (((packet[0] & 0x10) << 4) | packet[1]) << 2;
                
                /* byte 2: ay7 ay6 ay5 ay4 ay3 ay2 ay1 ay0 */
                _fing[0].y = _ymax - ((((packet[0] & 0x20) << 3) | packet[2]) << 2);
                
                /*
                 * byte 3:  .   .  by8 bx8  .   .   .   .
                 * byte 4: bx7 bx6 bx5 bx4 bx3 bx2 bx1 bx0
                 */
                
                _fing[1].x = (((packet[3] & 0x10) << 4) | packet[4]) << 2;
                
                /* byte 5: by7 by8 by5 by4 by3 by2 by1 by0 */
                _fing[1].y = _ymax - ((((packet[3] & 0x20) << 3) | packet[5]) << 2);
                
                /* Unknown so just report sensible values */
                _fing[0].pressure = _fing[1].pressure = 127;
                _fing[0].width = _fing[1].width = 7;
            }
            
            //
            //  ELAN v3 hardware
            //
            
            else {
                if (packet_type == PACKET_HEAD) {
                    /*
                     * byte 1:   .    .    .    .  ax11 ax10 ax9  ax8
                     * byte 2: ax7  ax6  ax5  ax4  ax3  ax2  ax1  ax0
                     */
                    _fing[0].x = ((packet[1] & 0x0f) << 8) | packet[2];
                    /*
                     * byte 4:   .    .    .    .  ay11 ay10 ay9  ay8
                     * byte 5: ay7  ay6  ay5  ay4  ay3  ay2  ay1  ay0
                     */
                    _fing[0].y = _ymax - (((packet[4] & 0x0f) << 8) | packet[5]);
                    
                    // wait for next packet
                    break;
                }
                
                /* packet_type == PACKET_TAIL */
                
                _fing[1].x = ((packet[1] & 0x0f) << 8) | packet[2];
                _fing[1].y = _ymax - (((packet[4] & 0x0f) << 8) | packet[5]);
            }
            
            
            //
            // Initialize x, y coordinates and finger ids
            //
            
            if (_packetsCount <= 1)
            {
                //
                // Set primary and secndary finger ids
                //
                
                _pid = 0; _sid = 1;
                
                //
                // Set start and last coordinates for both fingers
                //
                
                if ((_fing[_pid].start_x == 0 && _fing[_pid].start_y == 0) ||
                     !(_swipe.direction < 3 || (_lastFingers == 1 && _cFingers == 2)))
                {
                    _fing[_pid].start_x = _fing[_pid].x;
                    _fing[_pid].start_y = _fing[_pid].y;
                    
                    _fing[_pid].last_x = _fing[_pid].x;
                    _fing[_pid].last_y = _fing[_pid].y;
                }
                
                if ((_fing[_sid].start_x == 0 && _fing[_sid].start_y == 0) ||
                     !(_swipe.direction < 3 || (_lastFingers == 1 && _cFingers == 2)))
                {
                    _fing[_sid].start_x = _fing[_sid].x;
                    _fing[_sid].start_y = _fing[_sid].y;
                    
                    _fing[_sid].last_x = _fing[_sid].x;
                    _fing[_sid].last_y = _fing[_sid].y;
                }
                
                //
                // Edge swipe ?
                //
                
                if (!_swipe.isEdgeGesture)
                    Is2FingEdgeSwipe();
                
                if (_swipe.isEdgeGesture)
                {
                    //
                    // Calculate the delta from last x to find
                    // which finger coordinates are initialized before.
                    //
                    
                    if (_lastFingers == 1) {
                        dx1 = _fing[0].x - _fing[0].last_x; if (dx1 < 0) dx1 = 0 - dx1;
                        dx2 = _fing[1].x - _fing[0].last_x; if (dx2 < 0) dx2 = 0 - dx2;
                        
                        //
                        // Right finger was placed first?
                        // If yes, Swap finger 1(right) to finger 2(left).
                        //
                        
                        if (dx2 < dx1) {
                            // Swapping last fing[0] values to fing[1]
                            _fing[1].start_x = _fing[0].start_x;
                            _fing[1].start_y = _fing[0].start_y;
                            _fing[1].last_x = _fing[0].last_x;
                            _fing[1].last_y = _fing[0].last_y;
                            
                            // Set current finger values
                            _fing[0].start_x = _fing[0].x;
                            _fing[0].start_y = _fing[0].y;
                            _fing[0].last_x = _fing[0].x;
                            _fing[0].last_y = _fing[0].y;
                        }
                    }
                }
                else {
                    
                    //
                    // Reset the coordinates
                    //
                    
                    _fing[_pid].start_x = _fing[_pid].x;
                    _fing[_pid].start_y = _fing[_pid].y;
                    _fing[_sid].start_x = _fing[_sid].x;
                    _fing[_sid].start_y = _fing[_sid].y;
                    
                    _fing[_pid].last_x = _fing[_pid].x;
                    _fing[_pid].last_y = _fing[_pid].y;
                    _fing[_sid].last_x = _fing[_sid].x;
                    _fing[_sid].last_y = _fing[_sid].y;
                    
                }
            }
            
            //
            // Calculate total delta for two fingers
            //
            
            else  {
                dx1 = _fing[0].x - _fing[0].last_x;
                dy1 = _fing[0].y - _fing[0].last_y;
                _fing[0].tdx += dx1;
                _fing[0].tdy += dy1;
                
                dx2 = _fing[1].x - _fing[1].last_x;
                dy2 = _fing[1].y - _fing[1].last_y;
                _fing[1].tdx += dx2;
                _fing[1].tdy += dy2;
            }
            
            //
            // Process the event
            //
            
            Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
            
            // Set last x and y coordinates
            _fing[_pid].last_x = _fing[_pid].x;
            _fing[_pid].last_y = _fing[_pid].y;
            _fing[_sid].last_x = _fing[_sid].x;
            _fing[_sid].last_y = _fing[_sid].y;
            break;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Interpret complete data packet and process absolute mode input events
// for Elan hardware version 4 and Focal tech.
// (12 byte packet for multi fingers and 6 byte packet for single finger)
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_absolute_events_ELANv4AndFocalTech(UInt8 packet_type, UInt8 *packet)
{
    int delta_x[STD_MAX_FINGERS] = {0};
    int delta_y[STD_MAX_FINGERS] = {0};
    int weight = 0;
    int fid = 0;
    
    //
    // Palm detection
    //
    
    if ((_palm.detectionEnabled &&
          
          (((_cFingers == 2 && _lastFingers <= 2 && _fing[0].width > _palm.minWidth) ||
            (_cFingers == 1 && _lastFingers <= 1 && _fing[0].pressure > _palm.minPressure))
           ||
           _palm.detected)) || _kbEvent.showPressWidth)
    {
        /*
         * Get the finger pressure and width from the packet for palm if its 1f
         * as multi fingers don't report pressure and width in motion packets
         * and they report only for the first head packet.
         */
        
        if (_cFingers == 1) {
            _fing[0].pressure = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
            _fing[0].width = (packet[0] & 0xf0) >> 4;
        }
        
        //
        // Show Fingers, pressure and width of the touch
        //
        
        if (_kbEvent.showPressWidth) {
            IOLog("STD :: Touch Pressure %d, Width %d, Finger(s) on touchpad %d\n",
                  _fing[0].pressure, _fing[0].width, _cFingers);
        }
        else {
            if (!_palm.detected) {
                IOLog("STD :: Palm detected for the requested Pressure %d and Width %d (Detected Finger(s) %d, Pressure %d, Width %d)\n", _palm.minPressure, _palm.minWidth, _cFingers, _fing[0].pressure, _fing[0].width);
            }
            _palm.detected = true;
            _touchmode = MODE_PALM;    //     _touchmode = MODE_IDLE;
            
            if (packet_type != PACKET_STATUS)
                return;
        }
    }
    
    //
    // Stop processing on accidental input
    //
    
    if (_kbEvent.accidentalInput && _kbEvent.tapEnableTPad
        && (packet_type != PACKET_STATUS))
    {
        if (_gestureTime < _tap.clickTimeout)
            _touchmode = MODE_1_FING_TAP;
        else
            _touchmode = MODE_IDLE;
        
        return;
    }
    
    clock_get_uptime(TIME_NOW);
    
    //
    // Calculate time taken for the gesture
    //
    
    if (_packetsCount > 0)
        _gestureTime += (*(uint64_t*)&_now) - _lastDataTime;
    
    _lastDataTime = (*(uint64_t*)&_now);
    
    switch (packet_type) {
        case PACKET_STATUS:
            
            // Set last fingers count
            _lastFingers = _cFingers;
            
            /* Get fingers bit mask */
            _cFingers = packet[1] & 0x1f;
            
            /*for (int i = 0; i <= STD_MAX_FINGERS; i++)
            {
                IOLog("STD :: Checking finger(s) mask bit %d\n", i);
                if ((_cFingers & (1 << i)) == 0)
                {
                    _cFingers = i;
                    switch (i) {   
                        // Set bit(s) idenfiy fingers
                        case 0x01: //1F bit 0
                        case 0x03: //2F bit 0, 1
                        case 0x07: //3F bit 0, 1, 2
                        case 0x0f: //4F bit 0, 1, 2, 3
                        case 0x1f: //5F bit 0, 1, 2, 3, 4
                            break;
                            
                        default: // Clear bit (+1) idenfiy fingers
                            _cFingers++;
                            break;
                    }
                    break;
                }
            }*/
            
            /*
             * Special case:
             * Special value is received between x - (x -> x) transitions in ELAN.
             *
             * 2 -> 1 = 0x02
             * 3 -> 2 = 0x05 (0x0d?)
             * 4 -> 3 = 0x0b
             * 5 -> 4 = 0x17 (4?3?2? = 0x11
             */
            
            switch (_cFingers) {
                case 0x0:
                    _cFingers = 0;
                    
                    //
                    // Process buttons
                    //
                    
                    if (_button.isClickPad) {
                        Process_ClickPad_Virtual_Buttons(packet, packet_type, 0, 1);
                    }
                    else if (!_button.disableClicks)
                    {
                        _packetsCount++;
                        Process_Physical_Buttons(packet);
                    }
                    
                    /*
                     * An end status packet with pressure zero is received at the end
                     * of every packet stream in Elan v4 hardware and focaltech
                     * I'm using this for processing some gestures.
                     */
                    
                    if (_lastFingers > 0 && _touchmode != MODE_BUTTONS && !_kbEvent.showPressWidth)
                    {
                        Process_touch_end();
                        return;
                    }
                    break;
                   
                case 0x02:
                    // 1F Tap in 2F Move
                    if (_lastFingers == 2 && _gestureTime < _tap.clickTimeout && _button.isFingOnBtnArea)
                    {
                        dispatchRelativePointerPosition(0, 0, 0x1);
                        dispatchRelativePointerPosition(0, 0, 0x0);
                        _button.data = _button.state = 0;
                        DEBUG_LOG("STD :: Tap performd in 2F move.\n");
                    }
                    DEBUG_LOG("STD :: 2F Special case that happens in 1-2 or 2-1 gestues.\n");
                
                case 0x01:
                    _cFingers = 1;
                    break;
                    
                case 0x03:
                case 0x05:
                case 0x0d:
                    _cFingers = 2;
                    break;
                    
                case 0x07:
                case 0x0b:
                case 0x11:
                    _cFingers = 3;
                    break;
                    
                case 0x0f:
                case 0x17:
                    _cFingers = 4;
                    break;
                    
                case 0x1f:
                    _cFingers = 5;
                    break;
                    
                default:
                    IOLog("STD :: Invaid status packet for finger(s) count with bit mask 0x%x\n", _cFingers);
                    return;
            }
            
            Process_fingers_change();
            break;
            
        case PACKET_ABSOLUTE:
            
            /*
             * We receive 'x' number of absolute packets for 'x' fingers here.
             */
            
            if (_touchpadVendor == STD_FOCAL_TECH) {
                
                /*
                 * Get the primary finger id from the packet
                 */
                
                fid = (packet[1] >> 4) - 1;
                
                if (fid < 0)
                    break;
                
                /*
                 * Unknown report sensible value.
                 */
                
                _fing[fid ].pressure = 128;
                
                /*
                 * Note from linux:
                 * Focaltech touchpads report finger width in packet[5] of absolute packet.
                 * Range for width in raw format is 0x10 - 0x70. Second half-byte is always 0.
                 * 0xff is reported, when a large contact area is detected.
                 
                 * The firmware reports a value between 0 - 15.
                 */
                
                _fing[fid ].width = packet[5] >> 4;;
                
                /*
                 *
                 * byte 1:  .   .   .   .  x11 x10 x9  x8
                 * byte 2: x7  x6  x5  x4  x4  x2  x1  x0
                 */
                
                _fing[fid].x  = ((packet[1] & 0x0f) << 8) | packet[2];
                
                /*
                 * byte 3:  .   .   .   .  y11 y10 y9  y8
                 * byte 4: y7  y6  y5  y4  y3  y2  y1  y0
                 */
                
                _fing[fid].y = _ymax - (((packet[3] & 0x0f) << 8) | packet[4]);
            }
            else {
                
                /*
                 * Get the primary finger id from the packet
                 */
                
                fid = ((packet[3] & 0xe0) >> 5) - 1;
                
                if (fid < 0)
                    break;
                
                /*
                 * Get the finger pressure from the packet
                 */
                
                _fing[fid].pressure = (packet[1] & 0xf0) | ((packet[4] & 0xf0) >> 4);
                
                /*
                 * The firmware reports how many trace lines the finger spans.
                 */
                
                _fing[fid].width = (packet[0] & 0xf0) >> 4;
                
                /*
                 *
                 * byte 1:  .   .   .   .  x11 x10 x9  x8
                 * byte 2: x7  x6  x5  x4  x4  x2  x1  x0
                 */
                
                _fing[fid].x  = ((packet[1] & 0x0f) << 8) | packet[2];
                
                /*
                 * byte 4:  .   .   .   .  y11 y10 y9  y8
                 * byte 5: y7  y6  y5  y4  y3  y2  y1  y0
                 */
                
                _fing[fid].y = _ymax - (((packet[4] & 0x0f) << 8) | packet[5]);
            }
            
            DEBUG_MUL_FING_LOG("STD :: %dF Head Packet X %d Y %d ID %d, PID %d SID %d SX %d, SY %d, Last Finger %d, Pressure %d, Width %d, Button Data %d State %d, Button Area %s, PacketsCount %d, TMode %d, P %d\n", _cFingers, _fing[fid].x, _fing[fid].y, fid, _pid, _sid, _fing[fid].start_x, _fing[fid].start_y, _lastFingers, _fing[fid].pressure, _fing[fid].width, _button.data, _button.state, _button.isFingOnBtnArea?"YES":"NO", _packetsCount, _touchmode, _button.enablePointingOnBtnArea);
            
            //
            // Initialize x and y coordinates
            //
            
            if (_packetsCount == 0
                || (_fing[fid].start_x == 0 && _fing[fid].start_y == 0))
            {
                //
                // Set primary and secndary finger ids
                //
                
                if (_pid < 0)
                    _pid = fid;
                
                else if (_sid < 0)
                    _sid = fid;
                
                //
                // Initialize coordinates
                //
                
                if ((_fing[fid].start_x == 0 && _fing[fid].start_y == 0) ||
                    !(_swipe.direction > 0 || (_lastFingers == 1 && _cFingers == 2)))
                {
                    _fing[fid].tdx = _fing[fid].tdy = 0;
                    _fing[fid].start_x = _fing[fid].x;
                    _fing[fid].start_y = _fing[fid].y;
                    _fing[fid].last_x = _fing[fid].x;
                    _fing[fid].last_y = _fing[fid].y;
                }
            }
            
            switch (_cFingers) {
                    
                case 1:
                    
                    //
                    // Process physical buttons found in some touch pads
                    //
                    
                    if (!_button.isClickPad && !_button.disableClicks) {
                        Process_Physical_Buttons(packet);
                    }
                    
                    //
                    // Process virtual buttons,
                    // but skip first packet beacuse sometimes a Head packet
                    // for multi fingers are sent here which we should not process.
                    //
                    
                    else if (_packetsCount > 0) {
                        
                        //
                        // Finger is on button section?
                        //
                        
                        if (_fing[fid].y > (_ymax - _button.clickPadVBtnArea))
                        {
                            _button.isFingOnBtnArea  = true;
                            DEBUG_LOG("STD :: A finger is on button area at the location X %d.\n", _fing[fid].x);
                        }
                        else {
                            _button.isFingOnBtnArea  = false;
                        }
                        
                        Process_ClickPad_Virtual_Buttons(packet, packet_type, fid, 1);
                        
                        // Trigger click drag
                        if (_button.triggered && !_button.isFingOnBtnArea)
                        {
                            DEBUG_LOG("STD :: Click drag triggered.\n");
                            _drag.isHoldDrag = _drag.isDragging = true;
                            _fingPress.holdTime = 2000000001;
                        }
                    }
                    
                    //
                    // Process pointer movement
                    //
                    
                    Process_singlefinger_touch(_fing[fid].x, _fing[fid].y, fid);
                    break;
                    
                case 2:
                    
                    //
                    // Edge swipe ?
                    //
                    
                    if (!_swipe.isEdgeGesture)
                        Is2FingEdgeSwipe();
                    
                    if (!_swipe.isEdgeGesture) {
                        
                        //
                        // Reset the coordinates
                        //

                        DEBUG_SCROLL_SWIPES_LOG("STD :: Reset two fingers start.\n");
                        
                        _fing[_pid].start_x = _fing[_pid].x;
                        _fing[_pid].start_y = _fing[_pid].y;
                        _fing[_sid].start_x = _fing[_sid].x;
                        _fing[_sid].start_y = _fing[_sid].y;
                        
                        _fing[_pid].last_x = _fing[_pid].x;
                        _fing[_pid].last_y = _fing[_pid].y;
                        _fing[_sid].last_x = _fing[_sid].x;
                        _fing[_sid].last_y = _fing[_sid].y;
                    }
                    
                case 3:
                case 4:
                case 5:
                    //
                    // Check for the clickpad button press
                    //
                    
                    if (_button.isClickPad) {
                        Process_ClickPad_Virtual_Buttons(packet, packet_type, fid, _cFingers);
                    }
                    
                    if (fid >= 0) {
                        delta_x[fid]  = (_fing[fid].x - _fing[fid].last_x);
                        delta_y[fid]  = (_fing[fid].y - _fing[fid].last_y);
                        
                        _fing[fid].x += delta_x[fid];
                        _fing[fid].y += delta_y[fid];
                        
                        _fing[fid].tdx += delta_x[fid];
                        _fing[fid].tdy += delta_y[fid];
                    }
                    
                    if (_cFingers == 2 && _pid >= 0 && _sid >= 0) {
                        Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
                    }
                    else if (_cFingers > 2)
                        Process_multiFingers_touch(delta_x[fid], delta_y[fid], fid);
                    break;
                    
            }
            
            // Set last x and y coordinates
            _fing[fid].last_x = _fing[fid].x;
            _fing[fid].last_y = _fing[fid].y;
            break;
            
        case PACKET_RELATIVE:
            
            /*
             * We receive relative packet with delta x and y values for multi fingers.
             */
            
            //
            // Get the primary and secondary finger id
            //
            
            if (_touchpadVendor == STD_FOCAL_TECH)
            {
                _pid = ((packet[0] >> 4) & 0x7) - 1;
                _sid = ((packet[3] >> 4) & 0x7) - 1;
                weight = 1;
            }
            else {
                _pid = ((packet[0] & 0xe0) >> 5) - 1;
                _sid = ((packet[3] & 0xe0) >> 5) - 1;
                weight = (packet[0] & 0x10) ? ETD_WEIGHT_VALUE : 1;
            }
            
            // Initialize the delta x and y variables
            for (int i = 0; i < STD_MAX_FINGERS; i++) {
                delta_x[i] = 0;
                delta_y[i] = 0;
            }
            
            //
            //  Motion packet give us the delta of x, y values of two specific fingers,
            //  but in two's complement. Let the compiler do the conversion for us.
            //  Also _enlarge_ the numbers to int, in case of overflow.
            //
            
            if (_pid >= 0) {
                delta_x[_pid]  = (weight * ((SInt8)packet[1]));
                delta_y[_pid] -= (weight * ((SInt8)packet[2]));
                
                _fing[_pid].x += delta_x[_pid];
                _fing[_pid].y += delta_y[_pid];
                
                _fing[_pid].tdx += delta_x[_pid];
                _fing[_pid].tdy += delta_y[_pid];
            }
            
            if (_sid >= 0) {
                delta_x[_sid]  = (weight * ((SInt8)packet[4]));
                delta_y[_sid] -= (weight * ((SInt8)packet[5]));
                
                _fing[_sid].x += delta_x[_sid];
                _fing[_sid].y += delta_y[_sid];
                
                _fing[_sid].tdx += delta_x[_sid];
                _fing[_sid].tdy += delta_y[_sid];
            }
            
            DEBUG_MUL_FING_LOG("STD :: Multi Finger Motion ID %d SID %d, Fingers %d, Button Data %d State %d, Weight %d, BtnArea %s, Drag %d, PCounts %d, TMode %d, Finger used %d\n", _pid, _sid, _cFingers, _button.data, _button.state, weight, _button.isFingOnBtnArea?"YES":"NO", _drag.isDragging, _packetsCount, _touchmode, _button.fingerInUse);
            
            // Stop
            if (_pid < 0 || _sid < 0)
                break;
            
            DEBUG_MUL_FING_LOG("STD :: Motion X%dD %d, Y%dD %d, X%dD %d, Y%dD %d, X1 Y1 %d %d, X2 Y2 %d %d\n", _pid+1, delta_x[_pid], _pid+1, delta_y[_pid], _sid+1, delta_x[_sid], _sid+1, delta_y[_sid], _fing[_pid].x, _fing[_pid].y, _fing[_sid].x, _fing[_sid].y);
            
            
            switch (_cFingers) {
                case 2:
                    if (_button.isClickPad) {
                        
                        //
                        // Reset start x and y for first relative packet, because sometimes
                        // finger switching does not report one of the finger position after
                        // switch.
                        //
                        
                        if (_packetsCount == 1 && !_swipe.isEdgeGesture)
                        {
                            DEBUG_MUL_FING_LOG("STD :: Restting start X and Y\n");
                            
                            _fing[_pid].start_x = _fing[_pid].x;
                            _fing[_pid].start_y = _fing[_pid].y;
                            _fing[_sid].start_x = _fing[_sid].x;
                            _fing[_sid].start_y = _fing[_sid].y;
                        }
                        
                        Process_ClickPad_2F(packet, packet_type, delta_x[_pid], delta_y[_pid], delta_x[_sid], delta_y[_sid]);
                    }
                    else {
                        Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
                    }
                    break;
                    
                case 3:
                case 4:
                case 5:
                    
                    //
                    // Check for the clickpad button press
                    //
                    
                    if (_button.isClickPad) {
                        Process_ClickPad_Virtual_Buttons(packet, packet_type, _pid, _cFingers);
                    }
                    
                    if (!_drag.isDragging || _drag.is3FDrag) {
                        
                        Process_multiFingers_touch(delta_x[_pid], delta_y[_pid], _pid);
                        
                        if (_sid >= 0) {
                            _packetsCount--; // Decrease last increment as we need one increment for both
                            Process_multiFingers_touch(delta_x[_sid], delta_y[_sid], _sid);
                        }
                    }
                    break;
                    
            }
            
            // Set last x and y coordinates
            _fing[_pid].last_x = _fing[_pid].x;
            _fing[_pid].last_y = _fing[_pid].y;
            
            if (_sid >= 0) {
                _fing[_sid].last_x = _fing[_sid].x;
                _fing[_sid].last_y = _fing[_sid].y;
            }
            break;
            
        case PACKET_UNKNOWN:
        default:
            /* impossible to get here */
            break;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Interpret complete data packet and process absolute mode input events
// for Synaptics hardware. (6 byte packet)
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */


void ApplePS2SmartTouchpad::Synaptics_report_absolute(unsigned char *packet)
{
    int cPressure = 0;
    int cAbsX = 0, cAbsY = 0;
    
    if (SYN_MODEL_NEWABS(_synTPad.model_id)) {
        
        //
        // Get the value needed to detect fingers presence
        //
        
        _synTPad.w   = (((packet[0] & 0x30) >> 2) |
                        ((packet[0] & 0x04) >> 1) |
                        ((packet[3] & 0x04) >> 2));
        
        //
        // Extended w mode? which tracks multiple fingers and detects palm
        //
        
        if ((SYN_CAP_ADV_GESTURE(_synTPad.ext_cap_0c) || SYN_CAP_IMAGE_SENSOR(_synTPad.ext_cap_0c))
            && _synTPad.w == 2)
        {
            int ext_packet_type;
            _synTPad.secondary_x = _synTPad.secondary_y = 0;
            _synTPad.secondary_pressure = 0;
            
            ext_packet_type = (packet[5] & 0x30) >> 4;
            
            switch (ext_packet_type) {
                    
                case SYN_SECONDARY_FINGER:
                    _synTPad.secondary_x = (((packet[4] & 0x0f) << 8) | packet[1]) << 1;
                    _synTPad.secondary_y = (_ymax + _ymin) - ((((packet[4] & 0xf0) << 4) | packet[2]) << 1);
                    _synTPad.secondary_pressure = ((packet[3] & 0x30) | (packet[5] & 0x0f)) << 1;
                    
                    DEBUG_LOG("STD :: Secondary packet ID %d, _sid %d, SX %d, SY %d, Pressure %d, Fingers(s) %d Last %d\n", _pid, _sid, _synTPad.secondary_x, _synTPad.secondary_y, _synTPad.secondary_pressure, _cFingers, _lastFingers);
                    
                    //
                    // Sometimes invalid packets are received with wrong values
                    // at the end of the touch in some touchpads, so ignore them.
                    //
                    
                    if (_synTPad.secondary_x < XMIN_NOMINAL/2 && _synTPad.secondary_pressure > 0)
                    {
                        _synTPad.secondary_x = _synTPad.secondary_y = _synTPad.secondary_pressure = 0;
                        DEBUG_LOG("STD :: Invalid secondary packet with wrong coodinates received, ignoring.\n");
                        
                        // Stop smoothing acceleration
                        if (_pContinuousMove) {
                            StopPointerEnhanceTimer();
                        }
                        return;
                    }
                    _synTPad.secondary_wait = true;
                    break;
                    
                case SYN_FINGER_STATE: // Notifies if fingers went 3->4->5 and 4->5->3
                    _pid = packet[2];
                    _sid = packet[4];
                    
                    //
                    // Process fingers change
                    //
                    
                    if (_cFingers != packet[1]) {
                        _lastFingers = _cFingers;
                        _cFingers = packet[1];
                        DEBUG_LOG("STD :: %dF slot changed to _pid %d, _sid %d\n", _cFingers, _pid, _sid);
                        Process_fingers_change();
                    }
                    break;
                    
                case SYN_SCROLL_WHEEL:
                default:
                    IOLog("STD :: Unknown packet type %d received\n", ext_packet_type);
                    break;
            }
            
            // return and wait for Primary finger
            return;
        }
        
        //
        // Get the primary finger absolute position and pressure
        //
        
        cAbsX = (((packet[3] & 0x10) << 8) | ((packet[1] & 0x0f) << 8) | packet[4]);
        
        cAbsY = (_ymax + _ymin) - (((packet[3] & 0x20) << 7) | ((packet[1] & 0xf0) << 4) | packet[5]);
        
        cPressure = packet[2];
        
        DEBUG_LOG("STD :: Primary packet ID %d, _sid %d, X %d, Y %d, Pressure %d, Fingers(s) %d Last %d, Button Data %d State %d TouchEnded %d\n", _pid, _sid, cAbsX, cAbsY, cPressure, _cFingers, _lastFingers, _button.data, _button.state, _synTPad.touch_ended);
        
        //
        // Sometimes invalid packets are received with wrong values
        // at the end of the touch so ignore them.
        //
        
        if (cAbsX < XMIN_NOMINAL/2 && cPressure > 0) {
            DEBUG_LOG("STD :: Invalid primary packet with wrong coodinates received, ignoring.\n");
            
            // Stop smoothing acceleration
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            
            if (_touchmode != 0)
                Process_touch_end(); // end gesture
            
            _synTPad.touch_ended = true;
            return;
        }
        
        // TO DO: Linux forcepad logic
        
        if (!_button.isClickPad && SYN_CAP_MIDDLE_BUTTON(_synTPad.capabilities))
        {
            UInt8 btn = ((packet[0] ^ packet[3]) & 0x01) ? 0x4 : 0;
            // _button.data = ((packet[0] ^ packet[3]) & 0x01) ? 0x4 : 0;
            if (_synTPad.w == 2)
                _synTPad.m_scroll = (signed char)(packet[1]);
            
            IOLog("STD :: Ignored Middle button state %d.\n", btn);
        }
        
        if (SYN_CAP_FOUR_BUTTON(_synTPad.capabilities)) {
            // hw->up   = ((packet[0] ^ packet[3]) & 0x01) ? 1 : 0;
            // hw->down = ((packet[0] ^ packet[3]) & 0x02) ? 1 : 0;
            IOLog("STD :: Ignored on four buttons packet.\n");
        }
        
        if (SYN_CAP_MULTI_BUTTON_NO(_synTPad.ext_cap) &&
            ((packet[0] ^ packet[3]) & 0x02))
        {
            IOLog("STD :: Ignored ext buttons.\n");
            
            switch (SYN_CAP_MULTI_BUTTON_NO(_synTPad.ext_cap) & ~0x01) {
                default:
                    /*
                     * if nExtBtn is greater than 8 it should be
                     * con_sidered invalid and treated as 0
                     */
                    break;
                case 8:
                    _synTPad.ext_buttons |= ((packet[5] & 0x08)) ? 0x80 : 0;
                    _synTPad.ext_buttons |= ((packet[4] & 0x08)) ? 0x40 : 0;
                case 6:
                    _synTPad.ext_buttons |= ((packet[5] & 0x04)) ? 0x20 : 0;
                    _synTPad.ext_buttons |= ((packet[4] & 0x04)) ? 0x10 : 0;
                case 4:
                    _synTPad.ext_buttons |= ((packet[5] & 0x02)) ? 0x08 : 0;
                    _synTPad.ext_buttons |= ((packet[4] & 0x02)) ? 0x04 : 0;
                case 2:
                    _synTPad.ext_buttons |= ((packet[5] & 0x01)) ? 0x02 : 0;
                    _synTPad.ext_buttons |= ((packet[4] & 0x01)) ? 0x01 : 0;
            }
        }
    } else {
        
        //
        // Get the finger count, absolute position and pressure
        // for old ABS packet.
        //
        
        _synTPad.w = (((packet[1] & 0x80) >> 4) | ((packet[0] & 0x04) >> 1));
        
        cAbsX = (((packet[1] & 0x1f) << 8) | packet[2]);
        
        cAbsY = (_ymax + _ymin) - (((packet[4] & 0x1f) << 8) | packet[5]);
        
        cPressure = (((packet[0] & 0x30) << 2) | (packet[3] & 0x3F));
        
        DEBUG_LOG("STD :: Old Primary packet ID %d, _sid %d, X %d, Y %d, Pressure %d, Fingers(s) %d Last %d\n", _pid, _sid, cAbsX, cAbsY, cPressure, _cFingers, _lastFingers);
        
        //
        // Sometimes invalid packets are received with wrong values
        // at the end of the touch in some touchpads, so ignore them.
        //
        
        if (cAbsX < XMIN_NOMINAL && cPressure > 0) {
            DEBUG_LOG("STD :: Invalid primary packet with wrong coodinates received, ignoring.\n");
            
            // Stop smoothing acceleration
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            
            if (_touchmode != 0) {
                Process_touch_end(); // end gesture
            }
            _synTPad.touch_ended = true;
            return;
        }
    }
    
    //
    //  Convert wrap-around values to negative. (X|Y)_MAX_POSITIVE
    //  is used by some firmware to indicate a finger at the edge of
    //  the touchpad whose precise position cannot be determined, so
    //  convert these values to the maximum axis value.
    //
    
    if (cAbsX  > X_MAX_POSITIVE)
        cAbsX  -= 1 << ABS_POS_BITS;
    else if (cAbsX  == X_MAX_POSITIVE)
        cAbsX  = XMAX;
    
    if (cAbsY > Y_MAX_POSITIVE)
        cAbsY -= 1 << ABS_POS_BITS;
    else if (cAbsY == Y_MAX_POSITIVE)
        cAbsY = YMAX;
    
    //
    // Middle button Scroll event ?
    //
    
    if (_synTPad.m_scroll) {
        _synTPad.m_scroll += _synTPad.m_scroll;
        
        while (_synTPad.m_scroll >= 4) {
            IOLog("STD :: scroll backward button packet.\n");
            _synTPad.m_scroll -= 4;
        }
        while (_synTPad.m_scroll <= -4) {
            IOLog("STD :: scroll forward button packet.\n");
            _synTPad.m_scroll += 4;
        }
        IOLog("STD :: Ignored middle button scroll packet.\n");
        return;
    }
    
    //
    // Pass through packet for Ext buttons and track stick ?
    // 0x84 xx xx 0xc4 xx xx
    //
    // Bug in FW 8.1, buttons are reported only when ExtBit is 1
    // if (SYN_ID_FULL(priv->identity) == 0x801 &&
    //  !((psmouse->packet[0] ^ psmouse->packet[3]) & 0x02))
    //      return;
    //
    
    if (SYN_CAP_PASS_THROUGH(_synTPad.capabilities)
        && SYN_CAP_EXT_BUTTONS_STICK(_synTPad.ext_cap_10)
        && (packet[0] & 0xFC) == 0x84 && (packet[3] & 0xCC) == 0xc4)
    {
        Process_passthrough_events_Synaptics(packet, cAbsX, cAbsY);
#if DEBUG_TPD
        if (_debugLvl & 0x1)
            LogPacketData(packet, PACKET_PASS_THRU);
#endif
        return;
    }
    
    //
    // Get the primary finger width and
    // current fingers count.
    //
    
    if (cPressure > 0) {
        
        if (SYN_CAP_EXTENDED(_synTPad.capabilities)) {
            
            switch (_synTPad.w) {
                    
                case 1:
                    /* More than 3 fingers are on touchpad so need to check */
                    if (_cFingers > 3)
                        break;
                case 0:
                    if (SYN_CAP_MULTIFINGER(_synTPad.capabilities))
                    {
                        // Process fingers change
                        if (_cFingers != (_synTPad.w + 2)) {
                            _lastFingers = _cFingers;
                            _cFingers = _synTPad.w + 2;
                            Process_fingers_change();
                        }
                    }
                    break;
                    
                case 2:
                    /* Nothing, treat a pen as a single finger */
                    if (SYN_MODEL_PEN(_synTPad.model_id))
                        ;
                    break;
                    
                case 4 ... 15:
                    
                    // Process fingers change
                    if (_cFingers != 1) {
                        _lastFingers = _cFingers;
                        _cFingers = 1;
                        Process_fingers_change();
                    }
                    
                    if (_pid >= 0) {
                        if (SYN_CAP_PALMDETECT(_synTPad.capabilities))
                            _fing[_pid].width = _synTPad.w;
                        else
                            _fing[_pid].width = 5;
                    } else {
                        _pid = 0; _sid = -1;
                    }
                    break;
            }
        }
    }
    else {
        // Process fingers change
        if (_cFingers != 0) {
            _lastFingers = _cFingers;
            _cFingers = 0;
            
            // Clear existing pointer accel
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
        }
        
        if (_pid >= 0){ _fing[_pid].width = 0; }
        
        //
        // Process physical buttons found
        //
        
        if (!_button.isClickPad && !_button.disableClicks) {
            Process_Physical_Buttons(packet);
        }
    }
    
    //
    // Clear secondary finger wait
    //
    
    if (_synTPad.secondary_wait
        && (SYN_CAP_ADV_GESTURE(_synTPad.ext_cap_0c) || SYN_CAP_IMAGE_SENSOR(_synTPad.ext_cap_0c)))
    {
        _synTPad.secondary_wait = false;
    }
    
    //
    // Get the primary finger data
    //
    
    if (_pid >= 0) {
        
        if (cPressure > 0)
        {
            
            _fing[_pid].x = cAbsX;
            _fing[_pid].y = cAbsY;
            _fing[_pid].pressure = cPressure;
            
            //
            // Set start and last coordinates for first packet
            //
            
            if (_packetsCount == 0 && (!(_swipe.direction < 3 || (_lastFingers == 1 && _cFingers == 2)) ||
                                       (_fing[_pid].start_x == 0 && _fing[_pid].start_y == 0)))
            {
                _fing[_pid].start_x = _fing[_pid].x;
                _fing[_pid].start_y = _fing[_pid].y;
                
                _fing[_pid].last_x = _fing[_pid].x;
                _fing[_pid].last_y = _fing[_pid].y;
            }
        }
        else {
            _fing[_pid].pressure = 0;
        }
    }
    
    //
    // Get the secondary finger data
    //
    
    if (_sid >= 0) {
        
        if (_synTPad.secondary_pressure > 0)
        {
            _fing[_sid].x = _synTPad.secondary_x;
            _fing[_sid].y = _synTPad.secondary_y;
            _fing[_sid].pressure = _synTPad.secondary_pressure;
            
            //
            // Set start and last coordinates for first packet
            //
            
            if (_packetsCount == 0 && (!(_swipe.direction < 3 || (_lastFingers == 1 && _cFingers == 2)) ||
                                       (_fing[_sid].start_x == 0 && _fing[_sid].start_y == 0)))
            {
                _fing[_sid].start_x = _fing[_sid].x;
                _fing[_sid].start_y = _fing[_sid].y;
                
                _fing[_sid].last_x = _fing[_sid].x;
                _fing[_sid].last_y = _fing[_sid].y;
            }
            
            //
            // Check last x and y values with respect to current values
            // to assign correct start values on 1 -> 2 transition
            //

            if (_packetsCount < 2 && _lastFingers == 1 && _cFingers == 2 &&
                
                // PID finger location changed ?
                ((getAbsoluteValue(_fing[_pid].start_x - _fing[_pid].x) > MAX_NOISE_DELTA
                  && getAbsoluteValue(_fing[_sid].start_x - _fing[_sid].x) < MAX_NOISE_DELTA)
                 
                 ||
                 
                 (getAbsoluteValue(_fing[_pid].start_y - _fing[_pid].y) > MAX_NOISE_DELTA
                  && getAbsoluteValue(_fing[_sid].start_y - _fing[_sid].y) < MAX_NOISE_DELTA)))
            {
                // SID finger location also changed ?
                if (getAbsoluteValue(_fing[_pid].start_x - _fing[_sid].x) > MAX_NOISE_DELTA
                    || getAbsoluteValue(_fing[_pid].start_y - _fing[_sid].y) > MAX_NOISE_DELTA)
                {
                    _fing[_sid].start_x = _fing[_sid].x;
                    _fing[_sid].start_y = _fing[_sid].y;
                    DEBUG_LOG("STD :: Fixed start values on 1 -> 2 for SID\n");
                }
                else {
                    _fing[_sid].start_x = _fing[_pid].start_x;
                    _fing[_sid].start_y = _fing[_pid].start_y;
                }
                
                _fing[_pid].start_x = _fing[_pid].x;
                _fing[_pid].start_y = _fing[_pid].y;
                
                DEBUG_LOG("STD :: Fixed start values on 1 -> 2 for PID\n");
            }
        }
        else {
            _fing[_sid].pressure = 0;
        }
    }
    
   // IOLog("STD :: Fingers %d Last %d, Buttons %d State %d, ID %d, _sid %d, P %d P1 %d P2 %d, W1 %d W2 %d, PacketsCount %d, W %d, SWait %d, Mode %d, ES %d, Non H %d V %d, CMove %d, CScroll %d, TE %d\n", _cFingers, _lastFingers, _button.data, _button.state, _pid, _sid, cPressure, _fing[_pid].pressure, _fing[_sid].pressure, _fing[_pid].width, _fing[_sid].width, _packetsCount, _synTPad.w, _synTPad.secondary_wait, _touchmode, _swipe.isEdgeGesture, _swipe.nonEdgeHArea, _swipe.nonEdgeVArea, _pContinuousMove, _scroll.doCont, _synTPad.touch_ended);
    
    
    //
    // Palm detection
    //
    
    if ((_palm.detectionEnabled &&
         
         (((_cFingers == 1 && _fing[0].width > _palm.minWidth && _fing[0].pressure > _palm.minPressure) ||
           (_cFingers < 3 && _fing[0].pressure > _palm.minPressure))
          
          ||
          
          (_palm.detected && !(_cFingers == 0 && cPressure == 0))))
        
        || _kbEvent.showPressWidth  || (_cFingers == 1 && _lastFingers > 1 && _fing[0].width > _palm.minWidth))
    {
        // Ignore palm packets received after multi touch gesture
        if (_cFingers == 1 && _lastFingers > 1) {
            IOLog("STD :: Ignored Touch for Pressure %d, Width %d, Finger(s) count %d, Packets Count %d Last Finger %d\n",
                  _fing[0].pressure, _fing[0].width, _cFingers, _packetsCount, _lastFingers);
            return;
        }
        //
        // Show Fingers, pressure and width of the touch
        //
        
        if (_kbEvent.showPressWidth) {
            IOLog("STD :: Touch Primary ID %d Pressure %d Width %d, Secondary ID %d Pressure %d Width %d, Finger(s) count %d\n", _pid, ((_pid >= 0) ? _fing[_pid].pressure:-1),  ((_pid >= 0) ?_fing[_pid].width:-1), _sid,  (_sid >= 0) ?_fing[_sid].pressure:-1,  (_sid >= 0) ?_fing[_sid].width:-1, _cFingers);
        }
        else {
            if (!_palm.detected) {
                IOLog("STD :: Palm detected for the requested Pressure %d and Width %d (Detected Finger(s) %d, Pressure %d, Width %d)\n", _palm.minPressure, _palm.minWidth, _cFingers, _fing[0].pressure, _fing[0].width);
            }
            _palm.detected = true;
            _touchmode = MODE_PALM;
            return;
        }
    }
    
    //
    // Stop processing on accidental input
    //
    
    if (_kbEvent.accidentalInput && _kbEvent.tapEnableTPad
        && !(_cFingers == 0 && cPressure == 0))
    {
        if (_gestureTime < _tap.clickTimeout)
            _touchmode = MODE_1_FING_TAP;
        else
            _touchmode = MODE_IDLE;
        
        IOLog("STD :: Ignored Touch for accidental input\n");
        return;
    }
    
    //
    // Calculate time taken for the gesture
    //
    
    clock_get_uptime(TIME_NOW);
    
    if (_packetsCount > 0)
        _gestureTime += (*(uint64_t*)&_now) - _lastDataTime;
    
    _lastDataTime = (*(uint64_t*)&_now);
    
    
    //
    // Process the touch and dispatch
    //
    
    if (!_synTPad.touch_ended && cPressure == 0 && _cFingers == 0 && _button.data == 0
        && ((_pid >= 0 && _pid < 5) || _sid >= 0)) // end of touch
    {
        Process_touch_end();
    }
    else
    {
        int dx1 = 0, dx2 = 0, dy1 = 0, dy2 = 0;
        
        switch (_cFingers) {
                
            case 1:
                
                //
                // Process physical buttons found in some hardware
                //
                
                if (!_button.isClickPad && !_button.disableClicks) {
                    Process_Physical_Buttons(packet);
                }
                
                //
                // Process virtual buttons
                //
                
                //
                // Process virtual buttons,
                // but skip first packet beacuse sometimes Head packet
                // for multi fingers sent here which we should not process.
                //
                
                else if (_packetsCount > 0) {
                    
                    //
                    // Finger is on button zone ?
                    //
                    
                    if (IsFingerOnButtonArea(_fing[_pid].y))
                    {
                        _button.isFingOnBtnArea  = true;
                        DEBUG_1F_LOG("STD :: A finger is on button area at the location X %d.\n", _fing[_pid].x);
                    }
                    else {
                        _button.isFingOnBtnArea  = false;
                    }
                    
                    Process_ClickPad_Virtual_Buttons(packet, 0, _pid, 1);
                    
                    // Trigger click drag
                    if (_button.triggered && !_button.isFingOnBtnArea)
                    {
                        DEBUG_BTNS_LOG("STD :: Click drag triggered.\n");
                        _drag.isHoldDrag = _drag.isDragging = true;
                        _fingPress.holdTime = 2000000001;
                    }
                }
                
                //
                // Process pointer movement
                //
                
                Process_singlefinger_touch(_fing[_pid].x, _fing[_pid].y, _pid);
                
                // Set last x and y coordinates
                _fing[_pid].last_x = _fing[_pid].x;
                _fing[_pid].last_y = _fing[_pid].y;
                break;
                
            case 2:
                
                if (_packetsCount <= 1)
                {
                    //
                    // Edge swipe ?
                    //
                    
                    if (!_swipe.isEdgeGesture)
                        Is2FingEdgeSwipe();
                    
                    if (!_swipe.isEdgeGesture) {
                        
                        //
                        // Reset the coordinates
                        //
                            
                        _fing[_pid].start_x = _fing[_pid].x;
                        _fing[_pid].start_y = _fing[_pid].y;
                        _fing[_sid].start_x = _fing[_sid].x;
                        _fing[_sid].start_y = _fing[_sid].y;
                        
                        _fing[_pid].last_x = _fing[_pid].x;
                        _fing[_pid].last_y = _fing[_pid].y;
                        _fing[_sid].last_x = _fing[_sid].x;
                        _fing[_sid].last_y = _fing[_sid].y;
                    }
                }
                
                //
                // Process event
                //
                
                if (_button.isClickPad) {
                    
                    if (_packetsCount > 1)
                    {
                        dx1 = _fing[_pid].x - _fing[_pid].last_x;
                        dy1 = _fing[_pid].y - _fing[_pid].last_y;
                        dx2 = _fing[_sid].x - _fing[_sid].last_x;
                        dy2 = _fing[_sid].y - _fing[_sid].last_y;
                        
                        DEBUG_2F_LOG("STD :: Clickpad 2F X%dD %d, Y%dD %d, X%dD %d, Y%dD %d, X1 Y1 %d %d, X2 Y2 %d %d, Last X1 Y1 %d %d, X2 Y2 %d %d\n", _pid+1, dx1, _pid+1, dy1, _sid+1, dx2, _sid+1, dy2, _fing[_pid].x, _fing[_pid].y, _fing[_sid].x, _fing[_sid].y, _fing[_pid].last_x, _fing[_pid].last_y, _fing[_sid].last_x, _fing[_sid].last_y);
                        
                        Process_ClickPad_2F(packet, PACKET_ABSOLUTE, dx1, dy1, dx2, dy2);
                    }
                    else {
                        Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
                    }
                }
                else
                    Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
                
                _fing[_pid].last_x = _fing[_pid].x;
                _fing[_pid].last_y = _fing[_pid].y;
                _fing[_sid].last_x = _fing[_sid].x;
                _fing[_sid].last_y = _fing[_sid].y;
                break;
                
            case 3:
            case 4:
            case 5:
                //
                // Calculate total delta value for two fingers then Process event
                //
                
                if (_pid >= 0)
                {
                    dx1 = _fing[_pid].x - _fing[_pid].last_x;
                    dy1 = _fing[_pid].y - _fing[_pid].last_y;
                    _fing[_pid].tdx += dx1;
                    _fing[_pid].tdy += dy1;
                    
                    Process_multiFingers_touch(dx1, dy1, _pid);
                    
                    _fing[_pid].last_x = _fing[_pid].x;
                    _fing[_pid].last_y = _fing[_pid].y;
                }
                
                if (_pid >= 0 && _sid >= 0)
                {
                    //
                    // Check for the clickpad button press
                    //
                    
                    if (_button.isClickPad) {
                        Process_ClickPad_Virtual_Buttons(packet, PACKET_ABSOLUTE, 0, _cFingers);
                    }
                    
                    _packetsCount--; // Decrease last increment as we need one increment for pid and sid
                    
                    dx2 = _fing[_sid].x - _fing[_sid].last_x;
                    dy2 = _fing[_sid].y - _fing[_sid].last_y;
                    _fing[_sid].tdx += dx2;
                    _fing[_sid].tdy += dy2;
                    
                    Process_multiFingers_touch(dx2, dy2, _sid);
                    
                    _fing[_sid].last_x = _fing[_sid].x;
                    _fing[_sid].last_y = _fing[_sid].y;
                }
                break;
                
            default:
                /* do nothing */
                break;
        }
    }
    
}

void ApplePS2SmartTouchpad::Process_passthrough_events_Synaptics(unsigned char *packet, int AbsX, int AbsY)
{
    
    Process_Physical_Buttons(packet);
    
    //
    // In trackpoint (stick) packet bits 2, 4 and 5 are non-zero
    //
    
    if (packet[2] != 0 || packet[4] != 0 || packet[5] != 0)
    {
        _pid = 5;
        _fing[_pid].x = AbsX;
        _fing[_pid].y = AbsY;
        
        // Process track pointer movement (Experimental, still needs work)
        Process_singlefinger_touch(_fing[_pid].x, _fing[_pid].y, _pid);
        
        // Set last x and y coordinates
        _fing[_pid].last_x = _fing[_pid].x;
        _fing[_pid].last_y = _fing[_pid].y;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes fingers change.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_fingers_change()
{
    
    IOLog("STD :: Fingers Status changed - Current Finger(s) %d, Last Finger(s) %d, Button Data %d State %d, _touchmode %d, Touchtime %lld, PacketsCount %d, FingOnBtn %d, _pid %d, _sid %d, RM %d %d, ESwipes %d, Drag %d, Tap %d\n", _cFingers, _lastFingers, _button.data, _button.state, _touchmode, _gestureTime, _packetsCount, _button.isFingOnBtnArea, _pid, _sid, _rotate.isRotateGesture, _rotate.isRotateMode, _swipe.isEdgeGesture, _drag.isDragging, _drag.isTapDrag);
    
    //
    // Capable of multi touch? if so then update the finger slots (id)
    // as we get only two event reports in multi touch to track.
    //
    
    if ((_touchpadVendor == STD_SYNAPTICS) &&
        (SYN_CAP_ADV_GESTURE(_synTPad.ext_cap_0c) || SYN_CAP_IMAGE_SENSOR(_synTPad.ext_cap_0c)))
    {
        //
        // Update slots
        //
        
        switch (_synTPad.w) {
            case 0: // 2F
                _pid = 0; _sid = 1;
                DEBUG_LOG("STD :: 2F slot are _pid %d, _sid %d\n", _pid, _sid);
                break;
                
            case 1: // 3F
                _pid = 0; _sid = 2;
                DEBUG_LOG("STD :: 3F slot are _pid %d, _sid %d\n", _pid, _sid);
                break;
                
            case 4 ... 15:
                _pid = 0; _sid = -1;
                DEBUG_LOG("STD :: 1F slot are _pid %d, _sid %d\n", _pid, _sid);
                break;
                
            default: // 4F and 5f updated by finger information packet
                break;
        }
    }
    
    //
    // Cancel active timers (triggered by the last ended packet stream)
    // at the start of the new packet stream.
    //
    
    if (_doTimerJob || _scroll.doCont || _scroll.inertiaDeltaX != 0 || _scroll.inertiaDeltaY != 0) {
        
        _buttonDragTimer->cancelTimeout();
        StopScrollEnhanceTimer();
    }
    
    if (_pContinuousMove) {
        StopPointerEnhanceTimer();
    }
    
    //
    // Clear Tap/hold drag if fingers switched to greater than 2 or 0.
    // Also try to avoid hold getsure in rotate gesture if the constant finger
    // was held with enough pressure to trigger.
    //
    
    if ((_drag.isTapDrag ||
        (_drag.isHoldDrag && (_fing[_pid].tdx < _rotate.maxCFDelta || _fing[_pid].tdy < _rotate.maxCFDelta)))
        && ((_cFingers > 2 && !_drag.is3FDrag) || _cFingers == 0))
    {
        dispatchRelativePointerPosition(0, 0, 0);
        _button.data = _button.state = 0;
        
        _drag.isDragging = _drag.isHoldDrag = _drag.isTapDrag = _drag.is3FDrag = false;
        
        IOLog("STD :: Drag cleared\n");
    }
    
    //
    // Reset data
    //
    
    _synTPad.touch_ended = false;
    
    _tap.maxPressDtd    = 0;
    
    _cSample    = 0;
    _skipSample = 0;
    _pSmoothX   = 0;
    _pSmoothY   = 0;
    _pDx        = 0;
    _pDy        = 0;
    _pLastDx = 0;
    _pLastDy = 0;
    
    _zoom.zDx   = 0;
    _zoom.zDy   = 0;
    
    _scroll.totalDy = 0;
    _scroll.totalDx = 0;

    if (_lastFingers == 0 || _lastFingers > _cFingers || (_lastFingers < _cFingers && _packetsCount > 3))
    {
        int i = 0;
        
        if (!_drag.is3FDrag)
        {
            if (_lastFingers > _cFingers) {
                i = _cFingers;
            }
            else if (_lastFingers < _cFingers) {
                i = _lastFingers;
            }
        }
        
        IOLog("STD :: Data from finger %d are cleared\n", i+1);
        
        for (; i < STD_MAX_FINGERS; i++) {
            _fing[i].tdx = _fing[i].tdy = 0;
            _fing[i].sdx = _fing[i].sdy = 0;
            _fing[i].start_x = _fing[i].start_y = 0;
            
            //
            // Skip for primary and second finger id as we need that data
            // for subsequent finger quick transitions 1->2 or 2->1 because
            // we wont get absolute value for both fingers during trantsions
            // and only one is sent and other fingers is in last gesture.
            //
            
            if ((_packetsCount > 0 && i != _sid && i != _pid) || _lastFingers == 0 || _cFingers > 2)
            {
                _fing[i].x = _fing[i].y = 0;
                _fing[i].last_x  = _fing[i].last_y  = 0;
            }
            else {
                IOLog("STD :: Data reset from finger %d skipped\n", i+1);
            }
        }
    }
    
    for (int s = 0; s < 10; s++) {
        _lastSample[s].lastDX1 = 0;
        _lastSample[s].lastDY1 = 0;
        _lastSample[s].lastDX2 = 0;
        _lastSample[s].lastDY2 = 0;
        
        switch (_touchmode) {
            case MODE_VSCROLL:
            case MODE_HSCROLL:
            case MODE_EDGE_VSCROLL:
            case MODE_EDGE_HSCROLL:
            case MODE_CONT_SCROLL:
                // Skip
                break;
                
            default:
                _lastSample[s].lastSDX = 0;
                _lastSample[s].lastSDY = 0;
                break;
        }
    }
    
    //
    // Clear Tap mode from transitions  1 -> 2/3/4/5
    //
    
    if (_lastFingers <= _cFingers && _cFingers > 1)
    {
        switch (_touchmode) {
            case MODE_1_FING_TAP:
            case MODE_2_FING_TAP:
            case MODE_3_FING_TAP:
            case MODE_4_FING_TAP:
            case MODE_5_FING_TAP:
                _touchmode = MODE_IDLE;
                break;
                
            default:
                break;
        }
    }
    
    //
    // Clear Edge swipe and Rotate mode
    //
    
    if (_cFingers != 1 && _cFingers != 2 && _swipe.isEdgeGesture) {
        _swipe.isEdgeGesture = false;
        _swipe.direction = 0;
    }
    
    switch (_cFingers) {
        case 2:
            //
            // Reset last fingers count to avoid rotate conflicts for 2f in 1->2
            //
            
            /*if (_lastFingers == 1 && _packetsCount > 0 && _packetsCount < 6
                && (_fing[_pid].tdx != 0 || _fing[_pid].tdy != 0 || _packetsCount < 3))
            {
                _lastFingers = 0;
                DEBUG_LOG("STD :: Last fingers reset for TDX %d, TDY %d.\n", _fing[_pid].tdx, _fing[_pid].tdy);
            }*/
            
            //
            // Clear mode to enable rotate logic detection in 2->1 and 1->2
            //
            
            if (_rotate.isRotateGesture && _lastFingers != 2) {
                _touchmode = MODE_IDLE;
            }
            
            if (_lastFingers == 1 &&  _gestureTime > _rotate.minCFingIdleTime)
            {
                IOLog("STD :: Rotate mode activated for time %lld, Max %lld\n", _gestureTime, _rotate.minCFingIdleTime);
                _rotate.isRotateMode = true;
            }
            break;
            
        case 3 ... 5:
            if (_rotate.isRotateGesture) {
                _rotate.isRotateGesture = false;
                _rotate.isRotateMode = false;
                _touchmode = MODE_IDLE;
            }
            break;
            
        default:
            break;
    }
    
    
    //
    // Ignore this for physical buttons (_cFingers == 0)
    //
    
    if (_cFingers != 0)
    {
        _gestureTime    = 0;
        _packetsCount   = 0;
        _fingPress.holdTime = 0;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes hardware buttons.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_Physical_Buttons(unsigned char *packet)
{
    if (_button.disableClicks)
        return;
    
    if (_touchpadVendor == STD_SYNAPTICS
        && (packet[0] & 0xFC) == 0x84 && (packet[3] & 0xCC) == 0xc4
        && SYN_CAP_EXT_BUTTONS_STICK(_synTPad.ext_cap_10)) {
        
        //
        // Ext Button down ?
        //
        // byte 1   84  Button   00  c4  00  00
        //
        
        _button.data = packet[1] & 0x07;
    }
    else {
        /*
         * Get the button value
         * byte 0: n1  n0   .   .   .   .   R   L
         */
        _button.data = (packet[0] & 0x03);
    }
    
    // No need further processing
    if (_button.state == _button.data)
        return;
    
    switch (_button.data) {
        case 0x04:
        case 0x03:  // middle button (bit 0 and 1 set in packet[0])
            if (_button.enableMiddle) {
                _button.data = 0x4;
            }
            else {
                _button.data = 0x0;
            }
            
        case 0x01:  // left button (bit 0 set in packet[0])
            _button.triggered = true;
            _button.state |= _button.data;
            _touchmode = MODE_BUTTONS;
            
            DEBUG_BTNS_LOG("STD :: %s Button detected.\n", (_button.data == 1)?"Left":"Middle");
            
            // Dispatch
            dispatchRelativePointerPosition(0, 0, _button.data);
            break;
            
        case 0x02: // right button (bit 1 set in packet[0])
            _button.state |= _button.data;
            _touchmode = MODE_BUTTONS;
            _button.rightClick = true;
            _button.triggered = true; // Does this causes any problems ?

            DEBUG_BTNS_LOG("STD :: Right Button detected.\n");
            
            // Dispatch
            dispatchRelativePointerPosition(0, 0, _button.data);
            break;
            
        case 0:
        default:
            DEBUG_BTNS_LOG("STD :: No button detected.\n");
            
            // Release left/middle button and drag if it was triggered
            if (_button.triggered || _button.rightClick) {
                
                DEBUG_BTNS_LOG("STD :: Button released.\n");
                
                _button.state = 0;
                // Release button
                dispatchRelativePointerPosition(0, 0, _button.data);
                IODelay(50);
                dispatchRelativePointerPosition(0, 0, 0);
                
                _button.rightClick = false;
                _drag.isDragging = false;
                _drag.isTapDrag = _drag.isHoldDrag = _drag.is3FDrag = false;
                _fingPress.holdTime = 0;
                
                // Release the cmd key if the App switch gesture function is active
                if (_swipe.isAppSwitchDone)
                {
                    _device->dispatchPS2Notification(kPS2C_ReleaseCmdKey);
                    _swipe.isAppSwitchDone =  false;
                }
            }
            
            _button.data = 0;
            _button.triggered = false;
            
            //
            // Stop the enhanced pointer accleration if its active
            //
            
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            
            break;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes virtual buttons.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

bool ApplePS2SmartTouchpad::Process_ClickPad_Virtual_Buttons(unsigned char *packet,
                                                             UInt8 packetType,
                                                             UInt8 fid,
                                                             UInt8 fCount)
{
    if (_button.disableClicks)
        return false;
    
    bool pressed = false;
    
    switch (_touchpadVendor) {
        case STD_FOCAL_TECH:
            if (packetType == PACKET_ABSOLUTE) {
                pressed = (packet[0] >> 4) & 1;
            }
            else {
                pressed = packet[0] >> 7;
            }
            break;
            
            //
            //  Clickpad's button is also transmitted as middle button,
            //  however, since it is primary button, we will report
            //  it as BTN_LEFT.
            //
            
        case STD_SYNAPTICS:
            pressed = ((packet[0] ^ packet[3]) & 0x01) ? 0x1 : 0;
            break;
            
        case STD_ELAN_TECH:
            pressed = packet[0] & 0x01;
            break;
    }
    
    //
    // Button detected ?
    //
    
    if (pressed)
    {
        IOLog("STD :: Button detected, Data %d State %d, Fingers %d\n", _button.data, _button.state, fCount);
        
        //
        // Clear rotate gesture because it may get triggered
        // if the click is not triggered in Head packet but
        // gets triggered in motion packet.
        //
        
        _rotate.isRotateGesture = false;
        _rotate.isRotateMode = false;
        
        // Set button mode
        _button.triggered = true;
        
        switch (fCount) {
            case 1: // Dispatch 1f click
                
                _touchmode = MODE_BUTTONS;
                
                // Stop further processing
                if (_button.data != 0)
                    return true;
                //
                // Finger is on button section?
                //
                
                if (_button.isFingOnBtnArea && _button.clickPadHasVButtons)
                {
                    _button.isFingOnBtnArea  = true;
                    
                    IOLog("STD :: A finger is on button area at the location X %d, Middle area %d.\n", _fing[fid].x, (UInt32)((_xmax - _xmin) * 0.13));
                    
                    //
                    // 22% of vertical touchpad section at the bottom is used for virual buttons
                    // Left button if the touch location is less than centreX - 13% x plane
                    // Right button if the touch location is greater than centreX + 13% x of plane
                    // Middle Button if thet touch location is between left and right buttons touch area
                    //
                    
                    UInt32 middleButtonArea = 0;
                    
                    // Set middle button area if its enabled
                    if (_button.enableMiddle)
                        middleButtonArea = (UInt32)((_xmax - _xmin) * 0.13);
                    
                    // Left button
                    if (_fing[fid].x <= (_xcenter - middleButtonArea)) {
                        _button.data = 0x1;
                    }
                    // Right button
                    else if (_fing[fid].x > (_xcenter + middleButtonArea)) {
                        _button.data = 0x2;
                        _button.rightClick = true;
                    }
                    // Middle Button
                    else  {
                        _button.data = 0x4;
                    }
                    
                    IOLog("STD :: Virtual Button 0x%x detected.\n", _button.data);
                }
                else {
                    _button.data = 0x1;
                    IOLog("STD :: Left Button detected for the finger position %d.\n", _fing[fid].x);
                }
                break;
                
            case 2: // Dispatch 2f click
                
                _touchmode = MODE_BUTTONS;
                _button.is2FClick = true;

                // Skip if its in Drag mode (triggered by first finger)
                if (_button.state == 0x1)
                    break;
                
                //
                // Both Fingers are on button section?
                //
                
                // 2F Middle button click
                if (_button.clickPadHasVButtons &&
                    _fing[0].y > (_ymax - _button.clickPadVBtnArea) &&
                    _fing[1].y > (_ymax - _button.clickPadVBtnArea))
                {
                    IOLog("STD :: 2F click detected at Button area.\n");
                    if (_button.enableMiddle) {
                        _button.data = 0x4;
                    }
                    else {
                        if (_button.state) {
                            _button.data = _button.state;
                        }
                        else {
                            _button.state = _button.data = 0x1;
                        }
                    }
                    _button.isFingOnBtnArea = true;
                }
                //  2F Button click
                else if (!_button.clickPadHasVButtons ||
                         (!(_fing[0].y > (_ymax - _button.clickPadVBtnArea)) &&
                          !(_fing[1].y > (_ymax - _button.clickPadVBtnArea))))
                {
                    IOLog("STD :: 2F click detected at Pointer area.\n");
                    if (_fingPress.fings2Click) {
                        _button.data = _fingPress.fings2Click;
                        if (_fingPress.fings2Click == 0x2) { _button.rightClick = true; }
                    }
                    else {
                        if (_button.state) {
                            _button.data = _button.state;
                        }
                        else {
                            _button.state = _button.data = 0x1;
                        }
                    }
                    _button.isFingOnBtnArea = false;
                }
                else {
                    IOLog("STD :: Only one finger is in_side button area in 2F.\n");
                    _button.data = 0x1;
                    
                    //
                    // If it was 2f click before then reset,
                    // so pointer move wont have jumpy cursor.
                    //
                    
                    if (_button.is2FClick && _packetsCount > 1)
                        _packetsCount = 0;
                    
                    _button.is2FClick = false;
                    
                    if (!_button.clickPad2FMove)
                        _button.isFingOnBtnArea = false;
                    else
                        _button.isFingOnBtnArea = true;
                }
                break;
                
            default:
                break;
        }
        
        //
        // Stop the enhanced pointer accleration if its active
        //
        if (_pContinuousMove) {
            StopPointerEnhanceTimer();
        }
        
        // Release the cmd key if the App switch gesture function is active
        if (_swipe.isAppSwitchDone)
        {
            _device->dispatchPS2Notification(kPS2C_ReleaseCmdKey);
            _swipe.isAppSwitchDone =  false;
        }
        
        // Dispatch button (Skip for 2f middle button and no button)
        if (!(_button.is2FClick && (_button.data == 0x4 || _button.data == 0))) {
            _button.state = _button.data;
            dispatchRelativePointerPosition(0, 0, _button.data);
            IOLog("STD :: Button trigerred %d\n", _button.data);
        }
    }
    else
    {
        DEBUG_BTNS_LOG("STD :: No Button detected\n");
        
        if (_button.triggered || _button.rightClick)
        {
            IOLog("STD :: Button released 0x%x\n", _button.data);
            
            // Release button
            dispatchRelativePointerPosition(0, 0, _button.data);
            IODelay(50);
            dispatchRelativePointerPosition(0, 0, 0);
            
            _button.state = 0;
            _button.is2FClick = false;
            _button.triggered = false;
            _button.rightClick = false;
            
            // Clear drag states
            _drag.isDragging = false;
            _drag.isTapDrag = _drag.isHoldDrag = _drag.is3FDrag = false;
            
            _fingPress.holdTime = 0;
        }
        _button.data = 0;
    }
    
    return pressed;
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes trackpoint.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_trackpoint_events_ELAN(UInt8 packet_type, UInt8 *packet)
{
    /*
     * byte 0:  0   0  sx  sy   0   M   R   L
     * byte 1:~sx   0   0   0   0   0   0   0
     * byte 2:~sy   0   0   0   0   0   0   0
     * byte 3:  0   0 ~sy ~sx   0   1   1   0
     * byte 4: x7  x6  x5  x4  x3  x2  x1  x0
     * byte 5: y7  y6  y5  y4  y3  y2  y1  y0
     *
     * x and y are written in two's complement spread
     * over 9 bits with sx/sy the relative top bit and
     * x7..x0 and y7..y0 the lower bits.
     * The sign of y is opposite to what the input driver
     * expects for a relative movement
     */
    
    int x, y;
    x = packet[4] - (int)((packet[1]^0x80) << 1);
    y = (int)((packet[2]^0x80) << 1) - packet[5];
    
    // Set start x and y coordinates
    if (_packetsCount == 0) {
        _fing[0].start_x = 0;
        _fing[0].start_y = 0;
    }
    else {
        _fing[0].tdx += x;
        _fing[0].tdy += y;
    }
    
    // IOLog("STD :: %dF Touch X %d Y %d\n",fingers, x, y);
    
    Process_singlefinger_touch(x, y, 0);
    
    
    /*
     u32 t = get_unaligned_le32(&packet[0]);
     
     switch (t & ~7U) {
     case 0x06000030U:
     case 0x16008020U:
     case 0x26800010U:
     case 0x36808000U:
     x = packet[4] - (int)((packet[1]^0x80) << 1);
     y = (int)((packet[2]^0x80) << 1) - packet[5];
     break;
     
     default:
     break;
     }
     */
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes single finger touch.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_singlefinger_touch(int x, int y, int fid)
{
    //
    // Note: This is for ClickPad
    // Do not process if there is only one finger and its on button area for a click,
    // as they should trigger only button events.
    //
    
    if (_button.isClickPad && _button.isFingOnBtnArea && IsFingerOnButtonArea(_fing[_pid].start_y)
        && !_button.enablePointingOnBtnArea && _cFingers < 2)
    {
        if (_packetsCount < 255) {
            _packetsCount++;
        }
        if (_pContinuousMove) {
            StopPointerEnhanceTimer();
        }
        dispatchRelativePointerPosition(0, 0 , _button.state);
        DEBUG_1F_LOG("STD :: Skipping for finger on button area\n");
        return;
    }
    
    // Invalid id ?
    if (fid > 4)
    {
        return;
    }
    
    // signed x , y and edge delta values from last x, y
    SInt32 xDelta = 0, yDelta = 0;
    
    // unsigned x, y delta values from last x, y
    UInt16  us_dx = 0, us_dy= 0;
    
    // X and Y direction
    SInt8 xDir = 0, yDir = 0;
    
    // ------------------------------------------------------------------------------
    
    //
    // Enable edge swipe mode when the finger starts from either edge area or
    // middle of the touchpad for 2 way swipes.
    //

    if (!_button.triggered && _swipe.hasEdgeSwipes
        
        && (_packetsCount == 0 || // First packet for Swipe IN
        
        (_swipe.has2WayEdgeSwipes && // Finger is in edge ?
         (y > (_ymax - _swipe.edgeVArea) || y < (_ymin + _swipe.edgeVArea) ||
          x < (_xmin + _swipe.edgeHArea) || x > (_xmax - _swipe.edgeHArea)))))
    {
        
        // Finger X exists between non edge swipe area
        if (_fing[fid].start_x > (_xmin + _swipe.nonEdgeHArea) &&
            _fing[fid].start_x < (_xmax - _swipe.nonEdgeHArea))
        {
            if (_swipe.topEdge > 0 &&
                 ((_fing[fid].start_y < (_ymin + _swipe.edgeVArea)) ||
                  
                  (_fing[fid].start_y > (_ymin + _swipe.edgeVDistance)
                   && y < (_ymin + _swipe.edgeVArea) && _swipe.has2WayEdgeSwipes)))
            {
                _swipe.direction = 4; // Top
                _swipe.isEdgeGesture = true;
            }
            else if (_swipe.bottomEdge > 0 &&
                     ((_fing[fid].start_y > (_ymax - _swipe.edgeVArea)) ||
                      
                      (_fing[fid].start_y < (_ymax - _swipe.edgeVDistance)
                       && y > (_ymax - _swipe.edgeVArea) && _swipe.has2WayEdgeSwipes)))
            {
                _swipe.direction = 3; // Bottom
                _swipe.isEdgeGesture = true;
            }
        }
        
        // Finger Y exists between non edge swipe area
        else if (_fing[fid].start_y > (_ymin + _swipe.nonEdgeVArea) &&
                 _fing[fid].start_y < (_ymax - _swipe.nonEdgeVArea))
        {
            if (_swipe.leftEdge > 0 &&
                ((_fing[fid].start_x < (_xmin + _swipe.edgeHArea)) ||
                 (_fing[fid].start_x > (_xmin + _swipe.edgeHDistance)
                  && x < (_xmin + _swipe.edgeHArea) && _swipe.has2WayEdgeSwipes)))
            {
                _swipe.direction = 1; // Left
                _swipe.isEdgeGesture = true;
            }
            else if (_swipe.rightEdge > 0 &&
                     ((_fing[fid].start_x > (_xmax - _swipe.edgeHArea)) ||
                      (_fing[fid].start_x < (_xmax - _swipe.edgeHDistance)
                       && x > (_xmax - _swipe.edgeHArea) && _swipe.has2WayEdgeSwipes)))
            {
                _swipe.direction = 2; // Right
                _swipe.isEdgeGesture = true;
            }
        }
        
        
#if DEBUG_SCROLL_SWIPES_LOG
        if (_swipe.isEdgeGesture) {
            IOLog("STD :: %s edge swipe detected", (_swipe.direction == 1)?"Left":(_swipe.direction == 2)?"Right":(_swipe.direction == 3)?"Bottom":"Top");
        }
#endif

    }

    // ------------------------------------------------------------------------------

    //
    // Calculate delta values
    //

    switch (_cFingers) {
        case 2: // v4 hardware, thumb lies on button area and index finger used for moving ?
            if (!(_button.isFingOnBtnArea || _button.state == 0x1))
                break;
            
            xDelta = x;
            yDelta = y;
            
            // Calculate x and y co-ordinates,
            // because we only receive delta values in v4
            
            x = _fing[fid].last_x + x;
            y = _fing[fid].last_y + y;
            break;
            
        case 3:
            if (!_drag.is3FDrag)
                break;
        case 1:
        case 0:
            xDelta = (x - _fing[fid].last_x);
            yDelta = (y - _fing[fid].last_y);
            break;
    }
    
    //
    // Increment the processed packets count till threshold
    //
    
    if (_packetsCount < 255 && !(_packetsCount > 5 && xDelta == 0 && yDelta == 0))
        _packetsCount++;
    
    //
    // Smooth delta
    //

    if (_packetsCount != 0 && _pSmoothSamples > 0 && _fing[fid].pressure >= _tSenseLvl)
    {
        // Reset samples on direction change
        SInt8 lSample = (_cSample == 0)?_pSmoothSamples - 1:_cSample - 1;
        if ((_lastSample[lSample].lastDX1 > 0 && xDelta < 0) ||
            (_lastSample[lSample].lastDX1 < 0 && xDelta > 0) ||
            (_lastSample[lSample].lastDY1 > 0 && yDelta < 0) ||
            (_lastSample[lSample].lastDY1 < 0 && yDelta > 0))

        {
            DEBUG_1F_LOG("STD :: Direction reset for LXD %d LYD %d, XD %d YD %d, Last Sample Indx %d\n", _lastSample[lSample].lastDX1, _lastSample[lSample].lastDY1, xDelta, yDelta, lSample);

            _cSample = 0;
            for (int s = 0; s < _pSmoothSamples; s++) {
                _lastSample[s].lastDX1 = 0;
                _lastSample[s].lastDY1 = 0;
            }
        }
        
        _fing[fid].tdx += xDelta;
        _fing[fid].tdy += yDelta;
        
        _lastSample[_cSample].lastDX1 = xDelta;
        _lastSample[_cSample].lastDY1 = yDelta;
        
        if (xDelta != 0 || yDelta != 0)
            _cSample++;
        
        if (_cSample == _pSmoothSamples)
            _cSample = 0;
        
        _fing[fid].sdx = 0;
        _fing[fid].sdy = 0;
        for (int s = 0; s < _pSmoothSamples; s++) {
            _fing[fid].sdx += _lastSample[s].lastDX1;
            _fing[fid].sdy += _lastSample[s].lastDY1;
        }
    }
    
    //
    // Convert signed delta values to unsigned for logic and
    // set the direction.
    //
    
    if (xDelta < 0) {
        us_dx = 0 - xDelta;
        xDir = -1;
    }
    else if (xDelta > 0) {
        us_dx = xDelta;
        xDir = 1;
    }
    
    if (yDelta < 0) {
        us_dy = 0 - yDelta;
        yDir = -1;
    }
    else if (yDelta > 0) {
        us_dy = yDelta;
        yDir = 1;
    }
    
    DEBUG_1F_LOG("STD :: %dF Touch X %d Y %d, SX %d SY %d, LX %d LY %d, DX %d DY %d, MXD %d, MYD %d, LXD %d, LYD %d, Pressure %d Max %d, Width %d, Button Data %d State %d, PacketsCount %d, TouchTime %lld, TapTime %lld, FHoldTime %lld FPressTime %lld, FOnBtnArea %d BtnTriggered %d, Dragging %s, TMode %d, Last Finger %d, ContMove %d, QTap %d, FID %d\n", _cFingers, x, y, _fing[fid].start_x, _fing[fid].start_y, _fing[fid].last_x, _fing[fid].last_y, xDelta, yDelta, _scroll.totalDx, _scroll.totalDy, _pDx, _pDy, _fing[fid].pressure, _tap.maxPressDtd, _fing[fid].width, _button.data, _button.state, _packetsCount, _gestureTime, (_gestureTime - _tap.lastTime), _fingPress.holdTime, _fingPress.gestureTime, _button.isFingOnBtnArea, _button.triggered, _drag.isDragging?"Yes":"No", _touchmode, _lastFingers, _pContinuousMove, _drag.isQuickTap, fid);
    
    // ------------------------------------------------------------------------------

    //
    // Process Edge Swipes if x or y starts from the edge area
    //

    if ((_touchmode > 25 &&  _touchmode < 30) ||
        
        (_swipe.isEdgeGesture && _swipe.edgeFingers < 2 && !_button.triggered
         && _touchmode != MODE_CIR_VSCROLL && _scroll.totalDx < 30 && _scroll.totalDy < 30
         && !(x > (_xmax - _scroll.edgeVArea) && _fing[fid].start_x > (_xmax - _scroll.edgeVArea) && _scroll.hasEdgeScroll)
         && !(y > (_ymax - _scroll.edgeHArea) && _fing[fid].start_y > (_ymax - _scroll.edgeHArea) && _scroll.hasEdgeScroll)))
    {
        
        int u_sxDelta = 0, u_syDelta = 0;
        
        //
        // Get unsigned delta values from start x and y
        //
        
        u_sxDelta = _fing[fid].start_x - x;
        if (u_sxDelta < 0)
            u_sxDelta = 0 - u_sxDelta;
        
        u_syDelta = _fing[fid].start_y - y;
        if (u_syDelta < 0)
            u_syDelta = 0 - u_syDelta;
        
        DEBUG_1F_LOG("STD :: Swipe DX %d, DY %d\n", u_sxDelta, u_syDelta);
        
        //
        // Check whether the gesture has reached edge swipe conditional
        //
        
        if (((_swipe.topEdge > 0 || _swipe.bottomEdge > 0)
             && u_sxDelta < u_syDelta/2 && u_syDelta > _swipe.edgeVDistance/2)
            
            || ((_swipe.rightEdge > 0 || _swipe.leftEdge > 0)
                && u_syDelta < u_sxDelta/2 && u_sxDelta > _swipe.edgeHDistance/2))
        {
            Process_edgeSwipe(x, y, fid);
        }
        
        //
        // Return either if its in swipe mode (or)
        // delta for HSwipe was less than _scroll.edgeHArea to avoid pointer move.
        //
        
        if (_touchmode > 25 || u_syDelta < _scroll.edgeHArea) {
            
            // Stop enhanced pointer accell
            if (_pContinuousMove) {
               StopPointerEnhanceTimer();
            }
            return;
        }
    }
    
    // ------------------------------------------------------------------------------

    //
    // Process Corner Taps
    //

    if (_cornerTap.enabled && !_button.triggered
        && _gestureTime <= _tap.timeout
        && _lastFingers == 0 && _packetsCount > 2)
    {
        // Top Left Corner
        if (x < (_xmin + _cornerTap.tapAreaLength) && y < (_ymin + _cornerTap.tapAreaHeight)) {
            _cornerTap.corner = 1;
        }
        
        // Botton Left Corner
        else if (x < (_xmin + _cornerTap.tapAreaLength) && y > (_ymax - _cornerTap.tapAreaHeight)
                 && !_cornerTap.bottomCornersDisabled)
        {
            _cornerTap.corner = 2;
        }
        
        // Bottom Right Corner
        else if (x > (_xmax - _cornerTap.tapAreaLength) && y > (_ymax - _cornerTap.tapAreaHeight)
                 && _fing[fid].start_y > (_ymax - _scroll.edgeHArea)
                 && us_dx < 15 && us_dy < 15
                 && !_cornerTap.bottomCornersDisabled)
        {
            _cornerTap.corner = 3;
        }
        
        // Top Right Corner
        else if (x > (_xmax - _cornerTap.tapAreaLength) && y < (_ymin + _cornerTap.tapAreaHeight)
                 && _fing[fid].start_y < (_ymin + _scroll.edgeHArea)
                 && us_dx < 15 && us_dy < 15)
        {
            _cornerTap.corner = 4;
        }
        
        else if (_cornerTap.corner > 0) {
            _cornerTap.corner = 0;
            _touchmode = MODE_IDLE;
        }
        
        if (_cornerTap.corner > 0) {
            _touchmode = MODE_CORNER_TAP;
            dispatchRelativePointerPosition(0, 0, _button.state);
            
            DEBUG_1F_LOG("STD :: Corner Tap Length %d, Height %d\n", _cornerTap.tapAreaLength, _cornerTap.tapAreaHeight);
            
            DEBUG_1F_LOG("STD :: %s Corner Tap triggered.\n",
                         (_cornerTap.corner == 1)?"Top Left":
                         ((_cornerTap.corner == 2)?"Bottom Left":
                          ((_cornerTap.corner == 3)?"Bottom Right":"Top Right")));
            return;
        }
    }
    // Clear corner tap value after its threshold
    else if ((_gestureTime > _tap.timeout || _button.triggered) && _cornerTap.corner > 0) {
        _cornerTap.corner = 0;
        _touchmode = MODE_IDLE;
    }
    
    // ------------------------------------------------------------------------------

    //
    // Process touchpad modes
    //

    switch (_touchmode) {
            
        default:
            break;
            
            //
            // No processing required to already triggered gestures
            //
            
        case MODE_2_FING_TAP:
        case MODE_3_FING_TAP:
        case MODE_4_FING_TAP:
        case MODE_5_FING_TAP:
        case MODE_VSCROLL:
        case MODE_HSCROLL:
            
            //
            // Clear Multi touch modes on packets count > 2 for 1F touch.
            // otherwise return.
            //
            
            if (_packetsCount > 4) {
                _touchmode = MODE_IDLE;
                break;
            }
            
        case MODE_MUL_TOUCH_3:
        case MODE_MUL_TOUCH_4:
        case MODE_5_FING_PINCH:
        case MODE_TWO_FING_PRESS:
        case MODE_THREE_FING_PRESS:
            // Continue for 3f drag
            if (_drag.is3FDrag)
                break;
        case MODE_FOUR_FING_PRESS:
        case MODE_FIVE_FING_PRESS:
        case MODE_ZOOM:
        case MODE_ROTATE:
        case MODE_TOP_EDGE_SWIPE:
        case MODE_BOTTOM_EDGE_SWIPE:
        case MODE_LEFT_EDGE_SWIPE:
        case MODE_RIGHT_EDGE_SWIPE:
        case MODE_CORNER_TAP:
            
            //
            // Stop the enhanced scroll accleration if its active
            //
            
            StopScrollEnhanceTimer();
            DEBUG_1F_LOG("STD :: Skipping\n");
            return;
    }
   
    // ------------------------------------------------------------------------------
    
    //
    // Check for the Double Tap to drag based on last tap time
    //

    uint64_t curTouchTime = 0;
#if APPLESDK
    clock_get_uptime(&curTouchTime);
#else
    clock_get_uptime((uint64_t*)&curTouchTime);
#endif
    
    if (_cFingers == 1 && _lastFingers == 0 && !_button.triggered && _drag.isQuickTap)
    {
        if ((curTouchTime - _tap.lastTime) > _tap.timeout)
        {
            _tap.lastTime = 0;
            _drag.isQuickTap = false;
            DEBUG_1F_LOG("STD :: Quick tap detection timeout.\n");
        }
        else if ((curTouchTime - _tap.lastTime) <= _tap.timeout)
        {
            if (_isDragEnabled && _drag.checkTapDrag) {
                _button.data = 0x1;
                _button.state |= _button.data;
                _drag.isTapDrag = _drag.isDragging = true;
                _doTimerJob   = false;
                _buttonDragTimer->cancelTimeout();
                DEBUG_1F_LOG("STD :: Double tap Drag Started.\n");
            }
            else {
                _button.data = 0;
                _button.state &= 0x6; // Clear bit 0
                // _drag.isQuickTap = false;
                DEBUG_1F_LOG("STD :: Quick tap, but drag not enabled.\n");
            }
        }
        
        if (_drag.checkTapDrag)
            _drag.checkTapDrag = false;
    }
    else if (_cFingers > 1 && _drag.isQuickTap)
    {
        _drag.isQuickTap = false;
    }
    
    // ------------------------------------------------------------------------------
    
    //
    // Process Edge scroll modes
    //

    if (_packetsCount > 1
        &&  _scroll.hasEdgeScroll && !_button.triggered && _touchmode != MODE_MOVE )
    {
        
        //
        //  If the finger is outsdie the edge scroll area then
        //  set the scroll mode to 'circular' when circular is enabled, otherwise "move".
        //
        
        switch (_touchmode) {
            case MODE_EDGE_VSCROLL:
                if (x > (_xmax - (_scroll.edgeVArea + _scroll.edgeVAdaptArea))) {
                    break;
                }
                else if (!(_scroll.totalDy > MIN_ESCROLL_DELTA))
                {
                    _touchmode = MODE_MOVE;
                    break;
                }
                else {
                    // pass through for circular scroll detection
                }
            case MODE_CIR_VSCROLL:
                if (_scroll.hasEdgeCScroll)
                    _touchmode = MODE_CIR_VSCROLL;
                else
                    _touchmode = MODE_MOVE;
                break;
                
            case MODE_EDGE_HSCROLL:
                if (y > (_ymax - (_scroll.edgeHArea + _scroll.edgeHAdaptArea))) {
                    break;
                }
                else if (!(_scroll.totalDx > MIN_ESCROLL_DELTA)) {
                    _touchmode = MODE_MOVE;
                    break;
                }
                else {
                    // pass through for circular scroll detection
                }
            case MODE_CIR_HSCROLL:
                if (_scroll.hasEdgeCScroll)
                    _touchmode = MODE_CIR_HSCROLL;
                else
                    _touchmode = MODE_MOVE;
                break;
                
            default:
                if (x > (_xmax - _scroll.edgeVArea)
                    && _fing[fid].start_x > (_xmax - _scroll.edgeVArea))
                {
                    _touchmode = MODE_EDGE_VSCROLL;
                }
                
                else if (y > (_ymax - _scroll.edgeHArea)
                         && _fing[fid].start_y > (_ymax - _scroll.edgeHArea))
                {
                    _touchmode = MODE_EDGE_HSCROLL;
                }
                else {
                    _touchmode = MODE_MOVE;
                }
                break;
        }
        
        //
        // Stop the enhanced ponter accleration if its active
        //
        
        if (_pContinuousMove) {
            StopPointerEnhanceTimer();
        }
        
        //
        // Skip processing edge vscroll if the below condition is true
        // which happens when edge swipe is performed.
        //
        
        if (_touchmode == MODE_EDGE_VSCROLL
            && (_packetsCount < 50 || (xDelta == 0 && yDelta == 0))
            && ((us_dx >= us_dy && us_dx > 0) ||
                (us_dy > us_dx && (us_dy - us_dx) < 50 && us_dx > 10 && us_dy > 20)))
        {
            // IOLog("STD :: Edge swipe and scroll conflict.\n");
            if (_scroll.doCont)
                StopScrollEnhanceTimer();
            return;
        }
        
        //
        // Stop the enhanced scroll and ponter accleration if its active
        //
        
        if (_touchmode == MODE_MOVE) {
            StopScrollEnhanceTimer();
        }
    }
    
    // ------------------------------------------------------------------------------

    //
    // Process Pointer Move and Edge Scroll
    //

    bool isTapGesture = false;
    
    switch (_touchmode) {
        case MODE_1_FING_TAP: // Single finger tap
        case MODE_BUTTONS: // For dragging with left button
        case MODE_MOVE:
        case MODE_IDLE:
            
            if (_isDragEnabled &&
                
                (_drag.isTapDrag
                 
                 || (_drag.hasFPresDrag && !_rotate.isRotateGesture
                     
                     && ((_fingPress.gestureTime > _drag.fPressDragStartTimeOut && _drag.fPressDragFingersNum & 0x1)
                         || _drag.isHoldDrag))))
            {
                _button.state |= 0x1;
                
                // If its not tap drag then set hold drag
                if (!_drag.isTapDrag && !_drag.isHoldDrag) {
                    DEBUG_1F_LOG("STD :: Hold drag detected (%d)\n", _drag.isHoldDrag);
                    
                    _drag.isHoldDrag = true;
                    _drag.isDragging = true;
                }
                if (_drag.is3FDrag) {
                    DEBUG_1F_LOG("STD :: 3F drag detected\n");
                }
                else {
                    DEBUG_1F_LOG("STD :: %s drag triggered\n", _drag.isTapDrag?"Double tap":"Tap and Hold drag");
                }
            }
            // Button detected ?
            else if (_button.triggered)
            {
                _button.state |= _button.data;
                DEBUG_1F_LOG("STD :: Button %d is on hold\n", _button.state);
            }
            // No buttons
            else {
                _button.data = 0;
                
                if (_drag.isDragging)
                    _button.state |= 0x1;
                
                DEBUG_1F_LOG("STD :: No button (Dragging %s)\n", _drag.isDragging?"Yes":"No");
            }
            
            //
            // Corner tapped and _cornerTap.releaseStartABS is enabled ?
            // Then start the pointer from absolute position of the screen after corner tap
            //
            
            if (_cornerTap.corner > 0 && _cornerTap.releaseStartABS)
            {
                IOGPoint  boundsABS = { 0, 0 };
                IOGBounds boundsPAD = { static_cast<SInt16>(_xmin), static_cast<SInt16>(_xmax),
                    static_cast<SInt16>(_ymin), static_cast<SInt16>(_ymax)};
                
                boundsABS.x = x;
                boundsABS.y = y;
                
                clock_get_uptime(TIME_NOW);
                dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, _button.state, true, 0, 30, 160, 0, _now);
                _cornerTap.corner = 0;
                
                // Clear continuous move if its active
                if (_pContinuousMove)
                {
                    StopPointerEnhanceTimer();
                }
            }
            
            //
            // Dispatch pointer positions
            //
            
            else
            {
                //
                // Is it tap gesture ?
                // Note: Commented code for working tap to release in Drag timeout,
                // so enable this back and debug if any gestures have issues.
                //
                // Skip for 3f drag with 3-2-1 transitions.
                //
                // High sensitivy touchpads may generate big deltas for low pressure values,
                // so if pressure < 10 and max Press reached above tap Press then ignore.
                //
                
                if (_gestureTime < _tap.clickTimeout
                    && (!_drag.is3FDrag || (_cFingers == 1 && _lastFingers ==0))
                    /*&& (!_drag.isDragging || (_drag.isDragging && _isDragLockEnabled) || _drag.isQuickTap)*/)
                {
                    if (!_button.triggered &&
                        (_fing[fid].pressure >= _tap.pressure || _tap.maxPressDtd >= _tap.pressure ||
                         (xDelta == 0 && yDelta == 0)) &&
                        
                        ((_packetsCount < 11 && us_dx < _tap.noiseDelta/2 && us_dy < _tap.noiseDelta/2) ||
                         (_packetsCount < 11 && _fing[fid].pressure < _tap.maxTEndPressure) ||
                         (_packetsCount < 6 && us_dx < _tap.noiseDelta && us_dy < _tap.noiseDelta)))
                    {
                        isTapGesture = true;

                        if (_touchpadVendor == STD_SYNAPTICS && (xDelta != 0 || yDelta != 0))
                        {
                            //
                            // Note: For Synaptics touchpad (Seen in v7.5 fw)
                            // Special case which happens on very quick taps as we don't recceive end packets
                            // between them, but pressure < width in those cases.
                            //

                            // New tap begins
                            if (_synTPad.touch_ended && _fing[fid].pressure >= _tap.pressure)
                            {
                                _synTPad.touch_ended = false;
                            }
                            // Tap ended on first tap (_drag.isQuickTap = true for second tap)
                            else if (!_synTPad.touch_ended && !_drag.isQuickTap && _fing[fid].pressure < _fing[fid].width)
                            {
                                _touchmode = MODE_1_FING_TAP;
                                Process_touch_end();
                                _synTPad.touch_ended = true;
                                break;
                            }
                        }
                    }
                }
                
                //
                // Divisive smoothing using x samples delta + avg of last x samples
                //
                
                if (xDelta != 0 || yDelta != 0)
                {
                    int deltaDX = 0, deltaDY = 0;
                    int smoothDx = 0, smoothDy = 0;

                    if (_pLastDx != 0)
                        deltaDX = _pLastDx - xDelta;
                    
                    if (_pLastDy != 0)
                        deltaDY = _pLastDy - yDelta;

                   // IOLog("STD :: : DeltaX %d, DeltaY %d, ", deltaDX, deltaDY);

                    _pLastDx = xDelta;
                    _pLastDy = yDelta;

                    int absDeltaX = getAbsoluteValue(xDelta);
                    int absDeltaY = getAbsoluteValue(yDelta);
                    
                    if (absDeltaX > 0
                        && !(absDeltaX < _pSlowMoveNoiseDelta && absDeltaY > _pSlowMoveNoiseDelta * 2))
                    {
                        if (_pSmoothSamples > 0) {
                            smoothDx = (_fing[fid].sdx/_pSmoothSamples) + xDir;
                        }
                        else {
                            smoothDx = xDelta;
                        }
                    }
                    else if (xDelta != 0)
                        smoothDx = _pDx;
                    
                    if (absDeltaY > 0
                        && !(absDeltaY < _pSlowMoveNoiseDelta && absDeltaX > _pSlowMoveNoiseDelta * 2))
                    {
                        if (_pSmoothSamples > 0) {
                            smoothDy = (_fing[fid].sdy/_pSmoothSamples) + yDir;
                        }
                        else {
                            smoothDy = yDelta;
                        }
                    }
                    else if (yDelta != 0)
                        smoothDy = _pDy;


                    //
                    // Exponential acceleration
                    //
                    
                    if (_pExtAccelLevel > 0) {
                        
                        if (_pSmoothX == 0 && _pSmoothY == 0) {
                            _pSmoothX = smoothDx;
                            _pSmoothY = smoothDy;
                        }
                        
                        _pDx = smoothDx;
                        _pDy = smoothDy;
                        
                        UInt16 deltaToIgnore = _pExtAccelMinDelta * 2;
                        
                        if (smoothDx != 0 && getAbsoluteValue(xDelta) > deltaToIgnore) {

                            smoothDx = (smoothDx + _pSmoothX + (xDir * 2))/2;

                            SInt16 expDx = smoothDx/10 + xDir;

                            deltaDX = deltaDX + xDir;
                            deltaDX = deltaDX/((_pExtAccelLevel + 1) * 10);
                            deltaDX++;
                            
                            _pDx = smoothDx + (expDx * deltaDX) ;
                        }
                        
                        if (smoothDy != 0 && getAbsoluteValue(yDelta) > deltaToIgnore) {
                            
                            smoothDy = (smoothDy + _pSmoothY + (yDir * 2))/2;

                            SInt16 expDy = smoothDy/10 + yDir;

                            deltaDY = deltaDY + yDir;
                            deltaDY = deltaDY/((_pExtAccelLevel + 1) * 10);
                            deltaDY++;
                            
                            _pDy = smoothDy + (expDy * deltaDY);
                        }
                        
                        DEBUG_1F_LOG("STD :: Exp Accel DX %d, DY %d, Last DX %d DY %d, SDX %d SDY %d, UDX %d UDY %d, DirX %d Y %d, Touch %lld, Diff dx %d dy %d, SlowMoveFilter %d\n", xDelta, yDelta, _pLastDx, _pLastDy, smoothDx, smoothDy, _pDx, _pDy, xDir, yDir, _gestureTime, deltaDX, deltaDY, _pSlowMoveNoiseDelta);
                        
                        
                        _pSmoothX = smoothDx;
                        _pSmoothY = smoothDy;
                    }
                    else {
                        _pDx = smoothDx;
                        _pDy = smoothDy;
                    }
                }
                else if (xDelta == 0 && yDelta == 0 && _pContinuousMove)
                {
                    StopPointerEnhanceTimer();
                }
                
                //
                // No pointing (only buttons) for disabled edge areas.
                //
                
                if ((_tpLeftEdgeDArea && x < (_xmin + _tpLeftEdgeDArea)) ||
                    (_tpRightEdgeDArea && x > (_xmax - _tpRightEdgeDArea)) ||
                    (_tpTopEdgeDArea && y < (_ymin + _tpTopEdgeDArea)) ||
                    (_tpBottomEdgeDArea && y > (_ymax - _tpBottomEdgeDArea)))
                {
                    isTapGesture = false;
                    dispatchRelativePointerPosition(0, 0, _button.state);
                    
                    if (_pContinuousMove)
                     StopPointerEnhanceTimer();
                    
                    DEBUG_1F_LOG("STD :: Disabled edge area found, processing ignored.\n");
                }
                
                //
                // To avoid cursor jumps:
                // Initialize relative position with 0 on first packet,
                // and when tap drag gets initiated in quick taps till its timeout,
                // and when delta x & y < _tap.noiseDelta && delta x & y > 350
                // which might happen for quick taps or accidental inputs.
                //
                
                else if ((!_button.triggered || _packetsCount <= 2) &&
                    (_drag.isQuickTap || (us_dx > 350 || us_dy > 350) || isTapGesture))
                {
                    dispatchRelativePointerPosition(0, 0, _button.state);
                    _pDx = _pDy = 0;
                    
                    // Rollback to last sample as this is not valid
                    if (_cSample > 0) {
                        _cSample--;
                    }
                    else if (_cSample == 0) {
                        _cSample = _pSmoothSamples - 1;
                    }
                    
                    if (xDelta != 0 || yDelta != 0)
                    {
                        _lastSample[_skipSample].skippedX = xDelta;
                        _lastSample[_skipSample].skippedY = yDelta;
                        
                        if (_skipSample < 10)
                            _skipSample++;
                    }
                    
                    DEBUG_1F_LOG("STD :: Initiating pointer relative position for XD %d LXD %d, YD %d LYD %d, Accel %d, _drag.isTapDrag %d, Pressure %d Max %d, Tap %d\n", xDelta, _pDx, yDelta, _pDy, _pAccelCustom, _drag.isTapDrag, _fing[fid].pressure, _tap.maxPressDtd, isTapGesture);
                }
                else if ((_pDx != 0 || _pDy != 0) && (xDelta != 0 || yDelta != 0) && _fing[fid].pressure >= _tSenseLvl)
                {
                    
                   DEBUG_1F_LOG("STD :: Dispatching pointer positions (X %d, Y %d) XD %d LXD %d, YD %d LYD %d, Direction X(%d) %d, Y(%d) %d, Accel %d, Pressure %d Max %d, Packets %d, Samples %d Current %d, Last Avg X %d Y %d, CMove %d\n", us_dx, us_dy, xDelta, _pDx, yDelta, _pDy, xDir, _scroll.xPacketsDir, yDir, _scroll.yPacketsDir, _pAccelCustom, _fing[fid].pressure, _tap.maxPressDtd, _packetsCount, _pSmoothSamples, _cSample, _pSmoothX, _pSmoothY, _pContinuousMove);
                    
                    //
                    // Dispatch the skipped delta values from tap detection timeout
                    //
                    
                    if (_skipSample > 0)
                    {
                        for (int i = 1; i < _skipSample; i++) {
                            DEBUG_1F_LOG("STD :: Processing %d skipped SXD %d SYD %d, PC %d, Skipped %d\n", i+1, _lastSample[i].skippedX, _lastSample[i].skippedY, _packetsCount, _skipSample);
                            
                            dispatchRelativePointerPosition(_lastSample[i].skippedX, _lastSample[i].skippedY, _button.state);
                            IOSleep(1);
                            _lastSample[i].skippedX = _lastSample[i].skippedY = 0;
                        }
                        _lastSample[0].skippedX = _lastSample[0].skippedY = 0;
                        _skipSample = 0;
                    }
                    
                    //
                    // Use custom acceleration ?
                    //
                    
                    if (_pAccelCustom > 0)
                    {
                        dispatchRelativePointerPosition(_pDx, _pDy , _button.state);
                        
                        if (!_pContinuousMove) {
                            _pContinuousMove = true;
                            _pointerTimer->setTimeoutMS(0);
                        }
                    }
                    // Standard
                    else {
                        DEBUG_1F_LOG("STD :: XD %d LXD %d, YD %d LYD %d State %d\n", xDelta, _pDx, yDelta, _pDy, _button.state);
                        dispatchRelativePointerPosition(_pDx, _pDy , _button.state);
                    }
                }
                else {
                    DEBUG_1F_LOG("STD :: Skipped on XD %d LDX %d YD %d LDY %d, PC %d\n", xDelta, _pDx, yDelta, _pDy, _packetsCount);
                }
                
            }
            
            // Set mode to Move
            if (_packetsCount > 1) {
                _touchmode = MODE_MOVE;
            }
            
            //
            // Calculate finger press
            //
            
            if (_fing[fid].pressure > _drag.fPressDragPressure && !_drag.isDragging && !_button.triggered
                && _gestureTime < (_drag.fPressDragStartTimeOut + 3000000000)// 3 sec + timeout to detect
                && (xDelta < 10 && xDelta > -10 && yDelta < 10 && yDelta > -10)
                && x < (_fing[fid].start_x + 15) && x > (_fing[fid].start_x - 15)
                && y < (_fing[fid].start_y + 25) && y > (_fing[fid].start_y - 25))
            {
                _fingPress.gestureTime = _gestureTime;
            }
            else if (!_drag.isHoldDrag && _fingPress.gestureTime != 0) {
                _fingPress.gestureTime = 0;
            }
            
            //
            // Tap detection
            //
            
            if (_lastFingers < 2 && _packetsCount > 1 && isTapGesture)
            {
                _touchmode = MODE_1_FING_TAP;
            }
            else if (_touchmode == MODE_1_FING_TAP &&
                     (_gestureTime > _tap.clickTimeout || !isTapGesture))
            {
                _touchmode = MODE_IDLE;
            }
            
            // Set max pressure level it reached (Ignore Focaltech as we dont know pressure yet)
            if (_touchpadVendor != STD_FOCAL_TECH
                && ((_fing[fid].pressure >= _tap.maxPressDtd) || _tap.maxPressDtd == 0))
            {
                _tap.maxPressDtd = _fing[fid].pressure;
            }
            break;
            
        case MODE_CONT_SCROLL:
        case MODE_EDGE_VSCROLL:
        case MODE_EDGE_HSCROLL:

            //
            // Stop the enhanced pointer accleration if its active
            //
            
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            
            if (_packetsCount == 1) {
                yDelta = xDelta = 0;
            }
            
            //
            // Choose scroll type
            //
            
            if (_touchmode == MODE_EDGE_VSCROLL)
            {
                _scroll.edgeCirDxy = _scroll.dy;
            }
            else {
                _scroll.edgeCirDxy = _scroll.dx;
            }
            
            //
            // Process fingers hold
            //
            
            if (us_dx < _scroll.idleFingMaxDelta && us_dy < _scroll.idleFingMaxDelta)
            {
                clock_get_uptime(TIME_NOW);
                
                if (_fingPress.lastHoldTime == 0)
                    _fingPress.lastHoldTime = absoluteToUINT64(_now);
                
                else if (_fingPress.holdTime < _scroll.slingStartTimeout
                         || _fingPress.holdTime <  _scroll.idleFingDetectTimeout)
                {
                    _fingPress.holdTime += absoluteToUINT64(_now) - _fingPress.lastHoldTime;
                    _fingPress.lastHoldTime = absoluteToUINT64(_now);
                }
            }
            else {
                _fingPress.lastHoldTime = absoluteToUINT64(_now);
                _fingPress.holdTime = 0;
            }
            
            //
            // Check for sling effect
            //
            
            if (_scroll.hasEdgeSling &&  _scroll.maxLevelReached != 0) {
                
                if (!(y > (_ymax - _scroll.slingTriggerArea) || y < (_ymin + _scroll.slingTriggerArea)))
                {
                    _scroll.doSling = false;
                    _scroll.slingDepth = 0;
                }
                else if (!_scroll.doSling && _fingPress.holdTime > _scroll.slingStartTimeout)
                {
                    _scroll.doSling = true;
                    
                    if (_scroll.slingDepth == 0) {
                        
                        if (_scroll.maxLevelReached == 0)
                             _scroll.maxLevelReached = 1;
                        
                        // Con_sidering bottom finger release
                        if (y > (_ymax - _scroll.slingTriggerArea))
                            _scroll.slingDepth = 1 *  _scroll.maxLevelReached;
                        
                        // Con_sidering top finger release
                        else if (y < (_ymin + _scroll.slingTriggerArea))
                            _scroll.slingDepth = -1 *  _scroll.maxLevelReached;
                    }
                }
            }
            
            // Set pressure
            _lastSample[_scrollSample].pressure = _fing[fid].pressure;
            
            //
            // Igore packetts with low pressure
            // which are generated by the high sensitivity of synaptics touchpad
            // at the end.
            //
            
            if (/*_touchpadVendor == STD_SYNAPTICS
                && */(_fing[fid].pressure < _tSenseLvl || _fing[fid].pressure < _tap.maxTEndPressure))
            {
                //
                // Collect and process last 5 samples of scroll delta values
                // which are needed to control the scroll.
                //
                
                if (xDelta < 0)
                    _lastSample[_scrollSample].lastSDX = 0 - xDelta;
                else
                    _lastSample[_scrollSample].lastSDX = xDelta;
                
                if (yDelta < 0)
                    _lastSample[_scrollSample].lastSDY = 0 - yDelta;
                else
                    _lastSample[_scrollSample].lastSDY = yDelta;
                
                if (_scrollSample < 4)
                    _scrollSample++;
                else
                    _scrollSample = 0;
                
                break;
            }
            else {
                // Process scroll
                Process_scroll(_scroll.accelModeEdge, xDelta, yDelta, false);
            }
            break;
            
            
        case MODE_CIR_VSCROLL:
        case MODE_CIR_HSCROLL:
            
            //
            // process finger hold
            //
            
            if (_fingPress.holdTime <  _scroll.idleFingDetectTimeout
                && us_dy < _scroll.idleFingMaxDelta && us_dx < _scroll.idleFingMaxDelta && _scroll.edgeCirDxy != 0)
            {
                clock_get_uptime(TIME_NOW);
                
                if (_fingPress.lastHoldTime == 0)
                    _fingPress.lastHoldTime = absoluteToUINT64(_now);
                else {
                    _fingPress.holdTime += (absoluteToUINT64(_now) - _fingPress.lastHoldTime);
                    _fingPress.lastHoldTime = absoluteToUINT64(_now);
                }
            }
            else {
                //
                // Stop scroll when fingers stop
                // And reset scroll level on idle fingers hold (0.02 sec)
                //
                if (_fingPress.holdTime >  _scroll.idleFingDetectTimeout)
                {
                    if (!_scroll.hasEdgeCont) {
                        _scroll.edgeCirDxy = 0;
                    }
                    _scroll.noInertia = true;
                }
                _fingPress.lastHoldTime = absoluteToUINT64(_now);
                _fingPress.holdTime = 0;
            }
            
            // Process scroll
            if (_scroll.edgeCirDxy != 0 || xDelta != 0 || yDelta != 0)
            {
                Process_scroll(_scroll.accelModeEdge, xDelta, yDelta, false);
            }
            else if (_scroll.doCont && !_scroll.hasEdgeCont) {
                _scroll.doCont = false;
                _scroll.dx = _scroll.dy = 0;
                _scroll.edgeCirDxy = 0;
                _scrollTimer->cancelTimeout();
            }
            break;
            
        default:
            break;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes two finger touch.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_twofingers_touch(int x1, int x2, int y1, int y2)
{
    
    // signed x and y diff from last x & y values
    int dx1 = 0, dy1 = 0;
    int dx2 = 0, dy2 = 0;
        
    // signed x and y diff from start x & y values
    int x1_Dx = 0, y1_Dy = 0, x2_Dx = 0, y2_Dy = 0;
    
    //
    // Increment the processed packets count value
    // until max required threshold
    //
    
    if (_packetsCount < 255)
        _packetsCount++;
    
    //
    // If secondary finger id is negative then fix
    //
    
    if (_sid < 0) {
        switch (_pid) {
            case 0:
                _sid = 1;
                break;
                
            case 1:
                _sid = 0;
                break;
        }
    }
    
    //
    // Calculate the delta values from the coordinates
    //
    
    dx1 = x1 - _fing[_pid].last_x;
    dy1 = y1 - _fing[_pid].last_y;
    
    dx2 = x2 - _fing[_sid].last_x;
    dy2 = y2 - _fing[_sid].last_y;
    
    //
    // Calculate signed diff of x1, x2, y1 and y2 from the start coordinate value
    // and convert them to unsigned
    //
    
    x1_Dx = _fing[_pid].start_x - x1;
    y1_Dy = _fing[_pid].start_y - y1;
    x2_Dx = _fing[_sid].start_x - x2;
    y2_Dy = _fing[_sid].start_y - y2;
    

    //
    // Calculate delta between x1 & x1 and y1 & y2 from start value
    //
    
    _zoom.zDx = (_fing[_pid].start_x - _fing[_sid].start_x)  - (x1 - x2);
    _zoom.zDx = abs(_zoom.zDx);
    
    _zoom.zDy = (_fing[_pid].start_y - _fing[_sid].start_y) - (y1 - y2);
    _zoom.zDy = abs(_zoom.zDy);
    
    DEBUG_2F_LOG("STD :: 2F X1 %d, X2 %d, Y1 %d, Y2 %d, SX1 %d, SX2 %d, SY1 %d, SY2 %d, SDX1 %d, SDY1 %d, SDX2 %d, SDY2 %d, ZDX %d, ZDY %d, X1X2 %d Y1Y2 %d, RM %s ZIN %s ZOUT %s, ZRSamples %d PCounts %d, P1 %d P2 %d, W1 %d W2 %d, Time %lld, FHold %lld, TMode %d, Delta R %d Z %d Scale %d, ESwipe %s, MLvl %d, MDX %d, MDY %d, _tSenseLvl %d, LF %d\n", x1, x2, y1, y2, _fing[_pid].start_x, _fing[_sid].start_x, _fing[_pid].start_y, _fing[_sid].start_y,x1_Dx, y1_Dy, x2_Dx, y2_Dy, _zoom.zDx, _zoom.zDy, (dx1-dx2), (dy1-dy2), _rotate.isRotateGesture?"Yes":"No", _zoom.isZoomIn?"Yes":"No", _zoom.isZoomOut?"Yes":"No", _zoom.minSamples, _packetsCount, _fing[_pid].pressure, _fing[_sid].pressure, _fing[_pid].width, _fing[_sid].width, _gestureTime, _fingPress.gestureTime, _touchmode, _rotate.delta, _zoom.delta, _zoom.pinchDeltaScale, _swipe.isEdgeGesture?"Yes":"No", _scroll.maxLevelReached, _scroll.totalDx, _scroll.totalDy, _tSenseLvl, _lastFingers);
    
    //
    // Igore packetts with low pressure
    // which are generated by the high sensitivity of synaptics touchpad
    // at the end.
    //

    if (/*_touchpadVendor == STD_SYNAPTICS &&*/
        ((_fing[_pid].pressure < _tap.maxTEndPressure && _fing[_sid].pressure < _tap.maxTEndPressure) ||
         (_fing[_pid].pressure < _tSenseLvl && _fing[_sid].pressure < _tSenseLvl)))
    {
        switch (_touchmode) {
            case MODE_VSCROLL:
            case MODE_HSCROLL:
            case MODE_CONT_SCROLL:
                _lastSample[_scrollSample].pressure = _fing[_pid].pressure;
                _lastSample[_scrollSample].lastSDX = getAbsoluteValue(dx1);
                _lastSample[_scrollSample].lastSDY = getAbsoluteValue(dy1);
                
                if (_scrollSample < 4)
                    _scrollSample++;
                else
                    _scrollSample = 0;
                
                if (_scroll.dx > 0) _scroll.dx--; else if (_scroll.dx < 0) _scroll.dx++;
                if (_scroll.dy > 0) _scroll.dy--; else if (_scroll.dy < 0) _scroll.dy++;
                break;
                
            default:
                // Reset start to avoid issues with zoom
                if (_packetsCount < 5)
                    _packetsCount = 0;
                break;
        }
        
        DEBUG_2F_LOG("STD :: Ignoring low pressure packet %d, ID %d %d, Pressure %d %d, Sense %d\n", _packetsCount, _pid, _sid, _fing[_pid].pressure, _fing[_sid].pressure, _tSenseLvl);
        
        return;
    }
    
    //
    // Process Edge Swipes
    // if the gesture has reached edge swipe conditionals
    //
    
    if ((_swipe.isEdgeGesture || _touchmode > 25) && !_button.triggered)
    {
        if (((_swipe.topEdge > 0 || _swipe.bottomEdge > 0)
             && abs(x1_Dx) < abs(y1_Dy)/2 && abs(x2_Dx) < abs(y2_Dy)/2
             && abs(y1_Dy) > _swipe.edgeVDistance/2 && abs(y2_Dy) > _swipe.edgeVDistance/2)
            
            || ((_swipe.rightEdge > 0 || _swipe.leftEdge > 0)
                && abs(y1_Dy) < abs(x1_Dx)/2 && abs(y2_Dy) < abs(x2_Dx)/2
                && abs(x1_Dx) > _swipe.edgeHDistance/2 && abs(x2_Dx) > _swipe.edgeHDistance/2))
        {
            Process_edgeSwipe(x1, y1, 0);
            Process_edgeSwipe(x2, y2, 1);
        }
        
        if (!_swipe.has2WayEdgeSwipes || _touchmode > 25) {
            // Stop the enhanced scroll accleration if its active
            StopScrollEnhanceTimer();
            return;
        }
    }
    
    /*
     * it works very well mostly if gesture performed like below for ELAN v3 hardware.
     * Circular Rotate is detected based on the x and y differences
     * if y1-y2 == 0 then both fingers lie on x axis
     * if x1-x2 == 0 then both fingers lie on y axis
     * Circular is detected when fingers start either from x axis to y axis (or) y axis to x axis
     */
    
    if (_hw_version == 3 && _rotate.cirEnabled && _touchmode != MODE_ROTATE && _packetsCount > 6)
    {
        DEBUG_ZRP_LOG("STD :: Processing Circular rotate ...\n");
        
        if ((x1 - x2) == 0) {
            _rotate.rotateXCounter++;
        }
        else if ((y1 - y2) == 0) {
            _rotate.rotateYCounter++;
        }
        
        if (_rotate.xAxisPos == 0 || _rotate.yAxisPos == 0)
        {
            if ((x1-x2) == 0) {
                _rotate.xAxisPos = _rotate.yAxisPos + 1;
            }
            else if ((y1 - y2) == 0) {
                _rotate.yAxisPos = _rotate.xAxisPos + 1;
            }
        }
        
        int prxDelta = 0, pryDelta = 0;
        bool rotateCirLeft = false, rotateCirRight =false;
        
        prxDelta = x1 - x2; if (prxDelta < 0) prxDelta = 0 - prxDelta;
        pryDelta = y1 - y2; if (pryDelta < 0) pryDelta = 0 - pryDelta;
        
        if (_rotate.xAxisPos == 1 && (_rotate.yAxisPos == 2 || pryDelta < 100))
        {
            if (x1_Dx < 0 && x2_Dx > 0) {
                rotateCirLeft = true;
            }
            else if (x1_Dx > 0 && x2_Dx < 0) {
                rotateCirRight = true;
            }
        }
        else if (_rotate.yAxisPos == 1 && (_rotate.xAxisPos == 2 || prxDelta < 100))
        {
            if (y1_Dy < 0 && y2_Dy > 0) {
                rotateCirLeft = true;
            }
            else if (y1_Dy > 0 && y2_Dy < 0) {
                rotateCirRight = true;
            }
        }
        
        if (rotateCirLeft)
        {
            _device->dispatchPS2Notification(kPS2C_RotateLeft);
            DEBUG_ZRP_LOG("STD :: Circular left rotate detected.\n");
            
            _touchmode = MODE_ROTATE;
            _zoom.isZoomOut = _zoom.isZoomIn = false;
            _rotate.isRotateGesture = true;
            
            //
            // Stop enhanced scroll if active
            //
            
            StopScrollEnhanceTimer();
        }
        else if (rotateCirRight)
        {
            _device->dispatchPS2Notification(kPS2C_RotateRight);
            DEBUG_ZRP_LOG("STD :: Circular right rotate detected.\n");
            
            _touchmode = MODE_ROTATE;
            _zoom.isZoomOut = _zoom.isZoomIn = false;
            _rotate.isRotateGesture = true;
            
            //
            // Stop enhanced scroll if active
            //
            
            StopScrollEnhanceTimer();
        }
    }
    
    //
    // Process touchpad modes
    //
    
    switch (_touchmode) {
            
            //
            // Clear 1F modes
            //
            
        case MODE_CONT_SCROLL:
            if (_packetsCount > 3)
                break;
            
        case MODE_MOVE:
            // Dispatch 3f drag and return
            if (_drag.is3FDrag)
            {
                if (_drag.fPressDragFingersID & (_pid + _pid))
                {
                    Process_singlefinger_touch(dx1, dy1, _pid);
                }
                if (_drag.fPressDragFingersID & (_sid + _sid))
                {
                    Process_singlefinger_touch(dx2, dy2, _sid);
                }
                if (_drag.fPressDragFingersID & 1)
                {
                    if (_pid == 0) {
                        Process_singlefinger_touch(dx1, dy1, _pid);
                    }
                    else if (_sid == 0) {
                        Process_singlefinger_touch(dx2, dy2, _sid);
                    }
                }
                return;
            }
        case MODE_1_FING_TAP:
        case MODE_CORNER_TAP: _cornerTap.corner = 0;
        case MODE_EDGE_VSCROLL:
        case MODE_EDGE_HSCROLL:
        case MODE_CIR_VSCROLL:
        case MODE_CIR_HSCROLL:
            
            //
            // Stop the enhanced pointer and scroll if its active
            //
            
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            
            StopScrollEnhanceTimer();
            
            // IOLog("STD :: Clearing 1F\n");
            
            // Reset mode
            if (_packetsCount > 3)
                _touchmode = MODE_IDLE;
            
        default:
            
            //
            // No processing required to already triggered gestures
            // except when its a edge swipe and rotate gesture.
            //
            
            if (!_swipe.isEdgeGesture || (_swipe.isEdgeGesture && _swipe.has2WayEdgeSwipes))
                break;
            
        case MODE_ROTATE:
            if (!_rotate.isRotateGesture)
                break;
            
        case MODE_ZOOM:
        case MODE_TOP_EDGE_SWIPE:
        case MODE_BOTTOM_EDGE_SWIPE:
        case MODE_LEFT_EDGE_SWIPE:
        case MODE_RIGHT_EDGE_SWIPE:
        case MODE_MUL_TOUCH_3:
        case MODE_MUL_TOUCH_4:
        case MODE_5_FING_PINCH:
        case MODE_TWO_FING_PRESS:
        case MODE_THREE_FING_PRESS:
            // Dispatch 3f drag and return
            if (_drag.is3FDrag)
            {
                if (_drag.fPressDragFingersID & (_pid + _pid))
                {
                    Process_singlefinger_touch(dx1, dy1, _pid);
                }
                if (_drag.fPressDragFingersID & (_sid + _sid))
                {
                    Process_singlefinger_touch(dx2, dy2, _sid);
                }
                if (_drag.fPressDragFingersID & 1)
                {
                    if (_pid == 0) {
                        Process_singlefinger_touch(dx1, dy1, _pid);
                    }
                    else if (_sid == 0) {
                        Process_singlefinger_touch(dx2, dy2, _sid);
                    }
                }
            }
        case MODE_FOUR_FING_PRESS:
        case MODE_FIVE_FING_PRESS:
        case MODE_3_FING_TAP:
        case MODE_4_FING_TAP:
        case MODE_5_FING_TAP:
            
            //
            // Stop the enhanced scroll accleration if its active
            //
            
            StopScrollEnhanceTimer();
            DEBUG_1F_LOG("STD :: Skipping 2F\n");
            return;
            
        case MODE_IDLE:
            
            // Dispatch 3f drag and return
            if (_drag.is3FDrag)
            {
                if (_drag.fPressDragFingersID & (_pid + _pid))
                {
                    Process_singlefinger_touch(dx1, dy1, _pid);
                }
                if (_drag.fPressDragFingersID & (_sid + _sid))
                {
                    Process_singlefinger_touch(dx2, dy2, _sid);
                }
                if (_drag.fPressDragFingersID & 1)
                {
                    if (_pid == 0) {
                        Process_singlefinger_touch(dx1, dy1, _pid);
                    }
                    else if (_sid == 0) {
                        Process_singlefinger_touch(dx2, dy2, _sid);
                    }
                }
                return;
            }
            
            //
            // Return if last Fingers was > 2 for first 5 packets
            // because it could be acccidental packets when removing fingers.
            //
            
            if (_lastFingers > 2 && _packetsCount < 5){
                return;
            }
            else
                break;
    }
    
    //
    // Check for Zoom and Rotate
    //
    
    // Check delta between DY2 - DY1 and DX2 - DX1 to see if its rotation or pinch from scroll
    UInt16 zrDelta = (_zoom.delta > _rotate.delta)? _zoom.delta:_rotate.delta;
    
    if (( _scroll.maxLevelReached < 3
         || getAbsoluteValue(abs(x1_Dx) - abs(x2_Dx)) > zrDelta
         || getAbsoluteValue(abs(y1_Dy) - abs(y2_Dy)) > zrDelta)
        
        &&  _packetsCount > _zoom.minSamples && (_zoom.enabled || _rotate.enabled))
    {
        
        //
        // Swipe Rotate ?
        //
        
        if (_rotate.enabled && (y1 - y2) != 0 && _rotate.isRotateMode &&
                 
                 ((abs(x1_Dx) < _rotate.maxCFDelta && abs(x2_Dx) < _rotate.maxCFDelta &&
                   ((abs(y1_Dy) < abs(y2_Dy) && abs(y2_Dy) > _rotate.delta && abs(y1_Dy) < abs(y2_Dy)/3) ||
                    (abs(y2_Dy) < abs(y1_Dy) && abs(y1_Dy) > _rotate.delta && abs(y2_Dy) < abs(y1_Dy)/3)))
                  
                  ||
                  
                  (abs(y1_Dy) < _rotate.maxCFDeltaY && abs(y2_Dy) < _rotate.maxCFDeltaY &&
                   ((abs(x1_Dx) < abs(x2_Dx) && abs(x2_Dx) > _rotate.delta && abs(x1_Dx) < abs(x2_Dx)/3) ||
                    (abs(x2_Dx) < abs(x1_Dx) && abs(x1_Dx) > _rotate.delta && abs(x2_Dx) < abs(x1_Dx)/3)))))
        {
            //
            // Dispatch Rotate
            //
            
            if ((abs(y2_Dy) > _rotate.delta && y2_Dy < 0) ||
                (abs(y1_Dy) > _rotate.delta && y1_Dy > 0))
            {
                if (x1 < x2) {
                    _device->dispatchPS2Notification(kPS2C_RotateRight);
                    DEBUG_ZRP_LOG("STD :: Swipe Down Right Rotate performed.\n");
                } else {
                    _device->dispatchPS2Notification(kPS2C_RotateLeft);
                    DEBUG_ZRP_LOG("STD :: Swipe Up Left Rotate performed.\n");
                }
            }
            else if ((abs(y2_Dy) > _rotate.delta && y2_Dy > 0) ||
                     (abs(y1_Dy) > _rotate.delta && y1_Dy < 0))
            {
                if (x1 < x2) {
                    _device->dispatchPS2Notification(kPS2C_RotateLeft);
                    DEBUG_ZRP_LOG("STD :: Swipe Up Left Rotate performed.\n");
                } else {
                    _device->dispatchPS2Notification(kPS2C_RotateRight);
                    DEBUG_ZRP_LOG("STD :: Swipe Down Right Rotate performed.\n");
                }
            }
            
            else if ((abs(x2_Dx) > _rotate.delta && x2_Dx < 0) ||
                     (abs(x1_Dx) > _rotate.delta && x1_Dx < 0))
            {
                _device->dispatchPS2Notification(kPS2C_RotateRight);
                DEBUG_ZRP_LOG("STD :: Swipe Right Rotate performed.\n");
                
            }
            else if ((abs(x2_Dx) > _rotate.delta && x2_Dx > 0) ||
                     (abs(x1_Dx) > _rotate.delta && x1_Dx > 0))
            {
                _device->dispatchPS2Notification(kPS2C_RotateLeft);
                DEBUG_ZRP_LOG("STD :: Swipe Left Rotate performed.\n");
                
            }
            
            _touchmode = MODE_ROTATE;
            _rotate.isRotateGesture = true;
            
            IOLog("STD :: Swipe Rotate performed.\n");
        }
        
        //
        // Arc Rotate ?
        //
        // Note:
        // Constant finger Y delta (X less mostly) also moves in conjunction with arc finger
        // a little, when user uses same hand to use both fingers instead of two. So, I'm allowing
        // 1/3 of arc finger delta for constant finger Y (1/4 of rotation delta for X)
        // which are safe for all users in my tests and works as expected.
        // Ref: Was using 80 for both before Synaptics.
        //
        
        else if (_rotate.enabled && (y1 - y2) != 0 && _rotate.isRotateMode
                 && abs(x1_Dx) < _rotate.maxCFDelta && abs(y1_Dy) < _rotate.maxCFDeltaY
                 && abs(x2_Dx) > (_rotate.delta * 0.8)
                 && abs(y2_Dy) > (_rotate.delta * 0.8))
        {
            //
            // Dispatch Rotate
            //
            
            if (x2_Dx < 0 && y2_Dy < 0) {
                _device->dispatchPS2Notification(kPS2C_RotateRight);
                DEBUG_ZRP_LOG("STD :: Arc Right Rotate performed from Left corner.\n");
            }
            else if (x2_Dx > 0 && y2_Dy > 0) {
                _device->dispatchPS2Notification(kPS2C_RotateLeft);
                DEBUG_ZRP_LOG("STD :: Arc Left Rotate performed from Left corner.\n");
            }
            // Sometimes the constant finger changes its position in v3
            else if (x2_Dx < 0 && y2_Dy > 0) {
                _device->dispatchPS2Notification(kPS2C_RotateRight);
                DEBUG_ZRP_LOG("STD :: Arc Right Rotate performed from Right corner.\n");
            }
            else if (x2_Dx > 0 && y2_Dy < 0) {
                _device->dispatchPS2Notification(kPS2C_RotateLeft);
                DEBUG_ZRP_LOG("STD :: Arc Left Rotate performed from Right corner.\n");
            }
            
            _touchmode = MODE_ROTATE;
            _rotate.isRotateGesture = true;
            
            IOLog("STD :: Arc Rotate performed with X1 Y1 Constant.\n");
            
        }
        else if (_rotate.enabled && (y1 - y2) != 0 && _rotate.isRotateMode
                 && abs(x2_Dx) < _rotate.maxCFDelta && abs(y2_Dy) < _rotate.maxCFDeltaY
                 && abs(x1_Dx) > (_rotate.delta * 0.8)
                 && abs(y1_Dy) > (_rotate.delta * 0.8))
        {
            
            //
            // Dispatch Rotate
            //
            
            if (x1_Dx < 0 && y1_Dy > 0) {
                _device->dispatchPS2Notification(kPS2C_RotateRight);
                DEBUG_ZRP_LOG("STD :: Arc Right Rotate performed from Right corner.\n");
            }
            else if (x1_Dx > 0 && y1_Dy < 0) {
                _device->dispatchPS2Notification(kPS2C_RotateLeft);
                DEBUG_ZRP_LOG("STD :: Arc Left Rotate performed from Right corner.\n");
            }
            
            // Sometimes the constant finger changes its position
            else if (x1_Dx < 0 && y1_Dy < 0) {
                _device->dispatchPS2Notification(kPS2C_RotateRight);
                DEBUG_ZRP_LOG("STD :: Arc Right Rotate performed from Left corner.\n");
            }
            else if (x1_Dx > 0 && y1_Dy > 0) {
                _device->dispatchPS2Notification(kPS2C_RotateLeft);
                DEBUG_ZRP_LOG("STD :: Arc Left Rotate performed from Left corner.\n");
            }
            
            _touchmode = MODE_ROTATE;
            _rotate.isRotateGesture = true;
            
            IOLog("STD :: Arc Rotate performed with X2 Y2 Constant.\n");
        }
        
        //
        // Pinch Zoom ?
        //
        
        else if (_zoom.enabled && !_rotate.isRotateGesture &&
                
                ((x1_Dx > 0 && x2_Dx < 0 && _zoom.zDx > _zoom.delta
                  && ((abs(x1_Dx) > abs(x2_Dx) && abs(x2_Dx) > abs(x1_Dx)/_zoom.pinchDeltaScale) ||
                      (abs(x2_Dx) < abs(x1_Dx) && abs(x1_Dx) > abs(x2_Dx)/_zoom.pinchDeltaScale))
                  && abs(x1_Dx) > _zoom.delta/_zoom.pinchDeltaScale
                  && abs(x2_Dx) > _zoom.delta/_zoom.pinchDeltaScale && x1 < x2) ||
                 
                 (x1_Dx < 0 && x2_Dx > 0 && _zoom.zDx > _zoom.delta
                  && ((abs(x1_Dx) > abs(x2_Dx) && abs(x2_Dx) > abs(x1_Dx)/_zoom.pinchDeltaScale) ||
                      (abs(x2_Dx) < abs(x1_Dx) && abs(x1_Dx) > abs(x2_Dx)/_zoom.pinchDeltaScale))
                  && abs(x1_Dx) > _zoom.delta/_zoom.pinchDeltaScale
                  && abs(x2_Dx) > _zoom.delta/_zoom.pinchDeltaScale && x1 > x2) ||
                 
                 (y1_Dy > 0 && y2_Dy < 0 && _zoom.zDy > _zoom.delta
                  && ((abs(y1_Dy) > abs(y2_Dy) && abs(y2_Dy) > abs(y1_Dy)/_zoom.pinchDeltaScale) ||
                      (abs(y2_Dy) < abs(y1_Dy) && abs(y1_Dy) > abs(y2_Dy)/_zoom.pinchDeltaScale))
                  && abs(y1_Dy) > _zoom.delta/_zoom.pinchDeltaScale
                  && abs(y2_Dy) > _zoom.delta/_zoom.pinchDeltaScale && y1 < y2) ||
                 
                 (y1_Dy < 0 && y2_Dy > 0 && _zoom.zDy > _zoom.delta
                  && ((abs(y1_Dy) > abs(y2_Dy) && abs(y2_Dy) > abs(y1_Dy)/_zoom.pinchDeltaScale) ||
                      (abs(y2_Dy) < abs(y1_Dy) && abs(y1_Dy) > abs(y2_Dy)/_zoom.pinchDeltaScale))
                  && abs(y1_Dy) > _zoom.delta/_zoom.pinchDeltaScale
                  && abs(y2_Dy) > _zoom.delta/_zoom.pinchDeltaScale && y1 > y2)))
            {
                _touchmode = MODE_ZOOM;
                _zoom.isZoomOut = true;
                _zoom.isZoomIn = false;
                IOLog("STD :: Zoom out Performed\n");
                return;
            }
            else if (_zoom.enabled && !_rotate.isRotateGesture &&
                     
                     ((x1_Dx < 0 && x2_Dx > 0 && _zoom.zDx > _zoom.delta
                       && ((abs(x1_Dx) > abs(x2_Dx) && abs(x2_Dx) > abs(x1_Dx)/_zoom.pinchDeltaScale) ||
                           (abs(x2_Dx) < abs(x1_Dx) && abs(x1_Dx) > abs(x2_Dx)/_zoom.pinchDeltaScale))
                       && abs(x1_Dx) > _zoom.delta/_zoom.pinchDeltaScale
                       && abs(x2_Dx) > _zoom.delta/_zoom.pinchDeltaScale && x1 < x2) ||
                      
                      (x1_Dx > 0 && x2_Dx < 0 && _zoom.zDx > _zoom.delta
                       && ((abs(x1_Dx) > abs(x2_Dx) && abs(x2_Dx) > abs(x1_Dx)/_zoom.pinchDeltaScale) ||
                           (abs(x2_Dx) < abs(x1_Dx) && abs(x1_Dx) > abs(x2_Dx)/_zoom.pinchDeltaScale))
                       && abs(x1_Dx) > _zoom.delta/_zoom.pinchDeltaScale
                       && abs(x2_Dx) > _zoom.delta/_zoom.pinchDeltaScale && x1 > x2) ||
                      
                      (y1_Dy < 0 && y2_Dy > 0 && _zoom.zDy > _zoom.delta
                       && ((abs(y1_Dy) > abs(y2_Dy) && abs(y2_Dy) > abs(y1_Dy)/_zoom.pinchDeltaScale) ||
                           (abs(y2_Dy) < abs(y1_Dy) && abs(y1_Dy) > abs(y2_Dy)/_zoom.pinchDeltaScale))
                       && abs(y1_Dy) > _zoom.delta/_zoom.pinchDeltaScale
                       && abs(y2_Dy) > _zoom.delta/_zoom.pinchDeltaScale && y1 < y2) ||
                      
                      (y1_Dy > 0 && y2_Dy < 0 && _zoom.zDy > _zoom.delta
                       && ((abs(y1_Dy) > abs(y2_Dy) && abs(y2_Dy) > abs(y1_Dy)/_zoom.pinchDeltaScale) ||
                           (abs(y2_Dy) < abs(y1_Dy) && abs(y1_Dy) > abs(y2_Dy)/_zoom.pinchDeltaScale))
                       && abs(y1_Dy) > _zoom.delta/_zoom.pinchDeltaScale
                       && abs(y2_Dy) > _zoom.delta/_zoom.pinchDeltaScale && y1 > y2)))
            {
                _touchmode = MODE_ZOOM;
                _zoom.isZoomIn = true;
                _zoom.isZoomOut = false;
                IOLog("STD :: Zoom in Performed\n");
                return;
            }
    }
    
    //
    // Process scroll
    //
    
    if (!_rotate.isRotateGesture && _touchmode != MODE_ROTATE
        
        && ((abs(y1_Dy) > 0 && abs(y2_Dy) > 0) || (abs(x1_Dx) > 0 && abs(x2_Dx) > 0)
            || _touchmode == MODE_VSCROLL || _touchmode == MODE_HSCROLL)
        
        && ((_hw_version < 4 && _hw_version > 1 && (y1 - y2) == 0)
            || _hw_version == 0 || _hw_version > 3))
    {
        //
        // Calculate delta
        //
        
        if (_packetsCount != 0 && _scroll.smoothSamples > 0) {
            
            _fing[_pid].tdx += dx1;
            _fing[_pid].tdy += dy1;
            _lastSample[_cSample].lastDX1 = dx1;
            _lastSample[_cSample].lastDY1 = dy1;
            
            _fing[_sid].tdx += dx2;
            _fing[_sid].tdy += dy2;
            _lastSample[_cSample].lastDX2 = dx2;
            _lastSample[_cSample].lastDY2 = dy2;
            
            if (dx1 != 0 || dy1 != 0)
                _cSample++;
            
            if (_cSample == _scroll.smoothSamples)
                _cSample = 0;
            
            _fing[_pid].sdx = 0;
            _fing[_pid].sdy = 0;
            _fing[_sid].sdx = 0;
            _fing[_sid].sdy = 0;
            
            for (int s = 0; s < _scroll.smoothSamples; s++) {
                _fing[_pid].sdx += _lastSample[s].lastDX1;
                _fing[_pid].sdy += _lastSample[s].lastDY1;
                _fing[_sid].sdx += _lastSample[s].lastDX2;
                _fing[_sid].sdy += _lastSample[s].lastDY2;
            }
        }
        
        //
        // Calclulate fingers hold time
        //
        
        if (abs(dx1) <= _scroll.idleFingMaxDelta && abs(dy1) <= _scroll.idleFingMaxDelta &&
            abs(dx2) <= _scroll.idleFingMaxDelta && abs(dy2) <= _scroll.idleFingMaxDelta)
        {
            clock_get_uptime(TIME_NOW);
            
            if (_fingPress.lastHoldTime == 0)
                _fingPress.lastHoldTime = absoluteToUINT64(_now);
            
            else if (_fingPress.holdTime < _scroll.slingStartTimeout
                     || _fingPress.holdTime <  _scroll.idleFingDetectTimeout)
            {
                _fingPress.holdTime += (absoluteToUINT64(_now) - _fingPress.lastHoldTime);
                _fingPress.lastHoldTime = absoluteToUINT64(_now);
            }
        }
        else {
            _fingPress.lastHoldTime = absoluteToUINT64(_now);
            _fingPress.holdTime = 0;
        }
        
        //
        // Process sling effect
        //
        
        if (_scroll.has2FSling &&  _scroll.maxLevelReached != 0) {
            
            if (!(y1 > (_ymax - _scroll.slingTriggerArea) || y1 < (_ymin + _scroll.slingTriggerArea)))
            {
                _scroll.doSling = false;
                _scroll.slingDepth = 0;
            }
            else if (!_scroll.doSling && _fingPress.holdTime > _scroll.slingStartTimeout)
            {
                _scroll.doSling = true;
                
                if (_scroll.slingDepth == 0) {
                    
                    if (_scroll.maxLevelReached == 0)
                         _scroll.maxLevelReached = 1;
                    
                    // Bottom finger release
                    if (y1 > (_ymax - _scroll.slingTriggerArea))
                        _scroll.slingDepth = 1 *  _scroll.maxLevelReached;
                    
                    // Top finger release
                    else if (y1 < (_ymin + _scroll.slingTriggerArea))
                        _scroll.slingDepth = -1 *  _scroll.maxLevelReached;
                }
            }
        }
        
        if ((_isHScrollEnabled || _isVScrollEnabled) && (dx1 != 0 || dx2 != 0 ||  dy1 != 0 || dy2 != 0))
        {
            
            int y1y2Diff = abs(y1_Dy) - abs(y2_Dy); if (y1y2Diff < 0) y1y2Diff = 0 - y1y2Diff;
            int x1x2Diff = abs(x1_Dx) - abs(x1_Dx); if (x1x2Diff < 0) x1x2Diff = 0 - x1x2Diff;
            
            if (_scroll.maxLevelReached > 2
                || (y1y2Diff < 150 && x1x2Diff < 150)
                || (abs(x1_Dx) > _rotate.maxCFDelta && abs(x2_Dx) > _rotate.maxCFDelta)
                || (abs(y1_Dy) > _rotate.maxCFDelta && abs(y2_Dy) > _rotate.maxCFDelta))
            {
                //
                // Check for Zoom conflicts
                //
                
                bool isZoomConflict = false;
                
                if (_zoom.enabled) {
                    
                    if (((x1_Dx > 0 && x2_Dx < 0  && x1 < x2) ||
                         (x1_Dx < 0 && x2_Dx > 0  && x1 > x2) ||
                         (y1_Dy > 0 && y2_Dy < 0  && y1 < y2) ||
                         (y1_Dy < 0 && y2_Dy > 0  && y1 > y2))
                        
                        /*&& (abs(dx1) > _scroll.idleFingMaxDelta || abs(dy1) > _scroll.idleFingMaxDelta)*/)
                    {
                        if (((_zoom.zDx > 50 || _zoom.zDy > 50) && _packetsCount < 5) ||
                            ((_zoom.zDx > 100 || _zoom.zDy > 100) && _packetsCount < 10) ||
                            ((_zoom.zDx > 180 || _zoom.zDy > 180) && _packetsCount > 10))
                        {
                            isZoomConflict = true;
                            
                            //
                            // Stop enhanced scroll if active
                            //
                            
                            StopScrollEnhanceTimer();
                        }
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Zoom out logic detected for XD %d YD %d, PC %d, ZDX %d, ZDY %d, Conflict %s\n", dx1, dy1, _packetsCount, _zoom.zDx, _zoom.zDy, isZoomConflict?"Yes":"No");
                    }
                    else if (((x1_Dx < 0 && x2_Dx > 0 && x1 < x2) ||
                              (x1_Dx > 0 && x2_Dx < 0 && x1 > x2) ||
                              (y1_Dy < 0 && y2_Dy > 0 && y1 < y2) ||
                              (y1_Dy > 0 && y2_Dy < 0 && y1 > y2))
                             
                             /*&& (abs(dx1) > _scroll.idleFingMaxDelta || abs(dy1) > _scroll.idleFingMaxDelta)*/)
                    {
                        if (((_zoom.zDx > 50 || _zoom.zDy > 50) && _packetsCount < 5) ||
                            ((_zoom.zDx > 100 || _zoom.zDy > 100) && _packetsCount < 10) ||
                            ((_zoom.zDx > 180 || _zoom.zDy > 180) && _packetsCount > 10))
                        {
                            isZoomConflict = true;
                            
                            //
                            // Stop enhanced scroll if active
                            //
                            
                            StopScrollEnhanceTimer();
                        }
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Zoom in logic detected XD %d YD %d, PC %d, ZDX %d, ZDY %d, Conflict %s\n", dx1, dy1, _packetsCount, _zoom.zDx, _zoom.zDy, isZoomConflict?"Yes":"No");
                    }
                }
                
                //
                // Dispatch scroll
                //
                
                if ((dx1 == 0 && dy1 == 0)
                    || (dy2 > dy1 && _touchmode == MODE_VSCROLL)
                    || (dx2 > dx1 && _touchmode == MODE_HSCROLL))
                {
                    Process_scroll(_scroll.accelMode2F, dx2, dy2, isZoomConflict);
                    _lastSample[_scrollSample].pressure = _fing[_sid].pressure;
                }
                else {
                    Process_scroll(_scroll.accelMode2F, dx1, dy1, isZoomConflict);
                    _lastSample[_scrollSample].pressure = _fing[_pid].pressure;
                }
            }
            else {
                 _scroll.maxLevelReached = 0;
                StopScrollEnhanceTimer();
            }
        }
        else if (_scroll.doCont && !_scroll.has2FCont)
        {
            StopScrollEnhanceTimer();
        }
    }
    
    //
    // Calculate finger press time
    //
    
    if (_scroll.totalDx < _tap.noiseMultiTapDelta && _scroll.totalDy < _tap.noiseMultiTapDelta)
    {
        if (!_button.triggered
            && _gestureTime < (_fingPress.timeout + _fingPress.timeout)// add sec + timeout to detect
            && x1 <= (_fing[_pid].start_x + 75) && x1 >= (_fing[_pid].start_x - 75)
            && y1 <= (_fing[_pid].start_y + 75) && y1 >= (_fing[_pid].start_y - 75)
            && x2 <= (_fing[_sid].start_x + 75) && x2 >= (_fing[_sid].start_x - 75)
            && y2 <= (_fing[_sid].start_y + 75) && y2 >= (_fing[_sid].start_y - 75))
        {
            _fingPress.gestureTime = _gestureTime;
            
            //
            // Stop enhanced scroll if active
            //
            
            if (_touchmode == MODE_VSCROLL || _touchmode == MODE_HSCROLL || _touchmode == MODE_CONT_SCROLL)
                StopScrollEnhanceTimer();
        }
        else
        {
            _fingPress.gestureTime = 0;
        }
        
        //
        //  Detect two finger press
        //
        
        if (_fingPress.gestureTime > _fingPress.timeout
            && _fingPress.timeout != 0
            && (_fingPress.fings2Press > 0 || _kbEvent.isToggleKeyOnHold)
            &&  _scroll.maxLevelReached < 2)
        {
            // Toggle 2f press
            if (_kbEvent.isToggleKeyOnHold) {
                
                _fingPress.fings2Press = 0 - _fingPress.fings2Press;
                if (_fingPress.fings2Press > 0)
                {
                    DEBUG_TOGGLE_LOG("STD :: 2 Fingers press is Enabled.\n");
                    dispatchRelativePointerPosition(0, -50, _button.state);
                }
                else
                {
                    DEBUG_TOGGLE_LOG("STD :: 2 Fingers press is Disabled.\n");
                    dispatchRelativePointerPosition(0, 50, _button.state);
                }
                _touchmode = MODE_TWO_FING_PRESS;
            }
            // Dispatch plist configured action
            else
            {
                Proces_Fingers_Press(2);
            }
        
            //
            // Stop enhanced scroll if active
            //
            
            StopScrollEnhanceTimer();
            
            DEBUG_2F_LOG("STD :: Two finger press performed.\n");
        }
    }
    else
    {
        _fingPress.gestureTime = 0;
    }
    
    //
    // Detect two fingers tap
    //
    
    if (_gestureTime < _tap.timeout
        && _packetsCount > 1
        && !_button.triggered
        && (abs(x1_Dx) < _tap.noiseMultiTapDelta && abs(y1_Dy) < _tap.noiseMultiTapDelta
            && abs(x2_Dx) < _tap.noiseMultiTapDelta && abs(y2_Dy) < _tap.noiseMultiTapDelta))
    {
        _touchmode = MODE_2_FING_TAP;
    }
    else if ((_gestureTime > _tap.timeout ||
              !(abs(x1_Dx) < _tap.noiseMultiTapDelta && abs(y1_Dy) < _tap.noiseMultiTapDelta &&
                abs(x2_Dx) < _tap.noiseMultiTapDelta && abs(y2_Dy) < _tap.noiseMultiTapDelta))
             && _touchmode == MODE_2_FING_TAP)
    {
        _touchmode = MODE_IDLE;
    }
}

void ApplePS2SmartTouchpad::Process_ClickPad_2F(unsigned char *packet, UInt8 packetType,
                                                int dx1, int dy1, int dx2, int dy2)
{
    // Check for the clickpad button press
    
    bool buttonPressed = false;
    buttonPressed = Process_ClickPad_Virtual_Buttons(packet, packetType, _pid, _cFingers);
    
    switch (_touchmode) {
        case MODE_CONT_SCROLL:
        case MODE_HSCROLL:
        case MODE_VSCROLL:
        case MODE_ZOOM:
        case MODE_ROTATE:
        case MODE_TWO_FING_PRESS:
        case MODE_2_FING_TAP:
            
            //
            // Process two fingers touch
            //
            
            Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
            break;
            
        default:
            
            //
            // Clikpad not doing 2f move / drag ? or Is it Multi finger drag ?
            //
            
            if (!(_button.clickPad2FMove || buttonPressed) || _drag.is3FDrag)
            {
                //
                // Process two fingers touch
                //
                
                Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
                break;
            }
            
            //
            // Clickpad 2f drag ?
            //
            
            if ((!_button.clickPadHasVButtons || _button.is2FClick) && buttonPressed)
            {
                // Skip for 2F click action
                if (_button.state != 0x1)
                    break;
                
                DEBUG_2F_LOG("STD :: 2F drag\n");
                
                _touchmode = MODE_MOVE;
                
                if ((dx1 != 0 || dy1 != 0) &&
                    ((_button.fingerInUse & 0x1) || (dx2 == 0 && dy2 == 0)))
                {
                    DEBUG_2F_LOG("STD :: Finger 1 used\n");
                    _button.fingerInUse = 0x1;
                    Process_singlefinger_touch(dx1, dy1, 0);
                }
                else if ((dx2 != 0 || dy2 != 0) &&
                         ((_button.fingerInUse & 0x2) || (dx1 == 0 && dy1 == 0)))
                {
                    DEBUG_2F_LOG("STD :: Finger 2 used\n");
                    _button.fingerInUse = 0x2;
                    Process_singlefinger_touch(dx2, dy2, 1);
                }
                
                //
                // Stop the enhanced pointer accleration if its active
                //
                
                else if (_pContinuousMove) {
                    StopPointerEnhanceTimer();
                }
                break;
            }
            
            //
            // Process 2f drag/move with thumb on button area and index to move
            //
            
            if (_fing[_sid].y > (_ymax - _button.clickPadVBtnArea) && (dx1 != 0 || dy1 != 0) &&
                
                (_button.fingerInUse & 0x1
                 
                 || (!_button.is2FClick &&
                     (dx1 != 0 || dy1 != 0) &&
                     (dx2 == 0 && dy2 == 0))
                 
                 || !(_fing[_pid].y > (_ymax - _button.clickPadVBtnArea))))
            {
                DEBUG_2F_LOG("STD :: Finger 2 in on button area, using Finger 1 data.\n");
                
                _touchmode = MODE_MOVE;
                _button.isFingOnBtnArea = true;
                
                if (!(_fing[_pid].y > (_ymax - _button.clickPadVBtnArea)))
                    _button.fingerInUse = 0x1;
                
                //
                // Check for the virtual button press
                //
                
                if (_button.isClickPad)
                    Process_ClickPad_Virtual_Buttons(packet, packetType, _sid, 1);
                
                //
                // Set button mode to 0x1 when 2f drag
                // started from button area to avoid middle click
                //
                
                if (_button.triggered && _button.data != 0x1) {
                    _button.state = _button.data = 0x1;
                    _button.is2FClick = false;
                }
                
                //
                // Dispatch pointer movement
                //
                
                Process_singlefinger_touch(dx1, dy1, _pid);
            }
            else if (_fing[_pid].y > (_ymax - _button.clickPadVBtnArea) && (dx2 != 0 || dy2 != 0) &&
                     
                     (_button.fingerInUse & 0x2
                      
                      || (!_button.is2FClick &&
                          (dx2 != 0 || dy2 != 0) &&
                          (dx1 == 0 && dy1 == 0))
                      
                      || !(_fing[_sid].y > (_ymax - _button.clickPadVBtnArea))))
            {
                DEBUG_2F_LOG("STD :: Finger 1 in on button area, using Finger 2 data.\n");
                
                _touchmode = MODE_MOVE;
                _button.isFingOnBtnArea = true;
                
                if (!(_fing[_sid].y > (_ymax - _button.clickPadVBtnArea)))
                    _button.fingerInUse = 0x2;
                
                //
                // Check for the virtual button press
                //
                
                if (_button.isClickPad)
                    Process_ClickPad_Virtual_Buttons(packet, packetType, _pid, 1);
                
                //
                // Set button mode to 0x1 when 2f drag
                // started from button area to avoid middle click
                //
                
                if (_button.triggered && _button.data != 0x1) {
                    _button.state = _button.data = 0x1;
                    _button.is2FClick = false;
                }
                
                //
                // Dispatch pointer movement
                //
                
                Process_singlefinger_touch(dx2, dy2, _sid);
            }
            
            //
            // Process two fingers touch
            //
            
            else if (!_button.triggered && !_drag.isDragging)
                Process_twofingers_touch(_fing[_pid].x, _fing[_sid].x, _fing[_pid].y, _fing[_sid].y);
            
            //
            // Stop the enhanced pointer accleration if its active
            //
            
            else if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            break;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes scroll.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_scroll(UInt8 sMode, int sdx, int sdy, bool isZoomConflict)
{
    SInt16  tmpDXY = 0; // temp vaiable for some scroll DXY calculations
    SInt16  sDelta = 0;
    SInt16  s_dx = 0, s_dy = 0;
    UInt8   scrollDifflvl = 0;
    
    //
    // Collect and process last 5 samples of scroll delta values
    // which are needed to control the scroll.
    //
    
    _lastSample[_scrollSample].lastSDX = abs(sdx);
    _lastSample[_scrollSample].lastSDY = abs(sdy);
    
    if (_scrollSample < 4)
        _scrollSample++;
    else
        _scrollSample = 0;
    
    int i = 0;
    do
    {
        //
        // Choose scroll delta to process
        //
        
        switch (_cFingers) {
            case 1:
                if (_touchmode == MODE_EDGE_HSCROLL || _touchmode == MODE_CIR_HSCROLL)
                    sDelta = sdx;
                else if (_touchmode == MODE_EDGE_VSCROLL || _touchmode == MODE_CIR_VSCROLL)
                    sDelta = sdy;
                break;
                
            case 2:
                
                //
                // Choose the scroll
                //
                
                if (_isHScrollEnabled && abs(sdx) >= _scroll.idleFingMaxDelta &&
                    (abs(sdx) > abs(sdy) || _scroll.dxPacketsCount >= 5))
                {
                    _touchmode = MODE_HSCROLL;
                }
                
                if (_isVScrollEnabled && abs(sdy) >= _scroll.idleFingMaxDelta &&
                    (abs(sdy) >= abs(sdx) || _scroll.dyPacketsCount >= 5))
                {
                    _touchmode = MODE_VSCROLL;
                }
                
                //
                // Smooth scroll by using Divisive smoothing using x samples delta + avg of last x samples
                // and collect last 5 samples of scroll delta values which are needed to control the scroll.
                //
                
                
                if (i == 0) {
                    
                    if (_scroll.smoothSamples > 0) {
                        s_dx = _fing[_pid].sdx + _fing[_sid].sdx;
                        s_dx = (s_dx + _pSmoothX)/(_scroll.smoothSamples + 1);
                    }
                    else
                        s_dx = (sdx + _pSmoothX)/2;
                    
                    _pSmoothX = s_dx;
                    
                    if (_scroll.smoothSamples > 0) {
                        s_dy = _fing[_pid].sdy + _fing[_sid].sdy;
                        s_dy = (s_dy + _pSmoothY)/(_scroll.smoothSamples + 1);
                    }
                    else
                        s_dy = (sdy + _pSmoothY)/2;
                    
                    _pSmoothY = s_dy;
                    
                    if (!_isHScrollEnabled || _touchmode == MODE_VSCROLL) i++; // skip Horizontal processing
                }
                
                if (i == 0) {
                    sDelta = s_dx;
                }
                else if (_touchmode != MODE_HSCROLL)
                {
                    sDelta = s_dy;
                }
                
                DEBUG_SCROLL_SWIPES_LOG("STD :: Sample DY %d %d Avg %d, DX %d %d Avg %d, Samples %d\n", _fing[_pid].sdy, _fing[_sid].sdy, (_fing[_pid].sdy/_scroll.smoothSamples) + (_fing[_sid].sdy/_scroll.smoothSamples) ,_fing[_pid].sdx, _fing[_sid].sdx, (_fing[_pid].sdx/_scroll.smoothSamples) + (_fing[_sid].sdx/_scroll.smoothSamples), _cSample);
                break;
                
            default:
                break;
        }
        
        switch (sMode) {
            case 0: // Optimized acceleration
                tmpDXY = sDelta/10;
                
                if (sDelta < 0) {
                    if ((tmpDXY % 2) != 0) tmpDXY++; // make it even number
                    sDelta = tmpDXY/2 - 1;
                }
                else {
                    if ((tmpDXY % 2) != 0) tmpDXY--; // make it even number
                    sDelta = tmpDXY/2 + 1;
                }
                break;
                
            case 1: // Fixed acceleration
                if (sDelta < 0)
                    sDelta = -5;
                else
                    sDelta = 5;
                break;
                
            case 2: // Extended acceleration
                tmpDXY = sDelta/10;
                
                if (sDelta < 0) {
                    sDelta = tmpDXY - 1;
                }
                else {
                    sDelta = tmpDXY + 1;
                }
                break;
                
            case 3: // Super acceleration
            case 4:
                break;
                
            default:
                break;
        }
        
        // Change direction
        sDelta = 0 - sDelta;
        
        //
        // Modify scroll delta
        //
        
        switch (_cFingers) {
            case 1:
                if (_touchmode == MODE_EDGE_HSCROLL || _touchmode == MODE_CIR_HSCROLL) {
                    s_dx = sDelta;
                }
                else if (_touchmode == MODE_EDGE_VSCROLL || _touchmode == MODE_CIR_VSCROLL) {
                    s_dy = sDelta;
                }
                
                scrollDifflvl = (sDelta < 0)?0 - sDelta:sDelta;
                break;
                
            case 2:
                if (i == 0) {
                    s_dx = sDelta;
                }
                else {
                    s_dy = sDelta;
                }
                
                // Skip vertical scroll Lvl on HScroll mode
                if ((i == 1 && _touchmode != MODE_HSCROLL) || i == 0)
                {
                    if (sMode > 2) {
                        scrollDifflvl = (sDelta/10 + 1) * 2;
                        scrollDifflvl *= 2;
                    }
                    else {
                        scrollDifflvl = (sDelta < 0)?0 - sDelta:sDelta;
                    }
                }
                if (!_isVScrollEnabled) i++; // skip Vertical processing
                break;
                
            default:
                break;
        }
        
        i++;
        
    } while (i < _cFingers);
    
    //
    // Reset scroll level on direction change
    //
    
    int directionFilter = 3;
    
    switch (_touchmode) {
        case MODE_HSCROLL:
        case MODE_EDGE_HSCROLL:
            
            if (((_scroll.dx > directionFilter && s_dx < -directionFilter && abs(sdx) > 0) ||
                 (_scroll.dx < -directionFilter && s_dx > directionFilter && abs(sdx) > 0))
                
                && scrollDifflvl != 0
                && ((_touchmode == MODE_HSCROLL && abs(sdx) > _scroll.contMaxDelta2F) ||
                    (_touchmode != MODE_HSCROLL && abs(sdx) > _scroll.contMaxDeltaEdge)))
            {
                _scroll.dx = s_dx;
                 _scroll.maxLevelReached = 0;
                _packetsCount = 0;
                
                _fing[_pid].start_x = _fing[_pid].x;
                _fing[_pid].start_y = _fing[_pid].y;
                _fing[_sid].start_x = _fing[_sid].x;
                _fing[_sid].start_y = _fing[_sid].y;
                
                _fing[_pid].last_x = _fing[_pid].x;
                _fing[_pid].last_y = _fing[_pid].y;
                _fing[_sid].last_x = _fing[_sid].x;
                _fing[_sid].last_y = _fing[_sid].y;
            }
            break;
            
        case MODE_VSCROLL:
        case MODE_EDGE_VSCROLL:
            
            if (((_scroll.dy > directionFilter && s_dy < -directionFilter && abs(sdy) > 0) ||
                 (_scroll.dy < -directionFilter && s_dy > directionFilter && abs(sdy) > 0))
                
                && scrollDifflvl != 0
                && ((_touchmode == MODE_VSCROLL && abs(sdy) > _scroll.contMaxDelta2F) ||
                    (_touchmode != MODE_VSCROLL && abs(sdy) > _scroll.contMaxDeltaEdge)))
            {
                _scroll.dy = s_dy;
                 _scroll.maxLevelReached = 0;
                _packetsCount = 0;
                
                _fing[_pid].start_x = _fing[_pid].x;
                _fing[_pid].start_y = _fing[_pid].y;
                _fing[_sid].start_x = _fing[_sid].x;
                _fing[_sid].start_y = _fing[_sid].y;
                
                _fing[_pid].last_x = _fing[_pid].x;
                _fing[_pid].last_y = _fing[_pid].y;
                _fing[_sid].last_x = _fing[_sid].x;
                _fing[_sid].last_y = _fing[_sid].y;
            }
            break;
            
        default:
            break;
    }
    
    
    //
    // Set Max Scroll Level
    //
    
    if (_scroll.maxLevelReached < scrollDifflvl) {
        DEBUG_SCROLL_SWIPES_LOG("STD :: Scroll Level updated to %d from %d\n", scrollDifflvl,  _scroll.maxLevelReached);
         _scroll.maxLevelReached = scrollDifflvl;
    }
    
    //
    // Ignore first 15 packets with small deltas total < 30
    // in order to avoid conflicts with tap gesture.
    //
    
    _scroll.totalDx += abs(sdx);
    _scroll.totalDy += abs(sdy);
    
    DEBUG_SCROLL_SWIPES_LOG("STD :: Scroll SXD %d, SYD %d, UXD %d UYD %d, XD %d YD %d, LDX %d LDY %d, SDX %d SDY %d, TMode %d, Pcounts %d, SF %d (T %d), SLvl %d, Max Lvl %d, FHoldTime %lld ns, Sling %s, NoInteria %d, ContScroll %s, ZoomConflict %d Depth %d\n", _scroll.totalDx, _scroll.totalDy, abs(sdx), abs(sdy), sdx, sdy, _scroll.dx, _scroll.dy, s_dx, s_dy, _touchmode, _packetsCount, (int)_scrollFactor, (int)(_scrollFactor * 10), scrollDifflvl,  _scroll.maxLevelReached, _fingPress.holdTime, _scroll.doSling?"Yes":"No", _scroll.noInertia, _scroll.doCont?"Yes":"No", isZoomConflict, _scroll.inertiaDepth);
    
    //
    // Monitor last 5 delta packets
    // which are needed to control direction of the scroll.
    //
    UInt8 contMaxDelta = 0;
    
    switch (_cFingers) {
        case 1:
            // Set Continuous Max Delta
            contMaxDelta = _scroll.contMaxDeltaEdge;
            
            //
            // Change direction
            //
            
            if (_touchmode == MODE_EDGE_VSCROLL)
            {
                if (sdy < 0) {
                    if (_scroll.yPacketsDir > -10) _scroll.yPacketsDir--;
                    if (_scroll.yPacketsDir < -_scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to UP\n");
                        
                        if (_scroll.dy < 0) _scroll.dy = 0 - _scroll.dy;
                    }
                }
                else if (sdy > 0) {
                    if (_scroll.yPacketsDir < 10) _scroll.yPacketsDir++;
                    if (_scroll.yPacketsDir > _scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to DOWN\n");
                        
                        if (_scroll.dy > 0) _scroll.dy = 0 - _scroll.dy;
                    }
                }
            }
            else if (_touchmode == MODE_EDGE_HSCROLL)
            {
                if (sdx < 0) {
                    if (_scroll.xPacketsDir > -10) _scroll.xPacketsDir--;
                    if (_scroll.xPacketsDir < -_scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to LEFT\n");
                        
                        if (_scroll.dx < 0) _scroll.dx = 0 - _scroll.dx;
                    }
                }
                else if (sdx > 0) {
                    if (_scroll.xPacketsDir < 10) _scroll.xPacketsDir++;
                    if (_scroll.xPacketsDir > _scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to RIGHT\n");
                        
                        if (_scroll.dx > 0) _scroll.dx = 0 - _scroll.dx;
                    }
                }
            }
            
            //
            // Stop and reset max scroll level
            // when fingers stop and timed out.
            //
            
            if (_fingPress.holdTime >  _scroll.idleFingDetectTimeout)
            {
                _scroll.noInertia = true;
                
                if (!_scroll.hasEdgeCont) {
                    scrollDifflvl = 0;
                    
                    if (!_scroll.hasEdgeSling) {
                         _scroll.maxLevelReached = 0;
                    }
                }
            }
            break;
            
        case 2:
            // Set Continuous Max Delta
            contMaxDelta = _scroll.contMaxDelta2F;
            
            // DX packet
            if (abs(sdx) > abs(sdy) || _touchmode == MODE_HSCROLL)
            {
                if (_scroll.dxPacketsCount < 255)
                    _scroll.dxPacketsCount++;
                
                if (abs(sdx) > _scroll.idleFingMaxDelta || _scroll.dxPacketsCount >= 5) {
                    if (sdx < 0 && _scroll.xPacketsDir > -10) _scroll.xPacketsDir--;
                    if (sdx > 0 && _scroll.xPacketsDir < 10) _scroll.xPacketsDir++;
                }
                
                //
                // Control direction
                //
                
                if (_scroll.dxPacketsCount >= 5)
                {
                    if (abs(sdx) > abs(sdy) && _scroll.dyPacketsCount > 0) _scroll.dyPacketsCount--;
                    
                    if (sdx < 0 && _scroll.xPacketsDir < -_scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to LEFT\n");
                        
                        if (_scroll.dy < 0) _scroll.dy = 0 - _scroll.dy;
                        if (_scroll.dx < 0) _scroll.dx = 0 - _scroll.dx;
                    }
                    else if (sdx > 0 && _scroll.xPacketsDir > _scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to RIGHT\n");
                        
                        if (_scroll.dy > 0) _scroll.dy = 0 - _scroll.dy;
                        if (_scroll.dx > 0) _scroll.dx = 0 - _scroll.dx;
                    }
                }
            }
            
            // DY packet
            if ((abs(sdy) >= abs(sdx)) || (_touchmode == MODE_VSCROLL))
            {
                if (_scroll.dyPacketsCount < 255)
                    _scroll.dyPacketsCount++;
                
                if (abs(sdy) > _scroll.idleFingMaxDelta || _scroll.dyPacketsCount >= 5) {
                    if (sdy < 0 && _scroll.yPacketsDir > -10) _scroll.yPacketsDir--;
                    if (sdy > 0 && _scroll.yPacketsDir < 10) _scroll.yPacketsDir++;
                }
                
                //
                // Control direction
                //
                
                if (_scroll.dyPacketsCount >= 5)
                {
                    if (abs(sdy) >= abs(sdx) && _scroll.dxPacketsCount > 0) _scroll.dxPacketsCount--;
                    
                    if (sdy < 0 && _scroll.yPacketsDir < -_scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to UP\n");
                        
                        if (_scroll.dy < 0) _scroll.dy = 0 - _scroll.dy;
                        if (_scroll.dx < 0) _scroll.dx = 0 - _scroll.dx;
                    }
                    else if (sdy > 0 && _scroll.yPacketsDir > _scroll.directionSamples)
                    {
                        DEBUG_SCROLL_SWIPES_LOG("STD :: Change direction to DOWN\n");
                        
                        if (_scroll.dy > 0) _scroll.dy = 0 - _scroll.dy;
                        if (_scroll.dx > 0) _scroll.dx = 0 - _scroll.dx;
                    }
                }
            }
            
            if (_packetsCount < 15 &&  _scroll.maxLevelReached < 2
                && _scroll.totalDx < _tap.noiseMultiTapDelta && _scroll.totalDy < _tap.noiseMultiTapDelta)
            {
                scrollDifflvl = s_dx = s_dy = 0;
            }
            
            //
            // Stop and reset max scroll level
            // when fingers stop and timed out.
            //
            
            if (_fingPress.holdTime >  _scroll.idleFingDetectTimeout)
            {
                _scroll.noInertia = true;
                
                if (!_scroll.has2FCont) {
                    scrollDifflvl = 0;
                    
                    if (!_scroll.has2FSling) {
                         _scroll.maxLevelReached = 0;
                    }
                }
            }
            break;
            
        default:
            break;
    }
    
    DEBUG_SCROLL_SWIPES_LOG("STD :: Fingers stop sample DX %d DY %d, DXCount %d, DYCount %d, Direction X %d Y %d, TMode %d, Idle %s, SLevel %d, LastTime %lld, Now %lld, IdleTimeout %lld, Lvl %d PC %d\n", s_dx, s_dy, _scroll.dxPacketsCount, _scroll.dyPacketsCount, _scroll.xPacketsDir, _scroll.yPacketsDir, _touchmode, (_fingPress.holdTime >  _scroll.idleFingDetectTimeout)?"Yes":"No", scrollDifflvl, _fingPress.lastHoldTime, absoluteToUINT64(_now),  _scroll.idleFingDetectTimeout,  _scroll.maxLevelReached, _packetsCount);
    
    //
    // Circular Scroll
    //
    
    if (_touchmode == MODE_CIR_VSCROLL || _touchmode == MODE_CIR_HSCROLL)
    {
        int c_dxy = _scroll.edgeCirDelta;
        
        if ((sdy > c_dxy && sdx > c_dxy && _fing[0].y < _ycenter && _fing[0].x > _xcenter) ||
            (sdy > c_dxy && sdx < -c_dxy && _fing[0].y > _ycenter && _fing[0].x > _xcenter) ||
            (sdy < -c_dxy && sdx < -c_dxy && _fing[0].y > _ycenter && _fing[0].x < _xcenter) ||
            (sdy < -c_dxy && sdx > c_dxy && _fing[0].y < _ycenter && _fing[0].x < _xcenter))
        {
            
            if (_touchmode == MODE_CIR_VSCROLL){
                if (s_dy > 0) s_dy = 0 - s_dy;
                _scroll.edgeCirDxy = s_dy;
            }
            else {
                if (s_dx < 0) s_dx = 0 - s_dx;
                _scroll.edgeCirDxy = s_dx;
            }
        }
        
        else if ((sdy < -c_dxy && sdx < -c_dxy && _fing[0].y < _ycenter && _fing[0].x > _xcenter) ||
                 (sdy < -c_dxy && sdx > c_dxy && _fing[0].y > _ycenter && _fing[0].x > _xcenter) ||
                 (sdy > c_dxy && sdx > c_dxy && _fing[0].y > _ycenter && _fing[0].x < _xcenter) ||
                 (sdy > c_dxy && sdx < -c_dxy && _fing[0].y < _ycenter && _fing[0].x < _xcenter))
        {
            if (_touchmode == MODE_CIR_VSCROLL) {
                if (s_dy < 0) s_dy = 0 - s_dy;
                _scroll.edgeCirDxy = s_dy;
            }
            else {
                if (s_dx > 0) s_dx = 0 - s_dx;
                _scroll.edgeCirDxy = s_dx;
            }
        }
        
        if (s_dy != 0 || s_dx != 0) {
            
            // Use Max scroll level speeds
            if (abs(sdx) >= _scroll.contMaxDeltaEdge || abs(sdy) >= _scroll.contMaxDeltaEdge)
            {
                if (_scroll.edgeCirDxy <  0)
                    _scroll.edgeCirDxy = 0 -  _scroll.maxLevelReached;
                else
                    _scroll.edgeCirDxy =  _scroll.maxLevelReached;
                
                 _scroll.maxLevelReached =  _scroll.maxLevelReached/2 + 1;
            }
            
            if (_scroll.accelModeEdge == 3)
            {
                if (_touchmode == MODE_CIR_VSCROLL)
                    dispatchScrollDifference((_scroll.edgeCirDxy * _scrollFactor), 0, 0);
                else
                    dispatchScrollDifference(0, (_scroll.edgeCirDxy * _scrollFactor), 0);
            }
            else {
                if (_touchmode == MODE_CIR_VSCROLL)
                    _scroll.dy = _scroll.edgeCirDxy;
                else
                    _scroll.dx = _scroll.edgeCirDxy;
                
                if (!_scroll.doCont) {
                    _scroll.doCont = true;
                    _scrollTimer->setTimeoutMS(0);
                }
            }
        }
        else if (_scroll.doCont && !_scroll.hasEdgeCont) {
            _scroll.doCont = false;
            _scroll.dx = _scroll.dy = 0;
            _scroll.edgeCirDxy = 0;
            _scrollTimer->cancelTimeout();
        }
        
        DEBUG_SCROLL_SWIPES_LOG("STD :: Edge Cscroll XD %d, YD %d, CDXY %d LXD %d, LYD %d, FingerTime %lld, Reached Idle %s, HasCont %d, Mode %d, Cont %d, Delta %d\n", s_dx, s_dy, _scroll.edgeCirDxy, _scroll.dx, _scroll.dy, _fingPress.holdTime, (_fingPress.holdTime >  _scroll.idleFingDetectTimeout)?"Yes":"No", _scroll.hasEdgeCont, _touchmode, _scroll.doCont, _scroll.edgeCirDelta);
    }
    
    //
    // Slow (or) Continous Scroll
    //
    
    else if ((abs(sdx) > 0 || abs(sdy) > 0)
             
             && abs(sdx) < contMaxDelta
             && abs(sdy) < contMaxDelta
             
             && (((_scroll.dx != 0 || _scroll.dy != 0) && sMode < 2) ||
                 ((s_dx != 0 || s_dy != 0) && sMode > 1))
             
             && scrollDifflvl != 0)
    {
        bool contSAcell = false;
        float sFactor = _scrollFactor;
        
        switch (_touchmode) {
            case MODE_HSCROLL:
                // _scroll.dx = s_dx;
                _scroll.dy = s_dy = 0;
                if (isZoomConflict && s_dx)
                {
                    _scroll.skippedDx[_skipSample] = s_dx;
                    _scroll.skippedDy[_skipSample] = s_dy;
                    if (_skipSample < 10)
                        _skipSample++;
                    
                    DEBUG_SCROLL_SWIPES_LOG("STD :: Skipping zoom conflict\n");
                    return;
                }
                if (_scroll.has2FCont)
                    _touchmode = MODE_CONT_SCROLL;
                break;
                
            case MODE_VSCROLL:
                _scroll.dx = s_dx = 0;
                // _scroll.dy = s_dy;
                if (isZoomConflict && s_dy)
                {
                    _scroll.skippedDx[_skipSample] = s_dx;
                    _scroll.skippedDy[_skipSample] = s_dy;
                    if (_skipSample < 10)
                        _skipSample++;
                    
                    DEBUG_SCROLL_SWIPES_LOG("STD :: Skipping zoom conflict\n");
                    return;
                }
                if (_scroll.has2FCont)
                    _touchmode = MODE_CONT_SCROLL;
                break;
                
            case MODE_EDGE_VSCROLL:
                // _scroll.dy = sDelta;
                _scroll.dx = 0;
                sFactor = (_scroll.edgeSpeedFactor/100) * sFactor;
                
                if (_scroll.hasEdgeCont)
                    _touchmode = MODE_CONT_SCROLL;
                break;
                
            case MODE_EDGE_HSCROLL:
                // _scroll.dx = sDelta;
                _scroll.dy = 0;
                sFactor = (_scroll.edgeSpeedFactor/100) * sFactor;
                
                if (_scroll.hasEdgeCont)
                    _touchmode = MODE_CONT_SCROLL;
                break;
                
            default:
                break;
        }
        
        if (_touchmode != MODE_CONT_SCROLL) {
            _scroll.noInertia = false;
        }
        else {
            contSAcell = true;
        }
        
        //
        // Dispatch skipped scroll from zoom logic
        //
        
        if (_cFingers == 2 && _skipSample > 0)
        {
            for (int i = 0; i < _skipSample; i++) {
                
                DEBUG_SCROLL_SWIPES_LOG("STD :: Dispatch skipped (# %d) H Delta %d V Delta %d from Total %d \n", (i + 1), _scroll.skippedDx[i], _scroll.skippedDy[i], _skipSample);
                
                if (sMode == 3 && !contSAcell)
                    dispatchScrollDifference((_scroll.skippedDy[i] * sFactor), (_scroll.skippedDx[i] * sFactor), 0);
                else
                    dispatchScrollDifference(_scroll.skippedDy[i]/2, _scroll.skippedDx[i]/2, 0);
                
                IOSleep(3);
                
                _scroll.skippedDx[i] = _scroll.skippedDy[i] = 0;
            }
            _skipSample = 0;
        }
        
        //
        // Dispatch received scroll
        //
        
        switch (sMode) {
            case 0:
            case 2:
                if (contSAcell) {
                    
                    if (_scroll.dy != 0) {
                        if (_scroll.dy < 0)
                            _scroll.dy = 0 -  _scroll.maxLevelReached;
                        else
                            _scroll.dy =  _scroll.maxLevelReached;
                    }
                    
                    if (_scroll.dx != 0) {
                        if (_scroll.dx < 0)
                            _scroll.dx = 0 -  _scroll.maxLevelReached;
                        else
                            _scroll.dx =  _scroll.maxLevelReached;
                    }
                }
                else if (abs(sdy) > 0) {
                    
                    if (_scroll.dy != 0) {
                        if (_scroll.dy < 0)
                            _scroll.dy = -1;
                        else
                            _scroll.dy = 1;
                    }
                    
                    if (_scroll.dx != 0) {
                        if (_scroll.dx < 0)
                            _scroll.dx = -1;
                        else
                            _scroll.dx = 1;
                    }
                }
                break;
                
            case 1:
                if (_scroll.dy != 0) {
                    if (_scroll.dy < 0)
                        _scroll.dy = -5;
                    else
                        _scroll.dy = 5;
                }
                
                if (_scroll.dx != 0) {
                    if (_scroll.dx < 0)
                        _scroll.dx = -5;
                    else
                        _scroll.dx = 5;
                }
                break;
                
            case 3:
            case 4:
                if (contSAcell)
                {
                    if (_scroll.dy != 0) {
                        if (_scroll.dy < 0)
                            _scroll.dy = 0 -  _scroll.maxLevelReached;
                        else
                            _scroll.dy =  _scroll.maxLevelReached;
                        
                        _scroll.dy = _scroll.dy * 2;
                    }
                    
                    if (_scroll.dx != 0) {
                        if (_scroll.dx < 0)
                            _scroll.dx = 0 -  _scroll.maxLevelReached;
                        else
                            _scroll.dx =  _scroll.maxLevelReached;
                        
                        _scroll.dx = _scroll.dx * 2;
                    }
                    
                    if (!_scroll.doCont) {
                        _scroll.doCont = true;
                        _scrollTimer->cancelTimeout();
                        _scrollTimer->setTimeoutMS(0);
                    }
                }
                else {
                    
                    _scroll.dy = s_dy;
                    _scroll.dx = s_dx;
                    
                    if (sMode == 3) {
                        if (_scroll.doCont) {
                            _scroll.doCont = false;
                            _scrollTimer->cancelTimeout();
                        }
                        dispatchScrollDifference((_scroll.dy * sFactor), (_scroll.dx * sFactor), 0);
                    }
                }
                break;
                
            default:
                break;
        }
        
        //
        // Dispatch scroll timer
        //
        
        if (!_scroll.doCont && (_scroll.dy != 0 || _scroll.dx != 0) && sMode != 3) {
            _scroll.doCont = true;
            _scrollTimer->cancelTimeout();
            _scrollTimer->setTimeoutMS(0);
        }
        
        DEBUG_SCROLL_SWIPES_LOG("STD :: C Level LXD %d, LYD %d, Max %d, Slevel %d, SF %d\n", _scroll.dx, _scroll.dy,  _scroll.maxLevelReached, scrollDifflvl, (int)sFactor);
    }
    
    //
    // Normal scroll
    //
    
    else if ((abs(sdx) > 0 || abs(sdy) > 0) && scrollDifflvl != 0)
    {
        _scroll.noInertia = false;
        _scroll.doCont = false;
        float sFactor = _scrollFactor;
        
        switch (_touchmode) {
            case MODE_HSCROLL:
                _scroll.dx = s_dx;
                _scroll.dy = s_dy = 0;
                if (isZoomConflict && s_dx)
                {
                    _scroll.skippedDx[_skipSample] = s_dx;
                    _scroll.skippedDy[_skipSample] = s_dy;
                    if (_skipSample < 10)
                        _skipSample++;
                    
                    DEBUG_SCROLL_SWIPES_LOG("STD :: Skipping zoom conflict\n");
                    return;
                }
                break;
                
            case MODE_VSCROLL:
                _scroll.dx = s_dx = 0;
                _scroll.dy = s_dy;
                if (isZoomConflict && s_dy)
                {
                    _scroll.skippedDx[_skipSample] = s_dx;
                    _scroll.skippedDy[_skipSample] = s_dy;
                    if (_skipSample < 10)
                        _skipSample++;
                    
                    DEBUG_SCROLL_SWIPES_LOG("STD :: Skipping zoom conflict\n");
                    return;
                }
                break;
                
            case MODE_EDGE_VSCROLL:
                _scroll.dy = sDelta;
                _scroll.dx = 0;
                sFactor = (_scroll.edgeSpeedFactor/100) * sFactor;
                break;
                
            case MODE_EDGE_HSCROLL:
                _scroll.dx = sDelta;
                _scroll.dy = 0;
                sFactor = (_scroll.edgeSpeedFactor/100) * sFactor;
                break;
                
            default:
                break;
        }
        
        //
        // Dispatch skipped scroll from zoom logic
        //
        
        if (_cFingers == 2 && _skipSample > 0)
        {
            for (int i = 0; i < _skipSample; i++) {
                
                DEBUG_SCROLL_SWIPES_LOG("STD :: Dispatch skipped (# %d) H Delta X %d V Delta X %d from Total %d \n", (i + 1), _scroll.skippedDx[i], _scroll.skippedDy[i], _skipSample);
                
                dispatchScrollDifference((_scroll.skippedDy[i] * _scrollFactor), (_scroll.skippedDx[i] * _scrollFactor), 0);
                IOSleep(4);
                
                _scroll.skippedDx[i] = _scroll.skippedDy[i] = 0;
            }
            _skipSample = 0;
        }
        
        //
        // Dispatch received scroll
        //
        
        switch (sMode) {
            case 0:
            case 1:
            case 4:
                if (!_scroll.doCont)
                {
                    _scroll.doCont = true;
                    _scrollTimer->cancelTimeout();
                    _scrollTimer->setTimeoutMS(0);
                }
                break;
                
            case 2:
                dispatchScrollDifference((s_dy * sFactor)/2, (s_dx * sFactor)/2, 0);
                break;
                
            case 3:
                dispatchScrollDifference(_scroll.dy * sFactor, _scroll.dx * sFactor, 0);
                break;
                
            default:
                break;
        }
        
        DEBUG_SCROLL_SWIPES_LOG("STD :: Scroll Level LXD %d, LYD %d, Max %d, Slevel %d PC %d, SF %d\n", _scroll.dx, _scroll.dy,  _scroll.maxLevelReached, scrollDifflvl, _packetsCount, (int)sFactor);
    }
    else if (scrollDifflvl == 0 && _scroll.doCont
             && ((_cFingers == 2 && !_scroll.has2FCont) || (_cFingers == 1 && !_scroll.hasEdgeCont)))
    {
        _scroll.doCont = false;
        _scroll.dx = _scroll.dy = 0;
        _scrollTimer->cancelTimeout();
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes multi finger touch.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_multiFingers_touch(int m_dx, int m_dy, int fid)
{
    //
    // Increment the processed packets count value
    // until max required threshold
    //
    
    if (_packetsCount < 255)
        _packetsCount++;
    
    //
    // Convert signed finger delta values to unsigned for conditional
    //
    
    unsigned int m_xDelta = 0,  m_yDelta = 0;
    
    if (_fing[fid].tdx < 0)
        m_xDelta = 0 - _fing[fid].tdx;
    else
        m_xDelta = _fing[fid].tdx;
    
    if (_fing[fid].tdy < 0)
        m_yDelta = 0 - _fing[fid].tdy;
    else
        m_yDelta = _fing[fid].tdy;
    
    DEBUG_MUL_FING_LOG("STD :: %d Fingers Touch - DX[%d] %d, DY[%d] %d, Pressure %d, Width %d, PacketsCount %d, Tap Counts %d, LastFing %d TouchTime %lld, FPressTime %lld, Mode %d, 3F Drag %d.\n", _cFingers, fid, m_dx, fid, m_dy, _fing[fid].pressure, _fing[fid].width, _packetsCount, _tap.count, _lastFingers, _gestureTime, _fingPress.holdTime, _touchmode, _drag.is3FDrag);

    DEBUG_MUL_FING_LOG("STD :: %d Fingers Touch - DX[%d] %d, DY[%d] %d", _cFingers, fid, m_dx, fid, m_dy);
    
    for (int i = 0; i < _cFingers; i++) {
        DEBUG_MUL_FING_LOG(" X[%d] %d, Y[%d] %d, TDX[%d] %d, TDY[%d] %d", i, _fing[i].x, i, _fing[i].y, i, _fing[i].tdx, i, _fing[i].tdy);
    }
    DEBUG_MUL_FING_LOG(" Pressure %d, Width %d, PacketsCount %d, Tap Counts %d, LastFing %d TouchTime %lld, FPressTime %lld, Mode %d, 3F Drag %d.\n", _fing[fid].pressure, _fing[fid].width, _packetsCount, _tap.count, _lastFingers, _gestureTime, _fingPress.holdTime, _touchmode, _drag.is3FDrag);
    
    
    //
    // Process touchpad modes
    //
    
    switch (_touchmode) {
            
            //
            // Clear 1F and 2F modes
            //
            
        case MODE_MOVE:
            // Dispatch 3f drag and return
            if (_drag.is3FDrag)
            {
                if ((_drag.fPressDragFingersID & (fid + fid)) ||
                    ((_drag.fPressDragFingersID & 1) && fid == 0))
                {
                    Process_singlefinger_touch(_fing[fid].x, _fing[fid].y, fid);
                }
                return;
            }
        case MODE_1_FING_TAP:
        case MODE_2_FING_TAP:
        case MODE_CORNER_TAP: _cornerTap.corner = 0;
        case MODE_EDGE_VSCROLL:
        case MODE_EDGE_HSCROLL:
        case MODE_CIR_VSCROLL:
        case MODE_CIR_HSCROLL:
        case MODE_CONT_SCROLL:
        case MODE_VSCROLL:
        case MODE_HSCROLL:
            
            //
            // Stop the enhanced pointer and scroll accleration if its active
            //
            
            if (_pContinuousMove) {
                StopPointerEnhanceTimer();
            }
            
            StopScrollEnhanceTimer();
            
            _touchmode = MODE_IDLE;
            break;
            
        case MODE_IDLE:
            // Dispatch 3f drag and return
            if (_drag.is3FDrag)
            {
                if ((_drag.fPressDragFingersID & (fid + fid)) ||
                    ((_drag.fPressDragFingersID & 1) && fid == 0))
                {
                    Process_singlefinger_touch(_fing[fid].x, _fing[fid].y, fid);
                }
                return;
            }
        case MODE_3_FING_TAP:
        case MODE_4_FING_TAP:
        case MODE_5_FING_TAP:
        default:
            
            //
            // Enable Tap mode if its within tap time and delta.
            //
            
            if (_gestureTime < _tap.mulFingsTimeout
                && m_xDelta < _tap.noiseMultiTapDelta && m_yDelta < _tap.noiseMultiTapDelta)
            {
                //
                // Check the delta values of all the fingers and
                // return if any have > _tap.noiseMultiTapDelta.
                //
                
                for (int i = 0; i < _cFingers; i++)
                {
                    if (_fing[i].tdx > _tap.noiseMultiTapDelta || _fing[i].tdx < -_tap.noiseMultiTapDelta ||
                        _fing[i].tdy > _tap.noiseMultiTapDelta || _fing[i].tdy < -_tap.noiseMultiTapDelta)
                    {
                        if (_touchmode == MODE_3_FING_TAP
                            ||  _touchmode ==MODE_4_FING_TAP
                            || _touchmode == MODE_5_FING_TAP)
                        {
                            _touchmode = MODE_IDLE;
                        }
                        
                        return;
                    }
                }
                
                if (_lastFingers <= _cFingers)
                {
                    switch (_cFingers) {
                        case 3:
                            _touchmode = MODE_3_FING_TAP;
                            break;
                            
                        case 4:
                            _touchmode = MODE_4_FING_TAP;
                            break;
                            
                        case 5:
                            _touchmode = MODE_5_FING_TAP;
                            break;
                            
                        default:
                            break;
                    }
                }
            }
            
            //
            // Clear the Tap mode as its over tap time and tap delta.
            //
            
            else
            {
                switch (_touchmode) {
                    case MODE_4_FING_TAP:
                    case MODE_5_FING_TAP:
                    case MODE_3_FING_TAP:
                        _touchmode = MODE_IDLE;
                        break;
                        
                    default:
                        break;
                }
            }
            
            //
            // No processing required to already triggered gestures
            // of Appswitch, 2F Click and Button.
            //
            
            if (!_swipe.isAppSwitchDone &&
                !((_button.is2FClick || _button.isFingOnBtnArea) && _button.triggered))
            {
                break;
            }
            
        case MODE_MUL_TOUCH_3:
        case MODE_MUL_TOUCH_4:
        case MODE_FEATURE_TOGGLE:
        case MODE_5_FING_PINCH:
        case MODE_4_FING_PINCH:
        case MODE_TWO_FING_PRESS:
        case MODE_THREE_FING_PRESS:
            // Dispatch 3f drag and return
            if (_drag.is3FDrag)
            {
                if ((_drag.fPressDragFingersID & (fid + fid)) ||
                    ((_drag.fPressDragFingersID & 1) && fid == 0))
                {
                    Process_singlefinger_touch(_fing[fid].x, _fing[fid].y, fid);
                }
                return;
            }

        case MODE_FOUR_FING_PRESS:
        case MODE_FIVE_FING_PRESS:
        case MODE_ZOOM:
        case MODE_ROTATE:
            
            //
            // Stop the enhanced scroll accleration if its active
            //
            
            StopScrollEnhanceTimer();
            return;
    }
    
    //
    // Do nothing on first packet
    //
    
    if (_packetsCount == 1)
        return;
    
    
    if ((_fingPress.fings3Press | _fingPress.fings4Press | _fingPress.fings5Press) > 0
        || _kbEvent.isToggleKeyOnHold)
    {
        //
        // Calculate finger press time
        //
        
        if (!_button.triggered
            && _gestureTime < (_fingPress.timeout + 4000000000) // 4 sec + timeout to detect
            && (m_xDelta < 50 && m_yDelta < 50))
        {
            _fingPress.gestureTime = _gestureTime;
        }
        else if (_fingPress.timeout != 0 && _fingPress.gestureTime != 0){
            _fingPress.gestureTime = 0;
            _gestureTime = 0;
        }
        
        //
        // Process finger press
        //
        
        if (_fingPress.gestureTime > _fingPress.timeout || _button.triggered)
        {
            switch(_cFingers)
            {
                case 3:
                    // Toggle 3f press
                    if (_kbEvent.isToggleKeyOnHold) {
                        
                        _fingPress.fings3Press = 0 - _fingPress.fings3Press;
                        if (_fingPress.fings3Press > 0)
                        {
                            DEBUG_TOGGLE_LOG("STD :: 3 Fingers press is enabled.\n");
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        }
                        else
                        {
                            DEBUG_TOGGLE_LOG("STD :: 3 Fingers press is disabled.\n");
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                        _touchmode = MODE_THREE_FING_PRESS;
                    }
                    // Trigger plist configured gesture
                    else {
                        Proces_Fingers_Press(3);
                    }
                    break;
                    
                case 4:
                    // Toggle 4f press
                    if (_kbEvent.isToggleKeyOnHold) {
                        
                        _fingPress.fings4Press = 0 - _fingPress.fings4Press;
                        if (_fingPress.fings4Press > 0)
                        {
                            DEBUG_TOGGLE_LOG("STD :: 4 Fingers press is enabled.\n");
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        }
                        else
                        {
                            DEBUG_TOGGLE_LOG("STD :: 4 Fingers press is disabled.\n");
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                    }
                    // Trigger plist configured gesture
                    else {
                        Proces_Fingers_Press(4);
                    }
                    _touchmode = MODE_FOUR_FING_PRESS;
                    break;
                    
                case 5:
                    // Toggle 5f press
                    if (_kbEvent.isToggleKeyOnHold) {
                        
                        _fingPress.fings5Press =  0 - _fingPress.fings5Press;
                        if (_fingPress.fings5Press > 0)
                        {
                            DEBUG_TOGGLE_LOG("STD :: 5 Fingers press is enabled.\n");
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        }
                        else
                        {
                            DEBUG_TOGGLE_LOG("STD :: 5 Fingers press is disabled.\n");
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                    }
                    // Trigger plist configured gesture
                    else {
                        Proces_Fingers_Press(5);
                    }
                    _touchmode = MODE_FIVE_FING_PRESS;
                    break;
                    
                default:
                    break;
            }
            
        }
        
        if (_touchmode == MODE_THREE_FING_PRESS
            || _touchmode == MODE_FOUR_FING_PRESS
            || _touchmode == MODE_FIVE_FING_PRESS)
        {
            return;
        }
    }
    
    //
    // Process Swipes
    //
    
    if ((_fing[fid].width > 0 || _hw_version > 4) && _tap.count == 0
        && (_packetsCount > 3 || m_xDelta > _swipe.mulFingDeltaX || m_yDelta > _swipe.mulFingDeltaY)
        
        && ((_swipe.has3FSwipes && _cFingers == 3 &&
             (_lastFingers <= 3 || (m_xDelta < 30 && m_yDelta < 30 && _lastFingers > 3))) ||
            
            (_swipe.has4FSwipes && _cFingers == 4 &&
             (_lastFingers <= 4 || (m_xDelta < 30 && m_yDelta < 30 && _lastFingers > 4)))))
    {
        
        if (_hw_version > 3) {
            
            //
            // Check the fingers swipe direction
            // in order avoid pinch like gestures from triggering swipes.
            //
            
            int fingersMovingDown = 0, fingersMovingUp = 0;
            int fingersMovingLeft = 0, fingersMovingRight = 0;
            
            for (int i = 0; i < STD_MAX_FINGERS; i++) {
                
                if (_fing[i].tdy > 60) {
                    fingersMovingDown++;
                }
                else if (_fing[i].tdy < -60) {
                    fingersMovingUp++;
                }
                
                if (_fing[i].tdx > 60) {
                    fingersMovingRight++;
                }
                else if (_fing[i].tdx < -60) {
                    fingersMovingLeft++;
                }
            }
            
            DEBUG_SCROLL_SWIPES_LOG("STD :: Swipe Fingers Moving Down %d, Up %d, Right %d, Left %d\n", fingersMovingDown, fingersMovingUp, fingersMovingRight, fingersMovingLeft);
            
            UInt8 fingersToMatch;
            
            switch (_touchpadVendor) {
                case STD_SYNAPTICS:
                    fingersToMatch = 2; // We get only two fingers tracking
                    break;
                   
                case STD_FOCAL_TECH:
                default: // Sometimes a finger data is not received enough
                    fingersToMatch = _cFingers - 1;
                    
                    //
                    // Hardcoded 4F pinch
                    //
                    
                    if (_cFingers == 4 && !_kbEvent.isToggleKeyOnHold
                        && fingersMovingDown == 3 && fingersMovingUp == 1)
                    {
                        if (_fings4PinchAction == 17) {
                            Open_NC();
                            ResetPointerToMiddle();
                        }
                        else
                            _device->dispatchPS2Notification(_fings4PinchAction);
                        
#if DEBUG_TPD
                        if (_debugLvl & 8) {
                            LogSwipeGestureAction(_fings4PinchAction);
                        }
#endif
                        
                        _touchmode = MODE_4_FING_PINCH;
                        DEBUG_MUL_FING_LOG("STD :: 4F Pinch in triggered.\n");
                        return;
                    }
                    
                    if (_cFingers == 4 && !_kbEvent.isToggleKeyOnHold
                        && fingersMovingUp == 3 && fingersMovingRight == 2)
                    {
                        if (_fings4PinchAction == 17) {
                            Open_NC();
                            ResetPointerToMiddle();
                        }
                        else {
                            _device->dispatchPS2Notification(_fings4PinchAction);
                        }
                        
#if DEBUG_TPD
                        if (_debugLvl & 8)  {
                            LogSwipeGestureAction(_fings4PinchAction);
                        }
#endif
                        _touchmode = MODE_4_FING_PINCH;
                        DEBUG_MUL_FING_LOG("STD :: 4F Pinch out triggered.\n");
                        return;
                    }
                    break;
            }
            
            if (!((fingersMovingDown >= fingersToMatch) ||
                  (fingersMovingUp >= fingersToMatch) ||
                  (fingersMovingLeft >= fingersToMatch) ||
                  (fingersMovingRight >= fingersToMatch)))
            {
                DEBUG_MUL_FING_LOG("STD :: Fingers not matched for swipes.\n");
                return;
            }
        }
        
        
        if (m_xDelta > m_yDelta && m_xDelta > _swipe.mulFingDeltaX && _fing[fid].tdx < 0) {
            _swipe.direction = 1; // left
        }
        else if (m_xDelta > m_yDelta && m_xDelta > _swipe.mulFingDeltaX && _fing[fid].tdx > 0) {
            _swipe.direction = 2; // right
        }
        else if (m_yDelta > m_xDelta && m_yDelta > _swipe.mulFingDeltaY && _fing[fid].tdy > 0) {
            _swipe.direction = 3; // down
        }
        else if (m_yDelta > m_xDelta && m_yDelta > _swipe.mulFingDeltaX && _fing[fid].tdy < 0) {
            _swipe.direction = 4; // up
        }
        
        //
        // Toggle features
        //
        
        if (_kbEvent.isToggleKeyOnHold && _swipe.direction > 0)
        {
            switch (_swipe.direction) {
                case 1:
                    _zoom.enabled = !_zoom.enabled;
                    
                    if (_zoom.enabled)
                    {
                        DEBUG_TOGGLE_LOG("STD :: Pinch Zoom is enabled.\n");
                        dispatchRelativePointerPosition(0, -50, _button.state); // up
                    }
                    else
                    {
                        DEBUG_TOGGLE_LOG("STD :: Pinch Zoom is disabled.\n");
                        dispatchRelativePointerPosition(0, 50, _button.state); // down
                    }
                    break;
                    
                case 2:
                    _rotate.enabled = !_rotate.enabled;
                    if (_rotate.enabled)
                    {
                        DEBUG_TOGGLE_LOG("STD :: Rotations are enabled.\n");
                        dispatchRelativePointerPosition(0, -50, _button.state); // up
                    }
                    else
                    {
                        DEBUG_TOGGLE_LOG("STD :: Rotations are disabled.\n");
                        dispatchRelativePointerPosition(0, 50, _button.state); // down
                    }
                    break;
                    
                case 3:
                    _swipe.hasEdgeSwipes = !_swipe.hasEdgeSwipes;
                    
                    if (_swipe.hasEdgeSwipes)
                    {
                        DEBUG_TOGGLE_LOG("STD :: Edge Swipes are enabled.\n");
                        dispatchRelativePointerPosition(0, -50, _button.state); // up
                    }
                    else
                    {
                        DEBUG_TOGGLE_LOG("STD :: Edge Swipes are disabled.\n");
                        dispatchRelativePointerPosition(0, 50, _button.state); // down
                    }
                    break;
                    
                case 4:
                    _cornerTap.enabled = !_cornerTap.enabled;
                    
                    if (_cornerTap.enabled)
                    {
                        DEBUG_TOGGLE_LOG("STD :: Corner Taps are enabled.\n");
                        dispatchRelativePointerPosition(0, -50, _button.state); // up
                    }
                    else
                    {
                        DEBUG_TOGGLE_LOG("STD :: Corner Taps are disabled.\n");
                        dispatchRelativePointerPosition(0, 50, _button.state); // down
                    }
                    break;
                    
                default:
                    break;
            }
            
            _touchmode = MODE_FEATURE_TOGGLE;
        }
        else if (_swipe.direction > 0)
        {
            if (_cFingers == 3)
                _touchmode = MODE_MUL_TOUCH_3;
            else if (_cFingers == 4)
                _touchmode = MODE_MUL_TOUCH_4;
        }
        
        DEBUG_MUL_FING_LOG("STD :: %d Fingers %s Swipe detected %s\n", _cFingers, (_swipe.direction == 1)?"Left":
                                                                    (_swipe.direction == 2)?"Right":
                                                                    (_swipe.direction == 3)?"Down":
                                                                    (_swipe.direction == 4)?"Up":"No",
              _kbEvent.isToggleKeyOnHold?"in Features toggle":"");
    }
    
    //
    // Process 4f & 5f pinch
    //
    
    else if (_cFingers == 5 && _fings5PinchAction > 0 && _packetsCount > 3
             && (m_xDelta > 50 || m_yDelta > 50))
    {
        int fingersMovingDown = 0, fingersMovingUp = 0;
        int fingersMovingLeft = 0, fingersMovingRight = 0;
        
        for (int i = 0; i < STD_MAX_FINGERS; i++) {
            
            if (_fing[i].tdy > 80)
                fingersMovingDown++;
            else if (_fing[i].tdy < -80)
                fingersMovingUp++;
            
            if (_fing[i].tdx > 80)
                fingersMovingRight++;
            else if (_fing[i].tdx < -80)
                fingersMovingLeft++;
        }
        
        DEBUG_MUL_FING_LOG("STD :: Pinch Fingers Moving Down %d, Up %d, Right %d, Left %d\n", fingersMovingDown, fingersMovingUp, fingersMovingRight, fingersMovingLeft);
        
        //
        // Trigger 5f pinch when atleast 3 fingers are moving same direction and
        // few fingers are in opposite direction.
        // Note: Its hardcoded for Synaptics.
        //
        
        if ((_touchpadVendor == STD_SYNAPTICS
             && fingersMovingDown == 1 && fingersMovingLeft == 1) ||
            
            ((fingersMovingDown > 2 && fingersMovingDown < 5 &&
              (fingersMovingUp != 0 || (fingersMovingLeft != 0 && fingersMovingRight != 0))) ||
             
             (fingersMovingUp > 2 && fingersMovingUp < 5 &&
              (fingersMovingDown != 0 || (fingersMovingLeft != 0 && fingersMovingRight != 0))) ||
             
             (fingersMovingLeft > 2 && fingersMovingLeft < 5 &&
              (fingersMovingRight != 0 || (fingersMovingDown != 0 && fingersMovingUp != 0))) ||
             
             (fingersMovingRight > 2 && fingersMovingRight < 5 &&
              (fingersMovingLeft != 0 || (fingersMovingUp != 0 && fingersMovingDown != 0)))))
        {
            if (_fings5PinchAction == 17) {
                Open_NC();
                ResetPointerToMiddle();
            }
            else
                _device->dispatchPS2Notification(_fings5PinchAction);
#if DEBUG_TPD
            if (_debugLvl & 8)
                LogSwipeGestureAction(_fings5PinchAction);
#endif
            
            _touchmode = MODE_5_FING_PINCH;
            DEBUG_MUL_FING_LOG("STD :: 5F Pinch triggered.\n");
        }
        
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes fingers press.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Proces_Fingers_Press(UInt8 fingers)

{
    UInt8 fPressAction = 0;
    switch (fingers) {
        case 2:
            if (_drag.hasFPresDrag
                && (_drag.fPressDragFingersNum & 0x2))
            {
                if (_fingPress.gestureTime > _drag.fPressDragStartTimeOut || _button.triggered) {
                    _drag.isDragging = _drag.isHoldDrag = true;
                    _drag.is3FDrag = true;
                    _touchmode = MODE_TWO_FING_PRESS;
                    IOLog("STD :: 2F Drag started\n");
                }
            }
            else {
                fPressAction = _fingPress.fings2Press;
                _touchmode = MODE_TWO_FING_PRESS;
            }
            break;
            
        case 3:
            if (_drag.hasFPresDrag
                && (_drag.fPressDragFingersNum & 0x4))
            {
                if (_fingPress.gestureTime > _drag.fPressDragStartTimeOut || _button.triggered) {
                    _drag.isDragging = _drag.isHoldDrag = true;
                    _drag.is3FDrag = true;
                    _touchmode = MODE_THREE_FING_PRESS;
                    IOLog("STD :: 3F Drag started\n");
                    
                }
            }
            else {
                fPressAction = _fingPress.fings3Press;
                _touchmode = MODE_THREE_FING_PRESS;
            }
            break;
            
        case 4:
            fPressAction = _fingPress.fings4Press;
            break;
            
        case 5:
            fPressAction = _fingPress.fings5Press;
            break;
            
        default:
            break;
    }
    
    if (fPressAction == kPS2C_Gesture_NC_Workaround)
    {
        Open_NC();
        ResetPointerToMiddle();
    }
    else if (fPressAction > 0)
    {
        _device->dispatchPS2Notification(fPressAction);
    }
    
    DEBUG_ZRP_LOG("STD :: %d Fingers Press Performed with Action:: ", fingers);
    
#if DEBUG_TPD
    if (_debugLvl & 64)
        LogSwipeGestureAction(fPressAction);
#endif

}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes edge swipes.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Is2FingEdgeSwipe()
{
    //
    // Enable edge swipes mode
    // if the finger(s) starts from edge area.
    //
    
    if (_swipe.hasEdgeSwipes
        && (_swipe.edgeFingers == 0 || _swipe.edgeFingers == 2)
        && _fing[0].y != 0 && _fing[1].y != 0 &&
        
        // Finger Y axis lies between non edge swipe area ? Left and right Edge swipes
        ((_fing[0].start_y > (_ymin + _swipe.nonEdgeVArea)
          && _fing[0].start_y < (_ymax - _swipe.nonEdgeVArea)
          
          && _fing[1].start_y > (_ymin + _swipe.nonEdgeVArea)
          && _fing[1].start_y < (_ymax - _swipe.nonEdgeVArea))
         
         ||
         
         // Finger X axis lies between non edge swipe area ? Top and bottom edge swipes
         (_fing[0].start_x > (_xmin + _swipe.nonEdgeHArea)
          && _fing[0].start_x < (_xmax - _swipe.nonEdgeHArea)
          
          && _fing[1].start_x > (_xmin + _swipe.nonEdgeHArea)
          && _fing[1].start_x < (_xmax - _swipe.nonEdgeHArea))))
    {
        
        // Top edge swipe ?
        if (_swipe.topEdge > 0 &&
            ((_fing[0].start_y < (_ymin + _swipe.edgeVArea) &&
              _fing[1].start_y < (_ymin + _swipe.edgeVArea)) ||
             
             (_fing[0].start_y > (_ymin + _swipe.edgeVDistance) &&
              _fing[1].start_y > (_ymin + _swipe.edgeVDistance) &&
              _swipe.has2WayEdgeSwipes && _swipe.isEdgeGesture)))
        {
            _swipe.direction = 4;
            _swipe.isEdgeGesture = true;
        }
        // Bottom edge swipe ?
        else if (_swipe.bottomEdge > 0 &&
                 ((_fing[0].start_y > (_ymax - _swipe.edgeVArea) &&
                   _fing[1].start_y > (_ymax - _swipe.edgeVArea)) ||
                  
                  (_fing[0].start_y < (_ymax - _swipe.edgeVDistance) &&
                   _fing[1].start_y < (_ymax - _swipe.edgeVDistance) &&
                   _swipe.has2WayEdgeSwipes && _swipe.isEdgeGesture)))
        {
            _swipe.direction = 3;
            _swipe.isEdgeGesture = true;
        }
        // Right edge swipe ?
        else if (_swipe.rightEdge > 0 &&
                 ((_fing[0].start_x > (_xmax - _swipe.edgeHArea) ||
                   _fing[1].start_x > (_xmax - _swipe.edgeHArea)) ||
                  
                  (_fing[0].start_x < (_xmax - _swipe.edgeHDistance) &&
                   _fing[1].start_x < (_xmax - _swipe.edgeHDistance) &&
                   _swipe.has2WayEdgeSwipes && _swipe.isEdgeGesture)))
        {
            _swipe.direction = 2;
            _swipe.isEdgeGesture = true;
        }
        // Left edge swipe ?
        else if (_swipe.leftEdge > 0 &&
                 ((_fing[0].start_x < (_xmin + _swipe.edgeHArea) ||
                   _fing[1].start_x < (_xmin + _swipe.edgeHArea)) ||
                  
                  (_fing[0].start_x > (_xmin + _swipe.edgeHDistance) &&
                   _fing[1].start_x > (_xmin + _swipe.edgeHDistance) &&
                   _swipe.has2WayEdgeSwipes && _swipe.isEdgeGesture)))
        {
            _swipe.direction = 1;
            _swipe.isEdgeGesture = true;
        }
    }
    
#if DEBUG_SCROLL_SWIPES_LOG
    if (_swipe.isEdgeGesture) {
        IOLog("STD :: Two fingers edge swipe detected with Direction %d for Distance H %d V %d Area H %d V %d, X Max %d Y Max %d, X Min %d Y Min %d\n", _swipe.direction, _swipe.edgeHDistance, _swipe.edgeVDistance, _swipe.edgeHArea, _swipe.edgeVArea, _xmax, _ymax, _xmin, _ymin);
    }
#endif
    
}

void ApplePS2SmartTouchpad::Process_edgeSwipe(int x, int y, int fid)
{
    //
    // 20% of Horizontal(left and right edges) and Vertical(top and bottom) area
    // are disabled for edge swipes, so it leaves 60% of area between them for swipes.
    // _swipe.nonEdgeVArea and _swipe.nonEdgeVArea defines area where edge swipe detection disabled.
    // _swipe.edgeHArea and _swipe.edgeVArea defines area where the edge swipe detection is done.
    //
    
    DEBUG_SCROLL_SWIPES_LOG("STD :: Edge Swipe X %d, Y %d, StartX %d, StartY %d, XMin %d XMax %d, YMin %d YMax %d, EdgeArea X %d Y %d, Non-EdgeArea X %d Y %d, SwipeDistance X %d Y %d, Mode %d, Direction %d\n", x, y, _fing[fid].start_x, _fing[fid].start_y, _xmin, _xmax, _ymin, _ymax, _swipe.edgeHArea, _swipe.edgeVArea, _swipe.nonEdgeHArea, _swipe.nonEdgeVArea, _swipe.edgeHDistance, _swipe.edgeVDistance, _touchmode, _swipe.direction);
    
    switch (_swipe.direction) {
        case 1:
            if (_swipe.leftEdge > 0 &&
                // OUT
                ((x < (_xmin + _swipe.edgeHArea) && _fing[fid].start_x > (_xmin + _swipe.edgeHDistance)
                  && (_touchmode == MODE_LEFT_EDGE_SWIPE || (_swipe.has2WayEdgeSwipes && _touchmode < 26))) ||
                 
                 // IN
                 (x > (_xmin + _swipe.edgeHDistance) /*&& _touchmode != MODE_EDGE_SWIPE*/
                  && _fing[fid].start_x < (_xmin + _swipe.edgeHArea))))
            {
                DEBUG_SCROLL_SWIPES_LOG("STD :: Left Edge Swipe Triggered with Action ");

                //
                // Dispatch
                //
                
                if (_fing[fid].start_x < (_xmin + _swipe.edgeHArea)) // IN
                {
                    Dispatch_edgeSwipe(_swipe.leftEdge, 0);
                }
                else if (x < (_xmin + _swipe.edgeHArea)) // OUT
                {
                    Dispatch_edgeSwipe(_swipe.leftEdge, 1);
                }
                
                // Set touch mode
                _touchmode = MODE_LEFT_EDGE_SWIPE;
                
                //
                // Reset Start X to make the reverse swipe to work without finger lift
                //
                
                switch (_cFingers) {
                    case 2:
                        if (_fing[fid].start_x < (_xmin + _swipe.edgeHArea)) {
                            _fing[0].start_x = x + _swipe.edgeHArea;
                            _fing[1].start_x = x + _swipe.edgeHArea;
                        }
                        
                        else if (_fing[fid].start_x > (_xmin + _swipe.edgeHDistance)) {
                            _fing[0].start_x = _xmin;
                            _fing[1].start_x = _xmin;
                        }
                        break;
                        
                    default:
                        if (_fing[fid].start_x < (_xmin + _swipe.edgeHArea))
                            _fing[fid].start_x = x + _swipe.edgeHArea;
                        
                        else if (_fing[fid].start_x > (_xmin + _swipe.edgeHDistance))
                            _fing[fid].start_x = _xmin;
                        break;
                }
                
            }
            break;
            
        case 2:
            if (_swipe.rightEdge > 0 &&
                
                // OUT
                ((x > (_xmax - _swipe.edgeHArea) && _fing[fid].start_x < (_xmax - _swipe.edgeHDistance)
                  && (_touchmode == MODE_RIGHT_EDGE_SWIPE || (_swipe.has2WayEdgeSwipes && _touchmode < 26)))
                
                 ||
                 
                 // IN
                 (x < (_xmax - _swipe.edgeHDistance) /*&& _touchmode != MODE_EDGE_SWIPE*/
                  && _fing[fid].start_x > (_xmax - _swipe.edgeHArea))))
            {
                
                DEBUG_SCROLL_SWIPES_LOG("STD :: Right Edge Swipe Triggered with Action ");

                //
                // Dispatch
                //
                
                if (_fing[fid].start_x > (_xmax - _swipe.edgeHArea)) // IN
                {
                    Dispatch_edgeSwipe(_swipe.rightEdge, 0);
                }
                else if (x > (_xmax - _swipe.edgeHArea)) // OUT
                {
                    Dispatch_edgeSwipe(_swipe.rightEdge, 1);
                }
                
                // Set touch mode
                _touchmode = MODE_RIGHT_EDGE_SWIPE;
                
                //
                // Reset Start X to make the reverse swipe to work without finger lift
                //
                
                switch (_cFingers) {
                    case 2:
                        if (_fing[fid].start_x < (_xmax - _swipe.edgeHDistance)) {
                            _fing[0].start_x = _xmax;
                            _fing[1].start_x = _xmax;
                        }
                        
                        else if (_fing[fid].start_x > (_xmax - _swipe.edgeHArea)) {
                            _fing[0].start_x = x - _swipe.edgeHArea;
                            _fing[1].start_x = x - _swipe.edgeHArea;
                        }
                        break;
                        
                    default:
                        if (_fing[fid].start_x < (_xmax - _swipe.edgeHDistance))
                            _fing[fid].start_x = _xmax;
                        
                        else if (_fing[fid].start_x > (_xmax - _swipe.edgeHArea))
                            _fing[fid].start_x = x - _swipe.edgeHArea;
                        break;
                }
                
            }
            break;
            
        case 3:
            if (_swipe.bottomEdge > 0 &&
                
                // OUT
                ((y > (_ymax - _swipe.edgeVArea) && _fing[fid].start_y < (_ymax - _swipe.edgeVDistance)
                  && (_touchmode == MODE_BOTTOM_EDGE_SWIPE || (_swipe.has2WayEdgeSwipes && _touchmode < 26)))
                 
                 ||
                 
                 // IN
                 (y < (_ymax - (_swipe.edgeVDistance)) /*&& _touchmode != MODE_EDGE_SWIPE*/
                  && _fing[fid].start_y > (_ymax - (_swipe.edgeVArea * 0.8)))))
            {
                
                DEBUG_SCROLL_SWIPES_LOG("STD :: Bottom Edge Swipe Triggered with Action ");

                //
                // Dispatch
                //
                
                if (_fing[fid].start_y > (_ymax - (_swipe.edgeVArea - 30))) // IN
                {
                    Dispatch_edgeSwipe(_swipe.bottomEdge, 0);
                }
                else if (y > (_ymax - _swipe.edgeVArea)) // OUT
                {
                    Dispatch_edgeSwipe(_swipe.bottomEdge, 1);
                }
                
                // Set touch mode
                _touchmode = MODE_BOTTOM_EDGE_SWIPE;
                
                //
                // Reset Start Y to make the reverse swipe to work without finger lift
                //
                
                switch (_cFingers) {
                    case 2:
                        if (_fing[fid].start_y > (_ymax - (_swipe.edgeVArea * 0.8))) {
                            _fing[0].start_y = y - _swipe.edgeVArea;
                            _fing[1].start_y = y - _swipe.edgeVArea;
                        }
                        
                        else if (_fing[fid].start_y < (_ymax - _swipe.edgeVDistance)) {
                            _fing[0].start_y = _ymax;
                            _fing[1].start_y = _ymax;
                        }
                        break;
                        
                    default:
                        if (_fing[fid].start_y > (_ymax - (_swipe.edgeVArea * 0.8)))
                            _fing[fid].start_y = y - _swipe.edgeVArea;
                        
                        else if (_fing[fid].start_y < (_ymax - _swipe.edgeVDistance))
                            _fing[fid].start_y = _ymax;
                        break;
                }
            }
            break;
            
        case 4:
            if (_swipe.topEdge > 0 &&
                
                // IN
                ((y > (_ymin + _swipe.edgeVDistance) /*&& _touchmode != MODE_EDGE_SWIPE*/
                  && _fing[fid].start_y < (_ymin + _swipe.edgeVArea))
                 
                 ||
                 
                 // OUT
                 (y < (_ymin + _swipe.edgeVArea) && _fing[fid].start_y > (_ymin + _swipe.edgeVDistance)
                  && (_touchmode == MODE_TOP_EDGE_SWIPE || (_swipe.has2WayEdgeSwipes && _touchmode < 26)))))
            {
                DEBUG_SCROLL_SWIPES_LOG("STD :: Top Edge Swipe Triggered with Action ");

                //
                // Dispatch
                //
                
                if (_fing[fid].start_y < (_ymin + _swipe.edgeVArea)) // IN
                {
                    Dispatch_edgeSwipe(_swipe.topEdge, 0);
                }
                else if (y < (_ymin + _swipe.edgeVArea)) // OUT
                {
                    Dispatch_edgeSwipe(_swipe.topEdge, 1);
                }
                
                // Set touch mode
                _touchmode = MODE_TOP_EDGE_SWIPE;
                
                //
                // Reset Start Y to make the reverse swipe to work without finger lift
                //
                
                switch (_cFingers) {
                    case 2:
                        if (_fing[fid].start_y < (_ymin + _swipe.edgeVArea)) {
                            _fing[0].start_y = y + _swipe.edgeVArea;
                            _fing[1].start_y = y + _swipe.edgeVArea;
                        }
                        
                        else if (_fing[fid].start_y > (_ymin + _swipe.edgeVDistance)) {
                            _fing[0].start_y = _ymin;
                            _fing[1].start_y = _ymin;
                        }
                        break;
                        
                    default:
                        if (_fing[fid].start_y < (_ymin + _swipe.edgeVArea))
                            _fing[fid].start_y = y + _swipe.edgeVArea;
                        
                        else if (_fing[fid].start_y > (_ymin + _swipe.edgeVDistance))
                            _fing[fid].start_y = _ymin;
                        break;
                }
            }
            break;
            
        default:
            break;
    }
}

void ApplePS2SmartTouchpad::Dispatch_edgeSwipe(UInt8 gestureAction, UInt8 direction)
{
    
    //
    // Dispatch gestures
    //
    
    switch (gestureAction) {
        case kPS2C_Gesture_DesktopSwitchLeft:
            if (direction == 1) // Switch to Right Space (OUT)
            {
                gestureAction = 7;
            }
            break;
            
        case kPS2C_Gesture_DesktopSwitchRight:
            if (direction == 1) // Switch to Right Space (OUT)
            {
                gestureAction = 6;
            }
            break;
            
        case kPS2C_Gesture_NC_Workaround:
            Open_NC();
            /*
             if (direction == 0) // Open Soft NC (IN)
             Open_NC();
             else // Send release by ESC key to Close NC (OUT)
             _device->dispatchPS2Notification(kPS2C_ESC);*/
            break;
            
        case kPS2C_Gesture_AppSwitch:
            _swipe.isAppSwitchDone = true;
            break;
            
        default:
            break;
    }
    
    DEBUG_SCROLL_SWIPES_LOG("STD :: Delay between swipes %lld\n", (_gestureTime - _ps2Mouse.lastSwipeTime));
    
    if (_ps2Mouse.lastSwipeTime != 0 && (_gestureTime - _ps2Mouse.lastSwipeTime) < 800000000) // < 1 sec
        IOSleep(250); // Wait for quick swipes
    
    _device->dispatchPS2Notification(gestureAction);
    
    _ps2Mouse.lastSwipeTime = _gestureTime;
    
#if DEBUG_TPD
    // Log gesture
    if (_debugLvl & 128) {
        LogSwipeGestureAction(gestureAction);
    }
#endif
    
    // Reset pointer (Skip for Soft NC)
    if (_swipe.resetPointerOnEdge && gestureAction != 17)
    {
        ResetPointerToMiddle();
    }
}

void ApplePS2SmartTouchpad::ResetPointerToMiddle()
{
    // Absolute Pointer position bounds for corner Taps
    IOGPoint boundsABS = { 0, 0 };
    IOGBounds boundsPAD = { static_cast<SInt16>(_xmin), static_cast<SInt16>(_xmax),
        static_cast<SInt16>(_ymin), static_cast<SInt16>(_ymax)};
    
    // set pointer to middle after performing gesture
    boundsABS.x = (_xmin + _xmax)/2;
    boundsABS.y = (_ymin + _ymax)/2;
    
    dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, _button.state, true, 0, 30, 160, 0, _now);
    dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, _button.state, true, 0, 30, 160, 0, _now);
    
}

void ApplePS2SmartTouchpad::Open_NC()
{
    IOGPoint boundsABS = { 0, 0 };
    IOGBounds boundsPAD = { static_cast<SInt16>(_xmin), static_cast<SInt16>(_xmax),
        static_cast<SInt16>(_ymin), static_cast<SInt16>(_ymax)};
    
    if (!_swipe.isInFullScreen) {
        boundsABS.x = _xmax - 10;
        boundsABS.y = _ymin + 5;
    }
    else {
        
        if (_osxVersion < 10) {
            switch (_hw_version) {
                case 2:
                    boundsABS.x = _xmax - 60;
                    break;
                    
                default:
                    boundsABS.x = _xmax - 110;
                    break;
            }
        }
        else
            boundsABS.x = _xmax - 60;
        
        boundsABS.y = _ymin + 8;
    }
    
    dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, 0x0, true, 0, 30, 160, 0, _now);
    
    _swipe.isNCDone = true;
    
    // Initiate release timer for NC
    if (_swipe.isInFullScreen) {
        _featureTimer->setTimeoutMS(NC_REL_TIME);
    }
    else {
        dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, 0x1, true, 0, 30, 160, 0, _now);
        dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, 0x0, true, 0, 30, 160, 0, _now);
        ResetPointerToMiddle();
    }
    
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which processes some gestures at the end of the input
// packet stream.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::Process_touch_end()
{
    IOGPoint boundsABS = { 0, 0 };
    IOGBounds boundsPAD = { static_cast<SInt16>(_xmin), static_cast<SInt16>(_xmax),
        static_cast<SInt16>(_ymin), static_cast<SInt16>(_ymax)};
    
    bool skipProcessing = false;
    UInt8 scrollStopCounts = 0;
    int sFactor = _scrollFactor;

    uint64_t doubleTapTime = 0;
    UInt8   swipeUpAction = 0, swipeDownAction = 0, swipeLeftAction = 0, swipeRightAction = 0;
    SInt8   swipeAction = 0;
    
    clock_get_uptime(TIME_NOW);
    
    switch (_touchmode)
    {
        case MODE_1_FING_TAP:
            
            DEBUG_1F_LOG("STD :: TouchTime %lld, Fingers %d, Last Finger(s) %d, Button Data %d State %d, Pressure %d, PacketsCount %d, QuickTap %d, Clicking %s\n", _gestureTime, _cFingers, _lastFingers, _button.data, _button.state, _fing[0].pressure, _packetsCount, _drag.isQuickTap, _isClickingEnabled?"Yes":"No");
            
            if (!_isClickingEnabled || !(_gestureTime > 0))
            {
                break;
            }
            
            //
            // Customized 1F Tap ? 35 is Left click
            //
            
            if (!_kbEvent.isToggleKeyOnHold && _tap.fingers1 != 35)
            {
                SInt8 gestureAction;
                
                gestureAction = _tap.fingers1;
                
#if DEBUG_TPD
                if (_debugLvl & 8)
                {
                    DEBUG_1F_LOG("STD :: 1F Finger Tap Performed with Action:: ");
                    LogSwipeGestureAction(gestureAction);
                }
#endif
                
                if (gestureAction < 0) {
                    ; //do nothing when gesture is disabled
                }
                else if (gestureAction == 1)
                {
                    _device->dispatchPS2Notification(kPS2C_Gesture_AppSwitch);
                    _swipe.isAppSwitchDone = true;// For releasing the Swipe Function 1 button
                }
                else if (gestureAction == 17)// Notification centre
                {
                    Open_NC();
                }
                else
                    _device->dispatchPS2Notification(gestureAction);
                
                break;
            }
            
            //
            // Enable Touch pad back if its disabled  for accidental input
            // and skip further processing.
            //
            
            if (_kbEvent.tapEnableTPad && _kbEvent.accidentalInput)
            {
                _kbEvent.accidentalInput = false;
                _kbEvent.kbKeysCount = 0;
                DEBUG_1F_LOG("STD :: Touchpad enabled back from Accidental input\n");
                break;
            }
            
            //
            // Release the cmd key if the App switch gesture is active
            //
            
            if (_swipe.isAppSwitchDone)
            {
                _device->dispatchPS2Notification(kPS2C_ReleaseCmdKey);
                _swipe.isAppSwitchDone =  false;
            }
            
            //
            // Soft controlled single/double Tap to click simulation and drag release.
            //
            
            if (_button.data == 0 || _drag.isTapDrag || _drag.isHoldDrag)
            {
                
                // Set _tap.lastTime to detect taap drag and double/triple taps
                if (!_drag.checkTapDrag && !_drag.isDragging)
                {
                    _tap.lastTime = (*(uint64_t*)&_now);
                    _drag.checkTapDrag = true;
                    DEBUG_1F_LOG("STD :: Tap drag timer set.\n");
                }
                
                //
                // Release last tap click (if its done) for double/triple click
                // also useful for other features
                //
                
                if (_drag.isQuickTap) {
                    dispatchRelativePointerPosition(0, 0, 0x0);
                    _button.data  = _button.state = 0;
                    IOSleep(10); // Need for Virtual box OS X
                    DEBUG_1F_LOG("STD :: Quick Tap released\n");
                }
                
                // Dispatch tap for double click (or) drag release
                if (_drag.isHoldDrag || _drag.isTapDrag) {
                    
                    DEBUG_1F_LOG("STD :: Double Tap to click/Drag release performed.\n");
                    
                    dispatchRelativePointerPosition(0, 0, 0x1);
                    dispatchRelativePointerPosition(0, 0, 0x0);
                    _button.data  = _button.state = 0;
                    
                    _tap.lastTime = 0;
                    _drag.checkTapDrag = false;
                    
                }
                // Dispatch Single tap click
                else {
                    _button.data = 0x1;
                    _button.state |= _button.data;
                    dispatchRelativePointerPosition(0, 0, _button.state);
                    DEBUG_1F_LOG("STD :: Single Tap to click performed.\n");
                }
                
                _drag.isTapDrag = _drag.isHoldDrag = _drag.isDragging = false;
                _drag.is3FDrag = false;
                
                _fingPress.holdTime = 0;
            }
            _drag.isQuickTap = true; // Enable for quick tap detection
            _tap.count = 0;
            break;
            
            //
            // Process Corner taps
            //
        case MODE_CORNER_TAP:
            
            DEBUG_1F_LOG("STD :: Corner Tap Input Time %lld, Max tap time %lld\n", _gestureTime, (uint64_t)_tap.timeout);
            
            if (_gestureTime <= _tap.timeout && _cornerTap.enabled) {
                int xd = 0, yd = 0;
                switch (_cornerTap.corner) {
                    case 1:
                        boundsABS.x = 0;
                        boundsABS.y = 0;
                        xd = yd = 50;
                        break;
                    case 2:
                        boundsABS.x = 0;
                        boundsABS.y = _ymax;
                        xd = 50;
                        yd = -50;
                        break;
                        
                    case 3:
                        boundsABS.x = _xmax;
                        boundsABS.y = _ymax;
                        xd = yd = -50;
                        break;
                        
                    case 4:
                        boundsABS.x = _xmax;
                        boundsABS.y = 0;
                        xd = -50;
                        yd = 50;
                        break;
                        
                    default:
                        break;
                }
                
                dispatchRelativePointerPosition(xd, yd, 0);
                
                IOSleep(10);
                clock_get_uptime(TIME_NOW);
                dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, 0, true, 0, 30, 160, 0, _now);
                
                //clock_get_uptime(TIME_NOW);
                //dispatchAbsolutePointerEvent(&boundsABS, &boundsPAD, 0, true, 0, 30, 160, 0, _now);
                
                DEBUG_1F_LOG("STD :: Corner Tapp %d done.\n", _cornerTap.corner);
            }
            break;
            
            //
            // Pinch Zoom
            //
        case MODE_ZOOM:
            
            if (_zoom.isZoomOut)
                _device->dispatchPS2Notification(kPS2C_ZoomPlus);
            else if (_zoom.isZoomIn)
                _device->dispatchPS2Notification(kPS2C_ZoomMinus);
            
            break;
            
            //
            // Process 2 and 3 Fingers Tap
            //
        case MODE_2_FING_TAP:
        case MODE_3_FING_TAP:
            
            if (!_isClickingEnabled)
                break;
            
            DEBUG_MUL_FING_LOG("STD :: %d Fingers Tap Input Time %lld, Max tap time %lld, Taps %d, Last Tap time %lld, Tap TimeDiff %lld , IsToggleMode %s\n", (_touchmode == MODE_2_FING_TAP)?2:3, _gestureTime, (_cFingers == 2)?(uint64_t)_tap.timeout:_tap.mulFingsTimeout, _tap.count, _tap.lastTime, (absoluteToUINT64(_now) - _tap.lastTime), _kbEvent.isToggleKeyOnHold?"Yes":"No");
            
            doubleTapTime = absoluteToUINT64(_now) - _tap.lastTime;
            
            if (_tap.hasMulFDoubleTaps || _kbEvent.isToggleKeyOnHold)
                _tap.count++;
            
            if (_touchmode == MODE_2_FING_TAP)
                _tap.fingersCount = 2;
            else
                _tap.fingersCount = 3;
            
            //
            // Dispatch Middle/Right Button or plist config gesture
            //
            
            if (!_kbEvent.isToggleKeyOnHold && !_tap.hasMulFDoubleTaps)
            {
                switch (_tap.fingersCount) {
                    case 2:
                        if ((_isTapSwapEnabled && (_tap.fingers2 == kPS2C_Gesture_MiddleClick))
                            || (_tap.fingers2 == kPS2C_Gesture_ContextMenu))
                        {
                            _button.data = 0x2;
                        }
                        else if (!_isTapSwapEnabled && (_tap.fingers2 == kPS2C_Gesture_MiddleClick)) {
                            _button.data = 0x4;
                        }
                        else if (_tap.fingers2 == kPS2C_Gesture_LeftClick) {
                            _button.data = 0x1;
                        }
                        else {
                            _button.data = 0;
                        }
                        break;
                        
                    case 3:
                        if (!_isTapSwapEnabled && (_tap.fingers3 == kPS2C_Gesture_ContextMenu)) {
                            _button.data = 0x2;
                        }
                        else if ((_tap.fingers3 == kPS2C_Gesture_MiddleClick) ||
                                 (_isTapSwapEnabled && (_tap.fingers3 == kPS2C_Gesture_ContextMenu)))
                        {
                            _button.data = 0x4;
                        }
                        else if (_tap.fingers3 == kPS2C_Gesture_LeftClick) {
                            _button.data = 0x1;
                        }
                        else {
                            _button.data = 0;
                        }
                        break;
                        
                    default:
                        break;
                }
                
                switch (_button.data) {
                    case 2:
                        _button.state |= _button.data;
                        dispatchRelativePointerPosition(0, 0, _button.data);
                        IODelay(100);
                        dispatchRelativePointerPosition(0, 0, 0);
                        break;
                        
                    case 4:
                        _button.state |= _button.data;
                        dispatchRelativePointerPosition(0, 0, _button.data);
                        IODelay(100);
                        dispatchRelativePointerPosition(0, 0, 0);
                        break;
                        
                    default:
                        if (!_kbEvent.isToggleKeyOnHold)
                        {
                            SInt8 gestureAction;
                            
                            if (_touchmode == MODE_2_FING_TAP) {
                                gestureAction = _tap.fingers2; // Default Right click
                            }
                            else {
                                gestureAction = _tap.fingers3; // Default Middle click
                            }
                            
#if DEBUG_TPD
                            if (_debugLvl & 8)
                            {
                                IOLog("STD :: %d Finger Tap Performed with Action:: ",(_touchmode == MODE_2_FING_TAP)?2:3);
                                LogSwipeGestureAction(gestureAction);
                            }
#endif
                            
                            if (gestureAction < 0)
                                ; //do nothing when gesture is disabled
                            
                            else if (gestureAction == 1)
                            {
                                _device->dispatchPS2Notification(kPS2C_Gesture_AppSwitch);
                                _swipe.isAppSwitchDone = true;// For releasing the Swipe Function 1 button
                            }
                            else if (gestureAction == 17)// Notification centre
                                Open_NC();
                            
                            else
                                _device->dispatchPS2Notification(gestureAction);
                        }
                        break;
                }
                
                _tap.count = _tap.fingersCount = 0;
                _button.data = _button.state = 0;
            }
            
            else if (_tap.count == 1 && !_kbEvent.isToggleKeyOnHold && _tap.hasMulFDoubleTaps) {
                _featureTimer->setTimeoutMS(MAX_DTAP_TIME);
                DEBUG_MUL_FING_LOG("STD :: Multi Tap timer started.\n");
            }
            
            // Dispatch Feature Toggle timer
            else if (_kbEvent.isToggleKeyOnHold && _tap.count > 1 && doubleTapTime < MAX_MUL_DTAP_TIME)
            {
                _featureTimer->setTimeoutMS(MAX_MUL_DTAP_TIME/1000000);
            }
            // Do nothing and just decrease tap counts
            else if (_kbEvent.isToggleKeyOnHold && _tap.count > 1 && doubleTapTime > MAX_MUL_DTAP_TIME)
            {
                _tap.count--;
            }
            
            clock_get_uptime(TIME_NOW);
            _tap.lastTime = absoluteToUINT64(_now);
            break;
            
            //
            // 4 Fingers Tap
            //
        case MODE_4_FING_TAP:
        case MODE_5_FING_TAP:
            
            if (!_isClickingEnabled)
                break;
            
            DEBUG_MUL_FING_LOG("STD :: %d Fingers Tap Input Touch Time %lld, Max Tap time %lld, Taps %d, Tap TimeDiff %lld\n",(_touchmode == MODE_4_FING_TAP)?4:5, _gestureTime, _tap.mulFingsTimeout, _tap.count, absoluteToUINT64(_now) - _tap.lastTime);
            
            doubleTapTime = absoluteToUINT64(_now) - _tap.lastTime;
            
            if (_tap.hasMulFDoubleTaps || _kbEvent.isToggleKeyOnHold)
                _tap.count++;
            
            if (_touchmode == MODE_4_FING_TAP)
                _tap.fingersCount = 4;
            else
                _tap.fingersCount = 5;
            
            if (!_kbEvent.isToggleKeyOnHold)
            {
                SInt8 gestureAction;
                
                if (_touchmode == MODE_4_FING_TAP)
                    gestureAction = _tap.fingers4; // Default Mission control
                else
                    gestureAction = _tap.fingers5; // Default Dashboard
                
#if DEBUG_TPD
                if (_debugLvl & 8)
                {
                    IOLog("STD :: %d Finger Tap Performed with Action:: ",(_touchmode == MODE_4_FING_TAP)?4:5);
                    LogSwipeGestureAction(gestureAction);
                }
#endif
                
                if (gestureAction < 0)
                    ; //do nothing when gesture is disabled
                
                else if (gestureAction == 1)
                {
                    _device->dispatchPS2Notification(kPS2C_Gesture_AppSwitch);
                    _swipe.isAppSwitchDone = true;// For releasing the Swipe Function 1 button
                }
                else if (gestureAction == 17)// Notification centre
                    Open_NC();
                
                else
                    _device->dispatchPS2Notification(gestureAction);
                
                _tap.count = _tap.fingersCount = 0;
            }
            
            if (_kbEvent.isToggleKeyOnHold && _tap.count > 1 && doubleTapTime < MAX_MUL_DTAP_TIME) {
                _featureTimer->setTimeoutMS(MAX_MUL_DTAP_TIME/1000000);
            }
            else if (_kbEvent.isToggleKeyOnHold && _tap.count > 1 &&  doubleTapTime > MAX_MUL_DTAP_TIME) {
                _tap.count--;
            }
            
            clock_get_uptime(TIME_NOW);
            _tap.lastTime = absoluteToUINT64(_now);
            break;
            
            //
            // 3 & 4 Fingers swipes
            //
        case MODE_MUL_TOUCH_3:
            
            swipeLeftAction = _swipe.left3F;      // Backward/Left desktop switch
            swipeRightAction = _swipe.right3F;    // Forward/Right desktop switch
            swipeDownAction = _swipe.down3F;      // App switch
            swipeUpAction = _swipe.up3F;          // Launchpad
            
            DEBUG_MUL_FING_LOG("STD :: Three Finger Gesture Values (D %d U %d R %d L %d) Direction %d:: ", swipeDownAction, swipeUpAction, swipeRightAction, swipeLeftAction, _swipe.direction);
            
        case MODE_MUL_TOUCH_4:
            
            // Four Fingers
            if (_touchmode == MODE_MUL_TOUCH_4)
            {
                swipeLeftAction = _swipe.left4F;      // Hide/Show dock
                swipeRightAction = _swipe.right4F;    // Show properties
                swipeDownAction = _swipe.down4F;      // Minimize app
                swipeUpAction = _swipe.up4F;          // Close app
                
                DEBUG_MUL_FING_LOG("STD :: Four Finger Gesture Values (D %d U %d R %d L %d) Direction %d :: ", swipeDownAction, swipeUpAction, swipeRightAction, swipeLeftAction, _swipe.direction);
            }
            
            switch (_swipe.direction) {
                case 1:
                    DEBUG_SCROLL_SWIPES_LOG("Swipe Left Performed with Action:: ");
                    swipeAction = swipeLeftAction;
                    break;
                    
                case 2:
                    DEBUG_SCROLL_SWIPES_LOG("Swipe Right Performed with Action:: ");
                    swipeAction = swipeRightAction;
                    break;
                    
                case 3:
                    DEBUG_SCROLL_SWIPES_LOG("Swipe Down Performed with Action:: ");
                    swipeAction = swipeDownAction;
                    break;
                    
                case 4:
                    DEBUG_SCROLL_SWIPES_LOG("Swipe Up Performed with Action:: ");
                    swipeAction = swipeUpAction;
                    break;
                    
                default:
                    swipeAction =0;
                    break;
            }
            if (swipeAction > 0)
            {
                
#if DEBUG_TPD
                if (_debugLvl & 128)
                    LogSwipeGestureAction(swipeAction);
#endif
                
                if (swipeAction < 0)
                    ; //do nothing when gesture is disabled
                
                else if (swipeAction == 1)// For Command key release on Cmd+Tab
                {
                    _device->dispatchPS2Notification(kPS2C_Gesture_AppSwitch);
                    _swipe.isAppSwitchDone = true;
                }
                else if (swipeAction == 17)// Notification centre
                    Open_NC();
                else
                    _device->dispatchPS2Notification(swipeAction);
                
            }
            _swipe.direction = 0;
            break;
            
            //
            // Process Inertial scroll effect
            //
            
        case MODE_CONT_SCROLL:
            
        case MODE_VSCROLL:
        case MODE_HSCROLL:
            
        case MODE_EDGE_HSCROLL:
        case MODE_EDGE_VSCROLL:
            
            switch (_touchmode) {
                case MODE_CONT_SCROLL:
                    break;
                    
                case MODE_HSCROLL:
                case MODE_VSCROLL:
                    if (!_scroll.has2FAuto && !_scroll.has2FInertia)
                        skipProcessing = true;
                    break;
                    
                case MODE_EDGE_HSCROLL:
                case MODE_EDGE_VSCROLL:
                    sFactor = (_scroll.edgeSpeedFactor/100) * sFactor;
                    
                    if (!_scroll.hasEdgeAuto && !_scroll.hasEdgeInertia)
                        skipProcessing = true;
                    break;
                    
                default:
                    break;
            }
            
            // Stop active scroll
            if (_scrollTimer)
                _scrollTimer->cancelTimeout();
            
            //
            // Analyze scroll stop samples to ignore inertia
            //
            
            if (!_scroll.noInertia) {
                for (int i = 0; i < 5 && _lastSample[i].pressure != 0; i++)
                {
                    DEBUG_SCROLL_SWIPES_LOG("STD :: Scroll stop Sample SDX[i] %d, SDY[i] %d, Pressurre %d, Loop %d PC %d, Delta %d\n", _lastSample[i].lastSDX, _lastSample[i].lastSDY, _lastSample[i].pressure, i, _packetsCount, _tap.maxTEndDelta);
                    
                    if ((_lastSample[i].lastSDX < _tap.maxTEndDelta &&
                         _lastSample[i].lastSDY < _tap.maxTEndDelta)
                        
                        || (/*_touchpadVendor == STD_SYNAPTICS
                        &&*/ _lastSample[i].pressure != 0 && _lastSample[i].pressure < _tap.maxTEndPressure))
                    {
                        scrollStopCounts++;
                    }
                }
            }
            
            //
            // Process post scroll effects
            //
            
            if (_scrollTimer
                && (!_scroll.noInertia || _scroll.doSling)
                && !skipProcessing && scrollStopCounts < _scroll.stopSamples
                && (_scroll.dy != 0 || _scroll.dx != 0))
            {
                int scrollDirection = 0;
                _scroll.inertiaDeltaX = _scroll.inertiaDeltaY = 0;
                
                // Vertical inertia
                if (_scroll.dy < 0) {
                    scrollDirection = -1;
                }
                else if (_scroll.dy > 0) {
                    scrollDirection = 1;
                }
                
                if (_scroll.maxLevelReached == 1 && sFactor < 4) {
                    _scroll.inertiaDeltaY = scrollDirection * 4;
                }
                else {
                    _scroll.inertiaDeltaY = scrollDirection * _scroll.maxLevelReached;
                }
                
                // Do sling effect ?
                if (_scroll.doSling) {
                    _scroll.inertiaDeltaY = _scroll.slingDepth;
                    _scroll.slingEffectTime = 0;
                    _scroll.lastSlingEffectTime = 0;
                    _doTimerJob = false;
                }
                // Do auto scroll ?
                else if ((_scroll.hasEdgeAuto || _scroll.has2FAuto))
                {
                    _scroll.doSling = false;
                    
                    if ((_scroll.hasEdgeInertia || _scroll.has2FInertia) && !_kbEvent.altFunction) {
                        _scroll.doAuto = false;
                        _doTimerJob = true;
                    }
                    else if (_scroll.inertiaDeltaY != 0) {
                        _scroll.doAuto = true;
                    }
                }
                else {
                    _doTimerJob = true;
                    _scroll.doSling = false;
                }
                
                // Horizontal inertia
                if (_scroll.dx < 0) {
                    scrollDirection = -1;
                }
                else if (_scroll.dx > 0) {
                    scrollDirection = 1;
                }
                else {
                    scrollDirection = 0;
                }
                
                // No Inertia ?
                if (_scroll.noHSInertia) {
                    _scroll.inertiaDeltaX = _scroll.curInertiaDeltaX = 0;
                }
                else {
                    if (_scroll.maxLevelReached == 1 && sFactor < 4) {
                        _scroll.inertiaDeltaX = scrollDirection * 4;
                    }
                    else {
                        _scroll.inertiaDeltaX = scrollDirection * _scroll.maxLevelReached;
                    }
                }
                
                // Calculate inertia effect depth
                if (_scroll.inertiaDeltaX != 0 || _scroll.inertiaDeltaY != 0)
                {
                   _scroll.inertiaLevel = 0;
                    
                    if (_scroll.accelModeEdge > 2 || _scroll.accelMode2F > 2)
                    {
                        _scroll.inertiaDeltaX *=  2;
                        _scroll.inertiaDeltaY *=  2;
                    }
                    _scroll.curInertiaDeltaY = _scroll.inertiaDeltaY;
                    _scroll.curInertiaDeltaX = _scroll.inertiaDeltaX;

                    // Start inertial scroll timer
                    _scrollTimer->cancelTimeout();
                    _scrollTimer->setTimeoutMS(1);
                }
                
                DEBUG_MUL_FING_LOG("STD :: Inertia Scroll Delta X %d Y %d, MaxLvl %d, LXD %d, LYD %d, TMode %d, Sling Effect %s Depth %d, SF %d\n", _scroll.inertiaDeltaX, _scroll.inertiaDeltaY, _scroll.maxLevelReached, _scroll.dx, _scroll.dy, _touchmode, _scroll.doSling?"YES":"NO", _scroll.slingDepth, (int)sFactor);
            }
            break;
            
        default:
            break;
    }
    
    //
    // Initiate release timer for tap click
    //
    
    if (_drag.checkTapDrag)
    {
        _buttonDragTimer->cancelTimeout();
        _buttonDragTimer->setTimeoutMS(150);
        _doTimerJob = true;
        DEBUG_1F_LOG("STD :: Release timer started for tap click.\n");
    }
    
    //
    // Initiate Drag release timer
    //
    
    if ((_drag.isTapDrag || _drag.isHoldDrag) && !_isDragLockEnabled && _drag.isDragging)
    {
        _buttonDragTimer->cancelTimeout();
        _buttonDragTimer->setTimeoutMS(_drag.releaseTime);

        _drag.releaseStartTime = absoluteToUINT64(_now);
        _doTimerJob = true;
        DEBUG_1F_LOG("STD :: Release timer started for Tap/Hold Drag.\n");
    }
    
    //
    // Initiate App Switch key release timer
    //
    
    if (_swipe.isAppSwitchDone) {
        _buttonDragTimer->setTimeoutMS(APP_GESTURE_TIMEOUT);
        DEBUG_MUL_FING_LOG("STD :: Release timer started for App switch\n");
        _doTimerJob = true;
    }
    
   IOLog("STD :: Drag %d Tdrag %d Hdrag %d DLock %d, Last Fings count %d Fingers %d, TapChk %d, PC %d, TM %d,MaxScroll Lvl %d, LXD %d, LYD %d, FTime %lld, _scroll.noInertia %d Samples %d(%d), DSling %d Depth %d, TimerJob %d, DoCont %d, SETime %lld, RunTime %lld, isToggleMode %s, CPad %d, IDepth %d, QT %d\n",_drag.isDragging,_drag.isTapDrag,_drag.isHoldDrag,_isDragLockEnabled, _lastFingers,_cFingers,_drag.checkTapDrag,_packetsCount,_touchmode, _scroll.maxLevelReached,_scroll.dx,_scroll.dy, _gestureTime, _scroll.noInertia, scrollStopCounts, _scroll.stopSamples, _scroll.doSling, _scroll.slingDepth, _doTimerJob, _scroll.doCont, _scroll.slingEffectTime, _scroll.slingEffectRunTime, _kbEvent.isToggleKeyOnHold?"Yes":"No", _button.isClickPad, _scroll.inertiaDepth, _drag.isQuickTap);
    
    
    //
    // Clear/reset variables
    //
    
    _ps2Mouse.lastSwipeTime = 0;

    _synTPad.touch_ended    = false;
    _synTPad.secondary_wait = false;
    
    _palm.detected = false;
    
    _button.isFingOnBtnArea = false;
    _button.fingerInUse = 0;

    
    _zoom.isZoomIn = _zoom.isZoomOut =false;
    
    _swipe.isEdgeGesture = false;
    _swipe.direction = 0;
    _rotate.isRotateGesture = false;
    _rotate.isRotateMode = false;
    
    _synTPad.m_scroll = 0;
    _rotate.rotateXCounter = _rotate.rotateYCounter = 0;
    _rotate.xAxisPos = _rotate.yAxisPos = 0;
    
    _scroll.doCont      = false;
    _scroll.noInertia   = true;

    _scroll.dxPacketsCount = 0;
    _scroll.dyPacketsCount = 0;
    _scroll.dx = _scroll.dy = 0;
    _scroll.xPacketsDir = 0;
    _scroll.yPacketsDir = 0;
    _scroll.slingDepth = 0;
    _scroll.maxLevelReached = 0;
    _scroll.maxInertiaLevel = 0;
    _scroll.inertiaLevel = 0;
    
    _fingPress.lastHoldTime = 0;
    _fingPress.holdTime     = 0;
    _fingPress.gestureTime  = 0;
    
    for (int i = 0; i < 10; i++) {
        _lastSample[i].skippedX = _lastSample[i].skippedY = 0;
        _scroll.skippedDx[i] = _scroll.skippedDy[i] = 0;
    }
    
    for (int i = 0; i < STD_MAX_FINGERS; i++) {
        _fing[i].x = _fing[i].y = 0;
        _fing[i].last_x  = _fing[i].last_y  = 0;
        _fing[i].tdx = _fing[i].tdy = 0;
        _fing[i].sdx = _fing[i].sdy = 0;
        _fing[i].pressure = _fing[i].width = 0;
        
        //
        // Need to skip on index finger move with thumb finger on button area (2F Move/Drag)
        // beacause we receive two fingers zero packet for this in v4 Hardware
        //
        
        if ((_lastFingers != 2 && _button.isClickPad) || !_button.isClickPad) {
            _fing[i].start_x = _fing[i].start_y = 0;
        }
    }
    
    _pContinuousMove = false;
    _pLastDx = _pLastDy = 0;
    _pDy = _pDx = 0;
    _pSmoothX = 0;
    _pSmoothY = 0;
    
    _lastSample[SAMPLES_COUNT] = {0};
    _skipSample     = 0;
    _scrollSample   = 0;
    _cSample        = 0;
    
    _lastFingers = 0;
    _pid = _sid = -1;
    
    _touchmode = MODE_IDLE;
    _packetsCount   = 0;
    _gestureTime    = 0;
    _lastDataTime   = 0;
    
    //
    // Trigger keyboard backlight off event
    //
    
    if (_hasAsusKBackLight && _hasKBLightAutoOff)
        receiveKeyboardNotifications(kPS2C_KeybrdBackLightOff);
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Method which logs gesture actions.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::LogSwipeGestureAction(int action)
{
    char * actionLog = (char *)"None/Unknown";
    
    switch(action){
            
        case -1:
            actionLog = (char *)" Gesture Disabled";
            break;
            
        case kPS2C_Gesture_AppSwitch:
            actionLog =(char *) "Apps Switch in Menu";
            break;
            
        case kPS2C_Gesture_AppClose:
            actionLog = (char *)"App Close";
            break;
            
        case  kPS2C_Gesture_Launchpad:
            actionLog = (char *)"Lauchpad";
            break;
            
        case kPS2C_Gesture_MissionControl:
            actionLog =(char *) "Mission Control";
            break;
        case kPS2C_Gesture_Dashboard:
            actionLog = (char *)"Dashboard";
            break;
            
        case kPS2C_Gesture_DesktopSwitchLeft:
            actionLog = (char *)"Switch to Left Space/Desktop";
            break;
            
        case kPS2C_Gesture_DesktopSwitchRight:
            actionLog = (char *)"Switch to Right Space/Desktop";
            break;
            
        case kPS2C_Gesture_AppWindows:
            actionLog = (char *)"Application Windows";
            break;
            
        case kPS2C_Gesture_Minimize:
            actionLog = (char *)"App minimize";
            break;
            
        case kPS2C_Gesture_FullScreen:
            actionLog = (char *)"Full Screen toggle";
            break;
            
        case kPS2C_Gesture_Backward:
            actionLog = (char *)"Backward";
            break;
            
        case kPS2C_Gesture_Forward:
            actionLog = (char *)"Forward";
            break;
            
        case kPS2C_Gesture_Desktop:
            actionLog = (char *)"Desktop";
            break;
            
        case kPS2C_Gesture_NC_Shortcut:
            actionLog = (char *)"Notification Centre(Shortcut)";
            break;
            
        case kPS2C_Gesture_GetInfo:
            actionLog = (char *)"Show Info";
            break;
            
        case kPS2C_Gesture_ToggleDock:
            actionLog = (char *)"Hide/Show Dock";
            break;
            
        case kPS2C_Gesture_NC_Workaround://Notification Centre, Soft controlled
            actionLog = (char *)"Notification Centre (Soft control)";
            break;
            
        case kPS2C_Gesture_ZoomReset:
            actionLog = (char *)"Zoom reset to normal";
            break;
            
        case kPS2C_Gesture_Finder:
            actionLog = (char *)"Finder";
            break;
            
        case kPS2C_Gesture_ForceQuit:
            actionLog = (char *)"Force Quit";
            break;
           
        case kPS2C_Gesture_ContextMenu:
            actionLog = (char *)"Context menu";
            break;
            
        case kPS2C_Gesture_MiddleClick:
            actionLog = (char *)"Middle click";
            break;
       
        case kPS2C_Gesture_LeftClick:
            actionLog = (char *)"Left click";
            break;
            
        default: // F13 - F24 (21 - 32)
            if (action > 20 && action < 33) {
                IOLog("F%d.\n",action-8);
            }
            break;
    }
    
    IOLog("%s.\n",actionLog);
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Keyboard Notications Handler.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::receiveKeyboardNotifications(UInt32 data)
{
    clock_get_uptime(TIME_NOW);
    uint64_t timeout = (_kBLightAutoOffTimeout * 1000000);
    
    switch (data) {
            
        case kPS2C_ShowTouchPressureWidth:
            _kbEvent.showPressWidth = !_kbEvent.showPressWidth;
            IOLog("STD :: Display of Pressure and Width Log %s.\n", _kbEvent.showPressWidth?"Enabled":"Disabled");
            break;
            
        case kPS2C_AccidentalInput:
            _kbEvent.kbKeysCount++;
            
            if (_kbEvent.kbKeysCount >= _kbEvent.accInputKeysCount && _isTypeModeEnabled) {
                _kbEvent.accidentalInput = true;
                _kbEvent.kbKeysCount = 2;
            }
            
            _kbEvent.lastKeyPressTime = absoluteToUINT64(_now);
            DEBUG_LOG("STD :: Received keyboard inputs count %d for Accidental Input mode \n", _kbEvent.kbKeysCount);
            break;
            
        case kPS2C_DisableTouchpad:
            _kbEvent.disableTouchpad = true;
            DEBUG_LOG("STD :: Disabled touchpad.\n");
            break;
            
        case kPS2C_EnableTouchpad:
            _kbEvent.lastKeyPressTime = absoluteToUINT64(_now);
            _kbEvent.kbKeysCount = 0;
            _kbEvent.disableTouchpad = false;
            _kbEvent.accidentalInput    = false;
            DEBUG_LOG("STD :: Enabled touchpad.\n");
            break;
            
        case kPS2C_ESC:
            
            // Clear tap counts and full screen mode of edge swipe
            _swipe.isInFullScreen = false;
            _tap.count = 0;
            
            // Reset direction of scroll on Left Shift key clear
            if (_kbEvent.isToggleKeyOnHold) {
                if (_scrollFactor < 0)
                    _scrollFactor = 0 - _scrollFactor;
                // Clear Shift down
                _kbEvent.isToggleKeyOnHold = false;
            }
            
            // Release drag
            if (_drag.isDragging) {
                dispatchRelativePointerPosition(0, 0, 0);
                _button.data = _button.state = 0;
                _drag.isDragging = false;
                _drag.isTapDrag = _drag.isHoldDrag = _drag.is3FDrag = false;
            }
            
            // Release the cmd key if the App switch gesture function is active
            if (_swipe.isAppSwitchDone) {
                _device->dispatchPS2Notification(kPS2C_ReleaseCmdKey);
                _swipe.isAppSwitchDone = false;
            }
            
            _kbEvent.lastKeyPressTime = absoluteToUINT64(_now);
            DEBUG_LOG("STD :: Keyboard ESC key event.\n");
            break;
            
        case kPS2C_DoContextMenu:
            dispatchRelativePointerPosition(0, 0, 0x2);
            IODelay(100);
            dispatchRelativePointerPosition(0, 0, 0);
            _button.state = 0;
            break;
            
        case kPS2C_ScrollLock_Auto:
            _scroll.hasEdgeAuto = !_scroll.hasEdgeAuto;
            _scroll.has2FAuto = !_scroll.has2FAuto;
            if (_scroll.hasEdgeAuto || _scroll.has2FAuto)
            {
                DEBUG_TOGGLE_LOG("STD :: 2 Fingers auto scrolling %s, Edge auto scrollling %s.\n", _scroll.has2FAuto?"enabled":"disabled", _scroll.hasEdgeAuto?"enabled":"disabled");
                dispatchRelativePointerPosition(0, -50, _button.state);
            }
            else
            {
                DEBUG_TOGGLE_LOG("STD :: Auto scrolling disabled.\n");
                dispatchRelativePointerPosition(0, 50, _button.state);
            }
            break;
            
        case kPS2C_ScrollLock_Inertia:
            _scroll.has2FInertia = !_scroll.has2FInertia;
            _scroll.hasEdgeInertia = !_scroll.hasEdgeInertia;
            
            // Moving a pointer a little bit to show feature toggled
            if (_scroll.hasEdgeInertia)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Inertia effect enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Inertia effect disabled.\n");
            }
            break;
            
        case kPS2C_ScrollLock_Continuous:
            _scroll.has2FCont = !_scroll.has2FCont;//toggle Continuous scroll
            _scroll.hasEdgeCont = !_scroll.hasEdgeCont;
            
            // Moving a pointer a little bit to show feature toggled
            if (_scroll.has2FCont)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Continuous scroll enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Continuous scroll disabled.\n");
                
            }
            break;
            
        case kPS2C_ShiftKey:
            _kbEvent.isToggleKeyOnHold = !_kbEvent.isToggleKeyOnHold;
            
            // change direction of scroll on Left Shift key down
            _scrollFactor = 0 - _scrollFactor;
            
            _swipe.isInFullScreen = !_swipe.isInFullScreen;
            
            if (!_kbEvent.isToggleKeyOnHold)
                _tap.count = 0;
            
            DEBUG_LOG("STD :: Keyboard Shift key is Down %d\n", _kbEvent.isToggleKeyOnHold);
            break;
            
        case kPS2C_CMDKeyDown:
            DEBUG_LOG("STD :: Keyboard Command key is Down\n");
            break;
            
        case kPS2C_CMDKeyUp:
            DEBUG_LOG("STD :: Keyboard Command key is Up\n");
            break;
            
        case kPS2C_CNTRLKeyDown:
            DEBUG_LOG("STD :: Keyboard Control key is Down\n");
            _ps2Mouse.isCtrlScrollSwipe = true;
            break;
            
        case kPS2C_CNTRLKeyUp:
            DEBUG_LOG("STD :: Keyboard Control key is Up\n");
            _ps2Mouse.isCtrlScrollSwipe = false;
            break;
            
        case kPS2C_CNTRL_Feature_on:
            DEBUG_LOG("STD :: Keyboard Command + ALT+ Control keys are down, speeding Up scroll Factor %d to %d\n", (int)_scrollFactor, (int)_scrollFactor * 2);
            /*
             *  Increase scroll speed by 2x if the cmd + control keys down
             *  ScrollFactor values : 0 2 3 4 5 6 7 8 9 10  x 2
             */
            _scrollFactor =  _scrollFactor * 2;
            break;
            
        case kPS2C_CNTRL_Feature_off:
            DEBUG_LOG("STD :: Keyboard Command + ALT + Control keys are not down, Speeding down scroll Factor %d to %d\n", (int)_scrollFactor, (int)_scrollFactor/2);
            /*
             *  Decrease scroll speed by 2x if the cmd+control keys up
             *  ScrollFactor values : 0 2 3 4 5 6 7 8 9 10  x 2
             */
            _scrollFactor =  _scrollFactor / 2;
            break;
            
        case kPS2C_ALT_Funtion_on:
            _kbEvent.altFunction = true;
            DEBUG_LOG("STD :: Keyboard Command + ALT keys are Down\n");
            break;
            
        case kPS2C_ALT_Funtion_off:
            _kbEvent.altFunction = false;
            _scroll.doAuto = false;
            DEBUG_LOG("STD :: Keyboard Command + ALT keys are Up\n");
            break;
            
            
        case kPS2C_EdgeScroll:
            _scroll.hasEdgeScroll = !_scroll.hasEdgeScroll;
            if (_scroll.hasEdgeScroll)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Edge scroll enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Edge scroll disabled.\n");
            }
            break;
            
        case kPS2C_Gestures_1FSwipes:
            _swipe.hasEdgeSwipes = !_swipe.hasEdgeSwipes;
            if (_swipe.hasEdgeSwipes)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Edge swipes enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Edge swipes disabled.\n");
            }
            break;
            
        case kPS2C_Gestures_3FSwipes:
            _swipe.has3FSwipes = !_swipe.has3FSwipes;
            if (_swipe.has3FSwipes)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Thee fingers swipes enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Thee fingers swipes disabled.\n");
            }
            break;
            
        case kPS2C_Gestures_4FSwipes:
            _swipe.has4FSwipes = !_swipe.has4FSwipes;
            if (_swipe.has4FSwipes)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD ::Four fingers swipes enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Four fingers swipes disabled.\n");
            }
            break;
            
        case kPS2C_Gestures_Rotate:
            _rotate.enabled = !_rotate.enabled;
            if (_rotate.enabled)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Rotations enabled.\n");
                
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Rotations disabled.\n");
            }
            break;
            
        case kPS2C_Gestures_Zoom:
            _zoom.enabled = !_zoom.enabled;
            if (_zoom.enabled)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Pinch Zoom enabled.\n");
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Pinch Zoom disabled.\n");
            }
            break;
            
        case kPS2C_Gestures_DoubleTaps:
            _tap.hasMulFDoubleTaps = !_tap.hasMulFDoubleTaps;
            if (_tap.hasMulFDoubleTaps)
            {
                dispatchRelativePointerPosition(0, -50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Multi fingers double taps enabled.\n");
            }
            else {
                dispatchRelativePointerPosition(0, 50, _button.state);
                DEBUG_TOGGLE_LOG("STD :: Multi fingers double taps disabled.\n");
            }
            break;
            
        case kPS2C_RestoreTouchpad:
            IOLog("STD :: Keyboard command to reconnect...\n");
            
            Reconnect_touchpad();
            
            _kbEvent.lastKeyPressTime = absoluteToUINT64(_now);
            _kbEvent.kbKeysCount = 0;
            _kbEvent.disableTouchpad = false;
            _kbEvent.accidentalInput    = false;
            _device->dispatchPS2Notification(kPS2C_EnableTouchpad);
            
            /*
            if (_debugLvl < 255)
                _debugLvl = 255;
            else
                _debugLvl = 0;*/
            break;
            
        case kPS2C_KeybrdBackLightOff:
            DEBUG_LOG("STD :: Triggering Asus Keyboard backlight turn off (Staus %s)...\n", _isKBLightOff?"OFF":"ON");
            
            if (!_isKBLightOff) {
                _kBackLightTimer->cancelTimeout();
                _kBackLightTimer->setTimeoutMS(_kBLightAutoOffTimeout);
            }
            break;
            
        case kPS2C_KeybrdBackLightOn:
            clock_get_uptime(TIME_NOW);
            
            DEBUG_LOG("STD :: Triggering Asus Keyboard backlight turn on (Staus %s), Time diff %lld, Timeout %d (%lld)...\n", _isKBLightOff?"OFF":"ON", (absoluteToUINT64(_now) - _lastKBLightTriggerTime), _kBLightAutoOffTimeout, timeout);
            
            if (_isKBLightOff || (absoluteToUINT64(_now) - _lastKBLightTriggerTime) > timeout) {
                
                _lastKBLightTriggerTime = 0;
                _kBackLightTimer->cancelTimeout();
                _kBackLightTimer->setTimeoutMS(0);
            }
            break;
            
        case kPS2C_KeybrdBackLightToggleOn:
            _isKBLightOff = false;
            DEBUG_LOG("STD :: Asus Keyboard backlight turned on (%s)\n", _isKBLightOff?"OFF":"ON");
            break;
            
        case kPS2C_KeybrdBackLightToggleOff:
            _isKBLightOff = true;
            DEBUG_LOG("STD :: Asus Keyboard backlight turned off (%s)\n", _isKBLightOff?"OFF":"ON");
            break;
            
        case kPS2C_OSXVersionNotify: // If requested
            // Inform for the incoming OS X version data
            _device->dispatchPS2Notification(kPS2C_OSXVersionNotify);
            // Send OS X version data
            _device->dispatchPS2Notification(_osxVersion);
            break;
            
        default:
            break;
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Feature toggle and button release events timer handlers.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad:: ProcessFeaturesToggle()
{
    // Release soft NC
    if (_swipe.isNCDone) {
        dispatchRelativePointerPosition(0, 0, 0x1);
        dispatchRelativePointerPosition(0, 0, 0x0);
        _button.data = _button.state = 0;
        DEBUG_SCROLL_SWIPES_LOG("STD :: NC timer release after %lld ns\n", *(uint64_t*)&btnRel_time - _tap.lastTime);
        
        ResetPointerToMiddle();
        _swipe.isNCDone = false;
    }
    
    DEBUG_MUL_FING_LOG("STD :: Taps %d, Fingers %d, isToggleMode %s,TapTimeDiff %lld\n", _tap.count,_tap.fingersCount,_kbEvent.isToggleKeyOnHold?"Yes":"No", *(uint64_t*)&btnRel_time - _tap.lastTime);
    
    switch (_tap.count) {
            
        case 1: // Single tap
            
            //
            // Middle Button/Right Button simulation when multi double taps enabled
            //
            
            if (_isTapSwapEnabled) {
                if (_tap.fingersCount == 2)
                    _button.data = 0x2;
                else
                    _button.data = 0x4;
            }
            else {
                if (_tap.fingersCount == 2)
                    _button.data = 0x4;
                else
                    _button.data = 0x2;
            }
            
            // Dispatch button
            switch (_button.data) {
                case 2:
                    _button.state |= _button.data;
                    dispatchRelativePointerPosition(0, 0, _button.data);
                    IODelay(100);
                    dispatchRelativePointerPosition(0, 0, 0);
                    break;
                    
                case 4:
                    _button.state |= _button.data;
                    dispatchRelativePointerPosition(0, 0, _button.data);
                    IODelay(100);
                    dispatchRelativePointerPosition(0, 0, 0);
                    break;
                default:
                    break;
            }
            
            _tap.count = _tap.fingersCount = 0;
            _button.data = _button.state = 0;
            
            DEBUG_MUL_FING_LOG("STD :: %d Fingers tap performed.\n", _tap.fingersCount);
            break;
            
        case 2: // Double taps
            switch (_tap.fingersCount) {
                case 2:
                    if (!_kbEvent.isToggleKeyOnHold)
                    {
                        _zoom.tapZoomDone = !_zoom.tapZoomDone;
                        if (_zoom.tapZoomDone)
                        {
                            _device->dispatchPS2Notification(kPS2C_ZoomPlus);
                            _device->dispatchPS2Notification(kPS2C_ZoomPlus);
                            dispatchRelativePointerPosition(0, -50, _button.state);
                            DEBUG_TOGGLE_LOG("STD :: Double tap Zoom Out triggered.\n");
                            
                        }
                        else
                        {
                            _device->dispatchPS2Notification(kPS2C_ZoomMinus);
                            _device->dispatchPS2Notification(kPS2C_ZoomMinus);
                            dispatchRelativePointerPosition(0, 50, _button.state);
                            DEBUG_TOGGLE_LOG("STD :: Double tap Zoom In triggered.\n");
                        }
                        
                    }
                    else //_kbEvent.isToggleKeyOnHold
                    {
                        _scroll.hasEdgeScroll = !_scroll.hasEdgeScroll;
                        if (_scroll.hasEdgeScroll)
                        {
                            DEBUG_TOGGLE_LOG("STD :: Edge Scrolling enabled.\n");
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        }
                        else
                        {
                            DEBUG_TOGGLE_LOG("STD :: Edge Scrolling disabled.\n");
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                        
                    }
                    break;
                    
                case 3:
                    if (!_kbEvent.isToggleKeyOnHold)
                    {
                        // Application window
                        _device->dispatchPS2Notification(kPS2C_Gesture_AppWindows);
                        
                        DEBUG_TOGGLE_LOG("STD :: Application window triggered.\n");
                        
                    }
                    else //_kbEvent.isToggleKeyOnHold
                    {
                        _scroll.accelMode2F++;
                        if (_scroll.accelMode2F > 4) {
                            _scroll.accelMode2F = 0;
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                        else
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        
                        _scroll.accelModeEdge++;
                        if (_scroll.accelModeEdge > 4) {
                            _scroll.accelModeEdge = 0;
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                        else
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        
                        IOLog("STD :: Scrolling acceleration set to mode 2F %d, Edge %d.\n", _scroll.accelMode2F, _scroll.accelModeEdge);
                    }
                    break;
                    
                case 4:
                    if (!_kbEvent.isToggleKeyOnHold)
                    {
                        
                    }
                    else //_kbEvent.isToggleKeyOnHold
                    {
                        _button.clickPadHasVButtons = !_button.clickPadHasVButtons;
                        if (_button.clickPadHasVButtons)
                        {
                            DEBUG_TOGGLE_LOG("STD :: Virtual buttons mode enabled.\n");
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        }
                        else
                        {
                            DEBUG_TOGGLE_LOG("STD :: Virtual buttons mode disabled.\n");
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                    }
                    break;
                    
                default:
                    break;
            }
            _tap.count = _tap.fingersCount  = 0;
            break;
            
        case 3: // Triple taps
            switch (_tap.fingersCount) {
                case 2:
                    if (!_kbEvent.isToggleKeyOnHold)
                    {
                        
                    }
                    else //_kbEvent.isToggleKeyOnHold
                    {
                        _scroll.hasEdgeCScroll = !_scroll.hasEdgeCScroll;
                        if (_scroll.hasEdgeCScroll)
                        {
                            DEBUG_TOGGLE_LOG("STD :: Edge Circular scrolling enabled.\n");
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        }
                        else
                        {
                            DEBUG_TOGGLE_LOG("STD :: Edge Circular scrolling disabled.\n");
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                    }
                    break;
                    
                case 3:
                    if (!_kbEvent.isToggleKeyOnHold)
                    {
                        
                    }
                    else //_kbEvent.isToggleKeyOnHold
                    {
                        /*_scroll.hasEdgeAuto = !_scroll.hasEdgeAuto;
                         _scroll.has2FAuto = !_scroll.has2FAuto;
                         if (_scroll.hasEdgeAuto || _scroll.has2FAuto)
                         {
                         IOLog("STD :: 2 Fingers auto scrolling %s, Edge auto scrollling %s.\n", _scroll.has2FAuto?"enabled":"disabled", _scroll.hasEdgeAuto?"enabled":"disabled");
                         dispatchRelativePointerPosition(0, -50, _button.state);
                         }
                         else
                         {
                         IOLog("STD :: Auto scrolling disabled.\n");
                         dispatchRelativePointerPosition(0, 50, _button.state);
                         }*/
                        
                        _pAccelCustom++;
                        if (_pAccelCustom > 20) {
                            _pAccelCustom = 0;
                            dispatchRelativePointerPosition(0, 50, _button.state);
                        }
                        else
                            dispatchRelativePointerPosition(0, -50, _button.state);
                        
                        IOLog("STD :: Pointer custom acceleration set to %d.\n", _pAccelCustom);
                        
                    }
                    break;
                    
                case 4:
                    if (!_kbEvent.isToggleKeyOnHold)
                    {
                        
                    }
                    else //_kbEvent.isToggleKeyOnHold
                    {
                        
                    }
                    break;
                    
                default:
                    break;
            }
            _tap.count = _tap.fingersCount  = 0;
            break;
            
        default:
            break;
    }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SmartTouchpad::Release_ButtonsAndDrag()
{
    AbsoluteTime btnDragRls_time;
    
#if APPLESDK
    clock_get_uptime(&btnDragRls_time);
#else
    clock_get_uptime((uint64_t*)&btnDragRls_time);
#endif
    
    DEBUG_1F_LOG("STD :: Button/Drag Release attempt after a timeout %lld ns, TimerJob %d\n", (*(uint64_t*)&btnDragRls_time) - _drag.releaseStartTime, _doTimerJob);
    
    //
    // Release the cmd key if the App switch gesture function is active
    //
    
    if (_swipe.isAppSwitchDone) {
        _device->dispatchPS2Notification(kPS2C_ReleaseCmdKey);
        DEBUG_1F_LOG("STD :: Gesture Released after a timeout %d ns.\n", APP_GESTURE_TIMEOUT);
        _swipe.isAppSwitchDone = false;
        _doTimerJob = false;
    }
    
    //
    // Single Tap click or Drag after a timeout
    //
    
    if ((_drag.isDragging && !_isDragLockEnabled && (_drag.isHoldDrag || _drag.isTapDrag))
        || (!_drag.isDragging && _drag.checkTapDrag))
    {
        _drag.checkTapDrag = false;
        
        _drag.isDragging = _drag.isTapDrag = _drag.isHoldDrag = false;
        _drag.is3FDrag = false;
        
        _fingPress.holdTime = 0;
        _button.data = _button.state = 0;
        dispatchRelativePointerPosition(0, 0, 0);
        DEBUG_1F_LOG("STD :: Click Released after %lld ns timeout\n", (*(uint64_t*)&btnDragRls_time) - _drag.releaseStartTime);
    }
    
    _drag.isQuickTap = false;
    _doTimerJob = false;
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Timer method which enhance the acceleration and speed of the pointer.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::EnhancePointerAcceleration()
{
    //
    // Stop processing on Palm and Accidental input
    //
    
    if (_palm.detected || _kbEvent.accidentalInput) {
        return;
    }
    
    if (_pContinuousMove)
    {
        AbsoluteTime point_time;
        
#if APPLESDK
        clock_get_uptime(&point_time);
#else
        clock_get_uptime((uint64_t*)&point_time);
#endif
        UInt8 pointerDelay = 15;
        int smoothFactor = _pSmoothLevel;

        //
        // If there was no data update for more than 3 sec,
        // then con_sider that touchpad might have some static interferences (seen in a laptop) or other problems.
        // So reset and reconnect the touchpad to make it normal.
        //
        
        if ((point_time - _lastDataTime) > (_pAccelCustom * 1000000)) {
            DEBUG_LOG("STD :: Data timeout for pointer acceleration %lld ns.\n", (point_time - _lastDataTime));
            if ((point_time - _lastDataTime) > 3000000000)
            {
                IOLog("STD :: Data timeout exceeded for pointer acceleration, processing stopped.\n");
                Process_touch_end();
                Reconnect_touchpad();
                return;
            }
        }
        else {
        
            DEBUG_LOG("STD :: Last Data time for pointer acceleration %lld ns.\n", (point_time - _lastDataTime));
            
            
            if (_pAccelCustom > 0) {
                pointerDelay = _pAccelCustom;
            }
            
            smoothFactor = _pSmoothLevel; // _pSmoothSamples + 5;
            
            UInt32 pDx = getAbsoluteValue(_pDx);
            UInt32 pDy = getAbsoluteValue(_pDy);
            
            if ((pDx != 0 && smoothFactor > pDx) || (pDy != 0 && smoothFactor > pDy))
            {
                if (pDx > pDy && pDy != 0)
                    smoothFactor = pDy;
                else if (pDx != 0)
                    smoothFactor = pDx;
            }
            
            if (smoothFactor > pointerDelay)
                smoothFactor = pointerDelay;

            int dxDivider = ((_pDx % 2) == 0)?0:1;
            int dyDivider = ((_pDy % 2) == 0)?0:1;
            
            //
            // Smoothing in quarters
            //
            
            int dxAxis = 0;
            
            if (_pDx < 0) {
                dxAxis = (_pDx - dxDivider)/smoothFactor;
                dxAxis = (dxAxis == 0)?-1:dxAxis;
            }
            else if (_pDx > 0)
            {
                dxAxis = (_pDx + dxDivider)/smoothFactor;
                dxAxis = (dxAxis == 0)?1:dxAxis;
            }
            
            int dyAxis = 0;
            
            if (_pDy < 0) {
                dyAxis = (_pDy - dyDivider)/smoothFactor;
                dyAxis = (dyAxis == 0)?-1:dyAxis;
            }
            else if (_pDy > 0)
            {
                dyAxis = (_pDy + dyDivider)/smoothFactor;
                dyAxis = (dyAxis == 0)?1:dyAxis;
            }
            
            dispatchRelativePointerPosition(dxAxis, dyAxis, _button.state);

           // IOLog("STD :: Optimized LDX %d, LDY %d, DX %d DY %d, DIVX %d DIVY %d, SmoothFactor %d, Button %d\n", _pDx, _pDy, dxAxis, dyAxis, dxDivider, dyDivider, smoothFactor, _button.state);
        }
        
        _pointerTimer->setTimeoutMS(pointerDelay/smoothFactor);
    }
}

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Timer method which enhance the acceleration and speed of the scroll.
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::EnhanceScrollAcceleration()
{
    //
    // Stop on Palm and Accidental input
    //
    
    if (_palm.detected || _kbEvent.accidentalInput)
    {
        return;
    }
    
    // Update scroll speed
    float sFactor = _scrollFactor;
    switch (_touchmode) {
        case MODE_EDGE_HSCROLL:
        case MODE_EDGE_VSCROLL:
            sFactor = (_scroll.edgeSpeedFactor/100) * sFactor;
            break;
            
        default:
            break;
    }
    
   // IOLog("STD :: IDelta X %d Y %d Curret X %d Y %d, Cont %d, Auto %d, Sling %d, Inertia Level %d Max %d Depth %d Delay %d\n", _scroll.inertiaDeltaX, _scroll.inertiaDeltaY, _scroll.curInertiaDeltaX, _scroll.curInertiaDeltaY, _scroll.doCont, _scroll.doAuto, _scroll.doSling, _scroll.inertiaLevel, _scroll.maxInertiaLevel, _scroll.inertiaDepth, _scroll.inertiaDelay);

    if (_scroll.inertiaDeltaX == 0 && _scroll.inertiaDeltaY == 0)
    {
        _doTimerJob = false;
        // IOLog("STD :: Continuous scroll...\n");

        //
        // Dispatch Continuous scroll
        //
        
        if (_scroll.doCont)
        {
            AbsoluteTime scroll_time;
            
#if APPLESDK
            clock_get_uptime(&point_time);
#else
            clock_get_uptime((uint64_t*)&scroll_time);
#endif
            
            //
            // If there was no data update for more than 1 sec,
            // then con_sider that touchpad might have some static interferences (seen in a laptop) or other problems.
            // So reset and reconnect the touchpad to make it normal.
            //
            
            if ((scroll_time - _lastDataTime) > 1000000000) {
                IOLog("STD :: Data timeout for scroll acceleration %lld ns.\n", (scroll_time - _lastDataTime));
                Reconnect_touchpad();
                Process_touch_end();
                StopScrollEnhanceTimer();
            }
            else {
                
               // IOLog("STD :: Last Data time for scroll acceleration %lld ns, SF %d, DY %d DX %d, Accel Delay %d\n", (scroll_time - _lastDataTime), (int)sFactor, _scroll.dy, _scroll.dx, _scroll.accelDelay);
                
                _scrollTimer->cancelTimeout();
                dispatchScrollDifference((_scroll.dy * sFactor)/2, (_scroll.dx * sFactor)/2, 0);
                _scrollTimer->setTimeoutMS(_scroll.accelDelay);
            }
        }
        else {
            _scroll.maxInertiaLevel = 0;
            _scroll.inertiaLevel = 0;
        }
        return;
    }
    
    //
    // Dispatch Sling scroll
    //
    
    else if (_scroll.doSling)
    {
        AbsoluteTime scroll_time;
        
#if APPLESDK
        clock_get_uptime(&scroll_time);
#else
        clock_get_uptime((uint64_t*)&scroll_time);
#endif
        
        // IOLog("STD :: Sling Scroll %d, ScrollTime %lld, LastTime %lld\n", _scroll.inertiaDelta, _scroll.slingEffectTime, _scroll.lastSlingEffectTime);
        /*
        if (_scroll.lastSlingEffectTime == 0) {
            _scroll.lastSlingEffectTime = *(uint64_t*)&scroll_time;
        }
        else {
            _scroll.lastSlingEffectTime = *(uint64_t*)&scroll_time - _scroll.lastSlingEffectTime;
            _scroll.slingEffectTime += _scroll.lastSlingEffectTime;
            _scroll.lastSlingEffectTime = *(uint64_t*)&scroll_time;
        }
        
        if (_scroll.slingEffectTime > _scroll.slingEffectRunTime || _scroll.inertiaDelta == 0) {
            _scroll.slingEffectTime = 0;
            _scroll.lastSlingEffectTime = 0;
            _scroll.doSling = false;
            // IOLog("STD :: Returned from Sling timeout\n");
            return;
        }
        else {
            // create decay effect for the scroll at the final sec
            if (_scroll.slingEffectTime > 3500000000 && _scroll.inertiaDelta != 0)
            {
                if (_scroll.inertiaDelta > 0) {
                    _scroll.inertiaDelta--;
                }
                else {
                    _scroll.inertiaDelta++;
                }
            }
            
            // Cancel last timer and dispatch new one
            _scrollTimer->cancelTimeout();
            _scrollTimer->setTimeoutMS(_scroll.accelDelay);
            dispatchScrollDifference((_scroll.inertiaDelta * sFactor)/2, 0, 0);
            return;
        }*/
    }
    
    //
    // Dispatch auto scroll
    //
    
    else if (_scroll.doAuto) {
       DEBUG_SCROLL_SWIPES_LOG("STD :: Auto scroll...\n");

        _scrollTimer->cancelTimeout();
        if (_scroll.inertiaDeltaX != 0 || _scroll.inertiaDeltaY != 0)
        {
            dispatchScrollDifference((_scroll.inertiaDeltaY * sFactor)/4, (_scroll.inertiaDeltaX * sFactor)/4, 0);
            _scrollTimer->setTimeoutMS(_scroll.accelDelay);
        }
        return;
    }
    
    //
    // Dispatch inertia scroll
    //
    
    else {
        
        if (_scroll.curInertiaDeltaY != 0)// vertical
        {
            ProcessInertia(MODE_VSCROLL);
        }
        
        if (_scroll.curInertiaDeltaX != 0)// horizontal
        {
            ProcessInertia(MODE_HSCROLL);
        }
    }
}

void ApplePS2SmartTouchpad::ProcessInertia(TouchPadModes mode)
{
    SInt16 scroll_delta = 0;
    SInt16 cur_scroll_delta = 0;

    switch (mode) {
        case MODE_VSCROLL:
            scroll_delta = _scroll.inertiaDeltaY;
            cur_scroll_delta = _scroll.curInertiaDeltaY;
            break;
            
        case MODE_HSCROLL:
            scroll_delta = _scroll.inertiaDeltaX;
            cur_scroll_delta = _scroll.curInertiaDeltaX;
            break;
            
        default:
            break;
    }
    
    // Get absolute values
    scroll_delta = abs(scroll_delta);

    // Initialize inertia level
    if (_scroll.inertiaLevel == 0)
    {
        // Make inertia depth a bit dynamic by dividing our delta into 3
        if (abs(cur_scroll_delta) > (scroll_delta * 0.66)) {
            _scroll.maxInertiaLevel = _scroll.inertiaDepth;
        }
        else if (abs(cur_scroll_delta) > (scroll_delta * 0.33)) {
            _scroll.maxInertiaLevel = _scroll.inertiaDepth + 1;
        }
        else {
            _scroll.maxInertiaLevel = (_scroll.inertiaDepth - 1);
        }
    }
    
    // Make it using same delta till we reach max level
    if (_scroll.inertiaLevel < _scroll.maxInertiaLevel)
    {
        _scroll.inertiaLevel++;
    }
    else
    {
        _scroll.inertiaLevel = 0;
    }
    
    //
    // Creating inertial effect by dispatching scrolls with delay
    // and decreasing scroll speed
    //
    
    if (cur_scroll_delta > 0) {
        
        // Decrease speed once reached max inertia depth
        if (_scroll.inertiaLevel == 0) {
            cur_scroll_delta--;
        }
    }
    else
    {
        // Decrease speed once reached max inertia depth
        if (_scroll.inertiaLevel == 0) {
            cur_scroll_delta++;
        }
    }
    
    switch (mode) {
        case MODE_VSCROLL:
            dispatchScrollDifference((_scroll.curInertiaDeltaY * _scrollFactor)/2, 0, 0);
            _scroll.curInertiaDeltaY = cur_scroll_delta;
            break;
            
        case MODE_HSCROLL:
            dispatchScrollDifference(0, (_scroll.curInertiaDeltaX * _scrollFactor)/4, 0);
            _scroll.curInertiaDeltaX = cur_scroll_delta;
            break;
            
        default:
            break;
    }
    
    cur_scroll_delta = abs(cur_scroll_delta);
    
    //
    // Split inertial delay into 4 levels to add linear decay
    //
    
    UInt8  inertialDelay = 1;
    if (_scroll.inertiaDelay < 4)
    {
        inertialDelay = 1;
    }
    else {
        inertialDelay = _scroll.inertiaDelay/4;
    }
    
    if (cur_scroll_delta > (scroll_delta * 0.75))
    {
        inertialDelay *=  1;
    }
    else if (cur_scroll_delta > (scroll_delta * 0.50))
    {
        inertialDelay *= 2;
    }
    else if (cur_scroll_delta > (scroll_delta * 0.25))
    {
        inertialDelay *= 3;
    }
    else {
        inertialDelay *= 5;
    }
    
    // IOLog("STD :: Inertial Mode %d, Scroll %d, ScrollFactor %d, Delay %d\n", mode, cur_scroll_delta, scroll_delta, inertialDelay);
    
    // Trigger timeout for another scroll after a little delay
    _scrollTimer->setTimeoutMS(inertialDelay);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void ApplePS2SmartTouchpad::ControlfKeybrdBackLight()
{
    if (_isKBLightOff || _lastKBLightTriggerTime == 0)   {
        _device->dispatchPS2Notification(kPS2C_KeybrdBackLightOn);
    }
    else {
        _device->dispatchPS2Notification(kPS2C_KeybrdBackLightOff);
    }
    clock_get_uptime(TIME_NOW);
    _lastKBLightTriggerTime = absoluteToUINT64(_now);
    
    DEBUG_LOG("STD :: Dispatched Asus Keyboard backlight State %s, Time %lld\n", _isKBLightOff?"OFF":"ON", _lastKBLightTriggerTime);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


//* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
//
// Utility Methods to detect OSX version, Touchpad vendor etc.,
//
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

void ApplePS2SmartTouchpad::detectOSXVersion()
{
    
    DEBUG_LOG("STD :: Detecting OS X version...\n");
    
    OSString*   iokitbuildObj  = 0;
    const char* iokitbuildStr   = NULL;
    char buildVerString[8]     = {NULL};
    int index = 0, strCount = 0;
    
    IORegistryEntry *ioregRoot = IORegistryEntry::getRegistryRoot();
    
    if (ioregRoot != NULL) {
        
        iokitbuildObj = OSDynamicCast(OSString, ioregRoot->getProperty("IOKitBuildVersion"));
        
        if (iokitbuildObj != NULL) {
            
            DEBUG_LOG("STD :: IOKitBuildVersion %s\n", iokitbuildObj->getCStringNoCopy());
            iokitbuildStr = iokitbuildObj->getCStringNoCopy();
            
            do {
                // Look for word terminators
                if (*iokitbuildStr == ' ' ||
                    *iokitbuildStr == '\0' ||
                    *iokitbuildStr == ':')
                {
                    // Found ":" then we reached end of Darwin kernel version string
                    if (*iokitbuildStr == ':')
                    {
                        buildVerString[index] = '\0'; // end
                    }
                    
                    index = 0;
                    strCount++;
                    
                    if (strCount == 4)
                    {
                        DEBUG_LOG("STD :: OS X Darwin Kernel version - %s\n", buildVerString);
                        break;
                    }
                    
                    continue;
                }
                
                // Found three strings "Dawin Kernel Version" ? Next is version
                if (strCount == 3) {
                    buildVerString[index] = *iokitbuildStr;
                    index++;
                }
                
            } while (*iokitbuildStr++);
            
            // Convert Darwin kernel version string xx.x.x to integer xx
            UInt8 osxVersion = strtol(buildVerString, NULL, 10);
            
            // Subtract 4 from darwin kernel version to get OS X version
            if (osxVersion > 4) osxVersion -= 4;
            
            IOLog("STD :: OS X version 10.%d.%ld\n", osxVersion, strtol(&buildVerString[3], NULL, 10));
            _osxVersion = osxVersion;
        }
        else
            IOLog("STD :: Failed to get IOKitBuildVersion\n");
    }
    else
    {
        IOLog("STD :: Failed to get an object for IOReg root\n");
    }
    
    // OSSafeRelease(ioregRoot);
    
}

void ApplePS2SmartTouchpad::detectTouchpadVendor()
{
    
    bool found;
    OSObject *tNameMatch ;
    IORegistryEntry *entry;
    
    //
    // Get the APCI Plane for our search
    //
    
    const IORegistryPlane * IOACPIPlane = IORegistryEntry::getPlane("IOACPIPlane");
    
    
    //
    // Search from the root of the ACPI plane for the PS2M Focaltech PNP
    //
    
    found = false;
    tNameMatch  = getProperty("FocalTechNameMatch");
    IORegistryIterator *fiter = IORegistryIterator::iterateOver(IOACPIPlane, kIORegistryIterateRecursively);
    
    BASIC_DBG_LOG("STD :: Searching for FocalTech touchpad...\n");
    
    if (fiter != NULL)
    {
        while((entry = fiter->getNextObject()))
        {
            if (entry->compareNames(tNameMatch)) {
                
                BASIC_DBG_LOG("STD :: ACPIPlane name %s\n", entry->getName());
                found = true;
                
                OSData *name = OSDynamicCast(OSData, entry->getProperty("name"));
                if (name) {
                    const char* id = (char*) name->getBytesNoCopy();
                    if (id) {
                        IOLog("STD :: Touchpad PNP match %s\n", id);
                    }
                }
                break;
            }
        }
        fiter->release();
    }
    
    if (found) {
        _touchpadVendor = STD_FOCAL_TECH;
        IOLog("STD :: FocalTech Touchpad found.\n");
    }
    else {
        
        BASIC_DBG_LOG("STD :: FocalTech Touchpad not found.\n");
        
        //
        // Search from the root of the ACPI plane for the PS2M Synaptics PNP
        //
        
        /*
         found = false;
         tNameMatch  = getProperty("SynapticsNameMatch");
         IORegistryIterator *siter = IORegistryIterator::iterateOver(IOACPIPlane, kIORegistryIterateRecursively);
         
         BASIC_DBG_LOG("STD :: Searching for Synaptics touchpad...\n");
         
         if (siter != NULL)
         {
         while((entry = siter->getNextObject()))
         {
         if (entry->compareNames(tNameMatch)) {
         
         BASIC_DBG_LOG("STD :: ACPIPlane name %s\n", entry->getName());
         found = true;
         
         OSData *name = OSDynamicCast(OSData, entry->getProperty("name"));
         if (name) {
         const char* pnpReg = (char*) name->getBytesNoCopy();
         
         if (pnpReg) {
         IOLog("STD :: Touchpad PNP match %s\n", pnpReg);
         
         
         if (OSDictionary *sMinMaxDict = OSDynamicCast(OSDictionary, getProperty(SynapticsTPadMinMax)))
         {
         if (OSDictionary *pnpDict = OSDynamicCast(OSDictionary, sMinMaxDict->getObject(pnpReg)))
         {
         if (OSNumber *numKey = OSDynamicCast(OSNumber, pnpDict->getObject(SynapticsTPadXmin))) {
         _xmin = numKey->unsigned32BitValue();
         IOLog("STD :: Touchpad XMIN %d\n", _xmin);
         }
         
         if (OSNumber *numKey = OSDynamicCast(OSNumber, pnpDict->getObject(SynapticsTPadXmax))) {
         _xmax = numKey->unsigned32BitValue();
         IOLog("STD :: Touchpad XMAX %d\n", _xmax);
         }
         
         if (OSNumber *numKey = OSDynamicCast(OSNumber, pnpDict->getObject(SynapticsTPadYmin))) {
         _ymin = numKey->unsigned32BitValue();
         IOLog("STD :: Touchpad YMIN %d\n", _ymin);
         }
         
         if (OSNumber *numKey = OSDynamicCast(OSNumber, pnpDict->getObject(SynapticsTPadYmax))) {
         _ymax = numKey->unsigned32BitValue();
         IOLog("STD :: Touchpad YMAX %d\n", _ymax);
         }
         }
         }
         
         }
         }
         break;
         }
         }
         siter->release();
         }
         
         if (!found) {
         IOLog("STD :: Synaptics Touchpad PNP match not found for custom config.\n");
         _touchpadVendor = STD_ELAN_TECH;
         }
         else {
         _touchpadVendor = STD_SYNAPTICS;
         IOLog("STD :: Synaptics Touchpad PNP match found for custom config.\n");
         }
         */
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


