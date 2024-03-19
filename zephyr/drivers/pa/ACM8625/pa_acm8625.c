/*
 * PA Module ACM8625
*/

#include <zephyr.h>
#include <board.h>
#include <drivers/i2c.h>
#include <device.h>
#include <stdlib.h>
#include <os_common_api.h>
#include <board_cfg.h>

#include <drivers/pa.h>
#include "pa_acm8625.h"

#define ACM8625S_I2C_ADDR_0     (0x58)
#define ACM8625S_I2C_ADDR_1     (0x5A)
#define ACM8625S_I2C_ADDR_2     (0x5C)
#define ACM8625S_I2C_ADDR_3     (0x5E)

#define ACM8625S_I2C_ADDR       (ACM8625S_I2C_ADDR_0 >> 1)

#define _countof(X)             (sizeof(X) / sizeof(X[0]))

#define ACM_REG_STATE_CTL       (0x04)
#define STATE_CTL_HIZ_L         (0x01 << 5)
#define STATE_CTL_HIZ_R         (0x01 << 4)
#define STATE_CTL_MUTE_L        (0x01 << 3)
#define STATE_CTL_MUTE_R        (0x01 << 2)

#define ACM_REG_I2S_DATA_FMT_1  (0x07)
#define I2S_DATA_44K_INPUT      (0x80)
#define I2S_DATA_44K_EN         (0x40)
#define I2S_DATA_FMT_TYPE       (0x30)
#define I2S_DATA_FMT_WIDTH      (0x03)

#define ACM_REG_GAIN_CH_L       (0x0F)
#define ACM_REG_GAIN_CH_R       (0x10)

#define ACM_DSP_REG_GAIN_CH_L   (0x7C)
#define ACM_DSP_REG_GAIN_CH_R   (0x80)

#define ACM_REG_PAGE_SEL        (0x00)

typedef struct 
{
    uint8_t reg;
    uint8_t val;
} reg_item_t;

