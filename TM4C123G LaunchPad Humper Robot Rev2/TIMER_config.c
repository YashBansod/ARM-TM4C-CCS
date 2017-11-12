/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Source containing function definitions for TIMER configuration
 * @file        TIMER_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "TIMER_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing TIMER0 Peripheral
void TIMER0_init(void){
    // Enable clock to Timer0 Peripheral
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    // Configure Timer0 to run in periodic mode
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Load the Timer with the calculated period (= 400ms).
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() * 4 / 10) - 1);
}

// Function for Enabling TIMER0 Peripheral
void TIMER0_enable(void){
    // Enable the Interrupt specific vector associated with Timer0A
    ROM_IntEnable(INT_TIMER0A);
    // Enables a specific event within the timer to generate an interrupt
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Enable the Timer0
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}
