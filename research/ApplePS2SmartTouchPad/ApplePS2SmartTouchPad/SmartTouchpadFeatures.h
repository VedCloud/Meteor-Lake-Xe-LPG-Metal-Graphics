//
//  SmartTouchpadFeatures.h
//  ApplePS2SmartTouchPad
//
//  Created by EMlyDinEsH on 5/16/16.
//  Copyright (c) 2016 EMlyDinEsH. All rights reserved.
//

#ifndef __ApplePS2SmartTouchPad__SmartTouchpadFeatures__
#define __ApplePS2SmartTouchPad__SmartTouchpadFeatures__

#include <IOKit/IOService.h>

/*
 * Track up to 5 fingers
 * ELAN v2 and v3 supports 3 fingers
 * ELAN v4, Synapticsand Focal tech supports 5 fingers
 * Some models have track point (stick), so its 5 + 1.
 */

#define STD_MAX_FINGERS			5 + 1

/*
 * Max time for Click, Tap detection and Drag Release timeout
 */

#define NC_REL_TIME         350  //  for NC, 0.35 sec
#define MAX_DTAP_TIME       300  //  for 2,3,4 fing double tap, 0.3 sec
#define APP_GESTURE_TIMEOUT 1500 //  for some gesture release, 1.5 sec

#define MAX_FING_PRESS      2000000000 // for finger(s) press, 2 sec
#define MAX_CLICK_TIME      120000000  // for Tap Click, 0.12 sec
#define MAX_TAP_TIME        150000000  // for 1,2 fing taps, 0.15 sec
#define MAX_MULF_TAP_TIME   180000000  // for 3,4 fing taps, 0.18 sec
#define MAX_MUL_DTAP_TIME   500000000  // for 2,3,4 fing double/triple taps, 0.5 sec

#define MAX_NOISE_DELTA     120 // Noise delta in two fingers on first packet
#define MIN_ESCROLL_DELTA   250 // Delta to trigger Edge scroll mode

#define MAX_GESTURE_ACTIONS     32
#define SAMPLES_COUNT           11

/*              7 6 5 4 3 2 1 0
 Debug Levels:  0 0 0 0 0 0 0 0               Levels   Debug Level Decimal Value (only bit feature)
 
 bit 0 - Packet data and Elan Init          - Level 1       1  (1)
 bit 1 - Single Finger debug data           - Level 2       3  (2)
 bit 2 - Two Fingers debug data             - Level 3       7  (4)
 bit 3 - Three and Four Fingers debug data  - Level 4       15  (8)
 bit 4 - Features Toggle data               - Level 5       31 (16)
 bit 5 - Buttons debug data                 - Level 6       63 (32)
 bit 6 - Zoom, Rotate and Press debug data  - Level 7       127 (64)
 bit 7 - Scroll and Swipes debug data       - Level 8       255 (128)
 */


#define DEBUG_TPD 0

#if DEBUG_TPD
#define DEBUG_LOG(fmt, args...) if ((_debugLvl & 255) == 255) IOLog(fmt, ## args)
#else
#define DEBUG_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define BASIC_DBG_LOG(fmt, args...) if (_debugLvl & 1) IOLog(fmt, ## args)
#else
#define BASIC_DBG_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DUMP_LOG(fmt, args...) if (_debugLvl & 1) IOLog(fmt, ## args)
#else
#define DUMP_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_1F_LOG(fmt, args...) if (_debugLvl & 2) IOLog(fmt, ## args)
#else
#define DEBUG_1F_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_2F_LOG(fmt, args...) if (_debugLvl & 4) IOLog(fmt, ## args)
#else
#define DEBUG_2F_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_MUL_FING_LOG(fmt, args...) if (_debugLvl & 8) IOLog(fmt, ## args)
#else
#define DEBUG_MUL_FING_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_TOGGLE_LOG(fmt, args...) if (_debugLvl & 16) IOLog(fmt, ## args)
#else
#define DEBUG_TOGGLE_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_BTNS_LOG(fmt, args...) if (_debugLvl & 32) IOLog(fmt, ## args)
#else
#define DEBUG_BTNS_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_ZRP_LOG(fmt, args...) if (_debugLvl & 64) IOLog(fmt, ## args)
#else
#define DEBUG_ZRP_LOG(fmt, args...)
#endif

#if DEBUG_TPD
#define DEBUG_SCROLL_SWIPES_LOG(fmt, args...) if (_debugLvl & 128) IOLog(fmt, ## args)
#else
#define DEBUG_SCROLL_SWIPES_LOG(fmt, args...)
#endif

// Convert -ve to +ve
#define abs(x) ((x) < 0 ? -(x) : (x))

class SmartTouchpadFeatures
{
    
private:
    UInt8 _venID;
    
protected:

