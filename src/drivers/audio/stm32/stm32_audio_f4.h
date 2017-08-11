/**
 * @file
 *
 * @data 10.08.2017
 * @author Alex Kalmuk
 */

#ifndef SRC_DRIVERS_AUDIO_STM32_STM32_AUDIO_F4_H_
#define SRC_DRIVERS_AUDIO_STM32_STM32_AUDIO_F4_H_

#include <assert.h>
#include <kernel/irq.h>
#include <framework/mod/options.h>

#include <stm32f4_discovery_audio.h>
#include <stm32f4xx_hal.h>
#include <stm32f4_discovery.h>
#include <stm32f4xx.h>

#define DEFAULT_VOLUME           OPTION_GET(NUMBER, volume)
#define DEFAULT_SAMPLE_RATE      OPTION_GET(NUMBER, sample_rate)

#define MAX_BUF_LEN     (160 * 6)
#define OUTPUT_CHAN_N   2
#define STM32_MAX_BUF_LEN  (MAX_BUF_LEN * OUTPUT_CHAN_N)

#define STM32_AUDIO_OUT_IRQ  (I2S3_DMAx_IRQ + 16)
static_assert(63 == STM32_AUDIO_OUT_IRQ);

extern I2S_HandleTypeDef hAudioOutI2s;

void audio_out_irq_handler(void) {
  HAL_DMA_IRQHandler(hAudioOutI2s.hdmatx);
}

#endif /* SRC_DRIVERS_AUDIO_STM32_STM32_AUDIO_F4_H_ */
