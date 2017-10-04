/*!
 * @author      Yash Bansod
 * @date        29th September 2017
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

void QEI1IntHandler(void);                          // ISR Prototype for QEI1 Interrupts
void UART0IntHandler(void);                          // ISR Prototype for UART0 Interrupts

#endif /* INT_HANDLERS_H_ */
