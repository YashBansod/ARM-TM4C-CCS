/*!
 * @author      Yash Bansod
 * @date        12th November 2017
 *
 * @brief       Header containing the global variables used in the program
 * @file        golbalVariables.h
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                                 // Library of Standard Integer Types
#include <stdbool.h>                                // Library of Standard Boolean Types

#ifndef GLOBALVARIABLES_H_
#define GLOBALVARIABLES_H_

/* -----------------------      Global Variables        --------------------- */
static bool boolTrigCondition = 1;                  // Variable to control the Trigger Pin Switching
static uint32_t ui32EchoDuration[2] = {0, 0};       // Array to store duration for which Echo Pin is high
static int8_t i8EchoChange = 0;                     // Variable to store the change in Echo Duration

static uint32_t ui32Period_PWM;                     // Variable to store PWM time period

static int16_t i16Adjust_PWM0_5[2] = {60, 90};      // Array to store Duty Cycles of Servo_RightFront
static int16_t i16Adjust_PWM1_2[2] = {100, 70};     // Array to store Duty Cycles of Servo_RightBack
static int16_t i16Adjust_PWM0_3[2] = {70, 100};     // Array to store Duty Cycles of Servo_LeftBack
static int16_t i16Adjust_PWM0_1[2] = {90, 60};      // Array to store Duty Cycles of Servo_LeftFront

static uint8_t ui8ServoStatus[2] = {0x00, 0x00};    // Array to store current and old servo status
static uint8_t ui8TerminateStatus = 0x00;           // Variable to store the terminate condition

#endif /* GLOBALVARIABLES_H_ */
