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
#define GATT_UUID_PROFILE                    0x6600
#define GATT_UUID_CHAR_V3_NOTIFY             0x6601
#define GATT_UUID_CHAR_V2_WRITE              0x6602

/** @} End of SIMP_UUIDs */

/** @} End of SIMP_Common_Exported_Macros */

/** @} End of SIMP_Service_CONFIG */

/** @} End of SIMP_Service */


#ifdef __cplusplus
}
#endif

#endif
