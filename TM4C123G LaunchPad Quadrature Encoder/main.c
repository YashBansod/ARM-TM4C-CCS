/*!
 * @author      Yash Bansod
 * @date        26th September 2017
 *
 * @brief       Quadrature Encoder
 * @details     The program interfaces the quadrature encoder peripheral to get the position
 *              and velocity information of a encoded DC motor. The QEI1 peripheral is used
 *              with PortC using PC4, PC5 and PC6 as Index1, PhA and PhB.
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              QEI1 Interrupts.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "inc/hw_qei.h"                     // Macros used when accessing the QEI hardware
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/qei.h"                  // Prototypes for the Quadrature Encoder Driver
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#define VEL_INT_FREQ    10000               // Macro to store the Interrupt frequency of QEI1
#define QEI1_PPR        257125              // Macro to store the PPR of the QEI1

/* -----------------------      Global Variables        --------------------- */
volatile uint32_t ui32Qei1Vel;              // Variable to store the velocity of QEI1
volatile uint32_t ui32Qei1Pos;              // Variable to store the position of QEI1
volatile int32_t i32Qei1Dir;                // Variable to store the direction of QEI1
volatile uint16_t ui16Qei1Rpm;              // Variable to store the RPM of QEI1

/* -----------------------      Function Prototypes     --------------------- */
void QEI1IntHandler(void);

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Enable the clock for peripherals PortC and QEI1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure the PC5, PC6 for QEI signals
    ROM_GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6);
    ROM_GPIOPinConfigure(GPIO_PC5_PHA1);
    ROM_GPIOPinConfigure(GPIO_PC6_PHB1);

    // Configure the QEI1 to increment for both PhA and PhB for quadrature input with "QEI1_PPR" PPR
    ROM_QEIConfigure(QEI1_BASE, QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_QUADRATURE, QEI1_PPR);
    // Configure the QEI1 for Velocity Calculation, Predivide by 1 at "VEL_INT_FREQ" Hz
    ROM_QEIVelocityConfigure(QEI1_BASE, QEI_VELDIV_1, ROM_SysCtlClockGet() / VEL_INT_FREQ);
    ROM_QEIVelocityEnable(QEI1_BASE);

    // Enable the Interrupts for Velocity Timer Expiration of QEI1
    void (*QEI1IntHandler_ptr)(void) = &QEI1IntHandler;
    QEIIntRegister(QEI1_BASE, *QEI1IntHandler_ptr);
    ROM_QEIIntEnable(QEI1_BASE, QEI_INTTIMER);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();
    // Enable the QEI1
    ROM_QEIEnable(QEI1_BASE);

    while (1);
}

/* -----------------------      Function Definition     --------------------- */
void QEI1IntHandler(void){
    // ISR for Quadrature Encoder Interface 1

    // Clear the Interrupt that is generated
    ROM_QEIIntClear(QEI1_BASE, ROM_QEIIntStatus(QEI1_BASE, true));
    // Calculate the number of quadrature ticks in "1 / VEL_INT_FREQ" time period
    ui32Qei1Vel = ROM_QEIVelocityGet(QEI1_BASE);
    // Update the position reading of the encoder
    ui32Qei1Pos = ROM_QEIPositionGet(QEI1_BASE);
    // Update the direction reading of the encoder
    i32Qei1Dir = ROM_QEIDirectionGet(QEI1_BASE);
    // Calculate the velocity in RPM
    ui16Qei1Rpm = ui32Qei1Vel * VEL_INT_FREQ * 60 / QEI1_PPR;
}
