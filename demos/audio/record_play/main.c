
#include "mxos.h"
#include "audio_interface.h"

#define app_log(M, ...) custom_log("APP", M, ##__VA_ARGS__)

// #define TEST_PLAY_IMMEDIATELY

/* callback from audio dac, notify all pcm is played*/
void audio_dac_empty(void)
{
    //app_log("DAC real empty");
}

#ifndef TEST_PLAY_IMMEDIATELY
static void reocrd(uint8_t *buf, int len)
{
    int offset = 0, ret;
    
    audio_adc_start();
    app_log("recording ...");
    audio_adc_channel_set(1); // 1 for mono, 2 for stereo
    audio_adc_samplerate_set(16000);
    audio_adc_gain_set(0x50, 0);  // set mic gain
    
    while(offset < len) {
        //app_log("offset %d, left %d", offset, len-offset);
        ret = audio_adc_read(&buf[offset], len-offset);
        offset += ret;
    }
    app_log("record done.");
    audio_adc_stop();
}
#endif /* !TEST_PLAY_IMMEDIATELY */

static void play(uint8_t *buf, int len)
{
    int offset = 0, block=1024;

    app_log("playing ...");
    audio_dac_start();
    audio_dac_channel_set(1); // 1 for mono, 2 for stereo
    audio_dac_samplerate_set(16000);
    audio_dac_volume_set(100); // volume from 0 to 100
    
    while(offset < len) {
        //app_log("offset %d, left %d", offset, len-offset);
        audio_dac_write(&buf[offset], block);
        offset += block;
    }
    app_log("play done.");
    audio_dac_stop();
}

int main( void )
{
    merr_t err = kNoErr;
    uint8_t *psbuf;
    uint32_t len;
	uint32_t read_offset = 0;
    
    /* Start mxos system functions according to mxos_config.h*/
	err = mxos_system_init();
	require_noerr(err, exit);

    app_log("build at %s %s for %sV%s", __TIME__, __DATE__, MODEL, HARDWARE_REVISION);

    audio_init();
    app_init_psram(); // enable PSram  
    app_log("audio_inited");

    // read from flash into psram
    len = 114688;
    psbuf = (uint8_t *)Psram_reserve_malloc(len);
    require_action(psbuf != NULL, exit, app_log("psram malloc (%ld) failed!!", len));

    app_log("read pcm ...");
	/* read data from user partition */
	err = mhal_flash_read(MODULE_PARTITION_USER, &read_offset, psbuf, len);
	require_noerr_action(err, exit, app_log("read offset: 0x%lx error!!", read_offset));

    // EMA5380 PA_EN, LOW enable
    mhal_gpio_open(MXKIT_PA_EN_PIN, OUTPUT_OPEN_DRAIN_NO_PULL);
    mhal_gpio_low(MXKIT_PA_EN_PIN);
    app_log("pa enabled");

    app_log("play welcome.");
    play(psbuf, len);
    Psram_reserve_free(psbuf);

#ifndef TEST_PLAY_IMMEDIATELY
    /* record data into psram(about 10s) then play */
    while(1) {
        len = 1024*256; // about 10s
        psbuf = (uint8_t *)Psram_reserve_malloc(len);
        
        app_log("start recored %ld bytes, buf=%p", len, psbuf);
        reocrd(psbuf, len);

        app_log("read %ld bytes, start play", len);
        play(psbuf, len);
        
        Psram_reserve_free(psbuf);
    }
#else
    /* record and play */
    audio_dac_start();
    audio_dac_channel_set(2); // 1 for mono, 2 for stereo
    audio_dac_samplerate_set(16000);
    audio_dac_volume_set(100); // volume from 0 to 100
    app_log("play started");

    audio_adc_start();
    app_log("record start");
    audio_adc_channel_set(2); // 1 for mono(left), 2 for stereo
    audio_adc_samplerate_set(16000);
    audio_adc_gain_set(0x50, 0);  // set mic gain

    len = 1024;
    psbuf = (uint8_t *)Psram_reserve_malloc(len);

    app_log("record -> play loop ...");
    while(1){
        len = audio_adc_read(psbuf, len);
        if(len){
            audio_dac_write(psbuf, len);
        }
    }

    Psram_reserve_free(psbuf);
#endif /* TEST_PLAY_IMMEDIATELY */

exit:
    mos_thread_delete( NULL );
    return err;
}
