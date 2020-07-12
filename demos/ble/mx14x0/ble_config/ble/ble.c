/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "ble.h"
#include "StringUtils.h"
#include "mbt.h"
#include "mxos.h"
#include <app_msg.h>
#include <gap.h>
#include <gap_adv.h>
#include <gap_bond_le.h>
#include <gap_conn_le.h>
#include <gap_msg.h>
#include <profile_server.h>
#include "ble_service.h"
#include "ble_profile.h"
#include "string.h"

#define app_log(M, ...)         MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

/** @defgroup  PERIPH_APP Peripheral Application
    * @brief This file handles BLE peripheral application routines.
    * @{
    */
/*============================================================================*
 *                              Variables
 *============================================================================*/

T_SERVER_ID suning_srv_id; /**< Simple ble service id*/

T_GAP_DEV_STATE gap_dev_state = {0, 0, 0, 0, 0};                  /**< GAP device state */
T_GAP_CONN_STATE gap_conn_state = GAP_CONN_STATE_DISCONNECTED; /**< GAP connection state */
T_APP_LINK link_state;

/*============================================================================*
 *                              Functions
 *============================================================================*/

void ble_set_service_id( T_SERVER_ID id )
{
    suning_srv_id = id;
    return;
}

WEAK void ble_recv_data( void *data, uint32_t len )
{
    return;
}

/**
 * @brief    Handle msg GAP_MSG_LE_DEV_STATE_CHANGE
 * @note     All the gap device state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] new_state  New gap device state
 * @param[in] cause GAP device state change cause
 * @return   void
 */
void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
{
    app_log("app_handle_dev_state_evt: init state %d, adv state %d, cause 0x%x",
            new_state.gap_init_state, new_state.gap_adv_state, cause);
    if (gap_dev_state.gap_init_state != new_state.gap_init_state)
    {
        if (new_state.gap_init_state == GAP_INIT_STATE_STACK_READY)
        {
            app_log("GAP stack ready");

            ble_adv_param_set( );

            /*stack ready*/
            mbt_le_adv_start();
        }
    }

    if (gap_dev_state.gap_adv_state != new_state.gap_adv_state)
    {
        if (new_state.gap_adv_state == GAP_ADV_STATE_IDLE)
        {
            if (new_state.gap_adv_sub_state == GAP_ADV_TO_IDLE_CAUSE_CONN)
            {
                app_log("GAP adv stoped: because connection created");
            }
            else
            {
                app_log("GAP adv stoped");
            }
        }
        else if (new_state.gap_adv_state == GAP_ADV_STATE_ADVERTISING)
        {
            app_log("GAP adv start");
        }
    }

    gap_dev_state = new_state;
}

//get wifi status
static bool get_wifi_status(void)
{
    mwifi_link_info_t link_status;

    memset(&link_status, 0, sizeof(link_status));

    mwifi_get_link_info(&link_status);

    return (bool)(link_status.is_connected);
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_STATE_CHANGE
 * @note     All the gap conn state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New gap connection state
 * @param[in] disc_cause Use this cause when new_state is GAP_CONN_STATE_DISCONNECTED
 * @return   void
 */
void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
{
    app_log("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
            conn_id, gap_conn_state, new_state, disc_cause);
    switch (new_state)
    {
    case GAP_CONN_STATE_DISCONNECTED:
    {
        if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE)) && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
        {
            app_log("app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
        }

        //check wifi status
        if ( get_wifi_status( ) == false )
        {
            mbt_le_adv_start( );
        }
    }
    break;

    case GAP_CONN_STATE_CONNECTED:
    {
        uint16_t conn_interval;
        uint16_t conn_latency;
        uint16_t conn_supervision_timeout;
        uint8_t remote_bd[6];
        T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

        le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
        le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
        le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
        le_get_conn_addr(conn_id, remote_bd, (void *)&remote_bd_type);

        memcpy(link_state.bd_addr, remote_bd, sizeof(remote_bd));
        link_state.bd_type = remote_bd_type;
        link_state.conn_id = conn_id;
        app_log("GAP_CONN_STATE_CONNECTED:, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                remote_bd_type,
                conn_interval, conn_latency, conn_supervision_timeout);
    }
    break;

    default:
        break;
    }
    gap_conn_state = new_state;
}

