/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Header containing the include files required by the program
 * @file        includes.h
 */
/* -----------------------          Include Files       --------------------- */

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
#include <stdlib.h>                         // Standard Library
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "inc/hw_qei.h"                     // Macros used when accessing the QEI hardware
#include "inc/hw_pwm.h"                     // Defines and Macros for Pulse Width Modulation (PWM) ports
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/qei.h"                  // Prototypes for the Quadrature Encoder Driver
#include "driverlib/pwm.h"                  // API function prototypes for PWM ports
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#endif /* INCLUDES_H_ */
