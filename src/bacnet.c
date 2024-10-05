#include <pthread.h>
#include <stdlib.h>

#include <bacnet/bactext.h>
#include <bacnet/basic/services.h>
#include <bacnet/basic/object/device.h>
#include <bacnet/datalink/datalink.h>
#include <bacnet/datalink/dlenv.h>

#include "bacnet.h"
#include "log.h"
#include "protocol/decode_call.h"

static pthread_t thread_id;
pthread_mutex_t exit_signal_lock = PTHREAD_MUTEX_INITIALIZER;
static bool should_exit = false;
static int bacnet_network_id = 1000;

static int init_service_handlers();
static void* event_loop(void* arg);

typedef int (*call_handler_t)(void* data);
static int handle_create_gateway(create_routed_device_t* device);
static int handle_create_routed_device(create_routed_device_t* device);

static call_handler_t CALL_HANDLERS_BY_TYPE[] = {
  (call_handler_t)handle_create_gateway,
  (call_handler_t)handle_create_routed_device,
};

/**
 * @brief Initializes BACnet services.
 *
 * Prepares the system to handle BACnet communications by setting up necessary
 * configurations and starting the event loop in a separate thread.
 *
 * @return Returns 0 on success, or -1 if thread creation fails.
 */
int bacnet_start_services()
{
  const char* network_id_raw = getenv("BACNET_NETWORK_ID");
  if (network_id_raw)
    bacnet_network_id = (int)strtol(network_id_raw, NULL, 0);

  should_exit = false;
  if (pthread_create(&thread_id, NULL, &event_loop, NULL) != 0) {
    LOG_ERROR("bacnetd: failed to create bacnet thread");
    return -1;
  }

  return 0;
}

/**
 * @brief Signals the BACnet services to stop.
 *
 * Indicates that the BACnet services should cease operation, allowing for a
 * clean shutdown process.
 *
 * @return Always returns 0.
 */
int bacnet_stop_services()
{
  pthread_mutex_lock(&exit_signal_lock);
  should_exit = true;
  pthread_mutex_unlock(&exit_signal_lock);

  return 0;
}

/**
 * @brief Waits for BACnet services to complete.
 *
 * Ensures that the calling thread waits until all BACnet operations have
 * finished, facilitating resource cleanup and graceful termination.
 *
 * @return Returns 0 on success, or -1 if stopping the thread fails.
 */
int bacnet_wait_until_done()
{
  if (pthread_join(thread_id, NULL) != 0) {
    LOG_ERROR("bacnetd: failed to join thread");
    return -1;
  }

  return 0;
}

/**
 * @brief Processes incoming BACnet requests.
 *
 * Manages the handling of BACnet requests, including decoding and preparing
 * responses based on the request's content.
 *
 * @param buffer A pointer to the buffer containing the incoming BACnet request.
 * @param index  A pointer to the current index in the buffer.
 * @param reply  A pointer to an `ei_x_buff` structure where the response
 *               will be stored.
 */
void handle_bacnet_request(char* buffer, int* index, ei_x_buff* reply)
{
  bacnet_call_type_t type = CALL_UNKNOWN;
  void*              data = NULL;

  bool is_bad_request =
       decode_bacnet_call_type(buffer, index, &type)
    || bacnet_call_malloc(type, &data)
    || decode_bacnet_call(buffer, index, type, data)
    || type == CALL_UNKNOWN;

  if (is_bad_request) {
    ei_x_encode_tuple_header(reply, 2);
    ei_x_encode_atom(reply, "error");
    ei_x_encode_atom(reply, "bad_request");
    goto cleanup;
  }

  call_handler_t handler = CALL_HANDLERS_BY_TYPE[type];

  if (handler(data)) {
    ei_x_encode_tuple_header(reply, 2);
    ei_x_encode_atom(reply, "error");
    ei_x_encode_atom(reply, "failed_processing");
  }
  else {
    ei_x_encode_atom(reply, "ok");
  }

cleanup:
  if (data) free(data);
}

static void* event_loop(void* arg)
{
  int NETWORK_IDS[2] = {bacnet_network_id, -1};
  uint8_t buffer[MAX_MPDU] = { 0 };

  LOG_DEBUG("bacnetd: starting event_loop");

  address_init();
  dlenv_init();
  init_service_handlers();
  atexit(datalink_cleanup);

  while (true) {
    BACNET_ADDRESS src_address = { 0 };

    if (should_exit)
      break;

    uint16_t timeout = apdu_timeout();
    int length = bip_receive(&src_address, &buffer[0], MAX_MPDU, timeout);
    if (length <= 0)
      continue;

    LOG_DEBUG("bacnetd: sending request to npdu handler");
    routing_npdu_handler(&src_address, NETWORK_IDS, &buffer[0], length);
  }

  pthread_exit(NULL);
}

static void
abort_handler(
  BACNET_ADDRESS* src,
  uint8_t invoke_id,
  uint8_t abort_reason,
  bool server)
{
  const char* reason = bactext_abort_reason_name(abort_reason);
  LOG_WARNING("bacnetd: aborting request %s", reason);
}

