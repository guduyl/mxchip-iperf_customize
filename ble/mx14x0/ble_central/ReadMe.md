# BLE central 示例

演示了模块作为central设备，完成基础的GAP操作，如扫描，连接

## 扫描

通过模块的调试串口，发送btscan命令来控制模块蓝牙扫描的开关

开启蓝牙扫描：
>btscan 1

停止蓝牙扫描：
>btscan 0

扫描结果会通过调试串口输出：

```text
GAP_MSG_LE_SCAN_INFO:adv_type 0x0, bd_addr 53:32:e9:20:46:87, remote_addr_type 1, rssi -80, data_len 14

ble_central_app_parse_scan_info: AD Structure Info: AD type 0x1, AD Data Length 1

GAP_ADTYPE_FLAGS: 0x1a

ble_central_app_parse_scan_info: AD Structure Info: AD type 0xff, AD Data Length 9

GAP_ADTYPE_MANUFACTURER_SPECIFIC: company_id 0x4c, len 7

GAP_MSG_LE_SCAN_INFO:adv_type 0x4, bd_addr 53:32:e9:20:46:87, remote_addr_type 1, rssi -80, data_len 0
```

## 连接

通过模块的调试串口，发送condev命令连接指定的蓝牙设备，第一个参数为指定设备的蓝牙地址，第二个参数为地址类型，这两个参数可以通过scan获取到。

例：
>condev 001122334455 1
