//*****************************************************************************
//
// touch.h - Prototypes for the touch screen driver.
//
//*****************************************************************************

#ifndef __TOUCH_H__
#define __TOUCH_H__

//*****************************************************************************
//
// The lowest ADC reading assumed to represent a press on the screen.  Readings
// below this indicate no press is taking place.
//
//*****************************************************************************
#define TOUCH_MIN 150

//*****************************************************************************
//
// Prototypes for the functions exported by the touch screen driver.
//
//*****************************************************************************
extern volatile short g_sTouchX;
extern volatile short g_sTouchY;
extern short g_sTouchMin;
extern void TouchScreenIntHandler(void);
extern void TouchScreenInit(void);
extern void TouchScreenCallbackSet(int32_t (*pfnCallback)(uint32_t ulMessage,
                                                       int32_t lX, int32_t lY));

#endif // __TOUCH_H__
