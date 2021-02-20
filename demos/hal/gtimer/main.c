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
#include "mhal/gtimer.h"

#define app_log(M, ...) MXOS_LOG(CONFIG_APP_DEBUG, "APP", M, ##__VA_ARGS__)

static int num = 0;

static void gtimer_cb(void *arg)
{
    num++;
}

int main(void)
{
	mhal_gtimer_open(GTIMER1, PERIOIC, 50, gtimer_cb, NULL);
    mhal_gtimer_start(GTIMER1);
    while(1) {
        app_log("gtimer val %d", num);
        mos_msleep(2000);
    }
    

	return 0;
}
