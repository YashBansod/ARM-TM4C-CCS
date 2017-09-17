/*!
 * @author      Yash Bansod
 * @date        17th September 2017
 *
 * @brief       Blink without delay
 * @details     The program controls the Green LED on the Tiva C board
 *              TM4C123G LaunchPad (with TM4C123GH6PM microcontroller) and
 *              blinks it with a specified time period.
 *              Timer0 is used to create periodic interrupts to control blinking.
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              Timer0A Interrupt.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                     // Library of Standard Integer Types
#include <stdbool.h>                    // Library of Standard Boolean Types
#include "inc/tm4c123gh6pm.h"           // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"              // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"               // Defines common types and macros
#include "driverlib/sysctl.h"           // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"        // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"             // Defines and macros for GPIO API of DriverLib
#include "driverlib/timer.h"            // Defines and macros for Timer API of driverLib

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32Period;                    // Variable to store the period to be inputted to Timer0
uint32_t ui32IntFrequency = 0x00000006; // Variable to determine the Blinking Frequency of LEDs

/* -----------------------      Function Prototypes     --------------------- */
void Timer0IntHandler(void);            // The prototype of the ISR for Timer0 Interrupt

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and enable the clock for peripheral Port F and Timer0
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Set the PF1, PF2, PF3 as output and configure Timer0 to run in periodic mode
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    // Calculate the Time period for 50% duty cycle while switching at "ui32IntFreqency" Hz.
    ui32Period = (SysCtlClockGet() / ui32IntFrequency) / 2;
    // Load the Timer with the calculated period.
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
    // Enable the Interrupt specific vector associated with Timer0A
    IntEnable(INT_TIMER0A);
    // Enables a specific event within the timer to generate an interrupt
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    // Enable the Timer
    TimerEnable(TIMER0_BASE, TIMER_A);

    while (1);
}

/* -----------------------      Function Definition     --------------------- */
void Timer0IntHandler(void)
{   // The ISR for Timer0 Interrupt Handling
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of the GPIO pin and write back the opposite state
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
    } else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
    }
}