    bool        _lidClosed;
    bool        _enableLidPolling;
    uint64_t    _acpiPollRate;
    
    //
    // Pointer properties
    //
    
    UInt32      _pointerResolution;

    UInt8       _pAccelPreference;
    UInt8       _pAccelCustom;
    UInt8       _pSmoothSamples;
    UInt8       _pSmoothLevel;
    
    UInt8       _tSenseLvl;
    UInt8       _pExtAccelLevel;
    UInt8       _pExtAccelMinDelta;
    UInt8       _pSlowMoveNoiseDelta;
    
    bool        _pContinuousMove;

    UInt16      _ymax;
    UInt16      _xmax;
    UInt16      _ymin;
    UInt16      _xmin;
    UInt16      _xcenter;
    UInt16      _ycenter;

    UInt8       _cSample, _skipSample, _scrollSample;

    struct Delta_samples {
        SInt16 skippedX;
        SInt16 skippedY;
        SInt16 lastDX1;
        SInt16 lastDX2;
        SInt16 lastDY1;
        SInt16 lastDY2;
        UInt16 lastSDX;
        UInt16 lastSDY;
        UInt8  pressure;
    } _lastSample[SAMPLES_COUNT];
    
    int     _pDx, _pDy;
    int     _pLastDx, _pLastDy;
    int     _pSmoothX, _pSmoothY;

    // Disabled Active Edge areas    
    UInt16       _tpLeftEdgeDArea;
    UInt16       _tpRightEdgeDArea;
    UInt16       _tpTopEdgeDArea;
    UInt16       _tpBottomEdgeDArea;

    //
    // Gesture time tracker
    //
    
    uint64_t    _gestureTime, _lastDataTime;
    
    //
    // Palm detection
    //
    
    struct PalmStruct {
        UInt8   minPressure, minWidth;
        bool    detectionEnabled, detected;
    } _palm;
    
    //
    // External mouse status
    //
    
    bool            _externalMouseFound;
    uint64_t        _lastExtMouseStatusCheckTime;
    uint64_t        _extMouseCheckPollRate;

    //
    // Timers
    //
    
    IOTimerEventSource*    _pointerTimer;
    IOTimerEventSource*    _scrollTimer;
    IOTimerEventSource*    _featureTimer;
    IOTimerEventSource*    _kBackLightTimer;
    IOTimerEventSource*    _buttonDragTimer;
    
    bool                   _doTimerJob;
    
    //
    // Trackpad prefpane values
    //
    
    bool				  _isDragEnabled, _isDragLockEnabled;
    bool                  _isHScrollEnabled, _isVScrollEnabled;
    bool                  _isClickingEnabled;
    bool                  _isTapSwapEnabled; // "Tap 2 fing as right click" swaps between 2F and 3F tap
    bool                  _isTypeModeEnabled; // Ignore Accidental Input ?
    bool                  _usbMouseDisablesTrackpad; // External mouse disables trackpad ?
    float                 _scrollFactor;
    UInt8                 _scrollLevels;
    UInt64                _scrollAccell;
    UInt64                _pointerAccell;
    uint64_t              _maxDoubleClickTime;

    //
    // ClickPad prefpane values
    //
    
    enum SecondaryClick
    {
        INPUT_2F_TAP,
        INPUT_LEFT_CORNER,
        INPUT_RIGHT_CORNER,
    };
    
    UInt8   _secondarayClickInput;
    bool    _3FTapLookUp;
    bool    _3FDrag;
    bool    _2FDoubleTapSZoom;
    
    //
    // Finger(s) properties
    //
    
    SInt8   _pid, _sid; // two fingers slot values in mutli touch
    UInt8   _cFingers, _lastFingers;
    
    struct FingerDataStruct {
        UInt16 x;
        UInt16 y;
        UInt16 last_x;
        UInt16 last_y;
        UInt16 start_x;
        UInt16 start_y;
        SInt32 tdx;
        SInt32 tdy;
        SInt32 sdx;
        SInt32 sdy;
        UInt8 pressure;
        UInt8 width;
    } _fing[STD_MAX_FINGERS];
    
    //
    // Properties that control dragging
    //
    
    struct DragStruct {
        bool        isDragging;
        bool        isQuickTap;
        bool        checkTapDrag;
        bool        isHoldDrag, isTapDrag;
        bool        is3FDrag;
        bool        hasFPresDrag;
        
        UInt8       fPressDragFingersID;
        UInt8       fPressDragFingersNum;
        UInt8       fPressDragPressure;
        UInt32      releaseTime;
        uint64_t    releaseStartTime, fPressDragStartTimeOut;
    } _drag;
    
    
    //
    // Properties that control Scroll
    //
    
    struct ScrollStruct {
        UInt32      resoultion;
        
        UInt8       accelMode2F;
        UInt8       accelModeEdge;
        UInt8       smoothSamples;
        
