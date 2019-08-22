/*
 * gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>

void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);
void MotionDetected();

#define BUTTON0_PORT 		gpioPortF
#define BUTTON0_PIN 		6
#define BUTTON1_PORT 		gpioPortF
#define BUTTON1_PIN 		7
#define MOTION_PORT         gpioPortC
#define MOTION_PIN          11
#define SENSOR_PORT         gpioPortC
#define SENSOR_PIN          10

/* Additional macros for user friendly calling of function */
#define LCD_ENABLE            GPIO_PinOutSet(gpioPortD, 15)
#define LCD_DISABLE           GPIO_PinOutClear(gpioPortD, 15)
#define DISPLAY_EXTCOMIN_HIGH GPIO_PinOutSet(gpioPortD, 13)
#define DISPLAY_EXTCOMIN_LOW  GPIO_PinOutClear(gpioPortD, 13)
#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 	1
#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED		1
#define SCHEDULER_SUPPORTS_DISPLAY_UPDATE_EVENT 1
#define TIMER_SUPPORTS_1HZ_TIMER_EVENT          1

#define SET_BUTTON0_INPUT     GPIO_PinModeSet(gpioPortF, 6, gpioModeInputPull, 1);
#define SET_BUTTON1_INPUT     GPIO_PinModeSet(gpioPortF, 7, gpioModeInputPull, 1);

#define SOUND_SENSOR_SETUP   GPIO_PinModeSet(SENSOR_PORT, SENSOR_PIN, gpioModePushPull, false);
#define SOUND_SENSOR_ENABLE  GPIO_PinOutSet(SENSOR_PORT, SENSOR_PIN)
#define SOUND_SENSOR_DISABLE GPIO_PinOutSet(SENSOR_PORT, SENSOR_PIN)

#endif /* SRC_GPIO_H_ */
