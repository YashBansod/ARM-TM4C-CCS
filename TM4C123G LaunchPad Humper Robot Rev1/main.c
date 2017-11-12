/*!
 * @author      Yash Bansod
 * @date        12nd November 2017
 *
 * @brief       Humper Robot Rev1
 * @details     This is the main file. This initializes and enables the
 *              various peripherals and defines the interrupt handlers.
 *              The Humper Robot is a Quadrupled Robot (4 x MG995 Servos)
 *              with a Range Sensor (Sharp GP2Y0A41SK0F) for measuring its
 *              distance from a wall.
 *
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller.
 * @file        main.c
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "globalVariables.h"
#include "Int_handlers.h"
#include "PWM_config.h"
#include "UART_config.h"
#include "ADC_config.h"
#include "GPIO_config.h"

/* -----------------------      Global Variables        --------------------- */

/* -----------------------      Function Prototypes     --------------------- */

/* -----------------------          Main Program        --------------------- */
int main(void) {

    // Set the System clock to 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    // Initialize the ADC1 module
    ADC1_10_init();
    // Initialize the UART0 module
    UART0_init();
    // Initialize the PWM0 module for output from PIN5
    PWM0_5_init();
    // Initialize the PWM1 module for output from PIN2
    PWM1_2_init();
    // Initialize the PWM0 module for output from PIN3
    PWM0_3_init();
    // Initialize the PWM0 module for output from PIN1
    PWM0_1_init();
    // Initialize the Onboard Buttons
    BUTTON_init();

    // Enable the ADC1 peripheral
    ADC1_10_enable();
    // Enable the UART0 peripheral
    UART0_enable();
    // Enable the PWM0 PIN5
    PWM0_5_enable();
    // Enable the PWM1 PIN2
    PWM1_2_enable();
    // Enable the PWM0 PIN3
    PWM0_3_enable();
    // Enable the PWM0 PIN1
    PWM0_1_enable();
    // Enable the Onboard Buttons
    BUTTON_enable();

    // Start an infinite loop
    while (true);
}

/* -----------------------      Function Definition     --------------------- */

void UART0IntHandler(void){
    // ISR Definition for UART0 Interrupts
    // Clear the asserted UART interrupts
    ROM_UARTIntClear(UART0_BASE, ROM_UARTIntStatus(UART0_BASE, true));

    // Disable the Timer
    ROM_TimerDisable(TIMER0_BASE, TIMER_A);

    // Glow the Blue LED
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_2);

    // While there is a character available at input
    while(ROM_UARTCharsAvail(UART0_BASE)){
        // Save the current Servo status as previous Servo status
        ui8ServoStatus[0] = ui8ServoStatus[1];
        // Update the current Servo status
        ui8ServoStatus[1] = (uint8_t)ROM_UARTCharGet(UART0_BASE);
    }

    // Convert the UART command message into motor control commands
    uint8_t ui8MotorControl[4];
    ui8MotorControl[0] = (ui8ServoStatus[1] & 0b00001000) >> 3;
    ui8MotorControl[1] = (ui8ServoStatus[1] & 0b00000100) >> 2;
    ui8MotorControl[2] = (ui8ServoStatus[1] & 0b00000010) >> 1;
    ui8MotorControl[3] = (ui8ServoStatus[1] & 0b00000001);

    // Update the PWM duty cycles of the motors
    PWM1_2_update(ui8MotorControl[0]);
    PWM0_5_update(ui8MotorControl[1]);
    PWM0_1_update(ui8MotorControl[2]);
    PWM0_3_update(ui8MotorControl[3]);

    // Load the Timer with the calculated period (= 500ms)
    // This is required to allow the Servo to actuate
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() >> 1) - 1);

    // Enable the Timer
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

