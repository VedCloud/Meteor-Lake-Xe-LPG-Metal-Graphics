This is a multitouch driver for ELAN, FocalTech and Synaptics
touchpad for Mac OS X based on the documentations available and some
ideas from VoodooPS2, ApplePS2 and Linux open source code. I have
successfully accomplished everything so far and added many new features
to take advantage the touchpad completely to make it close to Apple
touchpad in every way. Also my driver is implemented with my own
universal logic for all the features, so it can be adapted to all
touchpads easily and I\'m still improving these to achieve even better
results.\

My driver supports every hardware version of ELAN touchpads
except version 1 (which is too old and can\'t find in existing notebooks
will support that if i get some testers),  mostly all available
FocalTech touchpads and Synaptics touchpads (still experimental) with
firmware version v7.5+.

**NOTE: For Synaptics,  it was developed based on my touchpad hardware i
have, so support is limited to touchpad with firmware version 7.5+ only,
but will try to improve support other hardwares also if users interested
in my features and help me with beta testing.**\

**[Following are the features supported by the Smart touchpad driver in
Mac OSX:]{.underline}**

1.  Mouse mode

2.  Tapping

3.  Dragging

4.  Scrolling

5.  Swipes

6.  Zoom

7.  Rotation

8.  Clicking and Fingers Press 

9.  Features Toggle

10. Accidental inputs prevention and Palm detection

11. Apple Trackpad Prefpane support

12. Customization of features

13. Enhanced Keyboard support

14. Support for Asus Keyboard backlight with Idle Auto on/off

15. Prefpane for controlling touchpad features **\[coming later as its
    got my least prority\]**

**[Supported OS X Action values to config gestures:]{.underline}**

- 0 : To disable the gesture

- 1 - Applications Switch

- 2 - App close

- 3 - Launchpad

- 4 - Mission control

- 5 - Dashboard

- 6 - Left Space/Full Screen apps switch

- 7 - Right Space/Full Screen apps switch

- 8 - Application windows

- 9 - Minimize app

- 10 - Toggle Full screen Switch

- 11 - Backward 

- 12 - Forward 

- 13 - Desktop

- 14 - Notification center (keyboard shortcut)

- 15 - Show Properties/Info

- 16 - Hide/Show Dock

- 17 - Notification center (soft mode)

- 18 - Zoom reset (works in Photos, Browser etc., where it supports)

- 19 - Finder

- 20 - Force Quit

- 21 - Right click

- 22 - Middle click

- 23 - 34 as F13 - F24

<!-- -->

- 35 - Left click

**1. MOUSE MODE:**\
 \
Enables click-pad and touchpad with physical buttons to act like a mouse
with three left, right and middle buttons and pointing the cursor. \
 \
**Note: Click-pads**\
Click-pad has only single button under the whole touch area. So, the
touchpad bottom area is divided into three parts for the buttons left,
right and middle.\
 \
**BUTTONS:**\
**Left button      :**   Performs Left click\
**Right button    :  ** Performs Right click\
**Middle button  :   **Performs Middle click\
You can configure middle button for different actions from Mission
Control in ***\"Keyboard and Mouse Shortcuts\"***.\
 \
\
 \
**For Middle click:**\
**[Click-pad]{.underline}**[:]{.underline} Press and click at middle of
the touchpad button area\
**[Touchpad with physical left and right
buttons]{.underline}**[:]{.underline} Click both left and right buttons
same time\
 

**[Customization:]{.underline}**

You can control buttons with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Buttons** in the
plist.

1.  **ClickpadVirtualButtons (Yes/No) :** Enables virtual buttons for
    click-pads by dividng the bottom area into three parts, otherwise
    only one button \"Left\" will work default.

2.  **ClickpadVirtualButtonsArea (Value 0 -100) :** Sets the % of
    touchpad area (bottom) used by the virtual buttons.

3.  **DisablePhysicalClicks (Yes/No) :** Enable/Disable physical
    buttons.

4.  **VirtualMiddleButton (Yes/No) :** Enable/Disable virtual middle
    button for click-pad.

5.  **2FingersClickAction (Value 0 - 23) :** Configures the gesture 2
    fingers button click action from supported actions list in
    click-pad.

**POINTING:**\
This driver tries to provide extensive control over pointing the cursor
to give a smooth feel in acceleration. Following are the plist options
that controls the pointing. Located at
**IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Pointing** in the
plist.

1.  **AccelerationPrefValue (Value 0 - 4) :** These are acceleration
    tables used by apple for their devices. Each table has different
    acceleration curves, so try them and choose which its for you.

2.  **PointerCustomAcceleration (Value 0 - 20):** This value controls
    the dispatch rate of the pointer position and by doing so its can
    alter the acceleration level. Increasing the value will reduce
    acceleration and decreasing will speed up.

3.  **PointerResolution (Value 100 - 3000) :** This is the resolution of
    the pointer used by OS X. Increasing the value will smooth the
    acceleration by reducing its speed and decreasing will speed up but
    reduces smoothness.