const reg_item_t PA_ACM_INIT_TBL[] =
{
#if 0
    //initialization script
    { 0x00, 0x00 },
    { 0x04, 0x00 },
    { 0xfc, 0x86 },
    { 0xfd, 0x25 },
    { 0xfe, 0x15 },
    { 0x00, 0x01 },
    { 0x02, 0x20 },

    //Pre Volume
    { 0x00, 0x04 },
    { 0x88, 0x00 },
    { 0x89, 0xe2 },
    { 0x8a, 0xc4 },
    { 0x8b, 0x6b },
    { 0x80, 0x00 },
    { 0x81, 0x80 },
    { 0x82, 0x00 },
    { 0x83, 0x00 },
    { 0x7c, 0x00 },
    { 0x7d, 0x80 },
    { 0x7e, 0x00 },
    { 0x7f, 0x00 },
    //ClassD Control Registers
    { 0x00, 0x00 },
    { 0x11, 0x03 },
    { 0x02, 0x00 },
    { 0x06, 0x30 },
    { 0x28, 0x00 },
    { 0x05, 0x8e },
    { 0x03, 0x05 },
    { 0x01, 0x84 },
    { 0x00, 0x00 },
    { 0x04, 0x02 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x04, 0x03 },
#else
    //initialization script
    { 0x00, 0x00 },
    { 0x04, 0x00 },
    { 0xfc, 0x86 },
    { 0xfd, 0x25 },
    { 0xfe, 0x15 },
    { 0x00, 0x01 },
    { 0x02, 0x20 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
#endif
};

#ifdef CONFIG_ACM_AUDIO_DSP_EQ
const reg_item_t PA_ACM_EQ_TBL[] =
{
    //Digital Off during DSP configuration
    { 0x00, 0x00 },
    { 0x04, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    //EQ
    //EQ1 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x00, 0x06 },
    { 0xb0, 0x08 },
    { 0xb1, 0x00 },
    { 0xb2, 0x00 },
    { 0xb3, 0x00 },
    { 0xb4, 0x00 },
    { 0xb5, 0x00 },
    { 0xb6, 0x00 },
    { 0xb7, 0x00 },
    { 0xb8, 0x00 },
    { 0xb9, 0x00 },
    { 0xba, 0x00 },
    { 0xbb, 0x00 },
    { 0xbc, 0x00 },
    { 0xbd, 0x00 },
    { 0xbe, 0x00 },
    { 0xbf, 0x00 },
    { 0xc0, 0x00 },
    { 0xc1, 0x00 },
    { 0xc2, 0x00 },
    { 0xc3, 0x00 },
    //EQ2 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xc4, 0x08 },
    { 0xc5, 0x00 },
    { 0xc6, 0x00 },
    { 0xc7, 0x00 },
    { 0xc8, 0x00 },
    { 0xc9, 0x00 },
    { 0xca, 0x00 },
    { 0xcb, 0x00 },
    { 0xcc, 0x00 },
    { 0xcd, 0x00 },
    { 0xce, 0x00 },
    { 0xcf, 0x00 },
    { 0xd0, 0x00 },
    { 0xd1, 0x00 },
    { 0xd2, 0x00 },
    { 0xd3, 0x00 },
    { 0xd4, 0x00 },
    { 0xd5, 0x00 },
    { 0xd6, 0x00 },
    { 0xd7, 0x00 },
    //EQ3 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xd8, 0x08 },
    { 0xd9, 0x00 },
    { 0xda, 0x00 },
    { 0xdb, 0x00 },
    { 0xdc, 0x00 },
    { 0xdd, 0x00 },
    { 0xde, 0x00 },
    { 0xdf, 0x00 },
    { 0xe0, 0x00 },
    { 0xe1, 0x00 },
    { 0xe2, 0x00 },
    { 0xe3, 0x00 },
    { 0xe4, 0x00 },
    { 0xe5, 0x00 },
    { 0xe6, 0x00 },
    { 0xe7, 0x00 },
    { 0xe8, 0x00 },
    { 0xe9, 0x00 },
    { 0xea, 0x00 },
    { 0xeb, 0x00 },
    //EQ4 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xec, 0x08 },
    { 0xed, 0x00 },
    { 0xee, 0x00 },
    { 0xef, 0x00 },
    { 0xf0, 0x00 },
    { 0xf1, 0x00 },
    { 0xf2, 0x00 },
    { 0xf3, 0x00 },
    { 0xf4, 0x00 },
    { 0xf5, 0x00 },
    { 0xf6, 0x00 },
    { 0xf7, 0x00 },
    { 0xf8, 0x00 },
    { 0xf9, 0x00 },
    { 0xfa, 0x00 },
    { 0xfb, 0x00 },
    { 0xfc, 0x00 },
    { 0xfd, 0x00 },
    { 0xfe, 0x00 },
    { 0xff, 0x00 },
    //EQ5 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x00, 0x07 },
    { 0x04, 0x08 },
    { 0x05, 0x00 },
    { 0x06, 0x00 },
    { 0x07, 0x00 },
    { 0x08, 0x00 },
    { 0x09, 0x00 },
    { 0x0a, 0x00 },
    { 0x0b, 0x00 },
    { 0x0c, 0x00 },
    { 0x0d, 0x00 },
    { 0x0e, 0x00 },
    { 0x0f, 0x00 },
    { 0x10, 0x00 },
    { 0x11, 0x00 },
    { 0x12, 0x00 },
    { 0x13, 0x00 },
    { 0x14, 0x00 },
    { 0x15, 0x00 },
    { 0x16, 0x00 },
    { 0x17, 0x00 },
    //EQ6 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x18, 0x08 },
    { 0x19, 0x00 },
    { 0x1a, 0x00 },
    { 0x1b, 0x00 },
    { 0x1c, 0x00 },
    { 0x1d, 0x00 },
    { 0x1e, 0x00 },
    { 0x1f, 0x00 },
    { 0x20, 0x00 },
    { 0x21, 0x00 },
    { 0x22, 0x00 },
    { 0x23, 0x00 },
    { 0x24, 0x00 },
    { 0x25, 0x00 },
    { 0x26, 0x00 },
    { 0x27, 0x00 },
    { 0x28, 0x00 },
    { 0x29, 0x00 },
    { 0x2a, 0x00 },
    { 0x2b, 0x00 },
    //EQ7 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x2c, 0x08 },
    { 0x2d, 0x00 },
    { 0x2e, 0x00 },
    { 0x2f, 0x00 },
    { 0x30, 0x00 },
    { 0x31, 0x00 },
    { 0x32, 0x00 },
    { 0x33, 0x00 },
    { 0x34, 0x00 },
    { 0x35, 0x00 },
    { 0x36, 0x00 },
    { 0x37, 0x00 },
    { 0x38, 0x00 },
    { 0x39, 0x00 },
    { 0x3a, 0x00 },
    { 0x3b, 0x00 },
    { 0x3c, 0x00 },
    { 0x3d, 0x00 },
    { 0x3e, 0x00 },
    { 0x3f, 0x00 },
    //EQ8 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x40, 0x08 },
    { 0x41, 0x00 },
    { 0x42, 0x00 },
    { 0x43, 0x00 },
    { 0x44, 0x00 },
    { 0x45, 0x00 },
    { 0x46, 0x00 },
    { 0x47, 0x00 },
    { 0x48, 0x00 },
    { 0x49, 0x00 },
    { 0x4a, 0x00 },
    { 0x4b, 0x00 },
    { 0x4c, 0x00 },
    { 0x4d, 0x00 },
    { 0x4e, 0x00 },
    { 0x4f, 0x00 },
    { 0x50, 0x00 },
    { 0x51, 0x00 },
    { 0x52, 0x00 },
    { 0x53, 0x00 },
    //EQ9 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x54, 0x08 },
    { 0x55, 0x00 },
    { 0x56, 0x00 },
    { 0x57, 0x00 },
    { 0x58, 0x00 },
    { 0x59, 0x00 },
    { 0x5a, 0x00 },
    { 0x5b, 0x00 },
    { 0x5c, 0x00 },
    { 0x5d, 0x00 },
    { 0x5e, 0x00 },
    { 0x5f, 0x00 },
    { 0x60, 0x00 },
    { 0x61, 0x00 },
    { 0x62, 0x00 },
    { 0x63, 0x00 },
    { 0x64, 0x00 },
    { 0x65, 0x00 },
    { 0x66, 0x00 },
    { 0x67, 0x00 },
    //EQ10 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x68, 0x08 },
    { 0x69, 0x00 },
    { 0x6a, 0x00 },
    { 0x6b, 0x00 },
    { 0x6c, 0x00 },
    { 0x6d, 0x00 },
    { 0x6e, 0x00 },
    { 0x6f, 0x00 },
    { 0x70, 0x00 },
    { 0x71, 0x00 },
    { 0x72, 0x00 },
    { 0x73, 0x00 },
    { 0x74, 0x00 },
    { 0x75, 0x00 },
    { 0x76, 0x00 },
    { 0x77, 0x00 },
    { 0x78, 0x00 },
    { 0x79, 0x00 },
    { 0x7a, 0x00 },
    { 0x7b, 0x00 },
    //EQ11 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x7c, 0x08 },
    { 0x7d, 0x00 },
    { 0x7e, 0x00 },
    { 0x7f, 0x00 },
    { 0x80, 0x00 },
    { 0x81, 0x00 },
    { 0x82, 0x00 },
    { 0x83, 0x00 },
    { 0x84, 0x00 },
    { 0x85, 0x00 },
    { 0x86, 0x00 },
    { 0x87, 0x00 },
    { 0x88, 0x00 },
    { 0x89, 0x00 },
    { 0x8a, 0x00 },
    { 0x8b, 0x00 },
    { 0x8c, 0x00 },
    { 0x8d, 0x00 },
    { 0x8e, 0x00 },
    { 0x8f, 0x00 },
    //EQ12 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x90, 0x08 },
    { 0x91, 0x00 },
    { 0x92, 0x00 },
    { 0x93, 0x00 },
    { 0x94, 0x00 },
    { 0x95, 0x00 },
    { 0x96, 0x00 },
    { 0x97, 0x00 },
    { 0x98, 0x00 },
    { 0x99, 0x00 },
    { 0x9a, 0x00 },
    { 0x9b, 0x00 },
    { 0x9c, 0x00 },
    { 0x9d, 0x00 },
    { 0x9e, 0x00 },
    { 0x9f, 0x00 },
    { 0xa0, 0x00 },
    { 0xa1, 0x00 },
    { 0xa2, 0x00 },
    { 0xa3, 0x00 },
    //EQ13 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xa4, 0x08 },
    { 0xa5, 0x00 },
    { 0xa6, 0x00 },
    { 0xa7, 0x00 },
    { 0xa8, 0x00 },
    { 0xa9, 0x00 },
    { 0xaa, 0x00 },
    { 0xab, 0x00 },
    { 0xac, 0x00 },
    { 0xad, 0x00 },
    { 0xae, 0x00 },
    { 0xaf, 0x00 },
    { 0xb0, 0x00 },
    { 0xb1, 0x00 },
    { 0xb2, 0x00 },
    { 0xb3, 0x00 },
    { 0xb4, 0x00 },
    { 0xb5, 0x00 },
    { 0xb6, 0x00 },
    { 0xb7, 0x00 },
    //EQ14 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xb8, 0x08 },
    { 0xb9, 0x00 },
    { 0xba, 0x00 },
    { 0xbb, 0x00 },
    { 0xbc, 0x00 },
    { 0xbd, 0x00 },
    { 0xbe, 0x00 },
    { 0xbf, 0x00 },
    { 0xc0, 0x00 },
    { 0xc1, 0x00 },
    { 0xc2, 0x00 },
    { 0xc3, 0x00 },
    { 0xc4, 0x00 },
    { 0xc5, 0x00 },
    { 0xc6, 0x00 },
    { 0xc7, 0x00 },
    { 0xc8, 0x00 },
    { 0xc9, 0x00 },
    { 0xca, 0x00 },
    { 0xcb, 0x00 },
    //EQ15 - left; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xcc, 0x08 },
    { 0xcd, 0x00 },
    { 0xce, 0x00 },
    { 0xcf, 0x00 },
    { 0xd0, 0x00 },
    { 0xd1, 0x00 },
    { 0xd2, 0x00 },
    { 0xd3, 0x00 },
    { 0xd4, 0x00 },
    { 0xd5, 0x00 },
    { 0xd6, 0x00 },
    { 0xd7, 0x00 },
    { 0xd8, 0x00 },
    { 0xd9, 0x00 },
    { 0xda, 0x00 },
    { 0xdb, 0x00 },
    { 0xdc, 0x00 },
    { 0xdd, 0x00 },
    { 0xde, 0x00 },
    { 0xdf, 0x00 },
    //EQ1 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xe0, 0x08 },
    { 0xe1, 0x00 },
    { 0xe2, 0x00 },
    { 0xe3, 0x00 },
    { 0xe4, 0x00 },
    { 0xe5, 0x00 },
    { 0xe6, 0x00 },
    { 0xe7, 0x00 },
    { 0xe8, 0x00 },
    { 0xe9, 0x00 },
    { 0xea, 0x00 },
    { 0xeb, 0x00 },
    { 0xec, 0x00 },
    { 0xed, 0x00 },
    { 0xee, 0x00 },
    { 0xef, 0x00 },
    { 0xf0, 0x00 },
    { 0xf1, 0x00 },
    { 0xf2, 0x00 },
    { 0xf3, 0x00 },
    //EQ2 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xf4, 0x08 },
    { 0xf5, 0x00 },
    { 0xf6, 0x00 },
    { 0xf7, 0x00 },
    { 0xf8, 0x00 },
    { 0xf9, 0x00 },
    { 0xfa, 0x00 },
    { 0xfb, 0x00 },
    { 0xfc, 0x00 },
    { 0xfd, 0x00 },
    { 0xfe, 0x00 },
    { 0xff, 0x00 },
    { 0x00, 0x08 },
    { 0x04, 0x00 },
    { 0x05, 0x00 },
    { 0x06, 0x00 },
    { 0x07, 0x00 },
    { 0x08, 0x00 },
    { 0x09, 0x00 },
    { 0x0a, 0x00 },
    { 0x0b, 0x00 },
    //EQ3 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x0c, 0x08 },
    { 0x0d, 0x00 },
    { 0x0e, 0x00 },
    { 0x0f, 0x00 },
    { 0x10, 0x00 },
    { 0x11, 0x00 },
    { 0x12, 0x00 },
    { 0x13, 0x00 },
    { 0x14, 0x00 },
    { 0x15, 0x00 },
    { 0x16, 0x00 },
    { 0x17, 0x00 },
    { 0x18, 0x00 },
    { 0x19, 0x00 },
    { 0x1a, 0x00 },
    { 0x1b, 0x00 },
    { 0x1c, 0x00 },
    { 0x1d, 0x00 },
    { 0x1e, 0x00 },
    { 0x1f, 0x00 },
    //EQ4 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x20, 0x08 },
    { 0x21, 0x00 },
    { 0x22, 0x00 },
    { 0x23, 0x00 },
    { 0x24, 0x00 },
    { 0x25, 0x00 },
    { 0x26, 0x00 },
    { 0x27, 0x00 },
    { 0x28, 0x00 },
    { 0x29, 0x00 },
    { 0x2a, 0x00 },
    { 0x2b, 0x00 },
    { 0x2c, 0x00 },
    { 0x2d, 0x00 },
    { 0x2e, 0x00 },
    { 0x2f, 0x00 },
    { 0x30, 0x00 },
    { 0x31, 0x00 },
    { 0x32, 0x00 },
    { 0x33, 0x00 },
    //EQ5 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x34, 0x08 },
    { 0x35, 0x00 },
    { 0x36, 0x00 },
    { 0x37, 0x00 },
    { 0x38, 0x00 },
    { 0x39, 0x00 },
    { 0x3a, 0x00 },
    { 0x3b, 0x00 },
    { 0x3c, 0x00 },
    { 0x3d, 0x00 },
    { 0x3e, 0x00 },
    { 0x3f, 0x00 },
    { 0x40, 0x00 },
    { 0x41, 0x00 },
    { 0x42, 0x00 },
    { 0x43, 0x00 },
    { 0x44, 0x00 },
    { 0x45, 0x00 },
    { 0x46, 0x00 },
    { 0x47, 0x00 },
    //EQ6 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x48, 0x08 },
    { 0x49, 0x00 },
    { 0x4a, 0x00 },
    { 0x4b, 0x00 },
    { 0x4c, 0x00 },
    { 0x4d, 0x00 },
    { 0x4e, 0x00 },
    { 0x4f, 0x00 },
    { 0x50, 0x00 },
    { 0x51, 0x00 },
    { 0x52, 0x00 },
    { 0x53, 0x00 },
    { 0x54, 0x00 },
    { 0x55, 0x00 },
    { 0x56, 0x00 },
    { 0x57, 0x00 },
    { 0x58, 0x00 },
    { 0x59, 0x00 },
    { 0x5a, 0x00 },
    { 0x5b, 0x00 },
    //EQ7 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x5c, 0x08 },
    { 0x5d, 0x00 },
    { 0x5e, 0x00 },
    { 0x5f, 0x00 },
    { 0x60, 0x00 },
    { 0x61, 0x00 },
    { 0x62, 0x00 },
    { 0x63, 0x00 },
    { 0x64, 0x00 },
    { 0x65, 0x00 },
    { 0x66, 0x00 },
    { 0x67, 0x00 },
    { 0x68, 0x00 },
    { 0x69, 0x00 },
    { 0x6a, 0x00 },
    { 0x6b, 0x00 },
    { 0x6c, 0x00 },
    { 0x6d, 0x00 },
    { 0x6e, 0x00 },
    { 0x6f, 0x00 },
    //EQ8 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x70, 0x08 },
    { 0x71, 0x00 },
    { 0x72, 0x00 },
    { 0x73, 0x00 },
    { 0x74, 0x00 },
    { 0x75, 0x00 },
    { 0x76, 0x00 },
    { 0x77, 0x00 },
    { 0x78, 0x00 },
    { 0x79, 0x00 },
    { 0x7a, 0x00 },
    { 0x7b, 0x00 },
    { 0x7c, 0x00 },
    { 0x7d, 0x00 },
    { 0x7e, 0x00 },
    { 0x7f, 0x00 },
    { 0x80, 0x00 },
    { 0x81, 0x00 },
    { 0x82, 0x00 },
    { 0x83, 0x00 },
    //EQ9 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x84, 0x08 },
    { 0x85, 0x00 },
    { 0x86, 0x00 },
    { 0x87, 0x00 },
    { 0x88, 0x00 },
    { 0x89, 0x00 },
    { 0x8a, 0x00 },
    { 0x8b, 0x00 },
    { 0x8c, 0x00 },
    { 0x8d, 0x00 },
    { 0x8e, 0x00 },
    { 0x8f, 0x00 },
    { 0x90, 0x00 },
    { 0x91, 0x00 },
    { 0x92, 0x00 },
    { 0x93, 0x00 },
    { 0x94, 0x00 },
    { 0x95, 0x00 },
    { 0x96, 0x00 },
    { 0x97, 0x00 },
    //EQ10 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0x98, 0x08 },
    { 0x99, 0x00 },
    { 0x9a, 0x00 },
    { 0x9b, 0x00 },
    { 0x9c, 0x00 },
    { 0x9d, 0x00 },
    { 0x9e, 0x00 },
    { 0x9f, 0x00 },
    { 0xa0, 0x00 },
    { 0xa1, 0x00 },
    { 0xa2, 0x00 },
    { 0xa3, 0x00 },
    { 0xa4, 0x00 },
    { 0xa5, 0x00 },
    { 0xa6, 0x00 },
    { 0xa7, 0x00 },
    { 0xa8, 0x00 },
    { 0xa9, 0x00 },
    { 0xaa, 0x00 },
    { 0xab, 0x00 },
    //EQ11 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xac, 0x08 },
    { 0xad, 0x00 },
    { 0xae, 0x00 },
    { 0xaf, 0x00 },
    { 0xb0, 0x00 },
    { 0xb1, 0x00 },
    { 0xb2, 0x00 },
    { 0xb3, 0x00 },
    { 0xb4, 0x00 },
    { 0xb5, 0x00 },
    { 0xb6, 0x00 },
    { 0xb7, 0x00 },
    { 0xb8, 0x00 },
    { 0xb9, 0x00 },
    { 0xba, 0x00 },
    { 0xbb, 0x00 },
    { 0xbc, 0x00 },
    { 0xbd, 0x00 },
    { 0xbe, 0x00 },
    { 0xbf, 0x00 },
    //EQ12 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xc0, 0x08 },
    { 0xc1, 0x00 },
    { 0xc2, 0x00 },
    { 0xc3, 0x00 },
    { 0xc4, 0x00 },
    { 0xc5, 0x00 },
    { 0xc6, 0x00 },
    { 0xc7, 0x00 },
    { 0xc8, 0x00 },
    { 0xc9, 0x00 },
    { 0xca, 0x00 },
    { 0xcb, 0x00 },
    { 0xcc, 0x00 },
    { 0xcd, 0x00 },
    { 0xce, 0x00 },
    { 0xcf, 0x00 },
    { 0xd0, 0x00 },
    { 0xd1, 0x00 },
    { 0xd2, 0x00 },
    { 0xd3, 0x00 },
    //EQ13 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xd4, 0x08 },
    { 0xd5, 0x00 },
    { 0xd6, 0x00 },
    { 0xd7, 0x00 },
    { 0xd8, 0x00 },
    { 0xd9, 0x00 },
    { 0xda, 0x00 },
    { 0xdb, 0x00 },
    { 0xdc, 0x00 },
    { 0xdd, 0x00 },
    { 0xde, 0x00 },
    { 0xdf, 0x00 },
    { 0xe0, 0x00 },
    { 0xe1, 0x00 },
    { 0xe2, 0x00 },
    { 0xe3, 0x00 },
    { 0xe4, 0x00 },
    { 0xe5, 0x00 },
    { 0xe6, 0x00 },
    { 0xe7, 0x00 },
    //EQ14 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xe8, 0x08 },
    { 0xe9, 0x00 },
    { 0xea, 0x00 },
    { 0xeb, 0x00 },
    { 0xec, 0x00 },
    { 0xed, 0x00 },
    { 0xee, 0x00 },
    { 0xef, 0x00 },
    { 0xf0, 0x00 },
    { 0xf1, 0x00 },
    { 0xf2, 0x00 },
    { 0xf3, 0x00 },
    { 0xf4, 0x00 },
    { 0xf5, 0x00 },
    { 0xf6, 0x00 },
    { 0xf7, 0x00 },
    { 0xf8, 0x00 },
    { 0xf9, 0x00 },
    { 0xfa, 0x00 },
    { 0xfb, 0x00 },
    //EQ15 - right; Setting: Type - All Pass; Fc - 1000; Gain - 0.0; Q - 0.707; Bandwidth - 1000
    { 0xfc, 0x08 },
    { 0xfd, 0x00 },
    { 0xfe, 0x00 },
    { 0xff, 0x00 },
    { 0x00, 0x09 },
    { 0x04, 0x00 },
    { 0x05, 0x00 },
    { 0x06, 0x00 },
    { 0x07, 0x00 },
    { 0x08, 0x00 },
    { 0x09, 0x00 },
    { 0x0a, 0x00 },
    { 0x0b, 0x00 },
    { 0x0c, 0x00 },
    { 0x0d, 0x00 },
    { 0x0e, 0x00 },
    { 0x0f, 0x00 },
    { 0x10, 0x00 },
    { 0x11, 0x00 },
    { 0x12, 0x00 },
    { 0x13, 0x00 },
    //Pre volume
    { 0x00, 0x04 },
    { 0x80, 0x00 },
    { 0x81, 0x80 },
    { 0x82, 0x7a },
    { 0x83, 0xd9 },
    { 0x7c, 0x00 },
    { 0x7d, 0x80 },
    { 0x7e, 0x7a },
    { 0x7f, 0xd9 },
    //ClassD Control Registers
    { 0x00, 0x01 },
    { 0x01, 0x00 },
    { 0x00, 0x00 },
    { 0x11, 0x03 },
    // { 0x02, 0x00 }, /* class D gain : 29.5 (Max) */
    { 0x02, 0x1e }, /* class D gain : 5.24 (Min) */
    { 0x06, 0x30 },
    { 0x28, 0x00 },
    { 0x05, 0xae },
    { 0x03, 0x05 },
    { 0x01, 0x84 },
    { 0x00, 0x00 },
    { 0x04, 0x02 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x00, 0x00 },
    { 0x04, 0x03 },

};
#endif

const uint8_t ACM_VOLUME_SETTING_TBL[] = 
{
    0x00, 0x00, 0x92, 0x5f,     /* -47 dB */
    0x00, 0x00, 0xb8, 0x45,     /* -45 dB */
    0x00, 0x01, 0x47, 0xae,     /* -40 dB */
    0x00, 0x02, 0x46, 0xb5,     /* -35 dB */
    0x00, 0x04, 0x0c, 0x37,     /* -30 dB */
    0x00, 0x07, 0x32, 0xae,     /* -25 dB */
    0x00, 0x0c, 0xcc, 0xcd,     /* -20 dB */
    0x00, 0x16, 0xc3, 0x11,     /* -15 dB */
    0x00, 0x28, 0x7a, 0x27,     /* -10 dB */
    0x00, 0x47, 0xfa, 0xcd,     /* - 5 dB */
    0x00, 0x80, 0x00, 0x00,     /*   0 dB */
    0x00, 0xe3, 0x9e, 0xa9,     /*   5 dB */
    0x01, 0x94, 0xc5, 0x84,     /*  10 dB */
    0x02, 0xcf, 0xcc, 0x01,     /*  15 dB */
    0x05, 0x00, 0x00, 0x00,     /*  20 dB */
    0x08, 0xe4, 0x32, 0x99,     /*  25 dB */
    0x0f, 0xcf, 0xb7, 0x25,     /*  30 dB */
    0x1c, 0x3b, 0xf5, 0x95,     /*  35 dB */
    0x32, 0x00, 0x00, 0x00,     /*  40 dB */
    0x59, 0x6c, 0x8c, 0xb3,     /*  45 dB */
    0x6f, 0xef, 0xa1, 0x6d,     /*  47 dB */
};

#if 0
static uint8_t _pa_drv_i2c_readbytes(const struct device *i2c_dev, uint8_t reg_addr, uint8_t *pbuf, uint8_t size)
{
    uint8_t msg_num = 0;
    struct i2c_msg msg[2];

    msg[0].flags = I2C_MSG_WRITE;
    msg[0].buf   = reg_addr;
    msg[0].len   = 1;

    msg[1].flags = I2C_MSG_READ | I2C_MSG_RESTART;
    msg[1].buf   = pbuf;
    msg[1].len   = size;

    msg_num = 2;

    return i2c_transfer(i2c_dev, msg, msg_num, ACM8625S_I2C_ADDR);
}

static uint8_t _pa_drv_i2c_writebytes(const struct device *i2c_dev, uint8_t reg_addr, uint8_t *pbuf, uint8_t size)
{
    uint8_t msg_num = 0;
    struct i2c_msg msg[2];

    msg[0].flags = I2C_MSG_WRITE;
    msg[0].buf   = reg_addr;
    msg[0].len   = 1;

    msg[1].flags = I2C_MSG_WRITE;
    msg[1].buf   = pbuf;
    msg[1].len   = size;

    msg_num = 2;

    return i2c_transfer(i2c_dev, msg, msg_num, ACM8625S_I2C_ADDR);
}
#endif

static uint8_t _pa_drv_i2c_readbyte(const struct device *i2c_dev, uint8_t reg_addr, uint8_t *reg_val)
{
    struct i2c_msg msg[2];

    msg[0].flags = I2C_MSG_WRITE;
    msg[0].buf   = &reg_addr;
    msg[0].len   = 1;

    msg[1].flags = I2C_MSG_READ | I2C_MSG_RESTART;
    msg[1].buf   = reg_val;
    msg[1].len   = 1;

    return i2c_transfer(i2c_dev, msg, 2, ACM8625S_I2C_ADDR);
}

static uint8_t _pa_drv_i2c_writebyte(const struct device *i2c_dev, uint8_t reg_addr, uint8_t reg_val)
{
    uint8_t val_buf[2];
    struct i2c_msg msg;

    val_buf[0] = reg_addr;
    val_buf[1] = reg_val;

    msg.flags = I2C_MSG_WRITE;
    msg.buf   = val_buf;
    msg.len   = 2;

    return i2c_transfer(i2c_dev, &msg, 1, ACM8625S_I2C_ADDR);
}

int pa_acm8625_audio_init(const struct device *pa_dev)
{
    uint32_t i;
    struct  pa_device_data *pa_data = pa_dev->data;

    /* init */
    for (i = 0; i < _countof(PA_ACM_INIT_TBL); i ++)
    {
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, PA_ACM_INIT_TBL[i].reg, PA_ACM_INIT_TBL[i].val);
    }

#ifdef CONFIG_ACM_AUDIO_DSP_EQ
    /* delay 10 ms*/
    os_delay(10000);

    for (i = 0; i < _countof(PA_ACM_EQ_TBL); i ++)
    {
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, PA_ACM_EQ_TBL[i].reg, PA_ACM_EQ_TBL[i].val);
    }
#endif

    printk("PA ACM8625 init successful !\n");

    return true;
}

