#include "mxos.h"
#include "sflash.h"

void sFlash_DelayMs(uint32_t ms)
{
  mos_msleep(ms);
}

void sFlash_Command(uint8_t cmd, uint8_t *arg, uint32_t argc, uint8_t *buf, uint32_t n, sFlashDir_t dir)
{
  mhal_gpio_low(MXKIT_CS);
  mhal_spi_write_and_read(MXKIT_SPI, &cmd, NULL, 1);
  if (arg && argc > 0)
  {
    mhal_spi_write_and_read(MXKIT_SPI, arg, NULL, argc);
  }
  if (buf && n > 0)
  {
    if (dir == DIR_W)
    {
      mhal_spi_write_and_read(MXKIT_SPI, buf, NULL, n);
    }
    else
    {
      mhal_spi_write_and_read(MXKIT_SPI, NULL, buf, n);
    }
  }
  mhal_gpio_high(MXKIT_CS);
}