4.  **PointerSmoothLevel (Value 1 - 20) :** Controls the smoothness of
    the pointing. Increasing the value will smooth the acceleration by
    reducing its speed and decreasing will speed up but reduces
    smoothness.

5.  **PointerSmoothSamples (Value 0 - 10) :** Number of samples to used
    to smooth the pointing. Increasing the value will smooth the
    acceleration by reducing its speed and decreasing will speed up but
    reduces smoothness.

6.  **EnhancedAccelerationLevel (Value 0 - 15) :** It extends the
    acceleration of the pointer for speeds of the pointing higher that
    Minimum delta key.

7.  **EnhancedAccelerationMinDelta (Value 0 - 255) :** It sets the
    minimum delta needed to apply extend acceleration.

8.  **SlowMoveNoiseMaxDelta (Value 0 - 30) :** Some touchpad\'s generate
    noise (random x and y delta) during slow moves which causes the
    pointer to move like a wave. Use this value to ignore those noise
    data.

9.  **Sensitivity (Value 0 - 255) :** Sets the sensitivity (Pressure or
    Z value) of the touchpad.

**2. TAPPING:**

Need to enable the option **\"Clicking\"** in Trackpad prefpane from
System preferences for taps to work.\
 

- **Single finger tap:**

Single tap : Performs single click\
Double tap: Performs double click\
Triple tap  : Performs paragraph selection in texts

- **Two fingers tap:**

Single tap : Acts as middle click button action by default. It can
configured to other actions by editing plist with supported gesture
actions value. \
Double tap: Does zoom in and zoom out when **\"MulFingDoubleTaps\"** is
enabled in plist.

- **Three fingers tap: **

Single tap : Acts as right click button action by default. It can
configured to other actions by editing plist with supported gesture
actions value. \
Double tap: Shows application window when **\"MulFingDoubleTaps\"** is
enabled in plist.

- **Four fingers tap*:***

Single tap  : Opens Mission control by default. It can be configured to
other actions by editing plist with supported gesture actions value.

- **Five fingers tap*:** ***

Single tap  : Opens Dashboard by default. It can be configured to other
actions by editing plist with supported gesture actions value.

- **Corner taps:**

Need to enable the feature **\"Hot Corners\"** from Mission Control and
configure it. You can Tap on any of the four corners to trigger the
functions configured to each hot corner. \
 

**[Customization:]{.underline}**

You can configure taps with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Tapping** in the
plist.

1.  **2FingersTapAction (Value 0 -23) :** Sets the action to be
    triggered on two fingers tap. 

2.  **3FingersTapAction (Value 0 -23) :** Sets the action to be
    triggered on three fingers tap.

3.  **4FingersTapAction (Value 0 -23) :** Sets the action to be
    triggered on four fingers tap.

4.  **5FingersTapAction (Value 0 -23) :** Sets the action to be
    triggered on five fingers tap.

5.  **1FTapPressure (Value 0 - 255) :** Sets the pressure needed to
    detect the touch as tap.

6.  **1FClickTimeOut (Value 0 - 3000) :** Sets the max time (in milli
    sec) used to detect the touch as click.

7.  **12TapTimeOut (Value 0 - 3000) :** Sets the max time (in milli sec)
    used to detect the touch as Corner or 2 fingers tap.

8.  **MulFingTapTimeOut (Value 0 - 3000) :** Sets the max time (in milli
    sec) used to detect the touch as  3-5 fingers tap.

9.  **TapMaxDelta (Value 0 - 200) :** Sets the max delta (movement)
    allowed in detecting the touch as single finger tap.

10. **MulFingTapMaxDelta (Value 0 - 200) :** Sets the max delta
    (movement) allowed in detecting the touch as multi finger tap.

11. **MulFingDoubleTaps (Yes/No) :** Enables/Disables multi finger
    double taps.

12. **TrackpadTappingCorners (Yes/No) :** Enables/Disables corner taps.

13. **DisableBottomCornersTap (Yes/No) :** Enables/Disables only bottom
    corner taps when Corner taps are enabled.

14. **CornerTapStartABS (Yes/No) :** Want to start pointer from absolute
    position of the touch after corner tap instead of reset to center of
    the screen ?

15. **CornerTapAreaHeight (Value 0 - 100) :** Sets the % of the touchpad
    area used for the height of the corner detect area.

16. **CornerTapAreaLength (Value 0 - 100) :** Sets the % of the touchpad
    area used for the length of the corner detect area.

***[Note for 2 and 3 fingers tap:]{.underline}*** *You can swap Two and
Three finger single taps action in Trackpad prefpane by
enabling/disabling the option* ***\"Tap trackpad using two fingers for
secondary click\"****.*\
 

**[Note for Multi finger double taps:]{.underline}** Enabling the
option **\"MulFingDoubleTaps\"** in plist will make multi fingers single
tap to have some delay due to the detection of double taps.\
\

**3. DRAGGING**

 \
Need to enable the option **\"Dragging\"** in Trackpad prefpane from
System preferences for dragging to work.

- ***Left button drag :*** Click & Hold the left button then drag with
  another finger.

- ***Double tap drag :** ***Double tap and hold a single finger without
  lifting then move to drag.

