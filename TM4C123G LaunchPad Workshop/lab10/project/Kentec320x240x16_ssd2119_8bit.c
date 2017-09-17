//*****************************************************************************
//
// Kentec320x240x16_ssd2119_8bit.c - Display driver for the Kentec
//                                     K350QVG-V2-F TFT display with an SSD2119
//                                     controller.  This version assumes an
//                                     8080-8bit interface between the micro
//                                     and display (PS3-0 = 0011b).
//
// Copyright (c) 2008-2009 Texas Instruments Incorporated. All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 5450 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

//*****************************************************************************
//
//! \addtogroup display_api
//! @{
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
//#include "driverlib/epi.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "Kentec320x240x16_ssd2119_8bit.h"


//*****************************************************************************
//
// This driver operates in four different screen orientations.  They are:
//
// * Portrait - The screen is taller than it is wide, and the flex connector is
//              on the left of the display.  This is selected by defining
//              PORTRAIT.
//
// * Landscape - The screen is wider than it is tall, and the flex connector is
//               on the bottom of the display.  This is selected by defining
//               LANDSCAPE.
//
// * Portrait flip - The screen is taller than it is wide, and the flex
//                   connector is on the right of the display.  This is
//                   selected by defining PORTRAIT_FLIP.
//
// * Landscape flip - The screen is wider than it is tall, and the flex
//                    connector is on the top of the display.  This is
//                    selected by defining LANDSCAPE_FLIP.
//
// These can also be imagined in terms of screen rotation; if portrait mode is
// 0 degrees of screen rotation, landscape is 90 degrees of counter-clockwise
// rotation, portrait flip is 180 degrees of rotation, and landscape flip is
// 270 degress of counter-clockwise rotation.
//
// If no screen orientation is selected, "landscape flip" mode will be used.
//
//*****************************************************************************
#if ! defined(PORTRAIT) && ! defined(PORTRAIT_FLIP) && \
    ! defined(LANDSCAPE) && ! defined(LANDSCAPE_FLIP)
#define LANDSCAPE
//#define PORTRAIT
#endif

//*****************************************************************************
//
// Various definitions controlling coordinate space mapping and drawing
// direction in the four supported orientations.
//
//*****************************************************************************
#ifdef PORTRAIT
#define HORIZ_DIRECTION 0x28
#define VERT_DIRECTION 0x20
#define MAPPED_X(x, y) (319 - (y))
#define MAPPED_Y(x, y) (x)
#endif
#ifdef LANDSCAPE
#define HORIZ_DIRECTION 0x00
#define VERT_DIRECTION  0x08
#define MAPPED_X(x, y) (319 - (x))
#define MAPPED_Y(x, y) (239 - (y))
#endif
#ifdef PORTRAIT_FLIP
#define HORIZ_DIRECTION 0x18
#define VERT_DIRECTION 0x10
#define MAPPED_X(x, y) (y)
#define MAPPED_Y(x, y) (239 - (x))
#endif
#ifdef LANDSCAPE_FLIP
#define HORIZ_DIRECTION 0x30
#define VERT_DIRECTION  0x38
#define MAPPED_X(x, y) (x)
#define MAPPED_Y(x, y) (y)
#endif

//*****************************************************************************
//
// Defines for the pins that are used to communicate with the SSD2119.
//
//*****************************************************************************

#define LCD_DATAH_PERIPH        SYSCTL_PERIPH_GPIOB
#define LCD_DATAH_BASE          GPIO_PORTB_BASE
#define LCD_DATAH_PINS          0xFF

//
// LCD control line GPIO definitions.
//
#define LCD_CS_PERIPH           SYSCTL_PERIPH_GPIOA
#define LCD_CS_BASE             GPIO_PORTA_BASE
#define LCD_CS_PIN              GPIO_PIN_7
#define LCD_DC_PERIPH           SYSCTL_PERIPH_GPIOA
#define LCD_DC_BASE             GPIO_PORTA_BASE
#define LCD_DC_PIN              GPIO_PIN_6
#define LCD_WR_PERIPH           SYSCTL_PERIPH_GPIOA
#define LCD_WR_BASE             GPIO_PORTA_BASE
#define LCD_WR_PIN              GPIO_PIN_5
#define LCD_RD_PERIPH           SYSCTL_PERIPH_GPIOA
#define LCD_RD_BASE             GPIO_PORTA_BASE
#define LCD_RD_PIN              GPIO_PIN_4
//#define LCD_RST_PERIPH          SYSCTL_PERIPH_GPIOL
//#define LCD_RST_BASE            GPIO_PORTL_BASE
//#define LCD_RST_PIN             GPIO_PIN_4

