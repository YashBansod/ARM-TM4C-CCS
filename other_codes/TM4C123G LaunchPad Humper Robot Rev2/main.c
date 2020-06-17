/*!
 * @author      Yash Bansod
 * @date        12nd November 2017
 *
 * @brief       Humper Robot Rev2
 *
 * @details     This is the main file. This initializes and enables the
 *              various peripherals and defines the interrupt handlers.
 *              The Humper Robot is a Quadrupled Robot (4 x MG995 Servos)
 *              with a Range Sensor (HC-SR04) for measuring its
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
#include "GPIO_config.h"
#include "TIMER_config.h"
#include "ULTRASONIC_config.h"

/* -----------------------      Global Variables        --------------------- */

/* -----------------------      Function Prototypes     --------------------- */

/* -----------------------          Main Program        --------------------- */
int main(void) {

    // Set the System clock to 80MHz
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Master interrupt enable API for all interrupts
    ROM_IntMasterEnable();

    // Initialize the ULTRASONIC module
    ULTRASONIC_init();
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
    // Initialize the Timer0
    TIMER0_init();


    // Enable the ULTRASONIC peripheral
    ULTRASONIC_enable();
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
    // Enable the Timer0
    TIMER0_enable();

    // Start an infinite loop
    while (true);
}

/* -----------------------      Function Definition     --------------------- */
void Timer0IntHandler(void){
    // The ISR for Timer0 Interrupt Handling
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Load the Timer with value for generating a  delay of 10 uS.
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, (ROM_SysCtlClockGet() / 100000) -1);
    // Make the Trigger Pin (PA3) High
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
    // Enable the Timer0 to cause an interrupt when timeout occurs
    ROM_TimerEnable(TIMER1_BASE, TIMER_A);
}

void Timer1IntHandler(void){
    // The ISR for Timer1 Interrupt Handling
    // Clear the timer interrupt
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    // Disable the timer
    ROM_TimerDisable(TIMER1_BASE, TIMER_A);
    // Make the Trigger Pin (PA3) Low
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x00);
}

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

    // Load the Timer with the calculated period (= 400ms)
    // This is required to allow the Servo to actuate
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() * 4 / 10) - 1);

    // Enable the Timer
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
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

        // Load the Timer with the calculated period (= 400ms)
        // This is required to allow the Servo to actuate
        ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() * 4 / 10) - 1);

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
        ROM_UARTCharPut(UART0_BASE, i8EchoChange);
        ROM_UARTCharPut(UART0_BASE, ui8TerminateStatus);
        ROM_UARTCharPut(UART0_BASE, '\r');
        ROM_UARTCharPut(UART0_BASE, '\n');

        // Load the Timer with the calculated period (= 400ms)
        // This is required to allow the Servo to actuate
        ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, (ROM_SysCtlClockGet() * 4 /10) - 1);

        // While the SW2 is pressed, stall the program
        while(ROM_GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0)== GPIO_PIN_4);
        // Turn off the LED
        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);

        // Enable the Timer
        ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    }
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
        // Update the previous value of the Echo Duration
        ui32EchoDuration[0] = ui32EchoDuration[1];
        // Update the current value of the Echo Duration
        ui32EchoDuration[1] = ROM_TimerValueGet(TIMER2_BASE, TIMER_A);
        ui32EchoDuration[1] = ui32EchoDuration[1] >> 12;

        // Calculate the Change in Echo Duration
        i8EchoChange = (int8_t)(ui32EchoDuration[0] - ui32EchoDuration[1]);
        if(abs(i8EchoChange) > 10) i8EchoChange = 0;
        // Disable Timer2 to stop measuring duration for which Echo Pin is High
        ROM_TimerDisable(TIMER2_BASE, TIMER_A);

        // Transmit the UART Status messages to the computer
        ROM_UARTCharPut(UART0_BASE, ui8ServoStatus[0]);
        ROM_UARTCharPut(UART0_BASE, ui8ServoStatus[1]);
        ROM_UARTCharPut(UART0_BASE, i8EchoChange);
        ROM_UARTCharPut(UART0_BASE, ui8TerminateStatus);
        ROM_UARTCharPut(UART0_BASE, '\r');
        ROM_UARTCharPut(UART0_BASE, '\n');

        ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0x00);
    }

}
