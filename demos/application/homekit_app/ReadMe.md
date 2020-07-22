# READ ME
-------------------------------------------------------------------------------

## Demo for HomeKit ADK(Open Source Verison)

### ChangeLog

##### - Date 2020.07.16

* Pass HCA protocol test, fixed tests:
  - TCI014
  - TCSTA002
  - TCI005
  - *TCF042 (NFC test ignored)*

##### - Date 2020.07.15

* Add factory reset function by user key1 on [MXKit-Sensor](https://www.mxchip.com/documents) board without reboot.
* Add build && run guide.

##### - Date 2020.07.09

* Demo `Lightbulb` ported from [Homekit ADK](https://github.com/apple/)
* Test board [MXKit-Base + MXKit-Core3080](https://www.mxchip.com/documents)
* MbedTLS version `2.18.1`

### NOTE:

* The default verison of MbedTLS in MXOS is  `2.16.5`, *to use version `2.18.1`, please checkout branch `'dev_homekit'` in directory `MXOS/security/TLS/mbedtls/mbedtls-2.16.5/`*


### Build & flash & Run

1. In the root directory of mxos-demos project:  
   ```text
   mxos make clean  
   mxos make application.homekit_app@emw3080 download
   ```

2. Reboot MXKit board to run the demo.

3. See debug log on debug uart.
   115200bps/8/N/1

4. Press `ELINK` button on the MXKit-Sensor board over 5 seconds to reset wifi settings, and re-configure wifi ssid/password with [mxeasylink app](https://www.mxchip.com/documents).

5. Add acccessory in the `Home` app  
    Open the Home app on your `iPhone/iPad` and follow these steps:
- Tap on "Add Accessory".
- Choose the "I Don't Have a Code or Cannot Scan" option.
- Tap on `"Acme Light Bulb"` in the list of Nearby Accessories.
- Select the "Add Anyway" option for the "Uncertified Accessory" prompt.
- Enter `11122333` as the Setup code.
- You should eventually see the "Acme Light Bulb added" message.
- Give a custom name, assign to a room, create scenes as required and you are done.

6. Click `Device Identify` in the home app, the motor on MXKit-Sensor board will run 3 seconds.

7. Click `KEY1` on MXKit-Sensor board to turn on/off the RGB-LED, or use home app to control it.

8. Factory reset(clear paring): Long press `KEY1` over 4 seconds.