/**
 * @brief    Handle msg GAP_MSG_LE_AUTHEN_STATE_CHANGE
 * @note     All the gap authentication state events are pre-handled in this function.
 *           Then the event handling function shall be called according to the new_state
 * @param[in] conn_id Connection ID
 * @param[in] new_state  New authentication state
 * @param[in] cause Use this cause when new_state is GAP_AUTHEN_STATE_COMPLETE
 * @return   void
 */
void app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t cause)
{
    app_log("app_handle_authen_state_evt:conn_id %d, cause 0x%x", conn_id, cause);

    switch (new_state)
    {
    case GAP_AUTHEN_STATE_STARTED:
    {
        app_log("app_handle_authen_state_evt: GAP_AUTHEN_STATE_STARTED");
    }
    break;

    case GAP_AUTHEN_STATE_COMPLETE:
    {
        if (cause == GAP_SUCCESS)
        {
            app_log("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair success");
        }
        else
        {
            app_log("app_handle_authen_state_evt: GAP_AUTHEN_STATE_COMPLETE pair failed");
        }
    }
    break;

    default:
    {
        app_log("app_handle_authen_state_evt: unknown newstate %d", new_state);
    }
    break;
    }
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_MTU_INFO
 * @note     This msg is used to inform APP that exchange mtu procedure is completed.
 * @param[in] conn_id Connection ID
 * @param[in] mtu_size  New mtu size
 * @return   void
 */
void app_handle_conn_mtu_info_evt(uint8_t conn_id, uint16_t mtu_size)
{
    app_log("app_handle_conn_mtu_info_evt: conn_id %d, mtu_size %d", conn_id, mtu_size);
}

/**
 * @brief    Handle msg GAP_MSG_LE_CONN_PARAM_UPDATE
 * @note     All the connection parameter update change  events are pre-handled in this function.
 * @param[in] conn_id Connection ID
 * @param[in] status  New update state
 * @param[in] cause Use this cause when status is GAP_CONN_PARAM_UPDATE_STATUS_FAIL
 * @return   void
 */
void app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
{
    switch (status)
    {
    case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
    {
        uint16_t conn_interval;
        uint16_t conn_slave_latency;
        uint16_t conn_supervision_timeout;

        le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
        le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
        le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
        app_log("app_handle_conn_param_update_evt update success:conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                conn_interval, conn_slave_latency, conn_supervision_timeout);
    }
    break;

    case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
    {
        app_log("app_handle_conn_param_update_evt update failed: cause 0x%x", cause);
    }
    break;

    case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
    {
        app_log("app_handle_conn_param_update_evt update pending.");
    }
    break;

    default:
        break;
    }
}

/**
 * @brief    All the BT GAP MSG are pre-handled in this function.
 * @note     Then the event handling function shall be called according to the
 *           subtype of T_IO_MSG
 * @param[in] p_gap_msg Pointer to GAP msg
 * @return   void
 */
void mbt_app_handle_gap_msg(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    uint8_t conn_id;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));

    app_log("app_handle_gap_msg: subtype %d", p_gap_msg->subtype);
    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_DEV_STATE_CHANGE:
    {
        app_handle_dev_state_evt(gap_msg.msg_data.gap_dev_state_change.new_state,
                                 gap_msg.msg_data.gap_dev_state_change.cause);
    }
    break;

    case GAP_MSG_LE_CONN_STATE_CHANGE:
    {
        app_handle_conn_state_evt(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                  (T_GAP_CONN_STATE)gap_msg.msg_data.gap_conn_state_change.new_state,
                                  gap_msg.msg_data.gap_conn_state_change.disc_cause);
    }
    break;

    case GAP_MSG_LE_CONN_MTU_INFO:
    {
        app_handle_conn_mtu_info_evt(gap_msg.msg_data.gap_conn_mtu_info.conn_id,
                                     gap_msg.msg_data.gap_conn_mtu_info.mtu_size);
    }
    break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
    {
        app_handle_conn_param_update_evt(gap_msg.msg_data.gap_conn_param_update.conn_id,
                                         gap_msg.msg_data.gap_conn_param_update.status,
                                         gap_msg.msg_data.gap_conn_param_update.cause);
    }
    break;

    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
    {
        app_handle_authen_state_evt(gap_msg.msg_data.gap_authen_state.conn_id,
                                    gap_msg.msg_data.gap_authen_state.new_state,
                                    gap_msg.msg_data.gap_authen_state.status);
    }
    break;

    case GAP_MSG_LE_BOND_JUST_WORK:
    {
        conn_id = gap_msg.msg_data.gap_bond_just_work_conf.conn_id;
        le_bond_just_work_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
        app_log("GAP_MSG_LE_BOND_JUST_WORK");
    }
    break;

    case GAP_MSG_LE_BOND_PASSKEY_DISPLAY:
    {
        uint32_t display_value = 0;
        conn_id = gap_msg.msg_data.gap_bond_passkey_display.conn_id;
        le_bond_get_display_key(conn_id, &display_value);
        app_log("GAP_MSG_LE_BOND_PASSKEY_DISPLAY:passkey %ld", display_value);
        le_bond_passkey_display_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
    break;

    case GAP_MSG_LE_BOND_USER_CONFIRMATION:
    {
        uint32_t display_value = 0;
        conn_id = gap_msg.msg_data.gap_bond_user_conf.conn_id;
        le_bond_get_display_key(conn_id, &display_value);
        app_log("GAP_MSG_LE_BOND_USER_CONFIRMATION: passkey %ld", display_value);
        le_bond_user_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
    }
    break;

    case GAP_MSG_LE_BOND_PASSKEY_INPUT:
    {
        uint32_t passkey = 888888;
        conn_id = gap_msg.msg_data.gap_bond_passkey_input.conn_id;
        app_log("GAP_MSG_LE_BOND_PASSKEY_INPUT: conn_id %d", conn_id);
        le_bond_passkey_input_confirm(conn_id, passkey, GAP_CFM_CAUSE_ACCEPT);
    }
    break;
#if F_BT_LE_SMP_OOB_SUPPORT
//    case GAP_MSG_LE_BOND_OOB_INPUT:
//    {
//        uint8_t oob_data[GAP_OOB_LEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//        conn_id = gap_msg.msg_data.gap_bond_oob_input.conn_id;
//        app_log("GAP_MSG_LE_BOND_OOB_INPUT");
//        le_bond_set_param(GAP_PARAM_BOND_OOB_DATA, GAP_OOB_LEN, oob_data);
//        le_bond_oob_input_confirm(conn_id, GAP_CFM_CAUSE_ACCEPT);
//    }
//    break;
#endif
    default:
        app_log("app_handle_gap_msg: unknown subtype %d", p_gap_msg->subtype);
        break;
    }
}
/** @} */ /* End of group PERIPH_GAP_MSG */