- ***Finger press drag :*** Tap and hold a single finger with some
  pressure for 1.2 secs without lifting then move to drag. 

- ***Click drag (ClickPad):*** Click and hold the button under the whole
  touchpad area of click-pad then move to drag.

**[Note for finger press drag:]{.underline} ** If you want to disable
this drag then set the value of \"**FingerPressDragTimeOut\" **to 0.

**You can release drag in three ways:**

- **Finger lift to release**

Drag will be released immediately after the finger is removed from the
touchpad.

- **Timeout to release**

Drag will be released after a timeout. You can set timeout value by
editing plist entry \"**FLiftDragReleaseTimeOut*\".***

- **Tap to release**

Drag will be released after a single tap. You can enable this from the
Trackpad prefpane by enabling option \"Drag Lock\".\
 

**[Customization:]{.underline}**

You can control dragging with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Dragging** in the
plist.

1.  **FingerPressDrag (Yes/No) :** Enables/Disables finger press drag.

2.  **FingerPressDragPressure (Value 0 - 255) :** Sets the pressure
    needed to detect the touch as finger press drag.

3.  **FingerPressDragTimeOut (Value 0 - 3000) :** Sets the time (in
    milli sec) needed to to start the finger press drag.

4.  **FLiftDragReleaseTimeOut (Value 0 - 3000) :** Sets the time (in
    milli sec) used to release the drag after removing finger from
    touchpad.

**4. SCROLLING**

 \
Need to enable the options **\"Use two finger to scroll\"** and
**\"Allow horizontal scrolling\"** in Trackpad prefpane for enabling
scrolling.

***Note:**** You have to change the Scroll speed value from the Trackpad
pref for the first time after installing the driver to work.*

- ***Two Finger scroll:*** 

- Swipe two fingers either horizontally for horizontal scroll (or)
  vertically for vertical scroll.

- ***Edge Scrolling:*** 

- Swipe one finger vertically on the right edge of the touchpad for
  vertical scrolling and horizontally on the bottom edge of the touchpad
  for horizontal scrolling.

**Scroll Acceleration modes: (v3.8 or later)**

There are four modes of acceleration for scrolling in my driver, which
can be set by editing the plist entry ***\"*2FScrollAccelMode*\"*** *for
2f scroll and \"***EdgeScrollAccelMode\"** for Edge scroll.\
They are:

1.  Optimized Acceleration (mode 0) : This is optimized mode which works
    best across all the touchpads same.

2.  Fixed acceleration (mode 1) : This mode has fixed acceleration/speed
    and varies only with scroll speed value.

3.  Extended acceleration (mode 2) : Its an extended version of
    optimized mode with more acceleration.

4.  Super acceleration (mode 3) : This mode reflects the acceleration of
    your fingers and works based on that. It doesn\'t depend on scroll
    speed values from Trackpad prefpane but you can use it extend the
    speed even more for high resolution touch pads.

5.  Super Extended acceleration (mode 4) : This is same as mode 3 but
    extends the acceleration.

**NOTE: Additional controls**

Hold **\"Left shift\"** Key for reversing the direction of the scroll\
Hold **\"Command + Control\"** keys to speed up the scroll 2x\
Hold **\"Command + Option\"** keys for Auto scroll (as alt function)
when both Inertia (primary function) and Auto scroll are enabled.

**SCROLL EFFECTS**

- **Auto scroll:**

**       ** Use the finger(s) to flick (doing a quick swipe and removing
the fingers from touchpad) for scrolling in the direction you want to
get  

         the auto scroll (speed depends on the flick strength)**.**

- **Continuous scroll:**

Scroll the direction you want and hold the finger(s) firmly to get the
continuous scroll in the direction you\'ve scrolled.

- **Inertial scroll:**

Use the finger(s) to flick (doing a quick swipe and removing the fingers
from touchpad) for scrolling in the direction you want to get inertial
effect (speed and motion depends on the flick strength).

- **Sling scroll:**

Do the vertical scroll and hold the fingers at the very top/bottom of
the touchpad between 1-2 secs then release, which will do scroll in
reverse direction quickly to scroll back.

- **Circular scroll with Edge scrolling:**

Start the gesture on the edge and do some scroll before moving the
finger to the center of the touchpad, now try to draw a circle around
the center of the touchpad which will do scrolling continuously as long
as you draw the circle in clockwise (or) anti-clockwise for either
horizontal or vertical scroll.\
 

**[Customization:]{.underline}**

You can control scrolling with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Scrolling** in the
plist.

1.  **2FScrollAccelMode (Value 0 - 5) :** Sets the acceleration mode to
    use for 2 fingers scroll.

2.  **Inertial2FingScroll (Yes/No) :** Enables/Disables Inertia for 2
    fingers scroll.

3.  **Continuous2FingScroll (Yes/No) :** Enables/Disables continuous for
    2 fingers scroll.

4.  **Auto2FingScroll (Yes/No) :** Enables/Disables auto for 2 fingers
    scroll.

5.  **Sling2FingScroll (Yes/No) :** Enables/Disables sling for edge
    scroll.