static void
reject_handler(
  BACNET_ADDRESS* src,
  uint8_t invoke_id,
  uint8_t reject_reason)
{
  const char* reason = bactext_reject_reason_name(reject_reason);
  LOG_DEBUG("bacnetd: rejecting request %s", reason);
}

extern int Routed_Device_Read_Property_Local(BACNET_READ_PROPERTY_DATA* data);
extern bool Routed_Device_Write_Property_Local(BACNET_WRITE_PROPERTY_DATA* data);

static object_functions_t SUPPORTED_OBJECT_TABLE[] = {
  {
    .Object_Type = OBJECT_DEVICE,
    .Object_Init = NULL,
    .Object_Count = Device_Count,
    .Object_Index_To_Instance = Routed_Device_Index_To_Instance,
    .Object_Valid_Instance = Routed_Device_Valid_Object_Instance_Number,
    .Object_Name = Routed_Device_Name,
    .Object_Read_Property = Routed_Device_Read_Property_Local,
    .Object_Write_Property = Routed_Device_Write_Property_Local,
    .Object_RPM_List = Device_Property_Lists,
    .Object_RR_Info = DeviceGetRRInfo,
    .Object_Iterator =  NULL,
    .Object_Value_List =  NULL,
    .Object_COV =  NULL,
    .Object_COV_Clear =  NULL,
    .Object_Intrinsic_Reporting =  NULL,
    .Object_Add_List_Element = NULL,
    .Object_Remove_List_Element = NULL,
    .Object_Create = NULL,
    .Object_Delete = NULL,
    .Object_Timer = NULL,
  },
};

static int init_service_handlers()
{
  Device_Init(SUPPORTED_OBJECT_TABLE);

  apdu_set_unrecognized_service_handler_handler(handler_unrecognized_service);

  apdu_set_unconfirmed_handler(
    SERVICE_UNCONFIRMED_WHO_IS,
    handler_who_is_bcast_for_routing
  );

  apdu_set_unconfirmed_handler(
    SERVICE_UNCONFIRMED_WHO_HAS,
    handler_who_has_for_routing
  );

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_READ_PROPERTY,
    handler_read_property
  );

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_READ_PROP_MULTIPLE,
    handler_read_property_multiple
  );

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_WRITE_PROPERTY,
    handler_write_property
  );

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_WRITE_PROP_MULTIPLE,
    handler_write_property_multiple
  );

  apdu_set_confirmed_handler(SERVICE_CONFIRMED_READ_RANGE, handler_read_range);

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_REINITIALIZE_DEVICE,
    handler_reinitialize_device
  );

  apdu_set_unconfirmed_handler(
    SERVICE_UNCONFIRMED_UTC_TIME_SYNCHRONIZATION,
    handler_timesync_utc
  );

  apdu_set_unconfirmed_handler(
    SERVICE_UNCONFIRMED_TIME_SYNCHRONIZATION,
    handler_timesync
  );

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_SUBSCRIBE_COV,
    handler_cov_subscribe
  );

  apdu_set_unconfirmed_handler(
    SERVICE_UNCONFIRMED_COV_NOTIFICATION,
    handler_ucov_notification
  );

  apdu_set_confirmed_handler(
    SERVICE_CONFIRMED_DEVICE_COMMUNICATION_CONTROL,
    handler_device_communication_control
  );

  apdu_set_abort_handler(abort_handler);
  apdu_set_reject_handler(reject_handler);

  return 0;
}

static void set_device_address(DEVICE_OBJECT_DATA* device, int network_id)
{
  BACNET_ADDRESS address = { 0 };

  bip_get_my_address(&address);
  bacnet_address_copy(&device->bacDevAddr, &address);

  if (network_id > 0) {
    int virtual_mac = device->bacObj.Object_Instance_Number;
    encode_unsigned24(&device->bacDevAddr.adr[0], virtual_mac);

    device->bacDevAddr.len = 3;
    device->bacDevAddr.net = network_id;
  }
}

static int handle_create_gateway(create_routed_device_t* device)
{
  Add_Routed_Device(
    device->bacnet_id,
    &device->name,
    device->description,
    device->model,
    device->firmware_version);

  Routed_Device_Set_Object_Instance_Number(device->bacnet_id);
  Routed_Device_Set_Model(device->model, strlen(device->model));

  Routed_Device_Set_Object_Name(
    device->name.encoding,
    device->name.value,
    device->name.length
  );

  Routed_Device_Set_Description(
    device->description,
    strlen(device->description)
  );

  DEVICE_OBJECT_DATA* gateway = Get_Routed_Device_Object(0);
  set_device_address(gateway, -1);

  Device_Set_Object_Name(&device->name);
  Device_Set_System_Status(STATUS_OPERATIONAL, true);
  Device_Set_Model_Name(device->model, strlen(device->model));
  Device_Set_Description(device->description, strlen(device->description));

  Device_Set_Application_Software_Version(
    device->firmware_version,
    strlen(device->firmware_version)
  );

  return 0;
}

static int handle_create_routed_device(create_routed_device_t* device)
{
  int index =
    Add_Routed_Device(
      device->bacnet_id,
      &device->name,
      device->description,
      device->model,
      device->firmware_version);

  DEVICE_OBJECT_DATA* child = Get_Routed_Device_Object(index);
  set_device_address(child, bacnet_network_id);

  return 0;
}
