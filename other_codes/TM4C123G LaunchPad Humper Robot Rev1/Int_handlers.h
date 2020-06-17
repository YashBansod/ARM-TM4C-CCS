/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing prototypes for ISR
 * @file        Int_handlers.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"

/* -----------------------      Function Prototypes     --------------------- */
#ifndef INT_HANDLERS_H_
#define INT_HANDLERS_H_

void UART0IntHandler(void);                         // ISR Prototype for UART0 Interrupts
void ADC1_SS0IntHandler(void);                      // ISR Prototype for Timer0 Interrupts
void PortFIntHandler(void);                         // ISR Prototype for GPIOF Interrupts

#endif /* INT_HANDLERS_H_ */
