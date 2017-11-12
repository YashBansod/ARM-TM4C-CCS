/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Ultrasonin HC-SR04
 * @details     This is a sample code for HC-SR04. The code calculates the distance
 *              of an obstacle from the HC-SR04 sensor and publishes the data over
 *              the UART channel.
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              Timer0A and PortA Interrupts.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include <stdlib.h>                         // Library of Standard Datatype Conversions
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_timer.h"                   // Defines and macros used when accessing the timer
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/timer.h"                // Defines and macros for Timer API of driverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/uart.h"                 // Defines and Macros for the UART
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#define UART0_BAUDRATE  115200              // Macro for UART0 Baud rate

/* -----------------------      Global Variables        --------------------- */
volatile bool boolTrigCondition = 1;        // Variable to control the Trigger Pin Switching
volatile uint32_t ui32EchoDuration = 0;     // Variable to store duration for which Echo Pin is high
volatile uint32_t ui32ObstacleDist = 0;     // Variable to store distance of the Obstacle

uint8_t ui8WelcomeText[] = {"\n\rDistance: "};

/* -----------------------      Function Prototypes     --------------------- */
void Timer0IntHandler(void);                // The prototype of the ISR for Timer0 Interrupt
void PortAIntHandler(void);                 // Prototype for ISR of GPIO PortA

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and Enable the clock for peripherals PortA, Timer0, Timer2 and UART0
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    // Configure PA0 as UART0_Rx and PA1 as UART0_Tx
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Configure the baud rate and data setup for the UART0
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), UART0_BAUDRATE, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE );
    // Enable the UART0
    ROM_UARTEnable(UART0_BASE);

    // Set the PA3 port as Output. Trigger Pin
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    // Set the PA2 port as Input with a weak Pull-down. Echo Pin
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_2);
    ROM_GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);
    // Configure and enable the Interrupt on both edges for PA2. Echo Pin
    ROM_IntEnable(INT_GPIOA);
    ROM_GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_INT_PIN_2);

    // Configure Timer0 to run in one-shot down-count mode
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT);
    // Enable the Interrupt specific vector associated with Timer0A
    ROM_IntEnable(INT_TIMER0A);
    // Enables a specific event within the timer to generate an interrupt
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Configure Timer2 to run in one-shot up-count mode
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT_UP);

    // Transmit a New Page Character to the Terminal
    ROM_UARTCharPutNonBlocking(UART0_BASE, '\f');

    uint8_t iter;
    for (iter = 0; iter<sizeof(ui8WelcomeText); iter++ ) ROM_UARTCharPut(UART0_BASE, ui8WelcomeText[iter]);

    while (1){
        if (boolTrigCondition){
            // Load the Timer with value for generating a  delay of 10 uS.
            ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() / 100000) -1);
            // Make the Trigger Pin (PA3) High
            ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
            // Enable the Timer0 to cause an interrupt when timeout occurs
            ROM_TimerEnable(TIMER0_BASE, TIMER_A);
            // Disable the condition for Trigger Pin Switching
            boolTrigCondition = 0;
        }
    }
}

/* -----------------------      Function Definition     --------------------- */
void Timer0IntHandler(void){
    // The ISR for Timer0 Interrupt Handling
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Disable the timer
    ROM_TimerDisable(TIMER0_BASE, TIMER_A);
    // Make the Trigger Pin (PA3) Low
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x00);
}

void PortAIntHandler(void){
    // The ISR for GPIO PortA Interrupt Handling
    // Clear the GPIO Hardware Interrupt
    GPIOIntClear(GPIO_PORTA_BASE , GPIO_INT_PIN_2);

    // Condition when Echo Pin (PA2) goes high
    if (ROM_GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_2) == GPIO_PIN_2){
        // Initialize Timer2 with value 0
        HWREG(TIMER2_BASE + TIMER_O_TAV) = 0;
        // Enable Timer2 to start measuring duration for which Echo Pin is High
        ROM_TimerEnable(TIMER2_BASE, TIMER_A);
    }
    else{
        ui32EchoDuration = ROM_TimerValueGet(TIMER2_BASE, TIMER_A);
        // Disable Timer2 to stop measuring duration for which Echo Pin is High
        ROM_TimerDisable(TIMER2_BASE, TIMER_A);
        // Convert the Timer Duration to Distance Value according to Ultrasonic's formula
        ui32ObstacleDist = ui32EchoDuration / 4640;
        // Convert the Distance Value from Integer to Array of Characters
        char chArrayDistance[8];
        ltoa(ui32ObstacleDist, chArrayDistance);

        // Transmit the distance reading to the terminal
        uint8_t iter;
        for (iter = 0; iter<sizeof(chArrayDistance); iter++ ) ROM_UARTCharPut(UART0_BASE, chArrayDistance[iter]);
        for (iter = 0; iter<sizeof(ui8WelcomeText); iter++ ) ROM_UARTCharPut(UART0_BASE, ui8WelcomeText[iter]);

        // Enable condition for Trigger Pulse
        boolTrigCondition = 1;
    }

}
