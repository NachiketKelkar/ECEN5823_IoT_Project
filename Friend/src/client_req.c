/*
 * client_req.c
 *
 *  Created on: Apr 24, 2019
 *      Author: nachi
 */

#include <stdbool.h>
#include "client_req.h"
#include "log.h"
#include "display.h"
#include "mesh_generic_model_capi_types.h"
#include "bg_errorcodes.h"

bool is_humidity = false;
bool is_aqi = false;

void onoff_request(uint16_t model_id,
        		   uint16_t element_index,
				   uint16_t client_addr,
				   uint16_t server_addr,
				   uint16_t appkey_index,
				   const struct mesh_generic_request *request,
				   uint32_t transition_ms,
				   uint16_t delay_ms,
				   uint8_t request_flags)
{
	LOG_INFO("Client address is %d",client_addr);
	LOG_INFO("request_on_off - %d",request->on_off);
	LOG_INFO("transition_ms = %d",transition_ms);

	switch(client_addr)
	{
	case 1:
		update_onoff_state(1, request->on_off);
		break;
	case 2:
		update_onoff_state(2, request->on_off);
		break;
	}
}


void update_onoff_state(uint8_t source, uint8_t on_off_state)
{
//	LOG_INFO("on off update state");
	switch(source)
	{
	case 1:
		if(on_off_state == 1)
		{
			is_humidity = true;
			is_aqi = false;
		}
		else
		{
			is_aqi = true;
			is_humidity = false;
		}
		// Print the
//		onoff_update_publish(on_off_state);
		break;
	case 2:
		actuation_states(fire, on_off_state);
//		onoff_update_publish(on_off_state);
		break;
	}
}

void onoff_update_publish(uint8_t on_off_state)
{
	errorcode_t error;
	struct mesh_generic_state current, target;

	target.kind = mesh_generic_state_on_off;
	target.on_off.on = on_off_state;

	error = mesh_lib_generic_server_update(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
								   	   	   0,
										   NULL,
										   &target,
										   0);
	if(error != 0)
	{
		LOG_ERROR("Server update failed with %d response",error);
	}

	error = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
                                            0,
											mesh_generic_state_on_off);
	if(error != 0)
	{
		LOG_ERROR("Server publish failed with %d response",error);
	}
}


void level_request(uint16_t model_id,
        		   uint16_t element_index,
				   uint16_t client_addr,
				   uint16_t server_addr,
				   uint16_t appkey_index,
				   const struct mesh_generic_request *request,
				   uint32_t transition_ms,
				   uint16_t delay_ms,
				   uint8_t request_flags)
{
	LOG_INFO("In level_request function");
	LOG_INFO("Level value - %d",request->level);
	LOG_INFO("Client addr= %d", client_addr);
	switch(client_addr)
	{
	case 1:
		update_level_state(1, request->level);
		break;
	case 2:
		update_level_state(2, request->level);
		break;
	}
}


void update_level_state(uint8_t source, int16_t level)
{
//	LOG_INFO("Level update state");
	switch(source)
	{
	case 1:
		if(is_humidity == 1)
		{
			displayPrintf(DISPLAY_ROW_BTADDR2,"Humi-%d",level);
			actuation_states(humidity, level);
		}
		else if(is_aqi == 1)
		{
			displayPrintf(DISPLAY_ROW_CLIENTADDR,"Air Q - %dppm",level);
			actuation_states(air_quality, level);
		}
		break;
	case 2:
		displayPrintf(DISPLAY_ROW_CONNECTION,"Lux - %d",level);
		actuation_states(lux, level);
//		level_update_publish(level);
		break;
	}
}


void level_update_publish(int16_t level)
{
	errorcode_t error;
	struct mesh_generic_state target;

	target.kind        = mesh_generic_state_level;
	target.level.level = level;

	error = mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
								   	   	   0,
										   NULL,
										   &target,
										   0);
	if(error != 0)
	{
		LOG_ERROR("Server update failed with %d response",error);
	}

	error = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
                                            0,
											mesh_generic_state_level);
	if(error != 0)
	{
		LOG_ERROR("Server publish failed with %d response",error);
	}
}


void actuation_states(sensors sensor, int16_t data)
{
	displayPrintf(ACTUATION, "Actuators Status");
	switch(sensor)
	{
	case humidity:
		if(data < HUMIDIFIER_THRESHOLD)
		{
			//display humidifier ON
			displayPrintf(HUMIDITY_SENSOR, "Humidifier ON");
		}
		else if(data > HUMIDIFIER_THRESHOLD && data < DEHUMIDIFIER_THRESHOLD)
		{
			//display Humidifier and dehumidifier off
			displayPrintf(HUMIDITY_SENSOR, "Humi/Dehumi OFF");
		}
		else if(data > DEHUMIDIFIER_THRESHOLD)
		{
			//display Dehumidifier on
			displayPrintf(HUMIDITY_SENSOR, "Dehumidifier ON");
		}
		break;
	case air_quality:
		if(data > AIR_QUALITY_HIGH)
		{
			//display Fresh Air Intake ON
			displayPrintf(AIR_QUALITY_SENSOR, "Fresh Air Intake ON");
		}
		else if(data < AIR_QUALITY_LOW)
		{
			//Circulation ON
			displayPrintf(AIR_QUALITY_SENSOR, "Circulation ON");
		}
		else if(data > AIR_QUALITY_LOW && data < AIR_QUALITY_HIGH)
		{
			//Do not change the state of displayed message
		}
		break;
	case lux:
		if(data > LUX_HIGH)
		{
			//display Lights OFF
			displayPrintf(LUX_SENSOR, "Lights OFF");
		}
		else if(data < LUX_LOW)
		{
			//display Lights ON
			displayPrintf(LUX_SENSOR, "Lights ON");
		}
		else if(data > LUX_LOW && data < LUX_HIGH)
		{
			//Do not change the state of displayed message
		}
		break;
	case fire:
		if(data == 1)
		{
			//display FIRE
			displayPrintf(FIRE_SENSOR, "!!!!!!! FIRE !!!!!!!");
		}
		else
		{
			//Clear display
			displayPrintf(FIRE_SENSOR,"");
		}
		break;
	}
}