/** @defgroup  PERIPH_GAP_CALLBACK GAP Callback Event Handler
    * @brief Handle GAP callback event
    * @{
    */
/**
  * @brief Callback for gap le to notify app
  * @param[in] cb_type callback msy type @ref GAP_LE_MSG_Types.
  * @param[in] p_cb_data point to callback data @ref T_LE_CB_DATA.
  * @retval result @ref T_APP_RESULT
  */
#define BD_ADDR_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define BD_ADDR_ARG(x) (x)[5], (x)[4], (x)[3], (x)[2], (x)[1], (x)[0]
T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_CB_DATA *p_data = (T_LE_CB_DATA *)p_cb_data;

    switch (cb_type)
    {
#if F_BT_LE_4_2_DATA_LEN_EXT_SUPPORT
    case GAP_MSG_LE_DATA_LEN_CHANGE_INFO:
        app_log("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, tx octets 0x%x, max_tx_time 0x%x",
                p_data->p_le_data_len_change_info->conn_id,
                p_data->p_le_data_len_change_info->max_tx_octets,
                p_data->p_le_data_len_change_info->max_tx_time);
        break;
#endif
    case GAP_MSG_LE_MODIFY_WHITE_LIST:
        app_log("GAP_MSG_LE_MODIFY_WHITE_LIST: operation %d, cause 0x%x",
                p_data->p_le_modify_white_list_rsp->operation,
                p_data->p_le_modify_white_list_rsp->cause);
        break;

    case GAP_MSG_LE_SCAN_INFO:
        app_log("GAP_MSG_LE_SCAN_INFO:adv_type 0x%x, bd_addr " BD_ADDR_FMT ", remote_addr_type %d, rssi %d, data_len %d",
                p_data->p_le_scan_info->adv_type,
                BD_ADDR_ARG(p_data->p_le_scan_info->bd_addr),
                p_data->p_le_scan_info->remote_addr_type,
                p_data->p_le_scan_info->rssi,
                p_data->p_le_scan_info->data_len);

    default:
        //        app_log("app_gap_callback: unhandled cb_type 0x%x", cb_type);
        break;
    }
    return result;
}
/** @} */ /* End of group PERIPH_GAP_CALLBACK */

