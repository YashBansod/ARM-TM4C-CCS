/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Source containing function definitions for GPIO configuration
 * @file        GPIO_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "GPIO_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing PF0 and PF4
void BUTTON_init(void){
    // Enable the clock to the GPIOF peripheral
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Remove the Lock present on Switch SW2 (connected to PF0) and commit the change
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    // Set PF0 and PF4 as input. Connect to internal Pull-up resistors and set 2 mA current strength.
    // Set PF1, PF2, PF3 as output.
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

// Function for Enabling PF0 and PF4
void BUTTON_enable(void){
    // Configure and enable the Interrupt for PF0 and PF4
    ROM_IntEnable(INT_GPIOF);
    ROM_GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
}