6.  **2FContScrollMaxDelta (Value 0 - 200) :** Sets the max delta
    allowed to trigger continuous scroll.

<!-- -->

1.  **EdgeScrolling (Yes/No) :** Enables/Disables edge scroll.

2.  **EdgeScrollAccelMode (Value 0 -5) :** Sets the acceleration mode to
    use for edge scroll.

3.  **InertialEdgeScroll (Yes/No) :** Enables/Disables Inertia for edge
    scroll.

4.  **ContinuousEdgeScroll (Yes/No) :** Enables/Disables continuous for
    edge scroll.

5.  **AutoEdgeScroll (Yes/No) :** Enables/Disables auto for edge scroll.

6.  **SlingEdgeScroll (Yes/No) :** Enables/Disables sling for edge
    scroll.

7.  **EContScrollMaxDelta (Value 0 - 200) :** Sets the max delta allowed
    to trigger continuous scroll.

8.  **EdgeVScrollArea (Value 0 - 100) :** Sets the % of touchpad area on
    the bottom allowed to detect vertical edge scroll.

9.  **EdgeVScrollAdaptArea (Value 0 - 100) :** Sets extra % of touchpad
    area on the right allowed to extend vertical edge scroll to use when
    its active.

10. **EdgeHScrollArea (Value 0 - 100) :** Sets the % of touchpad area on
    the bottom allowed to detect horizontal edge scroll.

11. **EdgeHScrollAdaptArea (Value 0 - 100) :** Sets extra % of touchpad
    area on the right allowed to extend horizontal edge scroll to use
    when its active.

12. **EScrolSpeedFactor (Value 0 - 1000) :** Sets the % of the speed
    multiplier for edge scroll.

13. **EdgeCircularScroll (Yes/No) :** Enables/Disables circular for edge
    scroll.

14. **EdgeCircularScrollDelta (Value 0 - 100) :** Sets the delta needed
    to trigger circular scroll.

<!-- -->

1.  **NoInertiaForHScroll (Yes/No) : **Disables inertia for horizontal
    scroll.

2.  **InertialScrollLevel (Value 0 - 10) :** Sets the level of inertia.
    Higher the more inertia.

3.  **ScrollDirectionSamples (Value 0 - 10) :** Sets the number of
    samples are used to analyze the direction of scroll to change.

4.  **ScrollLevelAtBoot (Value 0 - 9) :** Sets the scroll speed level to
    use at boot. This is meant for those who need scroll level greater
    than 6 at boot, because there is a bug in OSX prefpane which is not
    saving the speed level above 6. Value 0 disables this and will try
    to load prefpane value at boot.

5.  **ScrollLevelFactor (Value 0 - 10) :** Sets the scroll speed
    multiplier. This will increase the speed levels of prefpane by
    multiplying.

6.  **ScrollLevelGranularity (Value 0 - 100) :** Sets the granularity of
    the scroll speed to get precise scroll levels. This will provide
    more control over the scroll speed by multiplying against the value
    we get from the combination of ScrollLevelFactor and Prefpane scroll
    speed. Default value is 70% of the value we get
    from ScrollLevelFactor and Prefpane scroll speed.

7.  **ScrollStopDetectSamples (Value 0 - 10) :** Sets the number of
    samples used to detect scroll stop. This is useful in touchpad which
    generate noise data due to its high sensitivity during idle or slow
    scrolls.

8.  **ScrollMaxIdleStateDelta (Value 0 - 100) :** Sets the max delta
    allowed to detect idle state after scroll in touchpad which generate
    noise data due to high sensitivity.

9.  **ScrollMaxIdleDetectTimeout (Value 0 - 100) :** Sets the max time
    (in milli sec) used in detecting idle state after scroll.

10. **SlingTriggerArea (Value 0 - 100) :** Sets the % of touchpad area
    used at top and bottom to trigger sling scroll.

11. **SlingScrollStartTimeOut (Value 0 - 3000) :** Sets the idle time
    (time spent) needed at trigger area to trigger sling scroll.

**5. SWIPES**

 \
Performing swipe gesture triggers the functions configured to them. The
functions can be configured by editing plist file with the value from
the the available **\"Supported OS X action values to config
gestures\"** list above.\
 

**Three Fingers:**

- ***Swipe Left and Right:***

-  Performs backward and forward functions in Browser, Finder and
  others. (ISO keyboards need keyboard layout config in keyboard plist)

- ***Swipe Down:*** Performs Application switch

- ***Swipe Up:*** Opens Launchpad

**Four Fingers:**

- ***Swipe Left:** ***Hide/show Dock

- ***Swipe Right:*** Shows Properties/info for the selected file/folder

- ***Swipe Down :*** Minimize app

- ***Swipe Up:*** Close App

**Edges:**\
Its is performed using one (or) two finger(s) by swiping from the edge
of the touchpad to the middle (Swipe IN) 

(or) middle of the touchpad to the edge (Swipe OUT). And Swipe OUT works
only after swipe IN in reverse without finger lift.

- ***Left Edge Swipe:***

