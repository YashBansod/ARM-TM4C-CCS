/*!
 * @author      Yash Bansod
 * @date        29th September 2017
 *
 * @brief       Header containing the Macros used in the program
 * @file        macros.h
 */

#ifndef MACROS_H_
#define MACROS_H_
/* -----------------------      Macro Definitions       --------------------- */

#define VEL_INT_FREQ    12                          // Macro for Velocity Calculation at 2^N frequency
#define QEI1_CPR        3072                        // Macro to store the PPR of the QEI1
#define PWM_FREQUENCY   400                         // Macro for the frequency of PWM signal in Hz
#define K_P             0.25                        // Macro for Error Multiplication factor
#define K_I             0.002                       // Macro for SumError Multiplication factor
#define K_D             0.015                       // Macro for Error Difference Multiplication factor
#define UART0_BAUDRATE  115200                      // Macro for UART0 Baud rate
#define CENTER_POSITION 1073741823                  // Macro for Center Position of Motor Shaft

#endif /* MACROS_H_ */
