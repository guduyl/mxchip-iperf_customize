
#include "mxos.h"
#include "audio_interface.h"

#define player_log(M, ...) custom_log("player", M, ##__VA_ARGS__)

/* callback from audio dac, notify all pcm is played*/
void audio_dac_empty(void)
{
    //player_log("DAC real empty");
}

static void reocrd(uint8_t *buf, int len)
{
    int offset = 0, ret;
    
    audio_adc_start();
    player_log("record start");
    audio_adc_channel_set(1); // 1 for mono, 2 for stereo
    audio_adc_samplerate_set(16000);
    
    while(offset < len) {
        player_log("offset %d, left %d", offset, len-offset);
        ret = audio_adc_read(&buf[offset], len-offset);
        offset += ret;
    }
    player_log("record done");
    audio_adc_stop();
}

static void play(uint8_t *buf, int len)
{
    int offset = 0, block=1024;

    audio_dac_start();
    audio_dac_channel_set(1); // 1 for mono, 2 for stereo
    audio_dac_samplerate_set(16000);
    audio_dac_volume_set(100); // volume from 0 to 100
    
    while(offset < len) {
        player_log("offset %d, left %d", offset, len-offset);
        audio_dac_write(&buf[offset], block);
        offset += block;
    }
    audio_dac_stop();
}

int main( void )
{
    merr_t err = kNoErr;
    uint8_t *psbuf;
    uint32_t len;
    
    /* Start mxos system functions according to mxos_config.h*/
	err = mxos_system_init();
	require_noerr(err, exit);

    audio_init();
    player_log("audio_inited");
    
    app_init_psram(); // enable PSram  

    player_log("record start");

    while(1) {
        len = 1024*1024;
        psbuf = Psram_reserve_malloc(len);
        
        player_log("start recored %d bytes, buf=%p", len, psbuf);
        reocrd(psbuf, len);

        player_log("read %d bytes, start play", len);
        play(psbuf, len);
        player_log("play done");
        
        Psram_reserve_free(psbuf);
    }

exit:
    mos_thread_delete( NULL );
    return err;
}
