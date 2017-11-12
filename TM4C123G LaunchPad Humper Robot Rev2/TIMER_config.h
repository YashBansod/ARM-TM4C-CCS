/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing function prototypes for TIMER configuration
 * @file        TIMER_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

/* -----------------------      Function Prototypes     --------------------- */
#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_

void TIMER0_init(void);                              // Function for Initializing TIMER0 Peripheral
void TIMER0_enable(void);                            // Function for Enabling TIMER0 Peripheral

#endif /* TIMER_CONFIG_H_ */