- Performs Left and Right Desktop/App Switch for IN and OUT swipes (Apps
  will switch only when they are in Full Screen).

<!-- -->

- ***Right Edge Swipe:***

- Opens (Swipe IN) and Closes (Swipe OUT) Notification centre.

<!-- -->

- ***Bottom Edge Swipe:***

- Default disabled.

<!-- -->

- ***Top Edge Swipe:***

- Toggles full screen.

**NOTE for NC (soft mode): ***From10.8 or later in Full screen and
10.7, NC (soft mode) works as Spotlight. So, [Hold]{.underline}*
***[Shift key]{.underline}*** *and do the gesture from 10.8 (or) later
to get NC working in Full screen.*\
 

**Assign Keyboard shortcut to Notification center function from 10.8 or
later instead of using Soft mode:**\
Assign key combination** (Command + Shift + N)** to Notification centre
shortcut at Keyboard Preferences as shown below in picture to get this
working using the function value 14.\
\
 

**[Customization:]{.underline}**

You can control swipes with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Swiping** in the
plist.

1.  **3FingerSwipeDownAction (Value 0 - 23) :** Sets the action
    triggered by the 3 fingers swipe down.

2.  **3FingerSwipeLeftAction (Value 0 - 23) :** Sets the action
    triggered by the 3 fingers swipe left.

3.  **3FingerSwipeRightAction (Value 0 - 23) :** Sets the action
    triggered by the 3 fingers swipe right.

4.  **3FingerSwipeUpAction (Value 0 - 23) :** Sets the action triggered
    by the 3 fingers swipe up.

5.  **4FingerSwipeDownAction (Value 0 - 23) :** Sets the action
    triggered by the 4 fingers swipe down.

6.  **4FingerSwipeLeftAction (Value 0 - 23) :** Sets the action
    triggered by the 4 fingers swipe down.

7.  **4FingerSwipeRightAction (Value 0 - 23) :** Sets the action
    triggered by the 4 fingers swipe down.

8.  **4FingerSwipeUpAction (Value 0 - 23) :** Sets the action triggered
    by the 4 fingers swipe down.

9.  **3FingerSwipes (Yes/No) :** Enables/Disables 3 fingers swipes.

10. **4FingerSwipes (Yes/No) :** Enables/Disables 4 fingers swipes.

11. **MultiFingerSwipesDeltaX (Value 0 - 500) :** Sets the X delta
    (movement) needed to detect 3F and 4F swipes.

12. **MultiFingerSwipesDeltaX (Value 0 - 500) :** Sets the Y delta
    (movement) needed to detect 3F and 4F swipes.

<!-- -->

1.  **EdgeSwipeBottom (Value 0 - 23) :** Sets the action triggered by
    the bottom edge swipe.

2.  **EdgeSwipeLeft (Value 0 - 23) :** Sets the action triggered by the
    left edge swipe.

3.  **EdgeSwipeRight (Value 0 - 23) :** Sets the action triggered by the
    right edge swipe

4.  **EdgeSwipeTop (Value 0 - 23) :** Sets the action triggered by the
    top edge swipe

5.  **EdgeSwipes (Yes/No) :** Enables/Disables edge swipes.

6.  **EdgeSwipes2Ways (Yes/No) :** Enables/Disables 2 ways edge swipes.
    It allows swipes to be triggered in both ways either we start from
    center to edge or from edge to center instead of starting only from
    edge to center.

7.  **EdgeSwipesResetPointer (Yes/No) :** Want to reset the pointer to
    center of the screen after edge swipe ?.

8.  **EdgeSwipeFingers (Value 0 - 2) :** Sets the number of fingers are
    used in edge swipes. 0 is for both 1f and 2f.

9.  **EdgeSwipeDetectHArea (Value 0 - 100) :** Sets the % of touchpad
    area on edges allowed to detect start of horizontal (left and right)
    edge swipes.

10. **EdgeSwipeDetectVArea (Value 0 - 100) :** Sets the % of touchpad
    area on edges allowed to detect start of vertical(top and bottom)
    edge swipes.

11. **EdgeSwipeDeltaX (Value 0 - 500) :** Sets the X delta (movement)
    needed to detect edge swipes.

12. **EdgeSwipeDeltaY (Value 0 - 500) :** Sets the Y delta
    (movement) needed to detect edge swipes.

**6. ZOOM**

- ***Two Fingers Pinch Zoom :***

Spreading and pinching two fingers triggers zoom in and out in single
step when fingers are removed from touchpad after the gesture.

- ***OS X Zooming using Keyboard Button and 2F Vertical swipe :***

Hold the key chosen from the Trackpad prefpane and swipe two fingers
vertically to zoom OS X. You can select the key in Trackpad prefpane
option at System preferences from the option  **\"Zoom while
holding\"**.

- **Four/Five fingers Pinch Gesture like Apple touchpads (Only for the
  touchpad with 5 fingers support) *:***

Opens launchpad.\
 

**[Customization:]{.underline}**

You can configure zoom with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Others** in the
plist.

1.  **PinchZoom (Yes/No) :** Enables/Disables zoom feature.

