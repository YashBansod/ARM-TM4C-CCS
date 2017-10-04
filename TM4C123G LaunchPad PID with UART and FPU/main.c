/*!
 * @author      Yash Bansod
 * @date        5th October 2017
 *
 * @brief       PID with UART and FPU
 * @details     The program interfaces the QEI, PWM and UART peripherals to implement
 *              a PID Position control of an Encoded DC Motor. The Motor used is
 *              Faulhaber 12V(17W) Coreless motor, 64:1 Planetary gearbox, 120RPM,
 *              12 PPR Encoder. This gives 3072 CPR of quadrature. Also the FPU is used
 *              for the floating point calculations during PID.
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
#include "UART_config.h"

/* -----------------------      Global Variables        --------------------- */

char UARTinput[8];                          // Buffer to store input from UART

/* -----------------------      Function Prototypes     --------------------- */
void PID_Update(void);                      // Function prototype for PID control
void concat(char s1[], char s2[]);          // Function prototype for concatenation of strings

/* -----------------------          Main Program        --------------------- */
int main(void) {

    // Enable FPU Lazy Stacking.
    // This will enable skipping stacking of Floating point nos. during interrupts
    ROM_FPULazyStackingEnable();

    // Enable Floating point number
    ROM_FPUEnable();

    // Set the System clock to 80MHz and the PWM Module clock to 1.25 MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    i32DesPosTick = CENTER_POSITION + (i32DesPosDeg * QEI1_CPR / 3600);

    // Initialize the UART0 module
    UART0_init();

    // Initialize the QEI1 module
    QEI1_init();

    // Initialize the PWM1 module for output from generator 0
    PWM1_0_init();

    // Enable the UART0 peripheral
    UART0_enable();

    // Enable the QEI1 peripheral
    QEI1_enable();

    // Enable the PWM1 Generator0
    PWM1_0_enable();

    uint8_t ui8WelcomeText[] = {"Enter Motor Position in Degrees (input will be divided by 10):\n\r"};
    uint8_t limit = sizeof(ui8WelcomeText);
    uint8_t iter;
    for (iter = 0; iter<limit; iter++ ) ROM_UARTCharPut(UART0_BASE, ui8WelcomeText[iter]);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    while (true){

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
    ui16Qei1Rpm = (ui32Qei1Vel << VEL_INT_FREQ) * 60 / QEI1_CPR;
    // Update the PID loop
    PID_Update();
}

// ISR Definition for UART0 Interrupts
void UART0IntHandler(void){
    // Clear the asserted UART interrupts
    ROM_UARTIntClear(UART0_BASE, ROM_UARTIntStatus(UART0_BASE, true));

    // While there is a character available at input
    while(ROM_UARTCharsAvail(UART0_BASE)){
        // Echo the character back to the user
        int32_t reprint = ROM_UARTCharGetNonBlocking(UART0_BASE);
        char temp= (char)reprint;
        ROM_UARTCharPutNonBlocking(UART0_BASE, reprint);
        concat(UARTinput, &temp);
        if (temp == '\r'){
            i32DesPosDeg = strtoimax(UARTinput, NULL, 10);
            i32DesPosTick = CENTER_POSITION + (i32DesPosDeg * QEI1_CPR / 3600);
            UARTinput[0] = '\0';
            ROM_UARTCharPutNonBlocking(UART0_BASE, '\n');
        }
    }
}

// Function Definition for PID Control
void PID_Update(void){
    volatile float f32P_Control = 0;
    volatile float f32I_Control = 0;
    volatile float f32D_Control = 0;
    int32_t i32ErrorOld = i32ErrorNew;

    // Calculate the Error and SumError for PI control
    i32ErrorNew = i32DesPosTick - ui32Qei1Pos;
    f32P_Control = i32ErrorNew * K_P;

    i32DiffError = (i32ErrorNew - i32ErrorOld);
    f32D_Control = i32DiffError * K_D;

    // Do not Use I_Control if P_Control gives >= 100% of Control signal
    // Also Limit I_Control to contribute max 50% of Control signal
    if (abs(f32P_Control) < 100){
        i32SumError += i32ErrorNew;
        f32I_Control = i32SumError * K_I;

        if (f32I_Control > 50) {
            i32SumError -= i32ErrorNew;
            f32I_Control = 50;
        }
        else if (f32I_Control < -50) {
            i32SumError -= i32ErrorNew;
            f32I_Control = -50;
        }
    }

    if (abs(i32ErrorNew) <= 5){
        // Write the output value to the GPIO PortD to control the PD2 and PD3
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x00);
        i32SumError = 0;
    }
    else {
        // Adjust the PWM1_0 Duty Cycle according to P and I control.
        i16Adjust_PWM1_0 = (int16_t)(f32P_Control + f32I_Control + f32D_Control);

        if (i16Adjust_PWM1_0 < 0){
            i16Adjust_PWM1_0 = abs(i16Adjust_PWM1_0);
            // Write the output value to the GPIO PortD to control the PD2 and PD3
            ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3);
        }
        else
            // Write the output value to the GPIO PortD to control the PD2 and PD3
            ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_2);

        // Limit the Control signal to 100% Duty Cycle.
        if (i16Adjust_PWM1_0 > 100) i16Adjust_PWM1_0 = 100;

        // Update PWM Duty Cycle only if Duty Cycle has changed
        if (i16Adjust_PWM1_0 != i16AdjustOld_PWM1_0)
            // Set the PWM duty cycle to a specified value
            ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, i16Adjust_PWM1_0 * ui32Period_PWM1_0 / 100);
    }

    // Save the current calculated PWM Duty Cycle as old
    i16AdjustOld_PWM1_0 = i16Adjust_PWM1_0;
}

// Function to concatenate strings
void concat(char s1[], char s2[]) {
    int i, j;
    i = strlen(s1);
    for (j = 0; s2[j] != '\0'; i++, j++) {
        s1[i] = s2[j];
    }
    s1[i] = '\0';
}