        SInt8       xPacketsDir, yPacketsDir;
        
        UInt8       contMaxDeltaEdge;
        UInt8       contMaxDelta2F;
        UInt8       contAccel;
        
        UInt8       edgeCirDelta;
        UInt16      edgeVArea, edgeHArea;
        UInt16      edgeVAdaptArea, edgeHAdaptArea;
        bool        hasEdgeScroll, hasEdgeCScroll;
        bool        hasEdgeInertia, hasEdgeCont, hasEdgeSling, hasEdgeAuto;

        bool        has2FInertia, has2FCont, has2FSling, has2FAuto;
        bool        doCont, doAuto, doSling;
        bool        noInertia;
        
        UInt8       accelDelay;
        UInt8       levelAtBoot, levelFactor, levelGranularity;
        UInt8       edgeSpeedFactor;

        UInt8       stopSamples;
        UInt8       directionSamples;
        UInt8       idleFingMaxDelta;
        uint64_t    idleFingDetectTimeout;
        
        SInt16      dx, dy, edgeCirDxy;
        UInt32      totalDx, totalDy;
        UInt8       maxLevelReached;
       
        UInt8       dxPacketsCount, dyPacketsCount;
        
        bool        noHSInertia;

        UInt8       inertiaDepth;
        UInt8       inertiaDelay;
        UInt8       inertiaLevel, maxInertiaLevel;

        SInt16      curInertiaDeltaX, inertiaDeltaX;
        SInt16      curInertiaDeltaY, inertiaDeltaY;

        SInt8       slingDepth;
        UInt16      slingTriggerArea;
        uint64_t    slingStartTimeout, slingEffectRunTime, slingEffectTime, lastSlingEffectTime;
        
        SInt16      skippedDx[SAMPLES_COUNT];
        SInt16      skippedDy[SAMPLES_COUNT];
    } _scroll;
    
    //
    // Properties that control Corner Tapping
    //
    struct CornerTapStruct {
        UInt8   corner;
        UInt16  tapAreaLength, tapAreaHeight;
        bool    enabled, bottomCornersDisabled, releaseStartABS;
    } _cornerTap;
    
    //
    // Properties that control Zoom and rotate
    //
    
    struct ZoomStruct {
        bool    enabled;
        bool    isZoomOut, isZoomIn;
        bool    tapZoomDone;
        
        SInt16  zDx, zDy;
        
        UInt8   minSamples;
        UInt8   pinchDeltaScale;
        UInt16  delta;

    } _zoom;
    
    struct RotataStruct {
        bool    enabled, cirEnabled;
        bool    isRotateGesture;
        bool    isRotateMode;

        SInt16     xAxisPos, yAxisPos;
        SInt16     rotateXCounter, rotateYCounter;
        
        UInt16  delta, maxCFDelta, maxCFDeltaY; // CF = Constant Finger
        uint64_t minCFingIdleTime;
    } _rotate;
    
    //
    // Properties that control Swipes
    //
    
    struct SwipeStruct {
        bool        has3FSwipes, has4FSwipes, hasEdgeSwipes;
        bool        has2WayEdgeSwipes;
        
        bool        isEdgeGesture, resetPointerOnEdge;
        bool        isAppSwitchDone, isInFullScreen, isNCDone;
        
        UInt8       up3F, down3F, right3F, left3F;
        UInt8       up4F, down4F, right4F, left4F;
        UInt8       leftEdge, rightEdge, topEdge, bottomEdge;
        
        UInt8       direction;
        UInt8       edgeFingers;
        
        UInt16      edgeHArea, edgeVArea;
        UInt16      nonEdgeHArea, nonEdgeVArea;
        UInt16      edgeHDistance, edgeVDistance;
        
        UInt16      mulFingDeltaX, mulFingDeltaY;
    } _swipe;
    
    //
    // Properties that control Tap and fingers press
    //
    
    struct TapStruct {
        SInt8       fingers1, fingers2, fingers3, fingers4, fingers5;
        
        uint64_t    lastTime;
        uint64_t    mulFingsTimeout;
        uint64_t    clickTimeout, timeout;

        bool        hasMulFDoubleTaps;
        SInt8       count, fingersCount;

        UInt8       noiseDelta, noiseMultiTapDelta;
        UInt8       pressure;
        UInt8       maxPressDtd;
        UInt8       maxTEndPressure;
        UInt16      maxTEndDelta;
    } _tap;
    
    struct FingerPressStruct {
        UInt8       fings2Click;
        SInt8       fings2Press, fings3Press, fings4Press, fings5Press;
        
        uint64_t    timeout;
        uint64_t    gestureTime;
        uint64_t    holdTime, lastHoldTime;
    } _fingPress;
    