2.  **PinchZoomDelta (Value 0 - 500) :** Sets the delta needed to
    trigger zoom. 

3.  **PinchRotateMinSamples (Value 0 - 10) :** Sets the minimum number
    of samples needed to detect zoom. 

**7. ROTATION**

 \
Rotation works on photos to rotate them and in browser to reload page.
We can do rotate in three ways:\
 

- ***Arc Rotation :** ***

Place a finger on the touchpad in a fixed position either at Left (or)
Right corner and draw an arc at the corner opposite to the fixed finger
position using second finger. Left arc does left rotate and right arc
does right rotate.

- ***Swipe Rotation :***

Place a finger on the touchpad in a fixed position at middle of the
touchpad vertically and swipe the second finger from up to down for
rotating Right or from down to up for rotating left.\
Place a finger on the touchpad in a fixed position at middle of the
touchpad horizontally and swipe the second finger from left to right for
rotating Right or from right to left for rotating left.

- ***Circular Rotation :*** (Only for ELAN v3 hardware)

Place two fingers on the touchpad vertically and move them to right
horizontal position to rotate right.\
Place two fingers on the touchpad horizontally and move them to left
vertical position to rotate left.\
 

**[Customization:]{.underline}**

You can configure rotation with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Others** in the
plist.

1.  **Rotation (Yes/No) :** Enables/Disables rotate feature.

2.  **CircularRotate (Yes/No) :** Enables/Disables circular rotate
    feature.

3.  **RotationDelta (Value 0 - 500) :** Sets the delta (movement) needed
    from moving finger to trigger rotate. 

4.  **RotationMaxCFDelta (Value 0 - 500) :** Sets the max delta
    (movement) allowed for the fixed finger in detecting rotate. Some
    high sensitive touchpad might need higher values to work better.

5.  **PinchRotateMinSamples (Value 0 - 10) :** Sets the minimum number
    of samples needed to detect rotate. 

**NOTE: I did my best to get this working as much as possible and works
very well if you do the gesture like i explained. And y*ou have to keep
the first finger on the touchpad for 1-2 secs before rotating with
other** **finger which** **I\'ve used to avoid conflicts with other
features. Will try to improve this in future as far as possible.***\
\

8\. CLICKING AND FINGERS PRESS

To perform Fingers press, Tap and hold your finger(s) firmly with some
pressure on the touchpad for 2-3 seconds.\
To perform clicking, press the button under the touchpad in click-pad.\
 

- ***Single finger Click/Press:** **(Finger press drag)***

Does dragging

- ***Two Fingers Click/Press:*** 

Default it does zoom reset. It can configured to other actions by
editing plist with supported gesture actions value. 

- ***Three Fingers Click/Press:***

Opens Finder window (or) Changes Language if you have more than one
input source. You can change this to Finder/other from keyboard
shortcuts for the shortcut **\"Command + Space\"**. It can configured to
other actions by editing plist with supported gesture actions value. 

- ***Four Fingers Click/Press:***

Opens Force quit window by default. It can configured to other actions
by editing plist with supported gesture actions value. 

- ***Five Fingers Click/Press:***

Shows Desktop by default. It can configured to other actions by editing
plist with supported gesture actions value. \
 

**[Customization:]{.underline}**

You can configure taps with the following plist options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Tapping** in the
plist.

1.  **2FingersPressAction (Value 0 -23) :** Sets the action to be
    triggered on two fingers press. 

2.  **3FingersPressAction (Value 0 -23) :** Sets the action to be
    triggered on three fingers press. 

3.  **4FingersPressAction (Value 0 -23) :** Sets the action to be
    triggered on four fingers press. 

4.  **5FingersPressAction (Value 0 -23) :** Sets the action to be
    triggered on five fingers press. 

5.  **FingersPressTimeOut (Value 0 -3000) :** Sets the time needed
    (after timeout) to trigger finger press. 

**9. FEATURES TOGGLE**

I implemented this feature in order to enable/disable some of the
features on the fly using gestures, which cannot be enabled/disabled
from Trackpad prefpane.\
 \
To perform this toggle, you have to Hold \"Left Shift Key\" in Keyboard
and then \"Swipe with three fingers\" (or) \"tap 2/3/4 fingers quickly\"
(or) \"press and hold 2/3/4 fingers firmly\" to enable/disable the
features.\
 

**[Indicator:]{.underline}**

On performing this toggle, the pointer will move a bit
Up/Down/Left/Right which indicates the feature is toggled. *If the*
***pointer*** *****moved*** *****Up/Right direction*** *then its*
***turned on****, if* ***moved*** *****Left/Down direction*** *then its*
***turned off****.*\
 \
**Three Fingers swipe toggles:**

- ***Down :*** *Toggles* Edge Swiping.

- ***Up :** ***Toggles Corner Tapping.

- ***Left :*** Toggles Pinch zoom.

- ***Right :** ***Toggles Rotations.

 \
**Two Fingers Tapping:**

- ***Two Times :*** Toggles Edge scrolling.

- ***Three Times :** ***Toggles Edge circular scrolling.

**Three Fingers Tapping:**