int pa_acm8625_audio_format_config(const struct device *pa_dev, const struct pa_i2s_format *p_i2s_fmt)
{
    uint8_t value = 0;
    struct pa_device_data *pa_data = pa_dev->data;

    _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_PAGE_SEL, 0x00);
    _pa_drv_i2c_readbyte((const struct device *)pa_data->i2c_dev, ACM_REG_I2S_DATA_FMT_1, &value);

    value &= ~(I2S_DATA_44K_INPUT | I2S_DATA_44K_EN | 
                I2S_DATA_FMT_TYPE | I2S_DATA_FMT_WIDTH);
    
    /* Set data type and width */
    value |= (((p_i2s_fmt->type << 4) & I2S_DATA_FMT_TYPE) | 
               (p_i2s_fmt->width      & I2S_DATA_FMT_WIDTH));

    /* Set sample rate */
    if ((p_i2s_fmt->rate == I2S_SAMPLE_RATE_44_1K) ||
        (p_i2s_fmt->rate == I2S_SAMPLE_RATE_88_2K) ||
        (p_i2s_fmt->rate == I2S_SAMPLE_RATE_176_4K)) {
        value |= (I2S_DATA_44K_INPUT | I2S_DATA_44K_EN);
    }

    _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_I2S_DATA_FMT_1, value);
    
    pa_data->state.type  = p_i2s_fmt->type;
    pa_data->state.rate  = p_i2s_fmt->rate;
    pa_data->state.width = p_i2s_fmt->width;

    return 0;
}

