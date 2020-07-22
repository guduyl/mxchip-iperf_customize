// Copyright (c) 2015-2019 The HomeKit ADK Contributors
//
// Licensed under the Apache License, Version 2.0 (the “License”);
// you may not use this file except in compliance with the License.
// See [CONTRIBUTORS.md] for the list of HomeKit ADK project authors.

// An example that implements the light bulb HomeKit profile. It can serve as a basic implementation for
// any platform. The accessory logic implementation is reduced to internal state updates and log output.
//
// This implementation is platform-independent.
//
// The code consists of multiple parts:
//
//   1. The definition of the accessory configuration and its internal state.
//
//   2. Helper functions to load and save the state of the accessory.
//
//   3. The definitions for the HomeKit attribute database.
//
//   4. The callbacks that implement the actual behavior of the accessory, in this
//      case here they merely access the global accessory state variable and write
//      to the log to make the behavior easily observable.
//
//   5. The initialization of the accessory state.
//
//   6. Callbacks that notify the server in case their associated value has changed.

#include "HAP.h"

#include "App.h"
#include "DB.h"

// hardware components on mxoskit extension board
#include "mxos.h"
#include "dc_motor.h"
#include "rgb_led.h"
#include "button.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Domain used in the key value store for application data.
 *
 * Purged: On factory reset.
 */
#define kAppKeyValueStoreDomain_Configuration ((HAPPlatformKeyValueStoreDomain) 0x00)

/**
 * Key used in the key value store to store the configuration state.
 *
 * Purged: On factory reset.
 */
#define kAppKeyValueStoreKey_Configuration_State ((HAPPlatformKeyValueStoreDomain) 0x00)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern volatile bool requestedFactoryReset;

/**
 * Global accessory configuration.
 */
typedef struct {
    struct {
        bool lightBulbOn;
    } state;
    HAPPlatformTimerRef identifyTimer;
    HAPAccessoryServerRef* server;
    HAPPlatformKeyValueStoreRef keyValueStore;
} AccessoryConfiguration;

static AccessoryConfiguration accessoryConfiguration;

static button_context_t ext_key1;
static mos_worker_thread_id_t key_event_worker_thread;

static void SaveAccessoryState(void);
HAP_RESULT_USE_CHECK
HAPError IdentifyAccessory(HAPAccessoryServerRef* server HAP_UNUSED,
		const HAPAccessoryIdentifyRequest* request HAP_UNUSED,
		void* _Nullable context HAP_UNUSED);

//----------------------------------------------------------------------------------------------------------------------

/**
 * HomeKit accessory that provides the Light Bulb service.
 *
 * Note: Not constant to enable BCT Manual Name Change.
 */
static HAPAccessory accessory = { .aid = 1,
                                  .category = kHAPAccessoryCategory_Lighting,
                                  .name = "Acme Light Bulb",
                                  .manufacturer = "Acme",
                                  .model = "LightBulb1,1",
                                  .serialNumber = "099DB48E9E28",
                                  .firmwareVersion = "1",
                                  .hardwareVersion = "1",
                                  .services = (const HAPService* const[]) { &accessoryInformationService,
                                                                            &hapProtocolInformationService,
                                                                            &pairingService,
                                                                            &lightBulbService,
                                                                            NULL },
                                  .callbacks = { .identify = IdentifyAccessory } };

//----------------------------------------------------------------------------------------------------------------------

/*
 * device identify control
 */
static void device_identify_init(void) {
	dc_motor_init();
    dc_motor_set(0); // off
}

static void device_identify_start(void) {
	dc_motor_set(1);  // on
}

static void device_identify_stop(void) {
	dc_motor_set(0); // off
}

/**
 * lightBulb on/off control
 */
static void device_rgb_led_init(void) {
    rgb_led_init();
}

static void device_rgb_led_on(void) {
    rgb_led_open(0, 0, 120); //TODO RGB
}

static void device_rgb_led_off(void) {
    rgb_led_close();
}

static void TurnOnLightBulb(void) {
	HAPLogInfo(&kHAPLog_Default, "%s: rgb led ON", __func__);
	device_rgb_led_on();
}

static void TurnOffLightBulb(void) {
	HAPLogInfo(&kHAPLog_Default, "%s: rgb led OFF", __func__);
	device_rgb_led_off();
}

/**
 * Callback to toggle the lightBulb
 */
static void ToggleLightBulbState(void) {
	accessoryConfiguration.state.lightBulbOn =
			!accessoryConfiguration.state.lightBulbOn;

	HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__,
			accessoryConfiguration.state.lightBulbOn ? "true" : "false");

	if (accessoryConfiguration.state.lightBulbOn) {
		TurnOnLightBulb();
	} else {
		TurnOffLightBulb();
	}

	SaveAccessoryState();

	HAPAccessoryServerRaiseEvent(accessoryConfiguration.server,
			&lightBulbOnCharacteristic, &lightBulbService, &accessory);
}