void ADC1_SS0IntHandler(void){
    // The ISR for ADC1SS0 Interrupt Handling
    // Clear the ADC Interrupt (if any generated) for Sequencer 0
    ROM_ADCIntClear(ADC1_BASE, 0);

    // Update the previous ADC value
    i32ADCAvg[0] = i32ADCAvg[1];

    // Store the values in sequencer 0 of ADC1 to an Array
    ROM_ADCSequenceDataGet(ADC1_BASE, 0, ui32ADC1Val);
    // Calculate the Average of the Readings
    i32ADCAvg[1] = (ui32ADC1Val[0] + ui32ADC1Val[1] + ui32ADC1Val[2] + ui32ADC1Val[3] + ui32ADC1Val[4] + ui32ADC1Val[5] + ui32ADC1Val[6] + ui32ADC1Val[7] + 4)/8;
    // Discard any fluctuations of +- 16 in the Average Value of ADC
    i32ADCAvg[1] = i32ADCAvg[1] >> 5;

    // Calculate the change in the ADC Value since the past Timer Elapse
    i8ADCChange = (int8_t)((i32ADCAvg[1] - i32ADCAvg[0]));
    if (abs(i8ADCChange) > 10) i8ADCChange = 0;

    // Transmit the UART Status messages to the computer
    ROM_UARTCharPut(UART0_BASE, ui8ServoStatus[0]);
    ROM_UARTCharPut(UART0_BASE, ui8ServoStatus[1]);
    ROM_UARTCharPut(UART0_BASE, i8ADCChange);
    ROM_UARTCharPut(UART0_BASE, ui8TerminateStatus);
    ROM_UARTCharPut(UART0_BASE, '\r');
    ROM_UARTCharPut(UART0_BASE, '\n');

    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);
}

void PortFIntHandler(void){
    // The ISR for GPIO PortF Interrupt Handling
    GPIOIntClear(GPIO_PORTF_BASE , GPIO_INT_PIN_4 | GPIO_INT_PIN_0);

    // If SW1(PF4) is pressed then reduce the duty cycle
    if(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_0){

        // Disable the Timer
        ROM_TimerDisable(TIMER0_BASE, TIMER_A);

        // Glow the Green LED
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_3);

        // Save the current Servo command and invert it
        ui8ServoStatus[0] = ui8ServoStatus[1];
        ui8ServoStatus[1] = (~ui8ServoStatus[1]) & 0x0F;

        // Convert the Servo command message into motor control commands
        uint8_t ui8MotorControl[4];
        ui8MotorControl[0] = (ui8ServoStatus[1] & 0b00001000) >> 3;
        ui8MotorControl[1] = (ui8ServoStatus[1] & 0b00000100) >> 2;
        ui8MotorControl[2] = (ui8ServoStatus[1] & 0b00000010) >> 1;
        ui8MotorControl[3] = (ui8ServoStatus[1] & 0b00000001);

        // Update the PWM duty cycles of the motors
        PWM1_2_update(ui8MotorControl[0]);
        PWM0_5_update(ui8MotorControl[1]);
        PWM0_1_update(ui8MotorControl[2]);
        PWM0_3_update(ui8MotorControl[3]);

        // Load the Timer with the calculated period (= 500ms)
        // This is required to allow the Servo to actuate
        ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() >> 1) - 1);

        // While the SW1 is pressed, stall the program
        while(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_0);
        // Turn off the LED
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);

        // Enable the Timer
        ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    }
    // If SW2(PW0) is pressed then increase the duty cycle
    if(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_4){

        // Disable the Timer
        ROM_TimerDisable(TIMER0_BASE, TIMER_A);

        // Glow the Red LED
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, GPIO_PIN_1);

        // Modify the terminate variable to send a terminate condition
        ui8TerminateStatus = 0xFF;

        // Transmit the UART Status messages to the computer
        ROM_UARTCharPut(UART0_BASE, ui8ServoStatus[0]);
        ROM_UARTCharPut(UART0_BASE, ui8ServoStatus[1]);
        ROM_UARTCharPut(UART0_BASE, i8ADCChange);
        ROM_UARTCharPut(UART0_BASE, ui8TerminateStatus);
        ROM_UARTCharPut(UART0_BASE, '\r');
        ROM_UARTCharPut(UART0_BASE, '\n');

        // Load the Timer with the calculated period (= 500ms)
        // This is required to allow the Servo to actuate
        ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() >> 1) - 1);

        // While the SW2 is pressed, stall the program
        while(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_4);
        // Turn off the LED
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);

        // Enable the Timer
        ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    }
}
