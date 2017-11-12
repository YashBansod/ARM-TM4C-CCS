/*!
 * @author      Yash Bansod
 * @date        12th November 2017
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
#include <string.h>                         // Library for String functions
#include <inttypes.h>                       // Library for conversions to Integer types
#include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "driverlib/timer.h"                // Defines and macros for Timer API of driverLib
#include "driverlib/adc.h"                  // Defines and macros for ADC API of driverLib
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/pwm.h"                  // API function prototypes for PWM ports
#include "driverlib/uart.h"                 // Defines and Macros for the UART
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#endif /* INCLUDES_H_ */
