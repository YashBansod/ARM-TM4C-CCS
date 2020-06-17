/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Source containing function definitions for UART configuration
 * @file        UART_config.c
 */
/* -----------------------          Include Files       --------------------- */
#include "UART_config.h"

/* -----------------------      Function Definition     --------------------- */
// Function for Initializing UART0 Peripheral
void UART0_init(void){
    // Enable Clock to UART0 and GPIO PortA peripherals
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure PA0 as UART0_Rx and PA1 as UART0_Tx
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the baud rate and data setup for the UART0
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), UART0_BAUDRATE, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE );
}

// Function for Enabling UART0 Peripheral
void UART0_enable(void){
    // Configure and enable the interrupt for UART0
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    // Enable the UART0 peripheral
    ROM_UARTEnable(UART0_BASE);
}
