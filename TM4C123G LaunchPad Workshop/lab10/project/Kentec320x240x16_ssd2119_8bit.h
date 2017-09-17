//*****************************************************************************
//
// Kentec320x240x16_ssd2119_8bit.h - Prototypes for the Kentec K350QVG-V2-F
//                                     display driver with an SSD2119
//                                     controller.
//*****************************************************************************

#ifndef __KENTEC320X240X16_SSD2119_8BIT_H__
#define __KENTEC320X240X16_SSD2119_8BIT_H__

//*****************************************************************************
//
// Bit definitions for the LCD control registers in the SRAM/Flash daughter
// board.
//
//*****************************************************************************
#define LCD_CONTROL_NRESET    0x04
#define LCD_CONTROL_YN        0x02
#define LCD_CONTROL_XN        0x01

//*****************************************************************************
//
// EPI addresses used to access the LCD when the SRAM/Flash daughter board is
// installed.
//
//*****************************************************************************
#define LCD_COMMAND_PORT    0x6C000002
#define LCD_DATA_PORT       0x6C000003
#define LCD_CONTROL_SET_REG 0x6C000000
#define LCD_CONTROL_CLR_REG 0x6C000001

//
// Read start bit.  This is ORed with LCD_COMMAND_PORT or LCD_DATA_PORT to
// initiate a read request from the LCD controller.
//
#define LCD_READ_START      0x00000004

//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************
extern void Kentec320x240x16_SSD2119Init(void);
extern const tDisplay g_sKentec320x240x16_SSD2119;
extern void Kentec320x240x16_SSD2119SetLCDControl(unsigned char ucMask,
                                                    unsigned char ucVal);
extern void LED_ON(void);
extern void LED_OFF(void);
#endif // __KENTEC320X240X16_SSD2119_H__
