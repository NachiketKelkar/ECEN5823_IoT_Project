/***************************************************************************//**
 * @file
 * @brief Silicon Labs BT Mesh Empty Example Project
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>
#include <em_core.h>
/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "src/ble_mesh_device_type.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "src/display.h"
#include "src/gpio.h"
#include "src/log.h"
#include "mesh_generic_model_capi_types.h"
#include "mesh_lighting_model_capi_types.h"
#include "mesh_lib.h"
#include "src/gecko_ble_errors.h"
#include "src/sensor.h"
#include "src/main.h"
#include "src/client_req.h"
/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
void mesh_native_bgapi_init(void);
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);


/* Global variables */
char device_name[15];
uint16_t result;
uint16_t primary_elem_addr;
bool human_presence;
uint64_t total_time = 1;
uint64_t occupied_time = 0;
uint32_t no_of_overflows = 0;
extern uint32_t external_evt;

/* Persistent data storage */
#define OCCUPIED_TIME_DATA 10
#define TOTAL_TIME_DATA    20


/* Project defines */
#define TIMER_FACTORY_RESET   200
#define TIMER_SENSOR_READINGS 201
#define RESTORE_UTILIZATION   202
#define STORE_RECENT_DATA     203

#define TIMER_MS_2_TIMERTICK(ms) ((32768 * ms) / 1000)

/* Function declarations */
void data_subscriber_init();
void init_all_models();
void server_publish();
void handle_external_events(uint32_t);
//errorcode_t onoff_update(uint16_t element_index);

int devices = 0;
/**
 * See light switch app.c file definition
 */
void gecko_bgapi_classes_init_server_friend(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	//gecko_bgapi_class_mesh_generic_client_init();
	gecko_bgapi_class_mesh_generic_server_init();
	gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
//	gecko_bgapi_class_mesh_lpn_init();
	gecko_bgapi_class_mesh_friend_init();
	mesh_native_bgapi_init();
}


/**
 * See main function list in soc-btmesh-switch project file
 */
void gecko_bgapi_classes_init_client_lpn(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();
	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
	gecko_bgapi_class_mesh_generic_client_init();
	//gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
//	gecko_bgapi_class_mesh_friend_init();

}

void gecko_main_init()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);

  if( DeviceUsesClientModel() ) {
	  gecko_bgapi_classes_init_client_lpn();
  } else {
	  gecko_bgapi_classes_init_server_friend();
  }

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}

void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
  switch (evt_id) {
    case gecko_evt_system_boot_id:
    	LOG_INFO("evt::gecko_evt_system_boot_id");
    	struct gecko_msg_system_get_bt_address_rsp_t *Addr = gecko_cmd_system_get_bt_address();
//    	displayPrintf(DISPLAY_ROW_NAME,"FRIEND");
    	sprintf(device_name,"FRIEND - %02x:%02x",Addr->address.addr[1],Addr->address.addr[0]);
    	displayPrintf(DISPLAY_ROW_NAME,device_name);
    	if(GPIO_PinInGet(BUTTON0_PORT, BUTTON0_PIN) == 0 || GPIO_PinInGet(BUTTON1_PORT, BUTTON1_PIN) == 0)
    	{
    		displayPrintf(DISPLAY_ROW_ACTION - 1, "***************");
    		displayPrintf(DISPLAY_ROW_ACTION, "Factory Reset");
    		displayPrintf(DISPLAY_ROW_ACTION + 1, "***************");
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_flash_ps_erase_all());
			BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(2000), TIMER_FACTORY_RESET, 1));
    	}
    	else
    	{
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(device_name), (uint8_t *)device_name));
    		// Initialize Mesh stack in Node operation mode, wait for initialized event
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_node_init());
//    		result = gecko_cmd_mesh_node_init_oob(0, 2, 3, 4, 2, 4, 1)->result;
//    		if(result) LOG_ERROR("Mesh node init failed with %d",result);
    	}
      break;

