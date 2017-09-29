/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Header containing function prototypes for QEI configuration
 * @file        QEI_config.h
 */
/* -----------------------          Include Files       --------------------- */
#include "includes.h"
#include "macros.h"
#include "Int_handlers.h"
#include "globalVariables.h"

/* -----------------------      Function Prototypes     --------------------- */
#ifndef QEI_CONFIG_H_
#define QEI_CONFIG_H_

void QEI1_init(void);                               // Function for Initializing QEI1 Peripheral
void QEI1_enable(void);                             // Function for Enabling QEI1 Peripheral

#endif /* QEI_CONFIG_H_ */
