/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing function prototypes for ADC configuration
 * @file        ADC_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

 /* -----------------------      Function Prototypes     --------------------- */
#ifndef ADC_CONFIG_H_
#define ADC_CONFIG_H_

 void ADC1_10_init(void);                           // Function for Initializing ADC1_10
 void ADC1_10_enable(void);                         // Function for Enabling ADC1_10

#endif /* ADC_CONFIG_H_ */
