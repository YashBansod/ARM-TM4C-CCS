/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Source containing function definitions for QEI configuration
 * @file        QEI_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "QEI_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing QEI1 Peripheral
void QEI1_init(void){
    // Enable the clock for peripherals PortC and QEI1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);

    // Configure the PC5 and PC6 as PhaseA and PhaseB of QEI1
    ROM_GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6);
    ROM_GPIOPinConfigure(GPIO_PC5_PHA1);
    ROM_GPIOPinConfigure(GPIO_PC6_PHB1);

    // Configure the QEI1 to increment for both PhA and PhB for quadrature input with "QEI1_CPR" PPR
    ROM_QEIConfigure(QEI1_BASE, QEI_CONFIG_CAPTURE_A_B | QEI_CONFIG_QUADRATURE, QEI1_CPR);
    // Configure the QEI1 for Velocity Calculation, Predivide Velocity by 1 at "VEL_INT_FREQ" Hz
    ROM_QEIVelocityConfigure(QEI1_BASE, QEI_VELDIV_1, ROM_SysCtlClockGet() >> VEL_INT_FREQ);
    ROM_QEIVelocityEnable(QEI1_BASE);

    // Enable the Interrupts for Velocity Timer Expiration of QEI1
    void (*QEI1IntHandler_ptr)(void) = &QEI1IntHandler;
    QEIIntRegister(QEI1_BASE, *QEI1IntHandler_ptr);
    ROM_QEIIntEnable(QEI1_BASE, QEI_INTTIMER);
}

// Function for Enabling QEI1 Peripheral
void QEI1_enable(void){
    // Enable the QEI1
    ROM_QEIEnable(QEI1_BASE);
}