/**
 * user key1 control (on ext board)
 */
static void key1_clicked_callback_process(void* _Nullable context, size_t contextSize){
    HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__, "EXT_KEY1 click process");
    ToggleLightBulbState();
}

static void key1_long_pressed_callback_process(void* _Nullable context, size_t contextSize){
    HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__, "EXT_KEY1 long pressed process");

    requestedFactoryReset = true;
    HAPAccessoryServerStop(accessoryConfiguration.server);
}

static merr_t key1_clicked_callback_worker(void* arg){
    HAPError hap_err;

    HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__, "EXT_KEY1 click worker.");
    hap_err = HAPPlatformRunLoopScheduleCallback(key1_clicked_callback_process, NULL, 0);
    if(kHAPError_None != hap_err){
        HAPLogError(&kHAPLog_Default, "%s: HAPPlatformRunLoopScheduleCallback error: %u", __func__, hap_err);
        return kNoResourcesErr;
    }
    else{
        return kNoErr;
    }
}

static merr_t key1_long_pressed_callback_worker(void* arg){
    HAPError hap_err;

    HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__, "EXT_KEY1 long pressed worker.");

    hap_err = HAPPlatformRunLoopScheduleCallback(key1_long_pressed_callback_process, NULL, 0);
    if(kHAPError_None != hap_err){
        HAPLogError(&kHAPLog_Default, "%s: HAPPlatformRunLoopScheduleCallback error: %u", __func__, hap_err);
        return kNoResourcesErr;
    }
    else{
        return kNoErr;
    }
}

static void ext_key1_clicked_callback(void){
    mos_worker_send_async_event( &key_event_worker_thread, key1_clicked_callback_worker, NULL );
}

static void ext_key1_long_pressed_callback(void){
    mos_worker_send_async_event( &key_event_worker_thread, key1_long_pressed_callback_worker, NULL );
}

static void ext_key1_init(void){
    ext_key1.gpio = EXT_KEY1;
    ext_key1.idle = IOBUTTON_IDLE_STATE_HIGH;
    ext_key1.pressed_func = ext_key1_clicked_callback;
    ext_key1.long_pressed_func = ext_key1_long_pressed_callback;
    ext_key1.long_pressed_timeout = 4000;
    button_init(&ext_key1);

    mos_worker_thread_new( &key_event_worker_thread, MOS_APPLICATION_PRIORITY, 0x1000, 3 );
}

/*
 * Hardware operations, e.g Lightbulb on/off
 */
static int hardware_init(void){
    merr_t ret = kNoErr;
    static bool _hw_inited = false;

    if(_hw_inited)
    {
        return kNoErr;
    }

    /* dc motor for device identify */
    device_identify_init();

    /* RGB LED for lightbulb */
    device_rgb_led_init();

    /* ext_key1 */
    ext_key1_init();

    _hw_inited = true;

    return ret;
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * Load the accessory state from persistent memory.
 */
static void LoadAccessoryState(void) {
    HAPPrecondition(accessoryConfiguration.keyValueStore);

    HAPError err;

    // Load persistent state if available
    bool found;
    size_t numBytes;

    err = HAPPlatformKeyValueStoreGet(
            accessoryConfiguration.keyValueStore,
            kAppKeyValueStoreDomain_Configuration,
            kAppKeyValueStoreKey_Configuration_State,
            &accessoryConfiguration.state,
            sizeof accessoryConfiguration.state,
            &numBytes,
            &found);

    if (err) {
        HAPAssert(err == kHAPError_Unknown);
        HAPFatalError();
    }
    if (!found || numBytes != sizeof accessoryConfiguration.state) {
        if (found) {
            HAPLogError(&kHAPLog_Default, "Unexpected app state found in key-value store. Resetting to default.");
        }
        HAPRawBufferZero(&accessoryConfiguration.state, sizeof accessoryConfiguration.state);
    }
}

/**
 * Save the accessory state to persistent memory.
 */
static void SaveAccessoryState(void) {
    HAPPrecondition(accessoryConfiguration.keyValueStore);

    HAPError err;
    err = HAPPlatformKeyValueStoreSet(
            accessoryConfiguration.keyValueStore,
            kAppKeyValueStoreDomain_Configuration,
            kAppKeyValueStoreKey_Configuration_State,
            &accessoryConfiguration.state,
            sizeof accessoryConfiguration.state);
    if (err) {
        HAPAssert(err == kHAPError_Unknown);
        HAPFatalError();
    }
}

//----------------------------------------------------------------------------------------------------------------------

/**
 * Control duration of Identify indication.
 */
static void IdentifyTimerExpired(HAPPlatformTimerRef timer,
		void* _Nullable context) {
	HAPLogInfo(&kHAPLog_Default, "%s", __func__);

	HAPPrecondition(!context);
	HAPPrecondition(timer == accessoryConfiguration.identifyTimer);

	accessoryConfiguration.identifyTimer = 0;

	device_identify_stop();
}

/**
 * Performs the Identify routine.
 */
static void DeviceIdentify(void) {
	HAPError err;

	if (accessoryConfiguration.identifyTimer) {
		HAPPlatformTimerDeregister(accessoryConfiguration.identifyTimer);
		accessoryConfiguration.identifyTimer = 0;
	}
	err = HAPPlatformTimerRegister(&accessoryConfiguration.identifyTimer,
			HAPPlatformClockGetCurrent() + 3 * HAPSecond, IdentifyTimerExpired,
			NULL);
	if (err) {
		HAPAssert(err == kHAPError_OutOfResources);
	}

	device_identify_start();
}

HAP_RESULT_USE_CHECK
HAPError IdentifyAccessory(
        HAPAccessoryServerRef* server HAP_UNUSED,
        const HAPAccessoryIdentifyRequest* request HAP_UNUSED,
        void* _Nullable context HAP_UNUSED) {
    HAPLogInfo(&kHAPLog_Default, "%s", __func__);
    DeviceIdentify();
    return kHAPError_None;
}

HAP_RESULT_USE_CHECK
HAPError HandleLightBulbOnRead(
        HAPAccessoryServerRef* server HAP_UNUSED,
        const HAPBoolCharacteristicReadRequest* request HAP_UNUSED,
        bool* value,
        void* _Nullable context HAP_UNUSED) {
    *value = accessoryConfiguration.state.lightBulbOn;
    HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__, *value ? "true" : "false");

    return kHAPError_None;
}

