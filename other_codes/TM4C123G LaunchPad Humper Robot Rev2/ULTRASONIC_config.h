/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing function prototypes for ULTRASONIC configuration
 * @file        ULTRASONIC_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

 /* -----------------------      Function Prototypes     --------------------- */
#ifndef ULTRASONIC_CONFIG_H_
#define ULTRASONIC_CONFIG_H_

 void ULTRASONIC_init(void);                           // Function for Initializing ULTRASONIC
 void ULTRASONIC_enable(void);                         // Function for Enabling ULTRASONIC

#endif /* ULTRASONIC_CONFIG_H_ */
