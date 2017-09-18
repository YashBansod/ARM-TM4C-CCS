/*!
 * @author      Yash Bansod
 * @date        18th September 2017
 *
 * @brief       ADC Potentiometer
 * @details     The program reads the ADC value from an externally connected
 *              potentiometer.
 *
 * @note        The code instructs the compiler to use the peripheral driver
 *              library from the ROM of the microcontroller instead of including it
 *              as part of the code and unnecessarily wasting Flash memory.
 */
/* -----------------------          Include Files       --------------------- */
#define TARGET_IS_BLIZZARD_RB1
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
#include "driverlib/adc.h"                  // Defines and macros for ADC API of driverLib
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32ADC0Value[8];                  // Array to store the ADC values
volatile uint32_t ui32ADCAvg;               // Variable to store the Average of ADC values

/* -----------------------      Function Prototypes     --------------------- */

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and enable the clock for peripherals ADC0 and GPIO PortB
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Enable hardware averaging on ADC0
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    // Configure to use ADC0, sample sequencer 0, processor to trigger sequence and use highest priority
    ROM_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    // Configure PB4 as Analog Input Pin
    ROM_GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);
    // Configure all 8 steps on sequencer 0 to sample temperature sensor
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_CH10);
    // Mark as last conversion on sequencer 0 and enable interrupt flag generation on sampling completion
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_CH10|ADC_CTL_IE|ADC_CTL_END);
    // Enable Sequencer 0
    ROM_ADCSequenceEnable(ADC0_BASE, 0);

    while(1){
        // Clear the ADC Interrupt (if any generated) for Sequencer 0
        ROM_ADCIntClear(ADC0_BASE, 0);
        // Trigger the ADC Sampling for Sequencer 0
        ROM_ADCProcessorTrigger(ADC0_BASE, 0);
        // Wait the program till the conversion isn't complete
        while(!ROM_ADCIntStatus(ADC0_BASE, 0, false));
        // Store the values in sequencer 0 of ADC0 to an Array
        ROM_ADCSequenceDataGet(ADC0_BASE, 0, ui32ADC0Value);
        // Calculate the Average of the Readings
        ui32ADCAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3]
                + ui32ADC0Value[4] + ui32ADC0Value[5] + ui32ADC0Value[6] + ui32ADC0Value[7] + 4)/8;
    }
}

/* -----------------------      Function Definition     --------------------- */