HAP_RESULT_USE_CHECK
HAPError HandleLightBulbOnWrite(
        HAPAccessoryServerRef* server,
        const HAPBoolCharacteristicWriteRequest* request,
        bool value,
        void* _Nullable context HAP_UNUSED) {
    HAPLogInfo(&kHAPLog_Default, "%s: %s", __func__, value ? "true" : "false");
    if (accessoryConfiguration.state.lightBulbOn != value) {
        accessoryConfiguration.state.lightBulbOn = value;

        SaveAccessoryState();

        if (value) {
			TurnOnLightBulb();
		} else {
			TurnOffLightBulb();
		}

        HAPAccessoryServerRaiseEvent(server, request->characteristic, request->service, request->accessory);
    }

    return kHAPError_None;
}

//----------------------------------------------------------------------------------------------------------------------

void AccessoryNotification(
        const HAPAccessory* accessory,
        const HAPService* service,
        const HAPCharacteristic* characteristic,
        void* ctx) {
    HAPLogInfo(&kHAPLog_Default, "Accessory Notification");

    HAPAccessoryServerRaiseEvent(accessoryConfiguration.server, characteristic, service, accessory);
}

void AppCreate(HAPAccessoryServerRef* server, HAPPlatformKeyValueStoreRef keyValueStore) {
    HAPPrecondition(server);
    HAPPrecondition(keyValueStore);

    HAPLogInfo(&kHAPLog_Default, "%s", __func__);

    HAPRawBufferZero(&accessoryConfiguration, sizeof accessoryConfiguration);
    accessoryConfiguration.server = server;
    accessoryConfiguration.keyValueStore = keyValueStore;
    LoadAccessoryState();

    hardware_init();

    if (accessoryConfiguration.state.lightBulbOn) {
		TurnOnLightBulb();
	} else {
		TurnOffLightBulb();
	}
}

void AppRelease(void) {
}

void AppAccessoryServerStart(void) {
    HAPAccessoryServerStart(accessoryConfiguration.server, &accessory);
}

//----------------------------------------------------------------------------------------------------------------------

void AccessoryServerHandleUpdatedState(HAPAccessoryServerRef* server, void* _Nullable context) {
    HAPPrecondition(server);
    HAPPrecondition(!context);

    switch (HAPAccessoryServerGetState(server)) {
        case kHAPAccessoryServerState_Idle: {
            HAPLogInfo(&kHAPLog_Default, "Accessory Server State did update: Idle.");
            return;
        }
        case kHAPAccessoryServerState_Running: {
            HAPLogInfo(&kHAPLog_Default, "Accessory Server State did update: Running.");
            return;
        }
        case kHAPAccessoryServerState_Stopping: {
            HAPLogInfo(&kHAPLog_Default, "Accessory Server State did update: Stopping.");
            return;
        }
    }
    HAPFatalError();
}

const HAPAccessory* AppGetAccessoryInfo() {
    return &accessory;
}

void AppInitialize(
        HAPAccessoryServerOptions* hapAccessoryServerOptions,
        HAPPlatform* hapPlatform,
        HAPAccessoryServerCallbacks* hapAccessoryServerCallbacks) {
    /*no-op*/
}

void AppDeinitialize() {
    /*no-op*/
}
