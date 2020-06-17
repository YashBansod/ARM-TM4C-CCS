/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing function prototypes for GPIO configuration
 * @file        GPIO_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

/* -----------------------      Function Prototypes     --------------------- */
#ifndef GPIO_CONFIG_H_
#define GPIO_CONFIG_H_

void BUTTON_init(void);                           // Function for Initializing PF0 and PF4
void BUTTON_enable(void);                         // Function for Enabling PF0 and PF4

#endif /* GPIO_CONFIG_H_ */
