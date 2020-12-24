#include "mxos.h"

#include "bluetooth.h"
#include "gatt.h"
#include "addr.h"
#include "hci_driver.h"

#define NAME_LEN                                30

#define  ble_log(M, ...) custom_log("ble", M, ##__VA_ARGS__)

void ble_controller_init( uint8_t task_priority );

int hci_driver_init(void);

int bt_enable(bt_ready_cb_t cb);

static void bt_enable_cb(int err)
{
    ble_log("bt_enable_cb (err %d)", err);
}

static bool data_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    uint8_t len;

    switch (data->type) {
    case BT_DATA_NAME_SHORTENED:
    case BT_DATA_NAME_COMPLETE:
        len = (data->data_len > NAME_LEN - 1)?(NAME_LEN - 1):(data->data_len);
        memcpy(name, data->data, len);
        return false;
    default:
        return true;
    }
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t evtype, struct net_buf_simple *buf)
{
    char        le_addr[BT_ADDR_LE_STR_LEN];
    char        name[NAME_LEN];

    (void)memset(name, 0, sizeof(name));
    bt_data_parse(buf, data_cb, name);
    bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));

    ble_log("[DEVICE]: %s, AD evt type %u, RSSI %i %s",le_addr, evtype, rssi, name);
}

void bt_scan(void)
{
    int err;
    struct bt_le_scan_param param;
    struct bt_le_scan_param scan_param = {
        .type       = BT_HCI_LE_SCAN_ACTIVE,
        .filter_dup = BT_HCI_LE_SCAN_FILTER_DUP_DISABLE,
        .interval   = 0x10,
        .window     = 0x10
    };

    ble_controller_init(0);
    hci_driver_init();
    err = bt_enable(bt_enable_cb);
    if (err) {
        ble_log("Bluetooth init failed (err %d)", err);
        return;
    }
    ble_log("Bluetooth init succeed.");

    bt_le_scan_start(&scan_param, device_found);

    mos_sleep(5);

    bt_le_scan_stop();

    return;
}

int main( void )
{
    bt_scan();

    return 0;
}