int pa_acm8625_audio_volume_set(const struct device *pa_dev, uint8_t channel, uint8_t volume)
{
    uint8_t gain, offset;
    struct  pa_device_data *pa_data = pa_dev->data;
    
    gain = volume/ACM_AUDIO_VOLUME_STEP;
    if (gain > ACM_AUDIO_VOLUME_LEVEL - 1) {
        gain = ACM_AUDIO_VOLUME_LEVEL - 1;
    }

    /* Left channel */
    if (channel & PA_AUDIO_CH_L) {
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_PAGE_SEL, 0x04);

        offset = gain * 4;
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_L + 0, ACM_VOLUME_SETTING_TBL[offset + 0]);
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_L + 1, ACM_VOLUME_SETTING_TBL[offset + 1]);
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_L + 2, ACM_VOLUME_SETTING_TBL[offset + 2]);
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_L + 3, ACM_VOLUME_SETTING_TBL[offset + 3]);
    
        pa_data->state.volume_l = volume;
    }

    /* Right channel */
    if (channel & PA_AUDIO_CH_R) {
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_PAGE_SEL, 0x04);

        offset = gain * 4;
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_R + 0, ACM_VOLUME_SETTING_TBL[offset + 0]);
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_R + 1, ACM_VOLUME_SETTING_TBL[offset + 1]);
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_R + 2, ACM_VOLUME_SETTING_TBL[offset + 2]);
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_DSP_REG_GAIN_CH_R + 3, ACM_VOLUME_SETTING_TBL[offset + 3]);
    
        pa_data->state.volume_r = volume;
    }

    printk("set acm8625 volume, channel: 0x%02x, volume: %d, gain: %d\n", channel, volume, gain);
    return true;
}

