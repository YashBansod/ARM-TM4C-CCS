#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


#define PWM_FREQUENCY 55

int main(void)
{
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	volatile uint8_t ui8Adjust;
	ui8Adjust = 83;

	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	ROM_SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	ROM_GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
	ROM_GPIOPinConfigure(GPIO_PD0_M1PWM0);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);

	ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
	ROM_PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
	ROM_PWMGenEnable(PWM1_BASE, PWM_GEN_0);

	while(1)
	{

		if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00)
		{
			ui8Adjust--;
			if (ui8Adjust < 56)
			{
				ui8Adjust = 56;
			}
			ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
		}

		if(ROM_GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00)
		{
			ui8Adjust++;
			if (ui8Adjust > 111)
			{
				ui8Adjust = 111;
			}
			ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
		}

		ROM_SysCtlDelay(100000);
	}

}
