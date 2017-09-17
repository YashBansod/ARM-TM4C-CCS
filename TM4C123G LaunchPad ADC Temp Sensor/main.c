/*!
 * @author      Yash Bansod
 * @date        17th September 2017
 *
 * @brief       ADC reading from Temperature Sensor
 * @details     The program reads the ADC value from the Temperature sensor
 *              in the TM4C123GH6PM chip. This is then converted to Celsius and
 *              Fahrenheit scale.
 * @note        The temperature sensor is not calibrated hence the readings won't
 *              be accurate. Nevertheless, the purpose of this example is using
 *              ADC peripheral and not reading the temperature of the microcontroller.
 *
 *              Also, the code instructs the compiler to use the peripheral driver
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
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/adc.h"                  // Defines and macros for ADC API of driverLib
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

/* -----------------------      Global Variables        --------------------- */
uint32_t ui32ADC0Value[8];                  // Array to store the ADC values
volatile uint32_t ui32TempAvg;              // Variable to store the Average of ADC values
volatile uint32_t ui32TempValueC;           // Variable to store the Temperature in celsius
volatile uint32_t ui32TempValueF;           // Variable to store the Temperature in fahrenheit

/* -----------------------      Function Prototypes     --------------------- */

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz and enable the clock for peripheral ADC0
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Enable hardware averaging on ADC0
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    // Configure to use ADC0, sample sequencer 0, processor to trigger sequence and use highest priority
    ROM_ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    // Configure all 8 steps on sequencer 0 to sample temperature sensor
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_TS);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_TS);
    // Mark as last conversion on sequencer 0 and enable interrupt flag generation on sampling completion
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 0, 7, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);

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
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3]
                + ui32ADC0Value[4] + ui32ADC0Value[5] + ui32ADC0Value[6] + ui32ADC0Value[6] + 4)/8;
        // Convert the reading to Celsius and Fahrenheit values
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
    }
}

/* -----------------------      Function Definition     --------------------- */