- ***Two Times :*** Toggles scroll acceleration modes.

- ***Three Times :** ***Toggles pointer acceleration value from 0 - 20.

**Four Fingers Tapping:**

- ***Two Times :*** Toggles virtual buttons mode.

 \
**Two Fingers Press:**

- Toggles two finger press

**Three Fingers Press:**

- Toggles three finger press

**Four Fingers Press: **

- Toggles four finger press.

**0. ACCIDENTAL INPUT PREVENTION AND PALM DETECTION**

 \
**ACCIDENTAL INPUT:**\
You can prevent accidental inputs that happen from the touchpad when you
are typing by enabling the Option \"**Ignore accidental input\"** in
Trackpad prefpane at System preferences. \
 

**[Customization:]{.underline}**

You can configure accidental input with the following plist
options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Keyboard** in the
plist.

1.  **TriggerAccInputForKeyPressCount (Value 0 - 10) :** Sets the number
    of key presses needed to trigger accidental input. 

2.  **TimeOutEnablesTPadAfterAccInput (Value 0 - 5000) :** Sets the
    timeout for enabling the touchpad back when it was disabled for
    accidental input.

3.  **TapEnablesTPadFromAccInput (Yes/No) :** Allows to enables touchpad
    back when it was disabled by accidental input using a single finger
    tap gesture instead of waiting for timeout.

**PALM DETECTION:**\
You can also enable/disable Palm detection which can be customized for
the pressure and width of the palm by editing plist options from
\"**Others**\" section of the touchpad plist. Default its enabled and
configured to the best values found by the testers in beta testing. If
you have problem disable this and PM me for support.\
 

**[Customization:]{.underline}**\
You can configure palm detection with the following plist
options. Located
at **IOKitPersonalities-\>Smart-Pad-\>Preferences-\>Others** in the
plist.

1.  **PalmDetection (Yes/No) :** Enables/Disables palm detection.

2.  **PalmCheckMinPressure (Value 0 - 255) :** Sets the minimum pressure
    needed to identify the touch as palm. 

3.  **PalmCheckMinWidth (Value 0 - 15) :** Sets the minimum width needed
    to identify the touch as palm. 

**12. CUSTOMIZATION OF FEATURES**

 \
We can customize the touchpad features by the editing \"info.plist\"
file located inside the kext (ApplePS2SmpartTouchpad.kext/Contents/). In
the plist, go to the section
\"IOKitPersonalities-\>Smart-Pad-\>Preferences\" and edit the value of
the feature like you want.\
 

**NOTE: You need to install the kext again after editing the plist. **

Values for boolean controlled features: [Yes/No if you use Xcode/Plist
editor]{.underline} (or)

[true/false if you use Text editor]{.underline} for editing the plist.\
 

- **SleepInitDelay:** Sets the delay after sleep before enabling the
  touchpad. Its needed for some touchpads as they fail if we try to
  enable immediately after sleep because they might be not ready yet.
  Default value is 1500 ms.

**BUTTONS:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
#2
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)]{.underline}\
 \
**POINTER:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
#2
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32284)]{.underline}\
 \
**DRAGGING:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
#4
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32286)]{.underline}\
 \
**SCROLLING:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
#5
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32287)]{.underline}\
 \
**SWIPING:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
#6
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32288)]{.underline}\
 \
**TAPPING:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
#3
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32285)]{.underline}\
 \
**FINGERS PRESS:**\
Read here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
#9
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32291)]{.underline}\
 \
**MOUSE:**

- **Resolution (Value 0 - 3) :** Sets the mouse resolution for basic
  mouse mode (or useful in unsupported touchpad).

- **SampleRate (Value 10, 20, 40, 60, 80, 100, 200) :** Sets the mouse
  sample for basic mouse mode (or useful in unsupported touchpad).

- **Scaling (Value 0 - x) : **Sets the mouse scaling for basic mouse
  mode (or useful in unsupported touchpad).

 \
**OTHERS:**

- **DisableTPadOnLIDClose (Yes/No) :** Disable touchpad when laptop lid
  is closed ?

- **TPadStatusCheckPollRate (Value 0 - 60000):** Sets the poll rate (in
  milli sec) for checking the status of the touchpad for Fn key disable
  in Asus. 

- **MaxTouchEndPressure (Value 0 - 255) :** Sets the max pressure used
  to detect the end of the touch.

- **MaxTouchEndDelta (Value 0 - 200) : **Sets the max delta used to
  detect the end of the touch.

