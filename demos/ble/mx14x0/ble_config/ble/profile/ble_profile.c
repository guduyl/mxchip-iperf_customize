#include <string.h>
#include "ble_profile.h"
#include <gap.h>
#include "mxos.h"

#define app_log(M, ...)             MXOS_LOG(CONFIG_APP_DEBUG, "sn_profile", M, ##__VA_ARGS__)

#define BLE_SERVICE_CHAR_V3_NOTIFY_INDEX         0x02
#define BLE_SERVICE_CHAR_NOTIFY_CCCD_INDEX       (BLE_SERVICE_CHAR_V3_NOTIFY_INDEX + 1)
#define BLE_SERVICE_CHAR_V2_WRITE_INDEX          0x05

T_SERVER_ID service_id;

/**<  Function pointer used to send event to application from simple profile. Initiated in simp_ble_service_add_service. */
static P_FUN_SERVER_GENERAL_CB pfn_ble_service_cb = NULL;

/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL ble_service_tbl[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_PROFILE),      /* service UUID */
            HI_WORD(GATT_UUID_PROFILE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /* <<Characteristic>>, demo for notify */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY)                 /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_V3_NOTIFY),
            HI_WORD(GATT_UUID_CHAR_V3_NOTIFY)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* permissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },

    /* <<Characteristic>> demo for write */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE_NO_RSP /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHAR_V2_WRITE),
            HI_WORD(GATT_UUID_CHAR_V2_WRITE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* permissions */
    },
};

/**
 * @brief read characteristic data from service.
 *
 * @param service_id          ServiceID of characteristic data.
 * @param attrib_index        Attribute index of getting characteristic data.
 * @param offset              Used for Blob Read.
 * @param p_length            length of getting characteristic data.
 * @param pp_value            data got from service.
 * @return Profile procedure result
 */
T_APP_RESULT ble_service_attr_read_cb( uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value )
{
    (void) offset;
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    switch ( attrib_index )
    {
        default:
            app_log("ble_service_attr_read_cb, Attr not found, index %d", attrib_index);
            cause = APP_RESULT_ATTR_NOT_FOUND;
            break;
    }

    return (cause);
}

void suning_write_post_callback( uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index, uint16_t length, uint8_t *p_value )
{
    (void) p_value;
    app_log("suning_write_post_callback: conn_id %d, service_id %d, attrib_index 0x%x, length %d", conn_id, service_id, attrib_index, length);
}
/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
 */
T_APP_RESULT ble_service_attr_write_cb( uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index, T_WRITE_TYPE write_type, \
                                             uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc )
{
    TSIMP_CALLBACK_DATA callback_data;
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    app_log("ble_service_attr_write_cb write_type = 0x%x", write_type);
    *p_write_ind_post_proc = suning_write_post_callback;

    if ( BLE_SERVICE_CHAR_V2_WRITE_INDEX == attrib_index )
    {
        /* Make sure written value size is valid. */
        if ( p_value == NULL )
        {
            cause = APP_RESULT_INVALID_VALUE_SIZE;
        } else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.conn_id = conn_id;
            callback_data.msg_data.write.opcode = SIMP_WRITE_V2;
            callback_data.msg_data.write.write_type = write_type;
            callback_data.msg_data.write.len = length;
            callback_data.msg_data.write.p_value = p_value;

            if ( pfn_ble_service_cb )
            {
                pfn_ble_service_cb( service_id, (void *) &callback_data );
            }
        }
    } else
    {
        app_log("ble_service_attr_write_cb Error: attrib_index 0x%x, length %d",attrib_index,length);
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return cause;
}

/**
 * @brief send notification of simple notify characteristic value.
 *
 * @param[in] conn_id           connection id
 * @param[in] service_id        service ID of service.
 * @param[in] p_value           characteristic value to notify
 * @param[in] length            characteristic value length to notify
 * @return notification action result
 * @retval 1 true
 * @retval 0 false
 */
bool ble_service_send_v3_notify( uint8_t conn_id, T_SERVER_ID service_id, void *p_value, uint16_t length )
{
    app_log("simp_ble_service_send_v3_notify");
    // send notification to client
    return server_send_data( conn_id, service_id, BLE_SERVICE_CHAR_V3_NOTIFY_INDEX, p_value, length, GATT_PDU_TYPE_ANY );
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           connection id.
 * @param service_id          Service ID.
 * @param index          Attribute index of characteristic data.
 * @param cccbits         CCCD bits from stack.
 * @return None
 */
void ble_service_cccd_update_cb( uint8_t conn_id, T_SERVER_ID service_id, uint16_t index, uint16_t cccbits )
{
    TSIMP_CALLBACK_DATA callback_data;
    bool is_handled = false;
    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;

    app_log("ble_service_cccd_update_cb: index = %d, cccbits 0x%x", index, cccbits);

    switch ( index )
    {
        case BLE_SERVICE_CHAR_NOTIFY_CCCD_INDEX:
            if ( cccbits & GATT_CLIENT_CHAR_CONFIG_NOTIFY )
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V3_ENABLE;
            } else
            {
                // Disable Notification
                callback_data.msg_data.notification_indification_index = SIMP_NOTIFY_INDICATE_V3_DISABLE;
            }

            is_handled = true;
            break;

        default:
            break;
    }

    /* Notify Application. */
    if ( pfn_ble_service_cb && (is_handled == true) )
    {
        pfn_ble_service_cb( service_id, (void *) &callback_data );
    }

    return;
}

/**
 * @brief Simple ble Service Callbacks.
 */
const T_FUN_GATT_SERVICE_CBS ble_service_cbs =
{
    ble_service_attr_read_cb,  // Read callback function pointer
    ble_service_attr_write_cb, // Write callback function pointer
    ble_service_cccd_update_cb // CCCD update callback function pointer
};

/**
 * @brief Add simple BLE service to the BLE stack database.
 *
 * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
 * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
 * @retval 0xFF Operation failure.
 * @retval others Service id assigned by stack.
 *
 */
T_SERVER_ID ble_service_add_service( void *p_func )
{
    if ( false == server_add_service( &service_id, (uint8_t *) ble_service_tbl, sizeof(ble_service_tbl), ble_service_cbs ) )
    {
        app_log("simp_ble_service_add_service: fail");
        service_id = 0xff;
        return service_id;
    }

    pfn_ble_service_cb = (P_FUN_SERVER_GENERAL_CB) p_func;

    return service_id;
}

