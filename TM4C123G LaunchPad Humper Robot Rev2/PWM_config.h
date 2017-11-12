/*!
 * @author      Yash Bansod
 * @date        2nd November 2017
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

 void PWM0_5_init(void);                            // Function for Initializing PWM0_5
 void PWM0_5_enable(void);                          // Function for Enabling PWM0_5
 void PWM0_5_update(uint8_t index);                 // Function for Updating PWM0_5

 void PWM1_2_init(void);                            // Function for Initializing PWM1_2
 void PWM1_2_enable(void);                          // Function for Enabling PWM1_2
 void PWM1_2_update(uint8_t index);                 // Function for Updating PWM1_2

 void PWM0_3_init(void);                            // Function for Initializing PWM0_3
 void PWM0_3_enable(void);                          // Function for Enabling PWM0_3
 void PWM0_3_update(uint8_t index);                 // Function for Updating PWM0_3

 void PWM0_1_init(void);                            // Function for Initializing PWM0_1
 void PWM0_1_enable(void);                          // Function for Enabling PWM0_1
 void PWM0_1_update(uint8_t index);                 // Function for Updating PWM0_1

#endif /* PWM_CONFIG_H_ */
