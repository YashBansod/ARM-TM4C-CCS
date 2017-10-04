/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Source containing function definitions for PWM configuration
 * @file        PWM_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "PWM_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing PWM1_0
void PWM1_0_init(void){
    // Set the PWM Clock as System Clock / 64
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the clock for peripherals PortD and PWM1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // Configure the PD0 for PWM signal (PWM module 1 generator 0)
    ROM_GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    ROM_GPIOPinConfigure(GPIO_PD0_M1PWM0);

    // Configure the PD1 and PD2 pins as Digital Output Pins
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    // Write the output value to the GPIO PortD to control the PD2 and PD3
    ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0x00);

    // Calculate the Timer period of the PWM Module.
    uint32_t ui32PWMClock = ROM_SysCtlClockGet() >> 6;
    ui32Period_PWM1_0 = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Configure thE PWM1 Genrator0 to work in Count Down Mode
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    // Load the calculated time period to the Generator0 of the PWM1 Module
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Period_PWM1_0);

    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, i16Adjust_PWM1_0 * ui32Period_PWM1_0 / 100);
    // Enable the PWM0 pin of the PWM Module 1 as output
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);

}

// Function for Enabling PWM1_0
void PWM1_0_enable(void){
    // Enable the PWM1 Generator0
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);
}
