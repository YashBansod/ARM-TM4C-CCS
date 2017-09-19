/*!
 * @author      Yash Bansod
 * @date        19th September 2017
 *
 * @brief       PWM Servo
 * @details     The program controls the position of an external servo motor by
 *              varying the Duty Cycle of the PWM. The Duty Cycle can be increased by
 *              clicking the onboard switch SW2(PF0) or decreased by clicking SW1(PF4).
 *              The PWM module .
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              PortF Interrupts.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "inc/hw_pwm.h"                     // Defines and Macros for Pulse Width Modulation (PWM) ports
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/pwm.h"                  // API function prototypes for PWM ports
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#define PWM_FREQUENCY 50                    // Define a macro for the frequency of the PWM signal in Hz

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32Period;                        // Variable to store PWM time period
volatile uint8_t ui8Adjust = 75;            // Variable to store Duty Cycle of Servo

/* -----------------------      Function Prototypes     --------------------- */
void PortFIntHandler(void);                 // Prototype for ISR of GPIO PortF

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and the PWM Module clock to 1.25 MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the clock for peripherals PortF and PortD and PWM1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the PD0 for PWM signal (PWM module 1 generator 0)
    ROM_GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    ROM_GPIOPinConfigure(GPIO_PD0_M1PWM0);

    // Remove the Lock present on Switch SW2 (connected to PF0) and commit the change
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    // Set the PF0, PF4 as Input and configure them to be Pulled-up
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure and enable the Interrupt for PF0 and PF4
    ROM_IntEnable(INT_GPIOF);
    ROM_GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_4);
    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    // Calculate the Timer period of the PWM Module.
    uint32_t ui32PWMClock = ROM_SysCtlClockGet() / 64;
    ui32Period = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Configure thE PWM1 Genrator0 to work in Count Down Mode
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    // Load the calculated time period to the Generator0 of the PWM1 Module
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Period);

    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Period / 1000);
    // Enable the PWM0 pin of the PWM Module 1 as output
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    // Enable the PWM Generator
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);

    while (1);
}

/* -----------------------      Function Definition     --------------------- */
void PortFIntHandler(void){
    // The ISR for GPIO PortF Interrupt Handling
    GPIOIntClear(GPIO_PORTF_BASE , GPIO_INT_PIN_4 | GPIO_INT_PIN_0);

    // If SW1(PF4) is pressed then reduce the duty cycle
    if(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_0){
        ui8Adjust--;
        if (ui8Adjust < 50){
            ui8Adjust = 50;
        }
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Period / 1000);
    }
    // If SW2(PW0) is pressed then increase the duty cycle
    if(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_4){
        ui8Adjust++;
        if (ui8Adjust > 100){
            ui8Adjust = 100;
        }
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Period / 1000);
    }
}
