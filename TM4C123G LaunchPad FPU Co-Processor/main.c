/*!
 * @author      Yash Bansod
 * @date        18th September 2017
 *
 * @brief       FPU Co-Processor
 * @details     The program enables the Floating Point Unit Co-Processor which is
 *              used to calculate a sine wave with a specified number of sampling points.
 *
 * @note        The code instructs the compiler to use the peripheral driver
 *              library from the ROM of the microcontroller instead of including it
 *              as part of the code and unnecessarily wasting Flash memory.
 */
/* -----------------------          Include Files       --------------------- */
#define TARGET_IS_BLIZZARD_RB1
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include <math.h>                           // Library for Math functions
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/fpu.h"                  // Prototypes for floating point manipulation routine
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#ifndef M_PI
#define M_PI 3.14159265358979323846         // Define M_PI if not already defined
#endif

#define SERIES_LENGTH 100                   // Define SERIES_LENGTH equal to no. of sample points

/* -----------------------      Global Variables        --------------------- */
float gSeriesData[SERIES_LENGTH];           // Create an array of floating point numbers of specified size
int32_t i32DataCount = 0;                   // Create a variable to keep track of number of points sampled

/* -----------------------      Function Prototypes     --------------------- */

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Create a temporary variable to store value of one sampling point in radians
    float fRadians;
    // Enable FPU Lazy Stacking.
    // This will enable skipping stacking of Floating point nos. during interrupts
    ROM_FPULazyStackingEnable();
    // Enable Floating point number
    ROM_FPUEnable();

    // Set the System clock to 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    // Calculate the value of one sampling point in radians
    fRadians = ((2 * M_PI) / SERIES_LENGTH);

    // Calculate the sine wave for 2*Pi radians with "SERIES_LENGTH" sampling points
    while(i32DataCount < SERIES_LENGTH){
        gSeriesData[i32DataCount] = sinf(fRadians * i32DataCount);
        i32DataCount++;
    }
    while(1);
}

/* -----------------------      Function Definition     --------------------- */