//*****************************************************************************
//
// Backlight control GPIO used with the Flash/SRAM/LCD daughter board.
//
//*****************************************************************************
#define LCD_BACKLIGHT_PERIPH    SYSCTL_PERIPH_GPIOF
#define LCD_BACKLIGHT_BASE      GPIO_PORTF_BASE
#define LCD_BACKLIGHT_PIN       GPIO_PIN_3

#define USER_LED_PERIPH    SYSCTL_PERIPH_GPIOF
#define USER_LED_BASE      GPIO_PORTF_BASE
#define USER_LED_R_PIN       GPIO_PIN_1
#define USER_LED_G_PIN       GPIO_PIN_2
#define USER_LED_B_PIN       GPIO_PIN_3

//*****************************************************************************
//
// Macro used to set the LCD data bus in preparation for writing a byte to the
// device.
//
//*****************************************************************************
#define SET_LCD_DATA(ucByte)                                                  \
{                                                                             \
    HWREG(LCD_DATAH_BASE + GPIO_O_DATA + (LCD_DATAH_PINS << 2)) = (ucByte);   \
}

//*****************************************************************************
//
// Various internal SD2119 registers name labels
//
//*****************************************************************************
#define SSD2119_DEVICE_CODE_READ_REG  0x00
#define SSD2119_OSC_START_REG         0x00
#define SSD2119_OUTPUT_CTRL_REG       0x01
#define SSD2119_LCD_DRIVE_AC_CTRL_REG 0x02
#define SSD2119_PWR_CTRL_1_REG        0x03
#define SSD2119_DISPLAY_CTRL_REG      0x07
#define SSD2119_FRAME_CYCLE_CTRL_REG  0x0B
#define SSD2119_PWR_CTRL_2_REG        0x0C
#define SSD2119_PWR_CTRL_3_REG        0x0D
#define SSD2119_PWR_CTRL_4_REG        0x0E
#define SSD2119_GATE_SCAN_START_REG   0x0F
#define SSD2119_SLEEP_MODE_REG        0x10
#define SSD2119_ENTRY_MODE_REG        0x11
#define SSD2119_GEN_IF_CTRL_REG       0x15
#define SSD2119_PWR_CTRL_5_REG        0x1E
#define SSD2119_RAM_DATA_REG          0x22
#define SSD2119_FRAME_FREQ_REG        0x25
#define SSD2119_VCOM_OTP_1_REG        0x28
#define SSD2119_VCOM_OTP_2_REG        0x29
#define SSD2119_GAMMA_CTRL_1_REG      0x30
#define SSD2119_GAMMA_CTRL_2_REG      0x31
#define SSD2119_GAMMA_CTRL_3_REG      0x32
#define SSD2119_GAMMA_CTRL_4_REG      0x33
#define SSD2119_GAMMA_CTRL_5_REG      0x34
#define SSD2119_GAMMA_CTRL_6_REG      0x35
#define SSD2119_GAMMA_CTRL_7_REG      0x36
#define SSD2119_GAMMA_CTRL_8_REG      0x37
#define SSD2119_GAMMA_CTRL_9_REG      0x3A
#define SSD2119_GAMMA_CTRL_10_REG     0x3B
#define SSD2119_V_RAM_POS_REG         0x44
#define SSD2119_H_RAM_START_REG       0x45
#define SSD2119_H_RAM_END_REG         0x46
#define SSD2119_X_RAM_ADDR_REG        0x4E
#define SSD2119_Y_RAM_ADDR_REG        0x4F

#define ENTRY_MODE_DEFAULT 0x6830
#define MAKE_ENTRY_MODE(x) ((ENTRY_MODE_DEFAULT & 0xFF00) | (x))

//*****************************************************************************
//
// The dimensions of the LCD panel.
//
//*****************************************************************************
#define LCD_VERTICAL_MAX 240
#define LCD_HORIZONTAL_MAX 320

