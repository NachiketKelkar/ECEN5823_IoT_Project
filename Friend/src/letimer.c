/*
 * letimer.c
 *
 *  Created on: Apr 12, 2019
 *      Author: nachi
 */



#include "letimer.h"
#include "main.h"


/*
 * Function name: letimer0_setup();
 * Description: Function initializes the LETIMER0 with comp0Top mode,
 * calculates the top value and enables the timer and global interrupts
 * and starts the timer
 * @param: void
 * @return: void
 */
void letimer0_setup()
{
	LETIMER_Init_TypeDef letimer0_config;

	letimer0_config.bufTop 	 = false;
	letimer0_config.comp0Top = true;
	letimer0_config.debugRun = false;
	letimer0_config.enable   = false;
	letimer0_config.out0Pol  = 0;
	letimer0_config.out1Pol  = 0;
	letimer0_config.repMode  = letimerRepeatFree ;
	letimer0_config.ufoa0    = letimerUFOANone;
	letimer0_config.ufoa1    = letimerUFOANone;

	LETIMER_Init(LETIMER0, &letimer0_config);
	LETIMER_CompareSet(LETIMER0, 0, PERIOD_IN_MSEC);
	LETIMER_IntEnable (LETIMER0, LETIMER_IF_UF);
	NVIC_EnableIRQ(LETIMER0_IRQn);
	LETIMER_Enable(LETIMER0, true);
 }

void ClkInit()
{
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_ClockDivSet(cmuClock_HF, cmuClkDiv_1);
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_1);
	CMU_ClockEnable(cmuClock_I2C0, true);

	CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_LFA ,cmuSelect_ULFRCO);
	CMU_ClockEnable(cmuClock_LFA, true);
	CMU_ClockDivSet(cmuClock_LETIMER0, cmuClkDiv_1);
	CMU_ClockEnable(cmuClock_LETIMER0, true);
}
