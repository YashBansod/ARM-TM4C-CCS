/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       PI Motor Control
 * @details     The program interfaces the QEI and PWM peripherals to implement
 *              a PI Position control of an Encoded DC Motor. The Motor used is
 *              Faulhaber 12V(17W) Coreless motor, 64:1 Planetary gearbox, 120RPM,
 *              12 PPR Encoder. This gives 3072 CPR of quadrature.
 *
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              QEI1 Interrupts.
 *              The Tuning is done by trial and error. For better results ZN tuning
 *              can also be implemented.
 *              For controlling the desired position of Motor, Constants for Proportional
 *              and Integral controller, see "macros.h"
 * @file        main.c
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "globalVariables.h"
#include "Int_handlers.h"
#include "QEI_config.h"
#include "PWM_config.h"

/* -----------------------      Global Variables        --------------------- */
/* -----------------------      Function Prototypes     --------------------- */
void PID_Update(void);

/* -----------------------          Main Program        --------------------- */
int main(void) {

    // Set the System clock to 80MHz and the PWM Module clock to 1.25 MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    // Initialize the QEI1 module
    QEI1_init();

    // Initialize the PWM1 module for output from generator 0
    PWM1_0_init();

    // Enable the QEI1 peripheral
    QEI1_enable();

    // Enable the PWM1 Generator0
    PWM1_0_enable();

    while (true){
        ui32DesPosTick = ui16DesPosDeg * QEI1_CPR / 360;
    }
}
/* -----------------------      Function Definition     --------------------- */
// ISR Definition for QEI1 Interrupts
void QEI1IntHandler(void){
    // Clear the Interrupt that is generated
    ROM_QEIIntClear(QEI1_BASE, ROM_QEIIntStatus(QEI1_BASE, true));
    // Calculate the number of quadrature ticks in "1 / VEL_INT_FREQ" time period
    ui32Qei1Vel = ROM_QEIVelocityGet(QEI1_BASE);
    // Update the position reading of the encoder
    ui32Qei1Pos = ROM_QEIPositionGet(QEI1_BASE);
    // Update the direction reading of the encoder
    i32Qei1Dir = ROM_QEIDirectionGet(QEI1_BASE);
    // Calculate the velocity in RPM
    ui16Qei1Rpm = ui32Qei1Vel * VEL_INT_FREQ * 60 / QEI1_CPR;
    // Update the PID loop
    PID_Update();
}

void PID_Update(void){
    volatile uint32_t ui32P_Control = 0;
    volatile uint32_t ui32I_Control = 0;

    // Calculate the Error and SumError for PI control
    i32Error = ui32DesPosTick - ui32Qei1Pos;
    ui32P_Control = abs(i32Error) >> K_P;

    // Do not Use I_Control if P_Control gives >= 100% of Control signal
    // Also Limit I_Control to contribute max 50% of Control signal
    if (ui32P_Control < 100){
        if (ui32I_Control <= 50){
            i32SumError += i32Error;
            ui32I_Control = abs(i32SumError) >> K_I;
        }else{
            i32SumError -= i32Error;
            ui32I_Control = abs(i32SumError) >> K_I;
        }
    }

    // Configure the Motor Direction according to the Error
    if (i32Error > 5)
        // Write the output value to the GPIO PortD to control the PD2 and PD3
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_2);
    else if (i32Error <= 5 && i32Error >= -5){
        // Write the output value to the GPIO PortD to control the PD2 and PD3
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x00);
        i32SumError = 0;
    }else
        // Write the output value to the GPIO PortD to control the PD2 and PD3
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3);


    // Adjust the PWM1_0 Duty Cycle according to P and I control.
    ui16Adjust_PWM1_0 = ui32P_Control + ui32I_Control;

    // Limit the Control signal to 100% Duty Cycle.
    if (ui16Adjust_PWM1_0 > 100) ui16Adjust_PWM1_0 = 100;

    // Update PWM Duty Cycle only if Duty Cycle has changed
    if (ui16Adjust_PWM1_0 != ui16AdjustOld_PWM1_0)
        // Set the PWM duty cycle to a specified value
        ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui16Adjust_PWM1_0 * ui32Period_PWM1_0 / 100);


    // Save the current calculated PWM Duty Cycle as old
    ui16AdjustOld_PWM1_0 = ui16Adjust_PWM1_0;
}
