# BLE peripheral 示例

演示了模块作为peripheral设备，如何使用GATT服务与客户端通讯

## 准备工作

手机上安装蓝牙调试软件，如Lightblue等。

编译并下载ble_peripheral固件，并确认固件已经在模块上运行成功。

## 建立连接

程序运行后，会自动进行蓝牙广播，使附近的蓝牙设备可以搜索到模块。

打开手机上的蓝牙调试软件，搜索名为“BLE_PERIPHERAL”的设备，并尝试建立连接。

## 通讯测试

示例程序中初始化了一个simple ble service的服务，其中定义了以下四个UUID，并分别配置了读、写、通知、指示操作属性

```c
#define GATT_UUID_CHAR_SIMPLE_V1_READ               0xB001
#define GATT_UUID_CHAR_SIMPLE_V2_WRITE              0xB002
#define GATT_UUID_CHAR_SIMPLE_V3_NOTIFY             0xB003
#define GATT_UUID_CHAR_SIMPLE_V4_INDICATE           0xB004
```

### Read

在手机APP上发起对UUID为B001的特性值的读取操作，手机会收到两个字节的数据：0x01 0x02，这里回复的数据可以在app_profile_callback回调函数中修改

```c
case SERVICE_CALLBACK_TYPE_READ_CHAR_VALUE:
{
    if (p_simp_cb_data->msg_data.read_value_index == SIMP_READ_V1)
    {
        uint8_t value[2] = {0x01, 0x02};
        app_log("SIMP_READ_V1");
        simp_ble_service_set_parameter(SIMPLE_BLE_SERVICE_PARAM_V1_READ_CHAR_VAL, 2, &value);
    }
}
```

### Write

在手机APP上发起对UUID为B002的特性值的写入操作，在手机上发送任意数据，模块收到后会打印出相应的hex数据

```c
case SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE:
{
    switch (p_simp_cb_data->msg_data.write.opcode)
    {
    case SIMP_WRITE_V2:
    {
        app_log("SIMP_WRITE_V2: write type %d, len %d", p_simp_cb_data->msg_data.write.write_type,
                p_simp_cb_data->msg_data.write.len);
        if (p_simp_cb_data->msg_data.write.len > 0)
        {
            str = DataToHexStringWithSpaces(p_simp_cb_data->msg_data.write.p_value, p_simp_cb_data->msg_data.write.len);
            app_log("%s", str);
            if (str)
                free(str);
        }
    }
    break;
```

### Notify

在手机APP上将UUID为B003的特性值的notify属性设置为enable，以便手机可以接收到模块发送的notification数据。

程序初始化时，增加了一个名为notify的cli命令，用户可以通过调试串口发送notification数据给客户端。

例：notify hello

### Indicate

在手机APP上将UUID为B004的特性值的indicate属性设置为enable，以便手机可以接收到模块发送的indication数据。

程序初始化时，增加了一个名为indicate的cli命令，用户可以通过调试串口发送indication数据给客户端

例：indicate hello
