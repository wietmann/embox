/**
 * @file
 * @brief Simple audio recorder
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-11-21
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <drivers/audio/portaudio.h>
#include <fs/file_format.h>
#include <util/math.h>
#include <framework/mod/options.h>

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"

#define AUDIO_SDRAM        OPTION_GET(BOOLEAN, wav_sdram)
#define AUDIO_BUF_SIZE     OPTION_GET(NUMBER,  audio_buf_size)

static void print_usage(void) {
	printf("Usage: record\n");
}

static uint8_t *in_buf;
static int cur_ptr;

extern uint32_t sdram_start_address(void);
static void audio_buf_init(void) {
	in_buf = (uint8_t*) (sdram_start_address() + sizeof(struct wave_header));
}
static void write_wave(char *name, uint8_t *buf, int len) {
#if !AUDIO_SDRAM
	int i, res;
	uint8_t *start = (uint8_t *)sdram_start_address();
#endif
	struct wave_header *hdr = (struct wave_header *) sdram_start_address();

	printf("\n record len = %d\n", len);
	_wave_header_fill(hdr, 2, 44100, 16, len);

#if !AUDIO_SDRAM
	for (i = 0; i < len / 512; i++) {
		res = BSP_SD_WriteBlocks((uint32_t*) (start + i * 512), i * 512, 512, 1);
		while (res != SD_OK) {
			printf("record: BSP_SD_WriteBlocks read %d block failed\n", i);
			res = BSP_SD_WriteBlocks((uint32_t*) (start + i * 512), i * 512, 512, 1);
		}
	}
#endif
}

extern uint8_t *get_audio_in_buffer(void);

static int record_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	/* FIXME */
	memcpy(&in_buf[cur_ptr], get_audio_in_buffer(), framesPerBuffer * 2 * 2);
	cur_ptr += framesPerBuffer * 4;

	if (cur_ptr > AUDIO_BUF_SIZE)
		return paComplete;
	return paContinue;
}

int main(int argc, char **argv) {
	int opt;
	int err;
	int sample_rate = 44100;
	int sleep_msec = 10000;
	PaStream *stream = NULL;

	struct PaStreamParameters in_par;

	while (-1 != (opt = getopt(argc, argv, "nsh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			printf("Unknown argument: %c", opt);
			return 0;
		}
	}

	audio_buf_init();

	cur_ptr = 0;
	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_exit;
	}

	in_par = (PaStreamParameters) {
		.device                    = Pa_GetDefaultInputDevice(),
		.channelCount              = 2,
		.sampleFormat              = paInt16,
		.suggestedLatency          = 10,
		.hostApiSpecificStreamInfo = NULL,
	};

	err = Pa_OpenStream(&stream,
			&in_par,
			NULL,
			sample_rate,
			256 * 1024 / 4,
			0,
			record_callback,
			NULL);

	if (err != paNoError) {
		printf("Portaudio error: could not open stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_StartStream(stream))) {
		printf("Portaudio error: could not start stream!\n");
		goto err_terminate_pa;
	}

	printf("Recording! Speak to the microphone\n");

	Pa_Sleep(sleep_msec);

	if (paNoError != (err = Pa_StopStream(stream))) {
		printf("Portaudio error: could not stop stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_CloseStream(stream))) {
		printf("Portaudio error: could not close stream!\n");
		goto err_terminate_pa;
	}

	write_wave(argv[1], in_buf, cur_ptr);

err_terminate_pa:
	if (paNoError != (err = Pa_Terminate()))
		printf("Portaudio error: could not terminate!\n");
err_exit:
	return 0;
}