    SInt8       _fings4PinchAction, _fings5PinchAction;

    //
    // Properties that control buttons
    //
    
    struct ButtonStruct {
        bool            isFingOnBtnArea, clickPad2FMove;
        bool            rightClick, triggered, is2FClick;
        bool            isClickPad, enableMiddle;
        bool            clickPadHasVButtons, disableClicks;
        bool            disbleTrackPoint;
        bool            enablePointingOnBtnArea;
        
        UInt16          clickPadVBtnArea;
        UInt8           state, data;
        UInt8           fingerInUse;
    } _button;
    
    
    //
    // Touchpad Modes
    //
    
    enum TouchPadModes {
        MODE_IDLE, MODE_MOVE, //0 and 1
        //2-9
        MODE_VSCROLL,
        MODE_HSCROLL,
        MODE_CSCROLL,
        MODE_EDGE_VSCROLL,
        MODE_EDGE_HSCROLL,
        MODE_CIR_HSCROLL,
        MODE_CIR_VSCROLL,
        MODE_CONT_SCROLL,
        //10-16
        MODE_5_FING_PINCH,
        MODE_5_FING_TAP,
        MODE_4_FING_TAP,
        MODE_3_FING_TAP,
        MODE_2_FING_TAP,
        MODE_1_FING_TAP,
        MODE_CORNER_TAP,
        //17
        MODE_BUTTONS,
        //18-21
        MODE_FIVE_FING_PRESS,
        MODE_FOUR_FING_PRESS,
        MODE_THREE_FING_PRESS,
        MODE_TWO_FING_PRESS,
        //22 and 23
        MODE_ROTATE, MODE_ZOOM,
        //24 and 25
        MODE_MUL_TOUCH_3, MODE_MUL_TOUCH_4,
        //26
        MODE_TOP_EDGE_SWIPE,
        MODE_BOTTOM_EDGE_SWIPE,
        MODE_LEFT_EDGE_SWIPE,
        MODE_RIGHT_EDGE_SWIPE,
        // 30
        MODE_FEATURE_TOGGLE,
        // 31
        MODE_4_FING_PINCH,
        MODE_PALM
    } _touchmode;
    
    //
    // Properties that control keyboard events
    //
    
    struct KeyboardEventStruct {
        bool        isToggleKeyOnHold;
        bool        accidentalInput, tapEnableTPad;
        bool        disableTouchpad;
        bool        altFunction, showPressWidth;

        UInt8       accInputKeysCount, kbKeysCount;
        uint64_t    lastKeyPressTime, accInputTimeOut;
    } _kbEvent;
    
    //
    // Asus Notebook properties
    //
    
    bool            _isAsusNotebook, _isFnKeyDisabledTouchpad;
    bool            _hasAsusKBackLight, _has16LvlKBLight;
    bool            _hasKBLightAutoOff, _isKBLightOff;
    
    UInt32          _kBLightAutoOffTimeout;
    
    uint64_t        _lastKBLightTriggerTime;
    uint64_t        _lastFnKeyStateChkTime;
    
public:
    
    virtual void Get_Gestures_Data() = 0; // Pure virtual function
    
    // Methods which processes all the features
    virtual void Process_singlefinger_touch(int x, int y, int fid);
    virtual void Process_twofingers_touch(int x1, int x2, int y1, int y2);
    virtual void Process_multiFingers_touch(int m_x, int m_y, int fid);
    virtual void Process_touch_end();
    virtual void Process_fingers_change();
    virtual void Process_scroll(UInt8 sMode, int sdx, int sdy, bool isZoomConflict);
    virtual void Proces_Fingers_Press(UInt8 fingers);
    virtual void Process_ClickPad_2F(unsigned char *packet, UInt8 packetType, int dx1, int dy1, int dx2, int dy2);
    virtual void Process_edgeSwipe(int x, int y, int fid);
    virtual void Dispatch_edgeSwipe(UInt8 gestureAction, UInt8 direction);
    virtual void Is2FingEdgeSwipe();
    
    virtual void Process_Physical_Buttons(unsigned char *packet);
    bool Process_ClickPad_Virtual_Buttons(unsigned char *packet, UInt8 packetType, UInt8 fid, UInt8 fCount);
    virtual void Release_ButtonsAndDrag();
    
    virtual void ResetPointerToMiddle();
    virtual void Open_NC();
    
    // Timer methods for intertial scroll, features toggle and button relelase
    virtual void EnhancePointerAcceleration();
    virtual void EnhanceScrollAcceleration();
    virtual void ProcessInertia(TouchPadModes mode);
    virtual void ProcessFeaturesToggle();
    
    // ACPI and IOREG Polling
    void Process_ACPI_IOREG_Polling();
};

#endif /* defined(__ApplePS2SmartTouchPad__SmartTouchpadFeatures__) */