int pa_acm8625_audio_mute_set(const struct device *pa_dev, uint8_t channel, bool mute)
{
    uint8_t value;
    struct pa_device_data *pa_data = pa_dev->data;

    /* Left channel */
    if (channel & PA_AUDIO_CH_L) {
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_PAGE_SEL, 0x00);

        _pa_drv_i2c_readbyte((const struct device *)pa_data->i2c_dev, ACM_REG_STATE_CTL, &value);
        if (mute) {
            value |=  (STATE_CTL_HIZ_L | STATE_CTL_MUTE_L);
        } else {
            value &= ~(STATE_CTL_HIZ_L | STATE_CTL_MUTE_L);
        }
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_STATE_CTL, value);

        pa_data->state.mute_l = mute;
    }

    /* Right channel */
    if (channel & PA_AUDIO_CH_R) {
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_PAGE_SEL, 0x00);

        _pa_drv_i2c_readbyte((const struct device *)pa_data->i2c_dev, ACM_REG_STATE_CTL, &value);
        if (mute) {
            value |=  (STATE_CTL_HIZ_R | STATE_CTL_MUTE_R);
        } else {
            value &= ~(STATE_CTL_HIZ_R | STATE_CTL_MUTE_R);
        }
        _pa_drv_i2c_writebyte((const struct device *)pa_data->i2c_dev, ACM_REG_STATE_CTL, value);

        pa_data->state.mute_r = mute;
    }

    return true;
}