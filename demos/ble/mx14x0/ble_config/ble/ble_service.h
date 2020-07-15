#ifndef _BLE_SERVICE_
#define _BLE_SERVICE_

#define BLE_MSG_QUEUE_SIZE       (10)

enum BLE_EVENT_E
{
    BLE_EVENT_CONNECT,
    BLE_EVENT_DISCONNECT,
    BLE_EVENT_RECV_DATA,
    BLE_EVENT_NOTIFY_ENABLE,
    BLE_EVENT_NOTIFY_DISABLE,
    BLE_EVENT_MAX,
    BLE_EVENT_NONE
};
typedef uint8_t BLE_EVENT_T;

typedef struct _BLE_MSG_S
{
    BLE_EVENT_T event;
    void *data;
    uint32_t len;
} BLE_MSG_S;

int ble_service_init( void );

void ble_adv_stop( void );

int ble_pop_msg( BLE_MSG_S *msg_p, uint32_t timeout_ms );

void ble_adv_param_set( void );

#endif