//*****************************************************************************
//
// Translates a 24-bit RGB color to a display driver-specific color.
//
// \param c is the 24-bit RGB color.  The least-significant byte is the blue
// channel, the next byte is the green channel, and the third byte is the red
// channel.
//
// This macro translates a 24-bit RGB color into a value that can be written
// into the display's frame buffer in order to reproduce that color, or the
// closest possible approximation of that color.
//
// \return Returns the display-driver specific color.
//
//*****************************************************************************
#define DPYCOLORTRANSLATE(c)    ((((c) & 0x00f80000) >> 8) |               \
                                 (((c) & 0x0000fc00) >> 5) |               \
                                 (((c) & 0x000000f8) >> 3))

//*****************************************************************************
//
// Function pointer types for low level LCD controller access functions.
//
//*****************************************************************************
typedef void (*pfnWriteData)(uint16_t usData);
typedef void (*pfnWriteCommand)(uint8_t ucData);

//*****************************************************************************
//
// Function pointers for low level LCD controller access functions.
//
//*****************************************************************************

static void WriteDataGPIO(uint16_t usData);
static void WriteCommandGPIO(uint8_t ucData);

pfnWriteData WriteData = WriteDataGPIO;
pfnWriteCommand WriteCommand = WriteCommandGPIO;

void LED_ON(void)
{
HWREG(LCD_BACKLIGHT_BASE + GPIO_O_DATA + (LCD_BACKLIGHT_PIN << 2)) = 0;
HWREG(USER_LED_BASE + GPIO_O_DATA + (USER_LED_R_PIN << 2)) = 0;
HWREG(USER_LED_BASE + GPIO_O_DATA + (USER_LED_G_PIN << 2)) = USER_LED_G_PIN;
HWREG(USER_LED_BASE + GPIO_O_DATA + (USER_LED_B_PIN << 2)) = 0;
}

void LED_OFF(void)
{
HWREG(LCD_BACKLIGHT_BASE + GPIO_O_DATA + (LCD_BACKLIGHT_PIN << 2)) = LCD_BACKLIGHT_PIN;
HWREG(USER_LED_BASE + GPIO_O_DATA + (USER_LED_R_PIN << 2)) = USER_LED_R_PIN;
HWREG(USER_LED_BASE + GPIO_O_DATA + (USER_LED_G_PIN << 2)) = 0;
HWREG(USER_LED_BASE + GPIO_O_DATA + (USER_LED_B_PIN << 2)) = USER_LED_B_PIN;
}

//*****************************************************************************
//
// Writes a data word to the SSD2119.  This function implements the basic GPIO
// interface to the LCD display.
//
//*****************************************************************************
static void
WriteDataGPIO(uint16_t usData)
{
	//
    // Write the most significant byte of the data to the bus.
    //
    SET_LCD_DATA(usData >> 8);

	HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = 0;
    
    //
    // Assert the write enable signal.  Do this three times to ensure that we
    // meet the display timing requirements (20nS per instruction at 50MHz
    // means we need to ensure at least 3 instructions between edges on WR)
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

    //
    // Deassert the write enable signal.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;

    //
    // Write the least significant byte of the data to the bus.
    //
    SET_LCD_DATA(usData);

    //
    // Assert the write enable signal.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = 0;

    //
    // Deassert the write enable signal.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + (LCD_WR_PIN << 2)) = LCD_WR_PIN;

	HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = LCD_CS_PIN;
    
}

//*****************************************************************************
//
// Writes a command to the SSD2119.  This function implements the basic GPIO
// interface to the LCD display.
//
//*****************************************************************************
static void
WriteCommandGPIO(uint8_t ucData)

