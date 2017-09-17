#include "stdint.h"
#include "stdbool.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "sensorlib/hw_isl29023.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/isl29023.h"
#define  DEBUG

#define ISL29023_I2C_ADDRESS    0x44		// ISL29023 I2C address
tI2CMInstance g_sI2CInst;			// I2C master driver structure
tISL29023 g_sISL29023Inst;			// ISL29023 sensor driver structure
volatile unsigned long g_vui8DataFlag;	// Data ready flag
volatile unsigned long g_vui8ErrorFlag;	// Error flag

//*************************************************************************
void
ISL29023AppCallback(void *pvCallbackData, uint_fast8_t ui8Status)
{
	if(ui8Status == I2CM_STATUS_SUCCESS)
	{
		g_vui8DataFlag = 1;
	}
	g_vui8ErrorFlag = ui8Status;
}
//*************************************************************************

//*************************************************************************
void
ISL29023I2CIntHandler(void)
{
	I2CMIntHandler(&g_sI2CInst);
}
//*************************************************************************



//*************************************************************************
void
ISL29023AppErrorHandler(char *pcFilename, uint_fast32_t ui32Line)
{
	while(1)
	{
	}
}
//*************************************************************************

//*************************************************************************
void
ISL29023AppI2CWait(char *pcFilename, uint_fast32_t ui32Line)
{
	while((g_vui8DataFlag == 0) && (g_vui8ErrorFlag == 0))
	{
	}
	if(g_vui8ErrorFlag)
	{
		ISL29023AppErrorHandler(pcFilename, ui32Line);
	}
	g_vui8DataFlag = 0;
}
//*************************************************************************

//*************************************************************************
int
main(void)
{
	float fAmbient;
	uint8_t ui8Mask;

	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
	ROM_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
	ROM_GPIOPinConfigure(GPIO_PD1_I2C3SDA);
	GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
	ROM_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

	ROM_IntMasterEnable();

	I2CMInit(&g_sI2CInst, I2C3_BASE, INT_I2C3, 0xFF, 0xFF, ROM_SysCtlClockGet());

	SysCtlDelay(SysCtlClockGet() / 3);

	ISL29023Init(&g_sISL29023Inst, &g_sI2CInst,
			ISL29023_I2C_ADDRESS,ISL29023AppCallback, &g_sISL29023Inst);
	ISL29023AppI2CWait(__FILE__, __LINE__);

	ui8Mask = (ISL29023_CMD_I_OP_MODE_M );

	ISL29023ReadModifyWrite(&g_sISL29023Inst, ISL29023_O_CMD_I, ~ui8Mask,
			(ISL29023_CMD_I_OP_MODE_ALS_CONT),
			ISL29023AppCallback, &g_sISL29023Inst);

	ISL29023AppI2CWait(__FILE__, __LINE__);

	while(1)
	{
		ISL29023DataRead(&g_sISL29023Inst, ISL29023AppCallback, &g_sISL29023Inst);
		ISL29023AppI2CWait(__FILE__, __LINE__);

		ISL29023DataLightVisibleGetFloat(&g_sISL29023Inst, &fAmbient);
	}
}

