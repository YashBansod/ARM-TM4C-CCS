/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Header containing function prototypes for PWM configuration
 * @file        PWM_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

 /* -----------------------      Function Prototypes     --------------------- */
#ifndef PWM_CONFIG_H_
#define PWM_CONFIG_H_

 void PWM1_0_init(void);                            // Function for Initializing PWM1_0
 void PWM1_0_enable(void);                          // Function for Enabling PWM1_0

#endif /* PWM_CONFIG_H_ */