{
    //
    // Write the most significant byte of the data to the bus. This is always
    // 0 since commands are no more than 8 bits currently.
    //
    SET_LCD_DATA(0);

	HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = 0;
    
    //
    // Assert the write enable and DC signals.  Do this 3 times to slow things
    // down a bit.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) = 0;

    //
    // Deassert the write enable and DC signals. We need to leave WR high for
    // at least 50nS so, again, stick in dummy writes to pad the timing.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) =
        (LCD_WR_PIN | LCD_DC_PIN);
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) =
        (LCD_WR_PIN | LCD_DC_PIN);
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) =
        (LCD_WR_PIN | LCD_DC_PIN);

    //
    // Write the least significant byte of the data to the bus.
    //
    SET_LCD_DATA(ucData);

    //
    // Assert the write enable signal.  Again, do this three times to ensure
    // we meet the timing spec.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) = 0;
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) = 0;

    //
    // Deassert the write enable and DC signals.  Make sure we add padding here
    // too since some compilers inline this function.
    //
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) =
        (LCD_WR_PIN | LCD_DC_PIN);
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) =
        (LCD_WR_PIN | LCD_DC_PIN);
    HWREG(LCD_WR_BASE + GPIO_O_DATA + ((LCD_WR_PIN | LCD_DC_PIN) << 2)) =
            (LCD_WR_PIN | LCD_DC_PIN);

	HWREG(LCD_CS_BASE + GPIO_O_DATA + (LCD_CS_PIN << 2)) = LCD_CS_PIN;
}

//*****************************************************************************
//
// Initializes the pins required for the GPIO-based LCD interface.
//
// This function configures the GPIO pins used to control the LCD display
// when the basic GPIO interface is in use.  On exit, the LCD controller
// has been reset and is ready to receive command and data writes.
//
// \return None.
//
//*****************************************************************************
static void
InitGPIOLCDInterface(uint32_t ulClockMS)
{
    //
    // Configure the pins that connect to the LCD as GPIO outputs.
    //
    GPIOPinTypeGPIOOutput(LCD_DATAH_BASE, LCD_DATAH_PINS);
    GPIOPinTypeGPIOOutput(LCD_DC_BASE, LCD_DC_PIN);
    GPIOPinTypeGPIOOutput(LCD_RD_BASE, LCD_RD_PIN);
    GPIOPinTypeGPIOOutput(LCD_WR_BASE, LCD_WR_PIN);
    GPIOPinTypeGPIOOutput(LCD_CS_BASE, LCD_CS_PIN);
    GPIOPinTypeGPIOOutput(LCD_BACKLIGHT_BASE, LCD_BACKLIGHT_PIN);
//    GPIOPinTypeGPIOOutput(LCD_RST_BASE, LCD_RST_PIN);LCD_BACKLIGHT_BASE

    //
    // Set the LCD control pins to their default values.  This also asserts the
    // LCD reset signal.
    //
    GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, LCD_CS_PIN);
    GPIOPinWrite(LCD_DATAH_BASE, LCD_DATAH_PINS, 0x00);
    GPIOPinWrite(LCD_DC_BASE, LCD_DC_PIN, 0x00);
    GPIOPinWrite(LCD_RD_BASE, LCD_RD_PIN, LCD_RD_PIN);
    GPIOPinWrite(LCD_WR_BASE, LCD_WR_PIN, LCD_WR_PIN);
//    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, 0x00);
	GPIOPinWrite(LCD_CS_BASE, LCD_CS_PIN, 0);

    //
    // Delay for 1ms.
    //
    SysCtlDelay(ulClockMS);

    //
    // Deassert the LCD reset signal.
    //
//    GPIOPinWrite(LCD_RST_BASE, LCD_RST_PIN, LCD_RST_PIN);

    //
    // Delay for 1ms while the LCD comes out of reset.
    //
    SysCtlDelay(ulClockMS);
}


