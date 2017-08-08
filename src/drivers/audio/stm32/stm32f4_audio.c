/**
 * @file
 * @brief Driver for STM32F4
 * @author Alex Kalmuk
 * @version 0.1
 * @date 07.08.2017
 */

#include <unistd.h>
#include <errno.h>
#include <util/log.h>
#include <stdio.h>
#include <assert.h>
#include <drivers/audio/portaudio.h>
#include <drivers/audio/audio_dev.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <framework/mod/options.h>

#include <stm32f4_discovery_audio_codec.h>
#include <stm32f4xx.h>

#define DEFAULT_VOLUME      OPTION_GET(NUMBER, volume)
#define DEFAULT_SAMPLE_RATE OPTION_GET(NUMBER, sample_rate)

extern void Audio_MAL_I2S_IRQHandler(void);
static irq_return_t stm32f4_audio_i2s_dma_interrupt(unsigned int irq_num,
        void *dev_id);

struct stm32f4_dev_priv {
	int devid;
	uint8_t *out_buf;
	uint32_t out_buf_len;
};

#define STM32F4_AUDIO_I2S_DMA_IRQ (AUDIO_I2S_DMA_IRQ + 16)

#define MAX_BUF_LEN     (160 * 6)
#define OUTPUT_CHAN_N   2
#define BUF_N           2

#define STM32F4_MAX_BUF_LEN  (MAX_BUF_LEN * OUTPUT_CHAN_N)

static uint8_t dac_out_bufs[STM32F4_MAX_BUF_LEN * BUF_N];

static void stm32f4_dev_start(struct audio_dev *dev) {
	if (0 != irq_attach(STM32F4_AUDIO_I2S_DMA_IRQ, stm32f4_audio_i2s_dma_interrupt,
				0, dev, "stm32f4_audio")) {
		log_error("irq_attach error");
	}

	EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);

	if (0 != EVAL_AUDIO_Init(OUTPUT_DEVICE_HEADPHONE, DEFAULT_VOLUME,
				DEFAULT_SAMPLE_RATE)) {
		log_error("EVAL_AUDIO_Init error");
	}

	if (0 != EVAL_AUDIO_Play((uint16_t*) &dac_out_bufs[0], sizeof(dac_out_bufs))) {
		log_error("EVAL_AUDIO_Play error");
	}
}

static void stm32f4_dev_pause(struct audio_dev *dev) {
	if (0 != EVAL_AUDIO_PauseResume(AUDIO_PAUSE)) {
		log_error("EVAL_AUDIO_PauseResume(AUDIO_PAUSE) error");
	}
}

static void stm32f4_dev_resume(struct audio_dev *dev) {
}

static void stm32f4_dev_stop(struct audio_dev *dev) {
	EVAL_AUDIO_Stop(CODEC_PDWN_SW);
	EVAL_AUDIO_DeInit();

	irq_detach(STM32F4_AUDIO_I2S_DMA_IRQ, NULL);
}

static int stm32f4_dev_ioctl(struct audio_dev *dev, int cmd, void *args) {
	switch(cmd) {
	case ADIOCTL_BUFLEN:
		return STM32F4_MAX_BUF_LEN;
	default:
		log_error("Unsupported ioctl cmd %d", cmd);
	}
	SET_ERRNO(EINVAL);
	return -1;
}

static struct audio_dev_info stm32f4_dac_info;

static const struct audio_dev_ops stm32f4_dev_ops = {
	.ad_ops_start  = stm32f4_dev_start,
	.ad_ops_pause  = stm32f4_dev_pause,
	.ad_ops_resume = stm32f4_dev_resume,
	.ad_ops_stop   = stm32f4_dev_stop,
	.ad_ops_ioctl  = stm32f4_dev_ioctl
};

static struct stm32f4_dev_priv stm32f4_dac = {
	.devid       = 1,
	.out_buf     = &dac_out_bufs[0],
	.out_buf_len = STM32F4_MAX_BUF_LEN,
};

AUDIO_DEV_DEF("stm32f4_dac", (struct audio_dev_ops *)&stm32f4_dev_ops, &stm32f4_dac, &stm32f4_dac_info);

uint8_t *audio_dev_get_in_cur_ptr(struct audio_dev *audio_dev) {
	return NULL;
}

uint8_t *audio_dev_get_out_cur_ptr(struct audio_dev *audio_dev) {
	struct stm32f4_dev_priv *priv = audio_dev->ad_priv;
	return priv->out_buf;
}

static void stm32f4_audio_irq_fill_buffer(int buf_index) {
	stm32f4_dac.out_buf = &dac_out_bufs[0] + buf_index * STM32F4_MAX_BUF_LEN;
	Pa_StartStream(NULL);
}

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size) {
	stm32f4_audio_irq_fill_buffer(0);
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size) {
	stm32f4_audio_irq_fill_buffer(1);
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void) {
	panic("EVAL_AUDIO_GetSampleCallBack");
	return 0;
}

uint32_t Codec_TIMEOUT_UserCallback(void) {
	panic("Codec_TIMEOUT_UserCallback");
	return 0;
}

static irq_return_t stm32f4_audio_i2s_dma_interrupt(unsigned int irq_num,
		void *audio_dev) {
	Audio_MAL_I2S_IRQHandler();
	return IRQ_HANDLED;
}

static_assert(63 == STM32F4_AUDIO_I2S_DMA_IRQ);
STATIC_IRQ_ATTACH(63, stm32f4_audio_i2s_dma_interrupt, NULL);
