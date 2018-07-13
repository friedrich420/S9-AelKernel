/* linux/drivers/video/decon_display/s6e3fa0_param.h
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __S6E3HA6_PARAM_H__
#define __S6E3HA6_PARAM_H__

#define S6E3HA6_ID_REG			0x04
#define S6E3HA6_RD_LEN			3
#define S6E3HA6_RDDPM_ADDR		0x0A
#define S6E3HA6_RDDSM_ADDR		0x0E

/* MCD */
static const unsigned char SEQ_TEST_KEY_ON_F0[] = {
	0xf0, 0x5a, 0x5a
};

static const unsigned char SEQ_TEST_KEY_ON_FC[] = {
	0xfc, 0x5a, 0x5a
};

static const unsigned char SEQ_DSC_EN[] = {
	0x01
};

static const unsigned char SEQ_PPS_SLICE2[] = {
	// QHD :2960x1440
	0x11, 0x00, 0x00, 0x89, 0x30, 0x80, 0x0B, 0x90,
	0x05, 0xA0, 0x00, 0x28, 0x02, 0xD0, 0x02, 0xD0,
	0x02, 0x00, 0x02, 0x68, 0x00, 0x20, 0x04, 0x6C,
	0x00, 0x0A, 0x00, 0x0C, 0x02, 0x77, 0x01, 0xE9,
	0x18, 0x00, 0x10, 0xF0, 0x03, 0x0C, 0x20, 0x00,
	0x06, 0x0B, 0x0B, 0x33, 0x0E, 0x1C, 0x2A, 0x38,
	0x46, 0x54, 0x62, 0x69, 0x70, 0x77, 0x79, 0x7B,
	0x7D, 0x7E, 0x01, 0x02, 0x01, 0x00, 0x09, 0x40,
	0x09, 0xBE, 0x19, 0xFC, 0x19, 0xFA, 0x19, 0xF8,
	0x1A, 0x38, 0x1A, 0x78, 0x1A, 0xB6, 0x2A, 0xF6,
	0x2B, 0x34, 0x2B, 0x74, 0x3B, 0x74, 0x6B, 0xF4,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_SLEEP_OUT[] = {
	0x11
};

static const unsigned char SEQ_TSP_HSYNC[] = {
	0xB9,
	0x00, 0x00, 0x14, 0x00, 0x18, 0x11, 0x03
};

static const unsigned char SEQ_SET_AREA[] = {
	0x1A, 0x1F, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char SEQ_TE_ON[] = {
	0x35
};

static const unsigned char SEQ_ERR_FG[] = {
	0xED, 0x44
};

static const unsigned char SEQ_TE_START_SETTING[] = {
	0xB9, 0x00, 0x0B, 0x8F, 0x00, 0x09
};

static const unsigned char SEQ_FFC[] = {
	0xCE,
	0x0D, 0x58, 0x14, 0x64, 0x38, 0xB8,	0xF2, 0x03,
	0x00, 0xFF, 0x02, 0x0A,	0x0A, 0x0A, 0x0A, 0x0F,
	0x23,
};

static const unsigned char SEQ_TEST_KEY_OFF_FC[] = {
	0xFC, 0xA5, 0xA5
};

static const unsigned char SEQ_TEST_KEY_OFF_F0[] = {
	0xF0, 0xA5, 0xA5
};

static const unsigned char SEQ_DISPLAY_ON[] = {
	0x29
};

static unsigned char SEQ_TSET_GLOBAL[] = {
	0xB0,
	0x01
};
#endif /* __S6E3HA6_PARAM_H__ */