//*****************************************************************************
//
//! Initializes the display driver.
//!
//! This function initializes the SSD2119 display controller on the panel,
//! preparing it to display data.
//!
//! \return None.
//
//*****************************************************************************
void
Kentec320x240x16_SSD2119Init(void)
{
    uint32_t ulClockMS, ulCount;

    //
    // Get the current processor clock frequency.
    //
    ulClockMS = SysCtlClockGet() / (3 * 1000);

	//
    // Enable the GPIO peripherals used to interface to the SSD2119.
    //
	SysCtlPeripheralEnable(LCD_DATAH_PERIPH);
    SysCtlPeripheralEnable(LCD_DC_PERIPH);
    SysCtlPeripheralEnable(LCD_RD_PERIPH);
    SysCtlPeripheralEnable(LCD_WR_PERIPH);
//    SysCtlPeripheralEnable(LCD_RST_PERIPH);
    SysCtlPeripheralEnable(LCD_CS_PERIPH);
    SysCtlPeripheralEnable(LCD_BACKLIGHT_PERIPH);

    //
    // Perform low level interface initialization depending upon how the LCD
    // is connected to the Stellaris microcontroller.  This varies depending
    // upon the daughter board connected it is possible that a daughter board
    // can drive the LCD directly rather than via the basic GPIO interface.
    //
        {
            //
            // Initialize the GPIOs used to interface to the LCD controller.
            //
            InitGPIOLCDInterface(ulClockMS);
        }

    //
    // Enter sleep mode (if we are not already there).
    //
    WriteCommand(SSD2119_SLEEP_MODE_REG);
    WriteData(0x0001);

    //
    // Set initial power parameters.
    //
    WriteCommand(SSD2119_PWR_CTRL_5_REG);
    WriteData(0x00BA);
    WriteCommand(SSD2119_VCOM_OTP_1_REG);
    WriteData(0x0006);

    //
    // Start the oscillator.
    //
    WriteCommand(SSD2119_OSC_START_REG);
    WriteData(0x0001);

    //
    // Set pixel format and basic display orientation (scanning direction).
    //
    WriteCommand(SSD2119_OUTPUT_CTRL_REG);
    WriteData(0x30EF);
    WriteCommand(SSD2119_LCD_DRIVE_AC_CTRL_REG);
    WriteData(0x0600);

    //
    // Exit sleep mode.
    //
    WriteCommand(SSD2119_SLEEP_MODE_REG);
    WriteData(0x0000);

    //
    // Delay 30mS
    //
    SysCtlDelay(30 * ulClockMS);

    //
    // Configure pixel color format and MCU interface parameters.
    //
    WriteCommand(SSD2119_ENTRY_MODE_REG);
    WriteData(ENTRY_MODE_DEFAULT);

    //
    // Enable the display.
    //
    WriteCommand(SSD2119_DISPLAY_CTRL_REG);
    WriteData(0x0033);

    //
    // Set VCIX2 voltage to 6.1V.
    //
    WriteCommand(SSD2119_PWR_CTRL_2_REG);
    WriteData(0x0005);

    //
    // Configure gamma correction.
    //
    WriteCommand(SSD2119_GAMMA_CTRL_1_REG);
    WriteData(0x0000);
    WriteCommand(SSD2119_GAMMA_CTRL_2_REG);
    WriteData(0x0400);
    WriteCommand(SSD2119_GAMMA_CTRL_3_REG);
    WriteData(0x0106);
    WriteCommand(SSD2119_GAMMA_CTRL_4_REG);
    WriteData(0x0700);
    WriteCommand(SSD2119_GAMMA_CTRL_5_REG);
    WriteData(0x0002);
    WriteCommand(SSD2119_GAMMA_CTRL_6_REG);
    WriteData(0x0702);
    WriteCommand(SSD2119_GAMMA_CTRL_7_REG);
    WriteData(0x0707);
    WriteCommand(SSD2119_GAMMA_CTRL_8_REG);
    WriteData(0x0203);
    WriteCommand(SSD2119_GAMMA_CTRL_9_REG);
    WriteData(0x1400);
    WriteCommand(SSD2119_GAMMA_CTRL_10_REG);
    WriteData(0x0F03);

    //
    // Configure Vlcd63 and VCOMl.
    //
    WriteCommand(SSD2119_PWR_CTRL_3_REG);
    WriteData(0x0007);
    WriteCommand(SSD2119_PWR_CTRL_4_REG);
    WriteData(0x3100);

    //
    // Set the display size and ensure that the GRAM window is set to allow
    // access to the full display buffer.
    //
    WriteCommand(SSD2119_V_RAM_POS_REG);
    WriteData((LCD_VERTICAL_MAX-1) << 8);
    WriteCommand(SSD2119_H_RAM_START_REG);
    WriteData(0x0000);
    WriteCommand(SSD2119_H_RAM_END_REG);
    WriteData(LCD_HORIZONTAL_MAX-1);
    WriteCommand(SSD2119_X_RAM_ADDR_REG);
    WriteData(0x00);
    WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    WriteData(0x00);

    //
    // Clear the contents of the display buffer.
    //
    WriteCommand(SSD2119_RAM_DATA_REG);
    for(ulCount = 0; ulCount < (320 * 240); ulCount++)
    {
        WriteData(0x0000);
    }
}

