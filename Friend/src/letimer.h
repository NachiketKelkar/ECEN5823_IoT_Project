/*
 * letimer.h
 *
 *  Created on: Apr 12, 2019
 *      Author: nachi
 */

#ifndef SRC_LETIMER_H_
#define SRC_LETIMER_H_


#include <stdbool.h>
#include "em_letimer.h"
#include <stdint.h>
#include "em_cmu.h"


void letimer0_setup();
void ClkInit();

#endif /* SRC_LETIMER_H_ */
