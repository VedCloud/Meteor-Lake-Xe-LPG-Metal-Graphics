//
//  Synaptics.h
//  ApplePS2Controller
//
//  Created by EMlyDinEsH on 2/24/15. Taken all of them from linux.
//  Copyright (c) 2015 Apple. All rights reserved.
//

#ifndef ApplePS2Controller_Synaptics_h
#define ApplePS2Controller_Synaptics_h

/* synaptics queries */
#define SYN_QUE_IDENTIFY		0x00
#define SYN_QUE_MODES			0x01
#define SYN_QUE_CAPABILITIES		0x02
#define SYN_QUE_MODEL			0x03
#define SYN_QUE_SERIAL_NUMBER_PREFIX	0x06
#define SYN_QUE_SERIAL_NUMBER_SUFFIX	0x07
#define SYN_QUE_RESOLUTION		0x08
#define SYN_QUE_EXT_CAPAB		0x09
#define SYN_QUE_FIRMWARE_ID		0x0a
#define SYN_QUE_EXT_CAPAB_0C		0x0c
#define SYN_QUE_EXT_MAX_COORDS		0x0d
#define SYN_QUE_EXT_MIN_COORDS		0x0f
#define SYN_QUE_MEXT_CAPAB_10       0x10

/* synatics modes */
#define SYN_BIT_ABSOLUTE_MODE		(1 << 7)
#define SYN_BIT_HIGH_RATE		(1 << 6)
#define SYN_BIT_SLEEP_MODE		(1 << 3)
#define SYN_BIT_DISABLE_GESTURE		(1 << 2)
#define SYN_BIT_FOUR_BYTE_CLIENT	(1 << 1)
#define SYN_BIT_W_MODE			(1 << 0)

/* synaptics model ID bits */
#define SYN_MODEL_ROT180(m)		((m) & (1 << 23))
#define SYN_MODEL_PORTRAIT(m)		((m) & (1 << 22))
#define SYN_MODEL_SENSOR(m)		(((m) >> 16) & 0x3f)
#define SYN_MODEL_HARDWARE(m)		(((m) >> 9) & 0x7f)
#define SYN_MODEL_NEWABS(m)		((m) & (1 << 7))
#define SYN_MODEL_PEN(m)		((m) & (1 << 6))
#define SYN_MODEL_SIMPLIC(m)		((m) & (1 << 5))
#define SYN_MODEL_GEOMETRY(m)		((m) & 0x0f)

/* synaptics capability bits */
#define SYN_CAP_EXTENDED(c)		((c) & (1 << 23))
#define SYN_CAP_MIDDLE_BUTTON(c)	((c) & (1 << 18))
#define SYN_CAP_PASS_THROUGH(c)		((c) & (1 << 7))
#define SYN_CAP_SLEEP(c)		((c) & (1 << 4))
#define SYN_CAP_FOUR_BUTTON(c)		((c) & (1 << 3))
#define SYN_CAP_MULTIFINGER(c)		((c) & (1 << 1))
#define SYN_CAP_PALMDETECT(c)		((c) & (1 << 0))
#define SYN_CAP_SUBMODEL_ID(c)		(((c) & 0x00ff00) >> 8)
#define SYN_EXT_CAP_REQUESTS(c)		(((c) & 0x700000) >> 20)
#define SYN_CAP_MULTI_BUTTON_NO(ec)	(((ec) & 0x00f000) >> 12)
#define SYN_CAP_PRODUCT_ID(ec)		(((ec) & 0xff0000) >> 16)

