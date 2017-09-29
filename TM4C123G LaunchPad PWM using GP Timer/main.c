/*!
 * @author      Yash Bansod
 * @date        17th September 2017
 *
 * @brief       PWM using General Purpose Timer
 * @details     The program controls the intensity of Green LED on the Tiva C
 *              board TM4C123G LaunchPad (with TM4C123GH6PM microcontroller) by
 *              varying the Duty Cycle of the PWM. The Duty Cycle can be increased by
 *              clicking the onboard switch SW2(PF0) or decreased by clicking SW1(PF4).
 *              Timer0 is used to create PWM at fixed frequency but variable Duty Cycle.
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              Timer0A and PortF Interrupts.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/timer.h"                // Defines and macros for Timer API of driverLib

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32Period;                        // Variable to store Timer Period
uint16_t ui16IntFrequency = 0x0080;         // Variable to store Timer Frequency
volatile uint16_t ui16DutyCycle = 0x0000;   // Variable to store Duty Cycle of LED
volatile uint32_t ui32OnPeriod;             // Variable to store On Time Period of LED
volatile uint32_t ui32OffPeriod;            // Variable to store Off Time Perod of LED
volatile uint32_t ui32PinStatus;            // Variable to store the Pin Status of GPIO PortF

/* -----------------------      Function Prototypes     --------------------- */
void Timer0IntHandler(void);                // Prototype for ISR of Timer 0
void PortFIntHandler(void);                 // Prototype for ISR of GPIO PortF

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and enable the clock for peripherals PortF and Timer0
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Remove the Lock present on Switch SW2 (connected to PF0) and commit the change
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;

    // Set the PF1, PF2, PF3 as output and PF0, PF4 as Input
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure and enable the Interrupt for PF0 and PF4
    IntEnable(INT_GPIOF);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);

    // Calculate the Timer period to get switching in ui32IntFreqency Hz.
    ui32Period = (SysCtlClockGet() / ui16IntFrequency);
    // Calculate the Timer period for On time and Off time of LED
    ui32OnPeriod = ui32Period * ui16DutyCycle / 100;
    ui32OffPeriod = ui32Period * (100 - ui16DutyCycle) / 100;
    // Configure Timer0 to run in periodic mode and enable interrupt generation on Timeout
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Master interrupt enable API for all interrupts
    IntMasterEnable();

    if (ui16DutyCycle > 0 && ui16DutyCycle < 100){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3);
        // Load the Timer with the calculated period.
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32OnPeriod - 1);
        // Enable the Timer
        TimerEnable(TIMER0_BASE, TIMER_A);
    }else if (ui16DutyCycle == 0) { // Turn off the Pins Completely
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, false);
    }else if (ui16DutyCycle == 100) {   // Turn on the Green Led Completely
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3);
    }

    while (1);
}

/* -----------------------      Function Definition     --------------------- */
void Timer0IntHandler(void)
{   // The ISR for Timer0 Interrupt Handling
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of the GPIO pin, if pin is On then
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3)){
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, false);
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32OffPeriod - 1);
    } else{
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32OnPeriod - 1);
    }
}

void PortFIntHandler(void){
    // The ISR for GPIO PortF Interrupt Handling
    // Read the status of Input
    ui32PinStatus = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    GPIOIntClear(GPIO_PORTF_BASE , GPIO_INT_PIN_4 | GPIO_INT_PIN_0);

    if (ui32PinStatus == GPIO_PIN_0){
        // If only SW1(PF4) pressed i.e. SW2(PF0) not pressed then
        if (ui16DutyCycle > 4){
            ui16DutyCycle -= 5;
        }
    }else if (ui32PinStatus == GPIO_PIN_4){
        // If only SW2(PF0) pressed i.e. SW1(PF4) not pressed then
        if (ui16DutyCycle < 96){
            ui16DutyCycle += 5;
        }
    }
    // Calculate the Timer period for On time and Off time of LED
    ui32OnPeriod = ui32Period * ui16DutyCycle / 100;
    ui32OffPeriod = ui32Period * (100 - ui16DutyCycle) / 100;
    if (ui16DutyCycle == 5 || ui16DutyCycle == 95){
        // Load the Timer with the calculated period.
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32OnPeriod - 1);
        // Enable the Timer
        TimerEnable(TIMER0_BASE, TIMER_A);
    }else if (ui16DutyCycle == 0) {
        // Disable the Timer
        TimerDisable(TIMER0_BASE, TIMER_A);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, false);
    }else if (ui16DutyCycle == 100) {
        // Disable the Timer
        TimerDisable(TIMER0_BASE, TIMER_A);
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3);
    }
}
