/*!
 * @author      Yash Bansod
 * @date        20th September 2017
 *
 * @brief       UART Stdio
 * @details     The program reads the input from a user on the serial monitor
 *              (terminal) and echoes it back to the user. Serial Activity is
 *              displayed by the Blue Led which glows for 1ms on every UartRx.
 *              The code demonstrates the use of UARTprintf() API of uartstdio.h
 *              library for printing strings.
 *
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              UART0 and Timer0A Interrupts.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/timer.h"                // Defines and macros for Timer API of driverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/uart.h"                 // Defines and Macros for the UART
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib
#include "uartStdio/uartstdio.h"            // Prototypes for the UART console functions

#define UART0_BAUDRATE 115200               // Macro for UART0 Baud rate

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32Period;                        // Variable to store the period to be inputed to Timer0

/* -----------------------      Function Prototypes     --------------------- */
void UARTIntHandler(void);                  // The prototype of the ISR for UART Interrupt
void Timer0IntHandler(void);                // The prototype of the ISR for Timer0 Interrupt

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and Enable the clock for peripherals PortA, PortF and UART0
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    // Configure PA0 as UART0_Rx and PA1 as UART0_Tx
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the baud rate and data setup for the UART0
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), UART0_BAUDRATE, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE );

    // Configure and enable the interrupt for UART0
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

    // Configure PF2 as output
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Configure Timer0 to run in periodic mode
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    // Enable the Interrupt specific vector associated with Timer0A
    ROM_IntEnable(INT_TIMER0A);
    // Enables a specific event within the timer to generate an interrupt
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Load the Timer with the calculated period.
    ui32Period = ROM_SysCtlClockGet() / 1000;
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    // Configure the UART0 for Standard Input Outputs
    UARTStdioConfig(0, UART0_BAUDRATE, ROM_SysCtlClockGet());
    UARTprintf("Enter Text: \n");

    while (1);
}

/* -----------------------      Function Definition     --------------------- */
void UARTIntHandler(void){
    // ISR for UART interrupt handling
    // Clear the asserted UART interrupts
    ROM_UARTIntClear(UART0_BASE, ROM_UARTIntStatus(UART0_BASE, true));

    // While there is a character available at input
    while(ROM_UARTCharsAvail(UART0_BASE)){
        // Echo the character back to the user
        ROM_UARTCharPutNonBlocking(UART0_BASE, ROM_UARTCharGetNonBlocking(UART0_BASE));

        // Blink the Led for approximately 1ms
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    }
}

void Timer0IntHandler(void)
{   // The ISR for Timer0 Interrupt Handling
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Read the current state of the GPIO pin and write back the opposite state
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
    // Disable the timer
    ROM_TimerDisable(TIMER0_BASE, TIMER_A);
}