//*****************************************************************************
//
//! Draws a pixel on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the pixel.
//! \param lY is the Y coordinate of the pixel.
//! \param ulValue is the color of the pixel.
//!
//! This function sets the given pixel to a particular color.  The coordinates
//! of the pixel are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kentec320x240x16_SSD2119PixelDraw(void *pvDisplayData, int32_t lX, int32_t lY,
                                   uint32_t ulValue)
{
    //
    // Set the X address of the display cursor.
    //
    WriteCommand(SSD2119_X_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX, lY));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX, lY));

    //
    // Write the pixel value.
    //
    WriteCommand(SSD2119_RAM_DATA_REG);
    WriteData(ulValue);
}

//*****************************************************************************
//
//! Draws a horizontal sequence of pixels on the screen.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the first pixel.
//! \param lY is the Y coordinate of the first pixel.
//! \param lX0 is sub-pixel offset within the pixel data, which is valid for 1
//! or 4 bit per pixel formats.
//! \param lCount is the number of pixels to draw.
//! \param lBPP is the number of bits per pixel; must be 1, 4, or 8.
//! \param pucData is a pointer to the pixel data.  For 1 and 4 bit per pixel
//! formats, the most significant bit(s) represent the left-most pixel.
//! \param pucPalette is a pointer to the palette used to draw the pixels.
//!
//! This function draws a horizontal sequence of pixels on the screen, using
//! the supplied palette.  For 1 bit per pixel format, the palette contains
//! pre-translated colors; for 4 and 8 bit per pixel formats, the palette
//! contains 24-bit RGB values that must be translated before being written to
//! the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kentec320x240x16_SSD2119PixelDrawMultiple(void *pvDisplayData, int32_t lX,
                                           int32_t lY, int32_t lX0, int32_t lCount,
                                           int32_t lBPP,
                                           const uint8_t *pucData,
                                           const uint8_t *pucPalette)
{
    uint32_t ulByte;

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(SSD2119_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(SSD2119_X_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX, lY));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX, lY));

    //
    // Write the data RAM write command.
    //
    WriteCommand(SSD2119_RAM_DATA_REG);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        //
        // The pixel data is in 1 bit per pixel format.
        //
        case 1:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount)
            {
                //
                // Get the next byte of image data.
                //
                ulByte = *pucData++;

                //
                // Loop through the pixels in this byte of image data.
                //
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    //
                    // Draw this pixel in the appropriate color.
                    //
                    WriteData(((uint32_t *)pucPalette)[(ulByte >>
                                                             (7 - lX0)) & 1]);
                }

                //
                // Start at the beginning of the next byte of image data.
                //
                lX0 = 0;
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 4 bit per pixel format.
        //
        case 4:
        {
            //
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            //
            switch(lX0 & 1)
            {
                case 0:
                    while(lCount)
                    {
                        //
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the
                        // palette.
                        //
                        ulByte = (*pucData >> 4) * 3;
                        ulByte = (*(uint32_t *)(pucPalette + ulByte) &
                                  0x00ffffff);

                        //
                        // Translate this palette entry and write it to the
                        // screen.
                        //
                        WriteData(DPYCOLORTRANSLATE(ulByte));

                        //
                        // Decrement the count of pixels to draw.
                        //
                        lCount--;

                        //
                        // See if there is another pixel to draw.
                        //
                        if(lCount)
                        {
                case 1:
                            //
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette.
                            //
                            ulByte = (*pucData++ & 15) * 3;
                            ulByte = (*(uint32_t *)(pucPalette + ulByte) &
                                      0x00ffffff);

                            //
                            // Translate this palette entry and write it to the
                            // screen.
                            //
                            WriteData(DPYCOLORTRANSLATE(ulByte));

                            //
                            // Decrement the count of pixels to draw.
                            //
                            lCount--;
                        }
                    }
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // The pixel data is in 8 bit per pixel format.
        //
        case 8:
        {
            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                ulByte = *pucData++ * 3;
                ulByte = *(uint32_t *)(pucPalette + ulByte) & 0x00ffffff;

                //
                // Translate this palette entry and write it to the screen.
                //
                WriteData(DPYCOLORTRANSLATE(ulByte));
            }

            //
            // The image data has been drawn.
            //
            break;
        }

        //
        // We are being passed data in the display's native format.  Merely
        // write it directly to the display.  This is a special case which is
        // not used by the graphics library but which is helpful to
        // applications which may want to handle, for example, JPEG images.
        //
        case 16:
        {
            uint16_t usByte;

            //
            // Loop while there are more pixels to draw.
            //
            while(lCount--)
            {
                //
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette.
                //
                usByte = *((uint16_t *)pucData);
                pucData += 2;

                //
                // Translate this palette entry and write it to the screen.
                //
                WriteData(usByte);
            }
        }
    }
}

