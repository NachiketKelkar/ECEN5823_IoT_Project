/*
 * client_req.h
 *
 *  Created on: Apr 24, 2019
 *      Author: nachi
 */

#ifndef SRC_CLIENT_REQ_H_
#define SRC_CLIENT_REQ_H_


#include <stdint.h>
#include "mesh_generic_model_capi_types.h"

#define HUMIDIFIER_THRESHOLD   123
#define DEHUMIDIFIER_THRESHOLD 150
#define AIR_QUALITY_LOW        600
#define AIR_QUALITY_HIGH       900
#define LUX_LOW                300
#define LUX_HIGH               500

typedef enum{
	humidity,
	air_quality,
	lux,
	fire,
}sensors;

/*
 * Function name: onoff_request()
 * Description: This function is called when the request is received from the client
 * parameters: This function is called back function where bluetooth stack passes all parameters
 * return: void*/
void onoff_request(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, const struct mesh_generic_request *, uint32_t, uint16_t, uint8_t);

/*
 * Function name: update_onoff_state()
 * Description  : The function takes the source and data from the model and displays
 *                the data on LCD screen.
 * @parameters  : uint8_t(source), uint8_t(on-off model state)
 * @return      : void
 */
void update_onoff_state(uint8_t, uint8_t);
//void onoff_update_publish(uint8_t on_off_state);

/*
 * Function name: level_request()
 * Description: This function is called when the request is received from the client
 * parameters: This function is called back function where bluetooth stack passes all parameters
 * return: void*/
void level_request(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t, const struct mesh_generic_request *, uint32_t, uint16_t, uint8_t);

/*
 * Function name: update_level_state()
 * Description  : The function takes the source, data and sensor from which data is
 *                received as parameters displays the data on LCD and also the
 *                actuation message on the screen.
 * @parameters  : uint8_t(source), int16_t(sensor data)
 * @return      : void
 */
void update_level_state(uint8_t, int16_t);
//void level_update_publish(int16_t);

void actuation_states(sensors, int16_t);
#endif /* SRC_CLIENT_REQ_H_ */