//    case gecko_evt_mesh_node_display_output_oob_id:
//    	LOG_INFO("evt::gecko_evt_mesh_node_display_output_oob_id");
//    	LOG_INFO("%d",evt->data.evt_mesh_node_display_output_oob.data);
//    	LOG_INFO("%d",evt->data.evt_mesh_node_display_output_oob.output_size);
//    	break;
//
//    case gecko_evt_mesh_node_static_oob_request_id:
//    	LOG_INFO("evt::gecko_evt_mesh_node_static_oob_request_id");
//    	uint8 oob_resp_data[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5};
//    	uint16_t result = gecko_cmd_mesh_node_static_oob_request_rsp(sizeof(oob_resp_data),oob_resp_data)->result;
//    	if(result!=0) LOG_ERROR("Sending static data failed %d",result);
//    	break;

    case gecko_evt_hardware_soft_timer_id:
    	LOG_INFO("evt::gecko_evt_hardware_soft_timer_id");
    	struct gecko_msg_flash_ps_load_rsp_t* loaded_val_octime;
    	uint8_t *toti;
    	switch(evt->data.evt_hardware_soft_timer.handle)
    	{
    	case TIMER_FACTORY_RESET:
    		gecko_cmd_system_reset(0);
    		break;
    	case TIMER_SENSOR_READINGS:
//    		human_presence = is_human_present();
//    		displayPrintf(DISPLAY_ROW_CLIENTADDR,"Human - %d",human_presence);
    		displayPrintf(DISPLAY_ROW_BTADDR,"Utlzt - %d",(uint8_t)get_occupancy(occupied_time, total_time));
    		break;
    	case RESTORE_UTILIZATION:
    		loaded_val_octime = gecko_cmd_flash_ps_load(OCCUPIED_TIME_DATA);
    		memcpy(&occupied_time,&loaded_val_octime->value.data,loaded_val_octime->value.len);
    		LOG_INFO("Restored occupied time is %d",occupied_time);
    		struct gecko_msg_flash_ps_load_rsp_t* loaded_val_totime = gecko_cmd_flash_ps_load(TOTAL_TIME_DATA);
    		memcpy(&total_time,&loaded_val_totime->value.data,loaded_val_totime->value.len);
    		LOG_INFO("Restored total time is %d",total_time);
    		break;
    	case STORE_RECENT_DATA:
    		toti = (uint8_t*)&total_time;
    		uint8_t *octi = (uint8_t*)&occupied_time;
    		LOG_INFO("Data to be stored total_time is %d",total_time);
    		LOG_INFO("Data to be stored occupied time is %d",occupied_time);
    		gecko_cmd_flash_ps_save(OCCUPIED_TIME_DATA,sizeof(occupied_time),octi);
    		gecko_cmd_flash_ps_save(TOTAL_TIME_DATA,sizeof(total_time),toti);
    		break;
    	}
    	break;

    case gecko_evt_mesh_node_initialized_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_initialized_id");
//    	uint8_t op_cd[1] = {1};
//    	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_vendor_model_init(0, 0x02ff, 0x0001, false, 1, op_cd));
//    	uint8_t op_cd1[1] = {1};
//    	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_vendor_model_init(0, 0x02ff, 0x0002, false, 1, op_cd1));
    	if (!evt->data.evt_mesh_node_initialized.provisioned) {
    		displayPrintf(DISPLAY_ROW_ACTION,"Unprovisioned");
    		// The Node is now initialized, start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
    		gecko_cmd_mesh_node_start_unprov_beaconing(0x3);
    	}
    	if((evt->data.evt_mesh_node_initialized.provisioned))
    	{
    		primary_elem_addr = evt->data.evt_mesh_node_initialized.address;
    		LOG_INFO("Node is provisioned with %x address and %ld ivi",primary_elem_addr,evt->data.evt_mesh_node_initialized.ivi);
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(2000), TIMER_SENSOR_READINGS, 0));
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(10), RESTORE_UTILIZATION, 1));
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(10000), STORE_RECENT_DATA, 0));
    		data_subscriber_init();
    	}
    	break;

    case gecko_evt_mesh_node_key_added_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_key_added_id");
    	break;

    case gecko_evt_mesh_node_model_config_changed_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_model_config_changed_id");
    	LOG_INFO("Element address is %x",evt->data.evt_mesh_node_model_config_changed.element_address);
    	LOG_INFO("Model id is %x",evt->data.evt_mesh_node_model_config_changed.model_id);
    	LOG_INFO("Vendor id is %x",evt->data.evt_mesh_node_model_config_changed.vendor_id);
        break;

    case gecko_evt_mesh_node_provisioning_started_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_provisioning_started_id");
    	displayPrintf(DISPLAY_ROW_ACTION,"Provisioning");
    	break;

    case gecko_evt_mesh_node_provisioned_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_provisioned_id");
    	displayPrintf(DISPLAY_ROW_ACTION, "Provisioned");
    	break;

    case gecko_evt_mesh_node_provisioning_failed_id:
    	LOG_INFO("evt::gecko_evt_mesh_node_provisioning_failed_id");
    	displayPrintf(DISPLAY_ROW_ACTION, "Provisioning Failed");
