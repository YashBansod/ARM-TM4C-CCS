/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Source containing function definitions for ADC configuration
 * @file        ADC_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "ADC_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing ADC1_10
void ADC1_10_init(void){
    // Enable the Clock to the ADC1, PortF and Timer0 Peripherals
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Enable hardware averaging on ADC1
    ROM_ADCHardwareOversampleConfigure(ADC1_BASE, 64);
    // Configure to use ADC1, sample sequencer 0, processor to trigger sequence and use highest priority
    ROM_ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_TIMER, 0);
    // Configure PB4 as Analog Input Pin
    ROM_GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_4);
    // Configure all 8 steps on sequencer 0 to sample temperature sensor
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH10);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH10);
    // Mark as last conversion on sequencer 0 and enable interrupt flag generation on sampling completion
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH10|ADC_CTL_IE|ADC_CTL_END);

    // Configure Timer0 to run in periodic mode
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC);
    // Load the Timer with the calculated period (= 500ms).
    uint32_t ui32Period = ROM_SysCtlClockGet() >> 1;
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
    // Enable triggering
    ROM_TimerControlTrigger(TIMER0_BASE, TIMER_A, true);
}

// Function for Enabling ADC1_10
void ADC1_10_enable(void){
    // Enable Interrupt for ADC1SS0 (also do the same for NVIC)
    ROM_ADCIntClear(ADC1_BASE, 0);
    ROM_IntEnable(INT_ADC1SS0);
    ROM_ADCIntEnable(ADC1_BASE, 0);
    // Enable Sequencer 0
    ROM_ADCSequenceEnable(ADC1_BASE, 0);
    // Enable the Timer
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}
