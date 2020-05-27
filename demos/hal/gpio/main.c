/**
 ******************************************************************************
 * @file    gpio.c
 * @author  Yang Haibo
 * @version V1.0.0
 * @date    21-May-2020
 * @brief   gpio demo, connect input pin with output pin.
 ******************************************************************************
 */

#include "mxos.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "gpio", M, ##__VA_ARGS__)

#define INPUT_PIN    MODULE_PIN_3
#define OUTPUT_PIN   MODULE_PIN_4

static mos_queue_id_t fall_queue, rising_queue;
static int fall_num=0, rising_num=0;

static void input_falling_cb(void)
{
    fall_num++;
    mos_queue_push(fall_queue, &fall_num, 0);
}

static void input_rising_cb(void)
{
    rising_num++;
    mos_queue_push(rising_queue, &rising_num, 0);
}

static void gpio_input_demo(void)
{
    int ret;
    uint32_t val;

    fall_num=0, rising_num=0;
    app_log("set GPIO input pullup, IRQ falling edge");

    fall_queue = mos_queue_new(sizeof(int), 100);
    rising_queue = mos_queue_new(sizeof(int), 100);
    mhal_gpio_open( INPUT_PIN, INPUT_PULL_UP );
    mhal_gpio_int_on( INPUT_PIN, IRQ_TRIGGER_FALLING_EDGE, input_falling_cb, NULL );
    while(1) {
        ret = mos_queue_pop(fall_queue, &val, 10*1000);
        if (ret != kNoErr) {
            app_log("wait IRQ timeout");
            break;
        } else {
            app_log("GOT IRQ, val=%d", val);
        }
    }

    app_log("set GPIO input pulldown, IRQ rising edge");
    mhal_gpio_open( INPUT_PIN, INPUT_PULL_DOWN );
    mhal_gpio_int_on( INPUT_PIN, IRQ_TRIGGER_RISING_EDGE, input_rising_cb, NULL );
    while(1) {
        ret = mos_queue_pop(rising_queue, &val, 10*1000);
        if (ret != kNoErr) {
            app_log("wait IRQ timeout");
            break;
        } else {
            app_log("GOT IRQ, val=%d", val);
        }
    }
}

static void gpio_output_demo(void)
{
    mhal_gpio_open( OUTPUT_PIN, OUTPUT_PUSH_PULL );
#ifdef CONFIG_MX1350
    portDISABLE_FIQ();
    portDISABLE_IRQ();
#endif
    while(1) {
        mhal_gpio_high(OUTPUT_PIN);
        mhal_gpio_low(OUTPUT_PIN);
    }
}

#ifdef CONFIG_MX1350
static void gpio_output_fast_demo(void)
{
    volatile uint32_t *pin = GPIO_ADDR(OUTPUT_PIN);
    uint32_t high = GPIO_HIGH, low=GPIO_LOW;
    
    mhal_gpio_open( OUTPUT_PIN, OUTPUT_PUSH_PULL );

    app_log("pin val %x", *pin);
    high = *pin | high;
    low = *pin & low;
    app_log("high %x, low %x", high, low);

    portDISABLE_FIQ();
    portDISABLE_IRQ();

    while(1) {
        *pin = high;
        *pin = low;
    }
    
}
#endif

int main(void)
{
    gpio_input_demo();
    
    gpio_output_demo();
#ifdef CONFIG_MX1350
    gpio_output_fast_demo();
#endif

    return 0;
}