/*
 * The following describes response for the 0x0c query.
 *
 * byte	mask	name			meaning
 * ----	----	-------			------------
 * 1	0x01	adjustable threshold	capacitive button sensitivity
 *					can be adjusted
 * 1	0x02	report max		query 0x0d gives max coord reported
 * 1	0x04	clearpad		sensor is ClearPad product
 * 1	0x08	advanced gesture	not particularly meaningful
 * 1	0x10	clickpad bit 0		1-button ClickPad
 * 1	0x60	multifinger mode	identifies firmware finger counting
 *					(not reporting!) algorithm.
 *					Not particularly meaningful
 * 1	0x80	covered pad		W clipped to 14, 15 == pad mostly covered
 * 2	0x01	clickpad bit 1		2-button ClickPad
 * 2	0x02	deluxe LED controls	touchpad support LED commands
 *					ala multimedia control bar
 * 2	0x04	reduced filtering	firmware does less filtering on
 *					position data, driver should watch
 *					for noise.
 * 2	0x08	image sensor		image sensor tracks 5 fingers, but only
 *					reports 2.
 * 2	0x01	uniform clickpad	whole clickpad moves instead of being
 *					hinged at the top.
 * 2	0x20	report min		query 0x0f gives min coord reported
 */
#define SYN_CAP_CLICKPAD(ex0c)		((ex0c) & 0x100000) /* 1-button ClickPad */
#define SYN_CAP_CLICKPAD2BTN(ex0c)	((ex0c) & 0x000100) /* 2-button ClickPad */
#define SYN_CAP_MAX_DIMENSIONS(ex0c)	((ex0c) & 0x020000)
#define SYN_CAP_MIN_DIMENSIONS(ex0c)	((ex0c) & 0x002000)
#define SYN_CAP_ADV_GESTURE(ex0c)	((ex0c) & 0x080000)
#define SYN_CAP_REDUCED_FILTERING(ex0c)	((ex0c) & 0x000400)
#define SYN_CAP_IMAGE_SENSOR(ex0c)	((ex0c) & 0x000800)

/*
 * The following descibes response for the 0x10 query.
 *
 * byte mask    name                    meaning
 * ---- ----    -------                 ------------
 * 1    0x01    ext buttons are stick   buttons exported in the extended
 *                                      capability are actually meant to be used
 *                                      by the tracktick (pass-through).
 * 1    0x02    SecurePad               the touchpad is a SecurePad, so it
 *                                      contains a built-in fingerprint reader.
 * 1    0xe0    more ext count          how many more extented queries are
 *                                      available after this one.
 * 2    0xff    SecurePad width         the width of the SecurePad fingerprint
 *                                      reader.
 * 3    0xff    SecurePad height        the height of the SecurePad fingerprint
 *                                      reader.
 */
#define SYN_CAP_EXT_BUTTONS_STICK(ex10) ((ex10) & 0x010000)
#define SYN_CAP_SECUREPAD(ex10)         ((ex10) & 0x020000)

#define SYN_CAP_EXT_BUTTON_STICK_L(eb)  (!!((eb) & 0x01))
#define SYN_CAP_EXT_BUTTON_STICK_M(eb)  (!!((eb) & 0x02))
#define SYN_CAP_EXT_BUTTON_STICK_R(eb)  (!!((eb) & 0x04))


/* synaptics modes query bits */
#define SYN_MODE_ABSOLUTE(m)		((m) & (1 << 7))
#define SYN_MODE_RATE(m)		((m) & (1 << 6))
#define SYN_MODE_BAUD_SLEEP(m)		((m) & (1 << 3))
#define SYN_MODE_DISABLE_GESTURE(m)	((m) & (1 << 2))
#define SYN_MODE_PACKSIZE(m)		((m) & (1 << 1))
#define SYN_MODE_WMODE(m)		((m) & (1 << 0))

/* synaptics identify query bits */
#define SYN_ID_MODEL(i)			(((i) >> 4) & 0x0f)
#define SYN_ID_MAJOR(i)			((i) & 0x0f)
#define SYN_ID_MINOR(i)			(((i) >> 16) & 0xff)
#define SYN_ID_FULL(i)			((SYN_ID_MAJOR(i) << 8) | SYN_ID_MINOR(i))
#define SYN_ID_IS_SYNAPTICS(i)		((((i) >> 8) & 0xff) == 0x47)
#define SYN_ID_DISGEST_SUPPORTED(i)	(SYN_ID_MAJOR(i) >= 4)

