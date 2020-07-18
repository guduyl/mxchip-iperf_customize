/* Define to prevent recursive inclusion **/
#ifndef _BLE_CONFIG_H_
#define _BLE_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup SIMP_Service Simple Ble Service
  * @brief Simple BLE service
  * @{
  */

/** @defgroup SIMP_Service_CONFIG SIMP Service Config
  * @brief Simple BLE service configuration file
  * @{
  */
/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup SIMP_Common_Exported_Macros SIMP Service Config Exported Constants
  * @brief
  * @{
  */

/** @defgroup SIMP_UUIDs SIMP UUIDs
  * @brief Simple BLE Profile UUID definitions
  * @{
  */

#define COMPANY_UUID                            0x0922
#define GATT_UUID_PROFILE                       COMPANY_UUID
#define GATT_UUID_CHAR_V3_NOTIFY                0xFED8
#define GATT_UUID_CHAR_V2_WRITE_NSP             0xFED7

//2Bytes
#ifndef BLE_LOCAL_NAME_BYTE0
#define BLE_LOCAL_NAME_BYTE0                          'M'
#endif

#ifndef BLE_LOCAL_NAME_BYTE1
#define BLE_LOCAL_NAME_BYTE1                          'X'
#endif

/** @} End of SIMP_UUIDs */

/** @} End of SIMP_Common_Exported_Macros */

/** @} End of SIMP_Service_CONFIG */

/** @} End of SIMP_Service */


#ifdef __cplusplus
}
#endif

#endif
