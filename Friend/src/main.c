/*
 Title: Bluetooth mesh project
 Author: Nachiket Kelkar
 File: main.c
 Date: 12rd April 2019
 Description: The main loop to get the bluetooth events.
 */
// The project skelton is the bluetooth mesh assignment 10.

#include <stdbool.h>
#include "native_gecko.h"
#include "log.h"
#include "display.h"
#include "gpio.h"
#include "letimer.h"
#include "gpiointerrupt.h"
#include "sensor.h"
#include "main.h"
#include <em_core.h>

extern void gecko_main_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);

bool display_update;
//uint32_t no_of_overflows = 0;
uint32_t external_evt;

int main(void)
{

  // Initialize stack
	gecko_main_init();
	ClkInit();
	letimer0_setup();
	gpioInit();
	logInit();
	displayInit();
	init_human_presence_sensors();
	SOUND_SENSOR_SETUP;
	LOG_INFO("started");


  /* Infinite loop */
  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
	}
  };
}


void LETIMER0_IRQHandler()
{
	volatile uint32_t interrupt_val;
	interrupt_val = LETIMER_IntGetEnabled(LETIMER0);
	LETIMER_IntClear(LETIMER0, interrupt_val);
	CORE_DECLARE_IRQ_STATE;
	if((interrupt_val & 0x04) == 0x04)
	{
		CORE_ENTER_CRITICAL();
		external_evt |= DISPLAY_UPDATE;
		CORE_EXIT_CRITICAL();
		gecko_external_signal(external_evt);
	}
}