//*****************************************************************************
//
//! Draws a horizontal line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX1 is the X coordinate of the start of the line.
//! \param lX2 is the X coordinate of the end of the line.
//! \param lY is the Y coordinate of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a horizontal line on the display.  The coordinates of
//! the line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kentec320x240x16_SSD2119LineDrawH(void *pvDisplayData, int32_t lX1, int32_t lX2,
                                   int32_t lY, uint32_t ulValue)
{
    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    WriteCommand(SSD2119_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

    //
    // Set the starting X address of the display cursor.
    //
    WriteCommand(SSD2119_X_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX1, lY));

    //
    // Set the Y address of the display cursor.
    //
    WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX1, lY));

    //
    // Write the data RAM write command.
    //
    WriteCommand(SSD2119_RAM_DATA_REG);

    //
    // Loop through the pixels of this horizontal line.
    //
    while(lX1++ <= lX2)
    {
        //
        // Write the pixel value.
        //
        WriteData(ulValue);
    }
}

//*****************************************************************************
//
//! Draws a vertical line.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param lX is the X coordinate of the line.
//! \param lY1 is the Y coordinate of the start of the line.
//! \param lY2 is the Y coordinate of the end of the line.
//! \param ulValue is the color of the line.
//!
//! This function draws a vertical line on the display.  The coordinates of the
//! line are assumed to be within the extents of the display.
//!
//! \return None.
//
//*****************************************************************************
static void
Kentec320x240x16_SSD2119LineDrawV(void *pvDisplayData, int32_t lX, int32_t lY1,
                                   int32_t lY2, uint32_t ulValue)
{
    //
    // Set the cursor increment to top to bottom, followed by left to right.
    //
    WriteCommand(SSD2119_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(VERT_DIRECTION));

    //
    // Set the X address of the display cursor.
    //
    WriteCommand(SSD2119_X_RAM_ADDR_REG);
    WriteData(MAPPED_X(lX, lY1));

    //
    // Set the starting Y address of the display cursor.
    //
    WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    WriteData(MAPPED_Y(lX, lY1));

    //
    // Write the data RAM write command.
    //
    WriteCommand(SSD2119_RAM_DATA_REG);

    //
    // Loop through the pixels of this vertical line.
    //
    while(lY1++ <= lY2)
    {
        //
        // Write the pixel value.
        //
        WriteData(ulValue);
    }
}