Read more here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
#7
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32289)]{.underline}\
Read more here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
#8
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/#entry32290)]{.underline}\
Read more here
: [[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
#11
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[
](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)[ ](http://forum.osxlatitude.com/index.php?/topic/5966-details-about-the-smart-touchpad-driver-features/page-2#entry32293)]{.underline}\
 \
**KEYBOARD:**

- **EnableAsusKBacklightSupport** - Enable Asus Keyboard backlight
  support? [Default:]{.underline} Disabled

- **AsusKBacklightHas16Levels** - Asus keyboard backlight has 16 levels
  support? [Default:]{.underline} Disabled

- ***EnableAsusKBacklightAutoOff -***  Enable Idle keyboard backlight
  on/off? [Default:]{.underline} Disabled

- ***AsusKBacklightAutoOffTimeOut -*** Idle Timeout value for keyboard
  backlight auto off. [Default:]{.underline} 10 sec (10000 ms)

**14. Support for Asus Keyboard backlight with Idle Auto on/off:**

My driver supports Asus keyboard backlight control using Fn keys mode in
keyboard and Idle Auto on/off which can be enabled from touchpad plist
keyboard section.\
 

**Following are the options that controls and enables Asus Keyboard
backlight:**\
Set the entry \"EnableAsusKBacklightSupport\" to Yes for enabling
keyboard backlight controls in F3 and F4 by default. \
Set the entry \"AsusKBacklightHas16Levels\" to Yes for enabling 16
keyboard backlight levels. \
Set the entry \"EnableAsusKBacklightAutoOff\" to Yes for enabling
keyboard backlight Auto on/off after idle timeout .\
Set the entry \"AsusKBacklightAutoOffTimeOut\" to your required timeout
value in milli seconds for enabling keyboard backlight Auto off after
idle timeout. Default timeout is 10 sec (10000ms).\
 

**You also need to use the following patches in DSDT:**\
 Add the following methods to your ATKD device section in DSDT (or) use
the attached patch files in DSDT editor.\
 

**NOTE: Some systems may need additional tweaking depending on their
DSDT.**\
**NOTE 2: Some notebooks might have \"*SBRG\"* instead of \"*LPCB\"* or
something different. So, try to check the method "SLKB" in your DSDT for
the the following line values and
modify "`^^PCI0.LPCB.EC0.WRAM (0xxxx, Local1)``”``.`**\
 

**4 Levels Keyboard backlight:**\

                Name (BOFF, Zero)
                Method (SKBL, 1, NotSerialized)
                {
                    If (Or (LEqual (Arg0, 0xED), LEqual (Arg0, 0xFD)))
                    {
                        If (And (LEqual (Arg0, 0xED), LEqual (BOFF, 0xEA)))
                        {
                            Store (Zero, Local0)
                            Store (Arg0, BOFF)
                        }
                        Else
                        {
                            If (And (LEqual (Arg0, 0xFD), LEqual (BOFF, 0xFA)))
                            {
                                Store (Zero, Local0)
                                Store (Arg0, BOFF)
                            }
                            Else
                            {
                                Return (BOFF)
                            }
                        }
                    }
                    Else
                    {
                        If (Or (LEqual (Arg0, 0xEA), LEqual (Arg0, 0xFA)))
                        {
                            Store (KBLV, Local0)
                            Store (Arg0, BOFF)
                        }
                        Else
                        {
                            Store (Arg0, Local0)
                            Store (Arg0, KBLV)
                        }
                    }
     
                    Store (DerefOf (Index (PWKB, Local0)), Local1)
                    ^^PCI0.LPCB.EC0.WRAM (0x04B1, Local1) // Haswell/Ivy
                    ^^PCI0.LPCB.EC0.WRAM (0x044B, Local1) // Sandy/Ivy
                    Return (Local0)
                }
     
                Method (GKBL, 1, NotSerialized)
                {
                    If (LEqual (Arg0, 0xFF))
                    {
                        Return (BOFF)
                    }
     
                    Return (KBLV)
                }

**16 Levels Keyboard backlight:**\

                Name (BOFF, Zero)
                Method (SKBL, 1, NotSerialized)
                {
                    If (Or (LEqual (Arg0, 0xED), LEqual (Arg0, 0xFD)))
                    {
                        If (And (LEqual (Arg0, 0xED), LEqual (BOFF, 0xEA)))
                        {
                            Store (Zero, Local0)
                            Store (Arg0, BOFF)
                        }
                        Else
                        {
                            If (And (LEqual (Arg0, 0xFD), LEqual (BOFF, 0xFA)))
                            {
                                Store (Zero, Local0)
                                Store (Arg0, BOFF)
                            }
                            Else
                            {
                                Return (BOFF)
                            }
                        }
                    }
                    Else
                    {
                        If (Or (LEqual (Arg0, 0xEA), LEqual (Arg0, 0xFA)))
                        {
                            Store (KBLV, Local0)
                            Store (Arg0, BOFF)
                        }
                        Else
                        {
                            Store (Arg0, Local0)
                            Store (Arg0, KBLV)
                        }
                    }
     
                    Store (DerefOf (Index (KBPW, Local0)), Local1)
                    ^^PCI0.LPCB.EC0.WRAM (0x04B1, Local1) // Haswell/Ivy
                    ^^PCI0.LPCB.EC0.WRAM (0x044B, Local1) // Sandy/Ivy
                    Return (Local0)
                }
     
                Name (KBPW, Buffer (0x10)
                {
                    /* 0000 */   0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                    /* 0008 */   0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
                })
                Method (GKBL, 1, NotSerialized)
                {
                    If (LEqual (Arg0, 0xFF))
                    {
                        Return (BOFF)
                    }
     
                    Return (KBLV)
                }
