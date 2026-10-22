//
//  ELAN.h
//  ApplePS2Controller
//
//  Created by EMlyDinEsH on 2/24/15.
//  Copyright (c) 2015 Apple. All rights reserved.
//

#ifndef ApplePS2Controller_ELAN_h
#define ApplePS2Controller_ELAN_h

/*
 * Taken from linux for Clickpad Detection:
 * For clickpads we are testing bit 12 (bit 20 used for Synaptics) in
 * fw_version for this is based on the following fw_version & caps table:
 *
 * Laptop-model:           fw_version:     caps:           buttons:
 * Acer S3                 0x461f00        10, 13, 0e      clickpad
 * Acer S7-392             0x581f01        50, 17, 0d      clickpad
 * Acer V5-131             0x461f02        01, 16, 0c      clickpad
 * Acer V5-551             0x461f00        ?               clickpad
 * Asus K53SV              0x450f01        78, 15, 0c      2 hw buttons
 * Asus G46VW              0x460f02        00, 18, 0c      2 hw buttons
 * Asus G750JX             0x360f00        00, 16, 0c      2 hw buttons
 * Asus UX31               0x361f00        20, 15, 0e      clickpad
 * Asus UX32VD             0x361f02        00, 15, 0e      clickpad
 * Avatar AVIU-145A2       0x361f00        ?               clickpad
 * Fujitsu H730            0x570f00        c0, 14, 0c      3 hw buttons (**)
 * Gigabyte U2442          0x450f01        58, 17, 0c      2 hw buttons
 * Lenovo L430             0x350f02        b9, 15, 0c      2 hw buttons (*)
 * Lenovo L530             0x350f02        b9, 15, 0c      2 hw buttons (*)
 * Lenovo Z51-70           0x594f03        70, 15, 0a      2 hw buttons
 * Samsung NF210           0x150b00        78, 14, 0a      2 hw buttons
 * Samsung NP770Z5E        0x575f01        10, 15, 0f      clickpad
 * Samsung NP700Z5B        0x361f06        21, 15, 0f      clickpad
 * Samsung NP900X3E-A02    0x575f03        ?               clickpad
 * Samsung NP-QX410        0x851b00        19, 14, 0c      clickpad
 * Samsung RC512           0x450f00        08, 15, 0c      2 hw buttons
 * Samsung RF710           0x450f00        ?               2 hw buttons
 * System76 Pangolin       0x250f01        ?               2 hw buttons
 * (*) + 3 trackpoint buttons
 * (**) + 0 trackpoint buttons
 * Note: Lenovo L430 and Lenovo L430 have the same fw_version/caps
 */

#define ELAN_CAP_CLICKPAD(ex0c)		((ex0c) & 0x1000) /* 1-button ClickPad */

/*
 * Command values for Synaptics style queries
 */
#define ETD_FW_ID_QUERY             0x00
#define ETD_FW_VERSION_QUERY		0x01
#define ETD_CAPABILITIES_QUERY		0x02
#define ETD_SAMPLE_QUERY            0x03
#define ETD_RESOLUTION_QUERY		0x04

/*
 * Command values for register reading or writing
 */
#define ETD_REGISTER_READ           0x10
#define ETD_REGISTER_WRITE          0x11
#define ETD_REGISTER_READWRITE		0x00

/*
 * Hardware version 2 custom PS/2 command value
 */
#define ETD_PS2_CUSTOM_COMMAND		0xf8

/*
 * Times to retry a ps2_command and millisecond delay between tries
 */
#define ETD_PS2_COMMAND_TRIES		3
#define ETD_PS2_COMMAND_DELAY		500

/*
 * Times to try to read back a register and millisecond delay between tries
 */
#define ETD_READ_BACK_TRIES		5
#define ETD_READ_BACK_DELAY		2000

/*
 * Register bitmasks for hardware version 1
 */
#define ETD_R10_ABSOLUTE_MODE		0x04
#define ETD_R11_4_BYTE_MODE         0x02

/*
 * Capability bitmasks
 */
#define ETD_CAP_HAS_ROCKER		0x04

/*
 * One hard to find application note states that X axis range is 0 to 576
 * and Y axis range is 0 to 384 for harware version 1.
 * Edge fuzz might be necessary because of bezel around the touchpad
 */
#define ETD_EDGE_FUZZ_V1		32

#define ETD_XMIN_V1			(  0 + ETD_EDGE_FUZZ_V1)
#define ETD_XMAX_V1			(576 - ETD_EDGE_FUZZ_V1)
#define ETD_YMIN_V1			(  0 + ETD_EDGE_FUZZ_V1)
#define ETD_YMAX_V1			(384 - ETD_EDGE_FUZZ_V1)

/*
 * The resolution for older v2 hardware doubled.
 * (newer v2's firmware provides command so we can query)
 */
#define ETD_XMIN_V2			0
#define ETD_XMAX_V2			1152
#define ETD_YMIN_V2			0
#define ETD_YMAX_V2			768

#define ETD_PMIN_V2			0
#define ETD_PMAX_V2			255
#define ETD_WMIN_V2			0
#define ETD_WMAX_V2			15

/*
 * Packet types
 */
#define PACKET_RESET			0xFF
#define PACKET_PASS_THRU		0x09

#define PACKET_UNKNOWN			0x06
#define PACKET_DEBOUNCE			0x07
#define PACKET_TRACKPOINT       0x08

// ELAN v3 hardware
#define PACKET_HEAD             0x03
#define PACKET_TAIL             0x04
#define PACKET_END              0x05

// ELAN v4 hardware
#define PACKET_STATUS           0x00
#define PACKET_ABSOLUTE         0x01
#define PACKET_RELATIVE         0x02

/*
 * weight value for v4 hardware
 */
#define ETD_WEIGHT_VALUE		5

/*
 * Debouce and acknowledge packet
 */
const UInt8 elan_debounce_packet_v2[]    = { 0x84, 0xff, 0xff, 0x02, 0xff, 0xff };
const UInt8 elan_debounce_packet_v3[]    = { 0xc4, 0xff, 0xff, 0x02, 0xff, 0xff };
const UInt8 elan_debounce_packet_v3_2[]  = { 0xcc, 0xff, 0xff, 0x0a, 0xff, 0xff };
const UInt8 elan_end_packet_v3[]         = { 0x04, 0x00, 0x00, 0x02, 0x00, 0x00 };


#endif