/** @defgroup  PERIPH_SEVER_CALLBACK Profile Server Callback Event Handler
    * @brief Handle profile server callback event
    * @{
    */
/**
    * @brief    All the BT Profile service callback events are handled in this function
    * @note     Then the event handling function shall be called according to the
    *           service_id
    * @param    service_id  Profile service ID
    * @param    p_data      Pointer to callback data
    * @return   T_APP_RESULT, which indicates the function call is successful or not
    * @retval   APP_RESULT_SUCCESS  Function run successfully
    * @retval   others              Function run failed, and return number indicates the reason
    */
T_APP_RESULT app_profile_callback(T_SERVER_ID service_id, void *p_data)
{
    char *str = NULL;
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    if (service_id == SERVICE_PROFILE_GENERAL_ID)
    {
        T_SERVER_APP_CB_DATA *p_param = (T_SERVER_APP_CB_DATA *)p_data;
        switch (p_param->eventId)
        {
        case PROFILE_EVT_SRV_REG_COMPLETE: // srv register result event.
            app_log("PROFILE_EVT_SRV_REG_COMPLETE: result %d",
                    p_param->event_data.service_reg_result);
            break;

        case PROFILE_EVT_SEND_DATA_COMPLETE:
            app_log("PROFILE_EVT_SEND_DATA_COMPLETE: conn_id %d, cause 0x%x, service_id %d, attrib_idx 0x%x, credits %d",
                    p_param->event_data.send_data_result.conn_id,
                    p_param->event_data.send_data_result.cause,
                    p_param->event_data.send_data_result.service_id,
                    p_param->event_data.send_data_result.attrib_idx,
                    p_param->event_data.send_data_result.credits);
            if (p_param->event_data.send_data_result.cause == GAP_SUCCESS)
            {
                app_log("PROFILE_EVT_SEND_DATA_COMPLETE success");
            }
            else
            {
                app_log("PROFILE_EVT_SEND_DATA_COMPLETE failed");
            }
            break;

        default:
            break;
        }
    }
    else if (service_id == suning_srv_id)
    {
        TSIMP_CALLBACK_DATA *p_simp_cb_data = (TSIMP_CALLBACK_DATA *)p_data;
        switch (p_simp_cb_data->msg_type)
        {
        case SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION:
        {
            switch (p_simp_cb_data->msg_data.notification_indification_index)
            {
            case SIMP_NOTIFY_INDICATE_V3_ENABLE:
            {
                app_log("SIMP_NOTIFY_INDICATE_V3_ENABLE");
            }
            break;

            case SIMP_NOTIFY_INDICATE_V3_DISABLE:
            {
                app_log("SIMP_NOTIFY_INDICATE_V3_DISABLE");
            }
            break;

            default:
                break;
            }
        }
        break;

        case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
        {
            app_log("SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE error, index:%d", p_simp_cb_data->msg_data.read_value_index);
        }
        break;
        case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
        {
            switch (p_simp_cb_data->msg_data.write.opcode)
            {
                case SIMP_WRITE_V2:
                {
                    app_log("SIMP_WRITE_V2: write type %d, len %d", p_simp_cb_data->msg_data.write.write_type, p_simp_cb_data->msg_data.write.len);
                    if (p_simp_cb_data->msg_data.write.len > 0)
                    {
                        str = DataToHexStringWithSpaces(p_simp_cb_data->msg_data.write.p_value, p_simp_cb_data->msg_data.write.len);
                        app_log("%s", str);
                        if (str)
                            free(str);
                    }

                    ble_recv_data( p_simp_cb_data->msg_data.write.p_value, (uint32_t)p_simp_cb_data->msg_data.write.len );
                }
                break;
                default:
                    break;
            }
        }
        break;

        default:
            break;
        }
    }

    return app_result;
}

bool ble_send_notify(void *data, uint32_t len)
{
    return ble_service_send_v3_notify(link_state.conn_id, suning_srv_id, data, len);
}
