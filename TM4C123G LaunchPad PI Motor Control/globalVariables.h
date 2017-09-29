/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Header containing the global variables used in the program
 * @file        golbalVariables.h
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types

#ifndef GLOBALVARIABLES_H_
#define GLOBALVARIABLES_H_

/* -----------------------      Global Variables        --------------------- */
static volatile uint32_t ui32Qei1Vel;               // Variable to store the velocity of QEI1
static volatile uint32_t ui32Qei1Pos;               // Variable to store the position of QEI1
static volatile int32_t i32Qei1Dir;                 // Variable to store the direction of QEI1
static volatile uint16_t ui16Qei1Rpm;               // Variable to store the RPM of QEI1

static volatile uint16_t ui16DesPosDeg = 355;       // Desired Shaft position in Degrees
static volatile uint32_t ui32DesPosTick;            // Desired Shaft position in ticks
static volatile int32_t i32Error = 0;               // Error in ticks
static volatile int32_t i32SumError = 0;            // Sum of Error in ticks

static uint32_t ui32Period_PWM1_0;                  // Variable to store PWM time period
static volatile uint16_t ui16Adjust_PWM1_0 = 0;     // Variable to store Duty Cycle of Servo
static volatile uint16_t ui16AdjustOld_PWM1_0 = 0;  // Variable to store Old Duty Cycle of Servo

#endif /* GLOBALVARIABLES_H_ */