//    	gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_FACTORY_RESET, 1);
    	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(2 * 32768, TIMER_FACTORY_RESET, 1));
    	break;

    case gecko_evt_le_connection_opened_id:
    	LOG_INFO("evt::gecko_evt_le_connection_opened_id");
    	break;

    case gecko_evt_le_connection_closed_id:
    	LOG_INFO("evt::gecko_evt_le_connection_closed_id");
      /* Check if need to boot to dfu mode */
      if (boot_to_dfu) {
        /* Enter to DFU OTA mode */
        gecko_cmd_system_reset(2);
      }
      break;

    case gecko_evt_mesh_generic_server_client_request_id:
    	LOG_INFO("evt::gecko_evt_mesh_generic_server_client_request_id");
    	mesh_lib_generic_server_event_handler(evt);
    	break;

    case gecko_evt_mesh_generic_server_state_changed_id:
    	LOG_INFO("evt::gecko_evt_mesh_generic_server_state_changed_id");
    	mesh_lib_generic_server_event_handler(evt);
    	break;

    case gecko_evt_gatt_server_user_write_request_id:
    	LOG_INFO("evt::gecko_evt_gatt_server_user_write_request_id");
      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
    	  LOG_INFO("OTA control");
        /* Set flag to enter to OTA mode */
        boot_to_dfu = 1;
        /* Send response to Write Request */
        gecko_cmd_gatt_server_send_user_write_response(
          evt->data.evt_gatt_server_user_write_request.connection,
          gattdb_ota_control,
          bg_err_success);

        /* Close connection to enter to DFU OTA mode */
        gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
      }
      break;

    case gecko_evt_mesh_friend_friendship_established_id:
    	devices ++;
    	LOG_INFO("evt::gecko_evt_mesh_friend_friendship_established_id");
    	LOG_INFO("lpn_address=%x", evt->data.evt_mesh_friend_friendship_established.lpn_address);
    	displayPrintf(DISPLAY_ROW_TEMPVALUE,"LPN count - %d",devices);
    	break;

    case gecko_evt_mesh_friend_friendship_terminated_id:
    	devices --;
    	LOG_INFO("evt::gecko_evt_mesh_friend_friendship_terminated_id");
    	LOG_INFO("reason=%x", evt->data.evt_mesh_friend_friendship_terminated.reason);
    	displayPrintf(DISPLAY_ROW_TEMPVALUE,"LPN count - %d",devices);
    	break;

    case gecko_evt_system_external_signal_id:
//    	LOG_INFO("evt::gecko_evt_system_external_signal_id");
    	handle_external_events(evt->data.evt_system_external_signal.extsignals);
    	break;

    default:
      break;
  }
}


void data_subscriber_init()
{
	/* Initialize the server */
	BTSTACK_CHECK_RESPONSE(gecko_cmd_mesh_generic_server_init());
	result = mesh_lib_init(malloc, free, 9);
	if(result != 0)
	{
		LOG_ERROR("Mesh lib init failed with %d response",result);
	}

	/* Initialize the friend */
	LOG_INFO("Initializing friend");
	struct gecko_msg_mesh_friend_init_rsp_t* res1 = gecko_cmd_mesh_friend_init();
	if(res1->result != 0)
	{
		LOG_ERROR("Friend init failed with %d response",res1->result);
	}
	else
	{
		displayPrintf(DISPLAY_ROW_TEMPVALUE,"LPN count - %d",devices);
	}
	init_all_models();
	server_publish();
}


void init_all_models()
{
	errorcode_t mesh_reg_response = mesh_lib_generic_server_register_handler(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, onoff_request, NULL);
	if(mesh_reg_response != 0)
	{
		LOG_ERROR("Handler register failed with %d response",mesh_reg_response);
	}
	mesh_reg_response = mesh_lib_generic_server_register_handler(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, level_request, NULL);
	if(mesh_reg_response != 0)
	{
		LOG_ERROR("Handler register failed with %d response",mesh_reg_response);
	}
}


void server_publish()
{
	errorcode_t error = mesh_lib_generic_server_publish(MESH_GENERIC_ON_OFF_SERVER_MODEL_ID, 0, mesh_generic_state_on_off);
	if(error != 0)
	{
		LOG_ERROR("Server publish failed with %x response",error);
	}
	error = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID, 0, mesh_generic_state_level);
	if(error != 0)
	{
		LOG_ERROR("Server publish failed with %x response",error);
	}
}


void handle_external_events(uint32_t external_events)
{
	uint32_t sound_data;
	CORE_DECLARE_IRQ_STATE;
	if((external_events & DISPLAY_UPDATE) == DISPLAY_UPDATE)
	{
		CORE_ENTER_CRITICAL();
		external_evt &= ~DISPLAY_UPDATE;
		CORE_EXIT_CRITICAL();
		total_time ++;
		no_of_overflows ++;
		displayUpdate();
	}
	if((external_events & MOTION_RAISING) == MOTION_RAISING)
	{
		SOUND_SENSOR_ENABLE;
		sound_data  = get_sound_data();
		if(sound_data > SOUND_THRESHOLD)
		{
			occupied_time ++;
		}
	}
	if((external_events & MOTION_FALLING) == MOTION_FALLING)
	{
		CORE_ENTER_CRITICAL();
		external_evt &= ~MOTION_RAISING;
		external_evt &= ~MOTION_FALLING;
		CORE_EXIT_CRITICAL();
		SOUND_SENSOR_DISABLE;
	}
}
