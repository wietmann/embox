/**
 * @file
 *
 * @data 10.08.2017
 * @author Alex Kalmuk
 */

#ifndef SRC_DRIVERS_AUDIO_STM32_STM32_AUDIO_F7_H_
#define SRC_DRIVERS_AUDIO_STM32_STM32_AUDIO_F7_H_

#include <assert.h>
#include <kernel/irq.h>
#include <framework/mod/options.h>

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_audio.h"

#define DEFAULT_VOLUME           OPTION_GET(NUMBER, volume)
#define DEFAULT_SAMPLE_RATE      OPTION_GET(NUMBER, sample_rate)

#define MAX_BUF_LEN     (160 * 6)
#define OUTPUT_CHAN_N   2
#define STM32_MAX_BUF_LEN  (MAX_BUF_LEN * OUTPUT_CHAN_N)

#define STM32_AUDIO_OUT_IRQ  (AUDIO_OUT_SAIx_DMAx_IRQ + 16)
static_assert(76 == STM32_AUDIO_OUT_IRQ);

extern SAI_HandleTypeDef haudio_out_sai;

static inline void audio_out_irq_handler(void) {
  HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

#endif /* SRC_DRIVERS_AUDIO_STM32_STM32_AUDIO_F7_H_ */