//*****************************************************************************
//
//! Fills a rectangle.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param pRect is a pointer to the structure describing the rectangle.
//! \param ulValue is the color of the rectangle.
//!
//! This function fills a rectangle on the display.  The coordinates of the
//! rectangle are assumed to be within the extents of the display, and the
//! rectangle specification is fully inclusive (in other words, both i16XMin and
//! i16XMax are drawn, along with i16YMin and i16YMax).
//!
//! \return None.
//
//*****************************************************************************
static void
Kentec320x240x16_SSD2119RectFill(void *pvDisplayData, const tRectangle *pRect,
                                  uint32_t ulValue)
{
    int32_t lCount;

    //
    // Write the Y extents of the rectangle.
    //
    WriteCommand(SSD2119_ENTRY_MODE_REG);
    WriteData(MAKE_ENTRY_MODE(HORIZ_DIRECTION));

    //
    // Write the X extents of the rectangle.
    //
    WriteCommand(SSD2119_H_RAM_START_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
    WriteData(MAPPED_X(pRect->i16XMax, pRect->i16YMax));
#else
    WriteData(MAPPED_X(pRect->i16XMin, pRect->i16YMin));
#endif

    WriteCommand(SSD2119_H_RAM_END_REG);
#if (defined PORTRAIT) || (defined LANDSCAPE)
    WriteData(MAPPED_X(pRect->i16XMin, pRect->i16YMin));
#else
    WriteData(MAPPED_X(pRect->i16XMax, pRect->i16YMax));
#endif

    //
    // Write the Y extents of the rectangle
    //
    WriteCommand(SSD2119_V_RAM_POS_REG);
#if (defined LANDSCAPE_FLIP) || (defined PORTRAIT)
    WriteData(MAPPED_Y(pRect->i16XMin, pRect->i16YMin) |
             (MAPPED_Y(pRect->i16XMax, pRect->i16YMax) << 8));
#else
    WriteData(MAPPED_Y(pRect->i16XMax, pRect->i16YMax) |
             (MAPPED_Y(pRect->i16XMin, pRect->i16YMin) << 8));
#endif

    //
    // Set the display cursor to the upper left of the rectangle (in application
    // coordinate space).
    //
    WriteCommand(SSD2119_X_RAM_ADDR_REG);
    WriteData(MAPPED_X(pRect->i16XMin, pRect->i16YMin));

    WriteCommand(SSD2119_Y_RAM_ADDR_REG);
    WriteData(MAPPED_Y(pRect->i16XMin, pRect->i16YMin));

    //
    // Tell the controller we are about to write data into its RAM.
    //
    WriteCommand(SSD2119_RAM_DATA_REG);

    //
    // Loop through the pixels of this filled rectangle.
    //
    for(lCount = ((pRect->i16XMax - pRect->i16XMin + 1) *
                  (pRect->i16YMax - pRect->i16YMin + 1)); lCount >= 0; lCount--)
    {
        //
        // Write the pixel value.
        //
        WriteData(ulValue);
    }

    //
    // Reset the X extents to the entire screen.
    //
    WriteCommand(SSD2119_H_RAM_START_REG);
    WriteData(0x0000);
    WriteCommand(SSD2119_H_RAM_END_REG);
    WriteData(0x013F);

    //
    // Reset the Y extent to the full screen
    //
    WriteCommand(SSD2119_V_RAM_POS_REG);
    WriteData(0xEF00);
}

//*****************************************************************************
//
//! Translates a 24-bit RGB color to a display driver-specific color.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//! \param ulValue is the 24-bit RGB color.  The least-significant byte is the
//! blue channel, the next byte is the green channel, and the third byte is the
//! red channel.
//!
//! This function translates a 24-bit RGB color into a value that can be
//! written into the display's frame buffer in order to reproduce that color,
//! or the closest possible approximation of that color.
//!
//! \return Returns the display-driver specific color.
//
//*****************************************************************************
static uint32_t
Kentec320x240x16_SSD2119ColorTranslate(void *pvDisplayData,
                                        uint32_t ulValue)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(DPYCOLORTRANSLATE(ulValue));
}

//*****************************************************************************
//
//! Flushes any cached drawing operations.
//!
//! \param pvDisplayData is a pointer to the driver-specific data for this
//! display driver.
//!
//! This functions flushes any cached drawing operations to the display.  This
//! is useful when a local frame buffer is used for drawing operations, and the
//! flush would copy the local frame buffer to the display.  For the SSD2119
//! driver, the flush is a no operation.
//!
//! \return None.
//
//*****************************************************************************
static void
Kentec320x240x16_SSD2119Flush(void *pvDisplayData)
{
    //
    // There is nothing to be done.
    //
}

//*****************************************************************************
//
//! The display structure that describes the driver for the Kentec
//! K350QVG-V2-F TFT panel with an SSD2119 controller.
//
//*****************************************************************************
const tDisplay g_sKentec320x240x16_SSD2119 =
{
    sizeof(tDisplay),
    0,
#if defined(PORTRAIT) || defined(PORTRAIT_FLIP)
    240,
    320,
#else
    320,
    240,
#endif
    Kentec320x240x16_SSD2119PixelDraw,
    Kentec320x240x16_SSD2119PixelDrawMultiple,
    Kentec320x240x16_SSD2119LineDrawH,
    Kentec320x240x16_SSD2119LineDrawV,
    Kentec320x240x16_SSD2119RectFill,
    Kentec320x240x16_SSD2119ColorTranslate,
    Kentec320x240x16_SSD2119Flush
};

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
