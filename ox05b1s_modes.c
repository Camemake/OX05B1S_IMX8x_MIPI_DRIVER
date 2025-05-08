
/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Placeholder register list for OX05B1S 2592×1944@60fps
 * Replace with vendor-provided sequence.
 */
#include "ox05b1s.h"


static const struct ox05b1s_regval ox05b1s_2592x1944_60fps_regs[] = {
    {0x0103, 0x01},
    {0x0100, 0x00},
    {0x0340, 0x08},
    {0x0341, 0x50},
    {0x0342, 0x01},
    {0x0343, 0x78},
    {0x0344, 0x00},
    {0x0345, 0x00},
    {0x0346, 0x00},
    {0x0347, 0x00},
    {0x0348, 0x0A},
    {0x0349, 0x20},
    {0x034A, 0x07},
    {0x034B, 0x98},
    {0x0100, 0x01},
};


const struct ox05b1s_mode ox05b1s_2592x1944_60fps_mode = {
    .width = 2592,
    .height = 1944,
    .hts = 376,
    .vts = 2128,
    .pclk = 350000000,
    .link_freq = 1050000000ULL,
    .list = ox05b1s_2592x1944_60fps_regs,
    .num_regs = ARRAY_SIZE(ox05b1s_2592x1944_60fps_regs),
};
