/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Source containing function definitions for PWM configuration
 * @file        PWM_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "PWM_config.h"

/* -----------------------      Function Definition     --------------------- */

// Function for Initializing PWM0_5
void PWM0_5_init(void){
    // Set the PWM Clock as System Clock / 64
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the clock for peripherals PortE and PWM0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configure the PE5 for PWM signal (PWM module 0 generator 2)
    ROM_GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_5);
    ROM_GPIOPinConfigure(GPIO_PE5_M0PWM5);

    // Calculate the Timer period of the PWM Module.
    uint32_t ui32PWMClock = ROM_SysCtlClockGet() >> 6;
    ui32Period_PWM = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Configure thE PWM0 Genrator2 to work in Count Down Mode
    ROM_PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    // Load the calculated time period to the Generator2 of the PWM0 Module
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, ui32Period_PWM);

    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, i16Adjust_PWM0_5[1] * ui32Period_PWM / 1000);
    // Enable the PWM5 pin of the PWM Module 0 as output
    ROM_PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);

}

// Function for Enabling PWM0_5
void PWM0_5_enable(void){
    // Enable the PWM0 Generator2
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_2);
}

// Function for Updating PWM0_5
void PWM0_5_update(uint8_t index){
    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, i16Adjust_PWM0_5[index] * ui32Period_PWM / 1000);
}

/*!-------------------------------------------------------------------------------------------------*/
/*!-------------------------------------------------------------------------------------------------*/
/*!-------------------------------------------------------------------------------------------------*/

// Function for Initializing PWM1_0
void PWM1_2_init(void){
    // Set the PWM Clock as System Clock / 64
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the clock for peripherals PortE and PWM1
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configure the PE4 for PWM signal (PWM module 1 generator 1)
    ROM_GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4);
    ROM_GPIOPinConfigure(GPIO_PE4_M1PWM2);

    // Calculate the Timer period of the PWM Module.
    uint32_t ui32PWMClock = ROM_SysCtlClockGet() >> 6;
    ui32Period_PWM = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Configure thE PWM1 Genrator1 to work in Count Down Mode
    ROM_PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    // Load the calculated time period to the Generator1 of the PWM1 Module
    ROM_PWMGenPeriodSet(PWM1_BASE, PWM_GEN_1, ui32Period_PWM);

    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, i16Adjust_PWM1_2[1] * ui32Period_PWM / 1000);
    // Enable the PWM2 pin of the PWM Module 1 as output
    ROM_PWMOutputState(PWM1_BASE, PWM_OUT_2_BIT, true);

}

// Function for Enabling PWM1_2
void PWM1_2_enable(void){
    // Enable the PWM1 Generator1
    ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_1);
}

// Function for Updating PWM1_2
void PWM1_2_update(uint8_t index){
    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2, i16Adjust_PWM1_2[index] * ui32Period_PWM / 1000);
}

/*!-------------------------------------------------------------------------------------------------*/
/*!-------------------------------------------------------------------------------------------------*/
/*!-------------------------------------------------------------------------------------------------*/

// Function for Initializing PWM0_3
void PWM0_3_init(void){
    // Set the PWM Clock as System Clock / 64
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the clock for peripherals PortB and PWM0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the PB6 for PWM signal (PWM module 0 generator 1)
    ROM_GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);
    ROM_GPIOPinConfigure(GPIO_PB5_M0PWM3);

    // Calculate the Timer period of the PWM Module.
    uint32_t ui32PWMClock = ROM_SysCtlClockGet() >> 6;
    ui32Period_PWM = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Configure thE PWM0 Genrator1 to work in Count Down Mode
    ROM_PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN);
    // Load the calculated time period to the Generator1 of the PWM0 Module
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, ui32Period_PWM);

    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, i16Adjust_PWM0_3[1] * ui32Period_PWM / 1000);
    // Enable the PWM3 pin of the PWM Module 0 as output
    ROM_PWMOutputState(PWM0_BASE, PWM_OUT_3_BIT, true);

}

// Function for Enabling PWM0_3
void PWM0_3_enable(void){
    // Enable the PWM0 Generator1
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_1);
}

// Function for Updating PWM0_3
void PWM0_3_update(uint8_t index){
    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, i16Adjust_PWM0_3[index] * ui32Period_PWM / 1000);
}

/*!-------------------------------------------------------------------------------------------------*/
/*!-------------------------------------------------------------------------------------------------*/
/*!-------------------------------------------------------------------------------------------------*/

// Function for Initializing PWM0_1
void PWM0_1_init(void){
    // Set the PWM Clock as System Clock / 64
    ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the clock for peripherals PortB and PWM0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the PB7 for PWM signal (PWM module 0 generator 0)
    ROM_GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_7);
    ROM_GPIOPinConfigure(GPIO_PB7_M0PWM1);

    // Calculate the Timer period of the PWM Module.
    uint32_t ui32PWMClock = ROM_SysCtlClockGet() >> 6;
    ui32Period_PWM = (ui32PWMClock / PWM_FREQUENCY) - 1;
    // Configure thE PWM0 Genrator0 to work in Count Down Mode
    ROM_PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    // Load the calculated time period to the Generator0 of the PWM0 Module
    ROM_PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ui32Period_PWM);

    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, i16Adjust_PWM0_1[1] * ui32Period_PWM / 1000);
    // Enable the PWM1 pin of the PWM Module 0 as output
    ROM_PWMOutputState(PWM0_BASE, PWM_OUT_1_BIT, true);

}

// Function for Enabling PWM0_1
void PWM0_1_enable(void){
    // Enable the PWM0 Generator0
    ROM_PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

// Function for Updating PWM0_1
void PWM0_1_update(uint8_t index){
    // Set the PWM duty cycle to a specified value
    ROM_PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, i16Adjust_PWM0_1[index] * ui32Period_PWM / 1000);
}
