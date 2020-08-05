# READ ME
-------------------------------------------------------------------------------

## Light Demo for Tencent IoT(qcloud)

### ChangeLog

##### - Date 2020.08.05

* Init.

### NOTE:
* Board: MXKit-base + CORE-3072 V5.0 with RGB LED.
* Demo: RGB light control with wechat app `腾讯连连`.

### Build & flash & Run

1. In the root directory of mxos-demos project:  
   ```text
   mxos make clean  
   mxos make application.qcloud_app@emw3072 download
   ```

2. Click `RESET` button on MXKit-base board to reboot and run demo.

3. See debug log on debug uart(UST2UART Enhanced COM Port).
   115200bps/8/N/1

4. Click `ELINK` button on the MXKit-base board(or press over 5 seconds to do factory reset) to start wifi configuration mode.

5. Add device in the wechat app `腾讯连连` by scan the device QR code.

6. Demo control: RGB light control with wechat app.