/* synaptics special commands */
#define SYN_PS_SET_MODE2		0x14
#define SYN_PS_CLIENT_CMD		0x28

/* synaptics packet types */
#define SYN_NEWABS			0
#define SYN_NEWABS_STRICT		1
#define SYN_NEWABS_RELAXED		2
#define SYN_OLDABS			3

/* synaptics extended w mode packet types */
#define SYN_SCROLL_WHEEL		0
#define SYN_SECONDARY_FINGER    1 // Gesture packet: (x, y, z) half resolution
#define SYN_FINGER_STATE        2 // AGM-CONTACT packet with fingers count, id and sid for two fingers

/* amount to fuzz position data when touchpad reports reduced filtering */
#define SYN_REDUCED_FILTER_FUZZ		8

/*
 * The x/y limits are taken from the Synaptics TouchPad interfacing Guide,
 * section 2.3.2, which says that they should be valid regardless of the
 * actual size of the sensor.
 * Note that newer firmware allows querying device for maximum useable
 * coordinates.
 */
#define XMIN 0
#define XMAX 6143
#define YMIN 0
#define YMAX 6143
#define XMIN_NOMINAL 1472
#define XMAX_NOMINAL 5472
#define YMIN_NOMINAL 1408
#define YMAX_NOMINAL 4448

/* Size in bits of absolute position values reported by the hardware */
#define ABS_POS_BITS 13

/*
 * These values should represent the absolute maximum value that will
 * be reported for a positive position value. Some Synaptics firmware
 * uses this value to indicate a finger near the edge of the touchpad
 * whose precise position cannot be determined.
 *
 * At least one touchpad is known to report positions in excess of this
 * value which are actually negative values truncated to the 13-bit
 * reporting range. These values have never been observed to be lower
 * than 8184 (i.e. -8), so we treat all values greater than 8176 as
 * negative and any other value as positive.
 */
#define X_MAX_POSITIVE 8176
#define Y_MAX_POSITIVE 8176


/* This list has been kindly provided by Synaptics. */
static const char * const topbuttonpad_pnp_ids[] = {
    "LEN0017",
    "LEN0018",
    "LEN0019",
    "LEN0023",
    "LEN002A",
    "LEN002B",
    "LEN002C",
    "LEN002D",
    "LEN002E",
    "LEN0033", /* Helix */
    "LEN0034", /* T431s, L440, L540, T540, W540, X1 Carbon 2nd */
    "LEN0035", /* X240 */
    "LEN0036", /* T440 */
    "LEN0037",
    "LEN0038",
    "LEN0039", /* T440s */
    "LEN0041",
    "LEN0042", /* Yoga */
    "LEN0045",
    "LEN0046",
    "LEN0047",
    "LEN0048",
    "LEN0049",
    "LEN2000",
    "LEN2001", /* Edge E431 */
    "LEN2002", /* Edge E531 */
    "LEN2003",
    "LEN2004", /* L440 */
    "LEN2005",
    "LEN2006",
    "LEN2007",
    "LEN2008",
    "LEN2009",
    "LEN200A",
    "LEN200B",
    NULL
};

static const char * const buttons_pad_pnp_ids[] = {
    "SYN2B37", // YMax covers buttons area
    
    NULL
};

#define SYN_TOUCH_END_PRESSURE    15

/*
 * Debouce and acknowledge packet
 */

const UInt8 syn_newabs_mask[]	= { 0xC8, 0x00, 0x00, 0xC8, 0x00 };
const UInt8 syn_newabs_rel_mask[]	= { 0xC0, 0x00, 0x00, 0xC0, 0x00 };
const UInt8 syn_newabs_rslt[]	= { 0x80, 0x00, 0x00, 0xC0, 0x00 };
const UInt8 syn_oldabs_mask[]	= { 0xC0, 0x60, 0x00, 0xC0, 0x60 };
const UInt8 syn_oldabs_rslt[]	= { 0xC0, 0x00, 0x00, 0x80, 0x00 };

#endif
