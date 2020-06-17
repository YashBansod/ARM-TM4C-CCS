/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing function prototypes for UART configuration
 * @file        UART_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

/* -----------------------      Function Prototypes     --------------------- */
#ifndef UART_CONFIG_H_
#define UART_CONFIG_H_

void UART0_init(void);                              // Function for Initializing UART0 Peripheral
void UART0_enable(void);                            // Function for Enabling UART0 Peripheral

#endif /* UART_CONFIG_H_ */
