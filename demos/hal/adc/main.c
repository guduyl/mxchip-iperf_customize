/**
 ******************************************************************************
 * @file    pwm.c
 * @author  guidx
 * @version V1.0.0
 * @date    17-May-2019
 * @brief   PWM demo
 ******************************************************************************
 */

#include "mxos.h"
#include "mhal/adc.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)



int main(void)
{
    float adc0, adc3;
    
	mhal_adc_open(ADC_CH0, PB_4);
    mhal_adc_open(ADC_CH3, PB_7);
    while(1) {
        adc0 = mhal_adc_read(ADC_CH0);
        adc3 = mhal_adc_read(ADC_CH3);
        app_log("adc0 %f, adc3 %f", adc0, adc3);
        mos_msleep(1000);
    }
	return 0;
}
