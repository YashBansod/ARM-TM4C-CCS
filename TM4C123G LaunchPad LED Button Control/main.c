/*!
 * @author      Yash Bansod
 * @date        17th September 2017
 *
 * @brief       LED Button Control
 * @details     The program controls the RGB LEDs on the Tiva C board
 *              TM4C123G LaunchPad (with TM4C123GH6PM microcontroller) using
 *              the onboard switches SW1(PF4) and SW2(PF0).
 *              Different combinations of button presses cause different LEDs to glow.
 * @note        The SW2(PF0) is locked by default and has to be unlocked before use.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                     // Library of Standard Integer Types
#include <stdbool.h>                    // Library of Standard Boolean Types
#include "inc/hw_memmap.h"              // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_gpio.h"                // Defines macros for GPIO hardware
#include "inc/hw_types.h"               // Defines common types and macros
#include "driverlib/sysctl.h"           // Defines and macros for System Control API of DriverLib
#include "driverlib/pin_map.h"          // Mapping of peripherals to pins of all parts
#include "driverlib/gpio.h"             // Defines and macros for GPIO API of DriverLib

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32PinStatus = 0x00000000;    // Variable to store the pin status of GPIO PortF

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Remove the Lock present on Switch SW2 (connected to PF0) and commit the change
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0 | GPIO_PIN_4;

    // Set the System clock to 80MHz and enable the clock for peripheral PortF.
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Set the PF1, PF2, PF3 as output and PF0 and PF4 as input.
    // Connect PF0, PF4 to internal Pull-up resistors and set 2 mA as current strength.
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Start an Infinite Loop
    while (true){
        // Read the status of Input
        ui32PinStatus = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);

        if (ui32PinStatus == 0x00){   // If both buttons pressed then
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_1);
        }else if (ui32PinStatus == GPIO_PIN_0){   // If only SW2 (PF0) NOT pressed then
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_2);
        }else if (ui32PinStatus == GPIO_PIN_4){   // If only SW1 (PF4) NOT pressed then
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_3);
        }else if (ui32PinStatus == GPIO_PIN_4 | GPIO_PIN_0){   // If both SW2 (PF0) and SW1 (PF4) NOT pressed then
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
        }
    }
}
