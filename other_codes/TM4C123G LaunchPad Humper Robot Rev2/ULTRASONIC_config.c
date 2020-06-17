/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Source containing function definitions for ULTRASONIC configuration
 * @file        ULTRASONIC_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "ULTRASONIC_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing ULTRASONIC
void ULTRASONIC_init(void){
    // Enable the clock to the PortA, Timer1 and Timer2
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    // Set the PA3 port as Output. Trigger Pin
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    // Set the PA2 port as Input with a weak Pull-down. Echo Pin
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
    ROM_GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);

    // Configure Timer1 to run in one-shot down-count mode
    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT);

    // Configure Timer2 to run in one-shot up-count mode
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT_UP);
}

// Function for Enabling ULTRASONIC
void ULTRASONIC_enable(void){
    // Configure and enable the Interrupt on both edges for PA2. Echo Pin
    ROM_IntEnable(INT_GPIOA);
    ROM_GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_INT_PIN_2);

    // Enable the Interrupt specific vector associated with Timer1A
    ROM_IntEnable(INT_TIMER1A);
    // Enables a specific event within the timer to generate an interrupt
    ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}
