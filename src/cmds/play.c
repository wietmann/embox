/**
 * @file
 * @brief Simple audio player
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-26
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <drivers/audio/portaudio.h>
#include <fs/file_format.h>
#include <util/math.h>
#include <framework/mod/options.h>

/* FIXME */
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sd.h"

#define MINIMAL_BUILD      OPTION_GET(BOOLEAN, minimal_build)
#if MINIMAL_BUILD
 #define FRAMES_PER_BUFF   160
 #define CHAN_N_DEFAULT    1
#else
 #define AUDIO_BUF_SIZE     OPTION_GET(NUMBER,  audio_buf_size)
 #define AUDIO_SDRAM        OPTION_GET(BOOLEAN, wav_sdram)
 #define FRAMES_PER_BUFF   (256 * 1024 / 4)
 #define CHAN_N_DEFAULT    2
 
 #define WAV_STORAGE       OPTION_GET(NUMBER, wav_storage)
#endif

#define WAV_FS     0
#define WAV_SDCARD 1
#define WAV_SDRAM  2

struct _wav_info {
	uint16_t format;
	uint16_t chan_n;
	uint32_t sample_rate;
	uint16_t bits_per_sample;
	uint32_t fdata_len;
};

static void print_usage(void) {
#if WAV_FS
	printf("Usage: play [WAVAUDIOFILE]\n"
	       "       play -s\n");
#else
	printf("Usage: play -s\n");
#endif
}

double _sin(double x) {
	double m = 1.;
	while (x > 2. * 3.14)
		x -= 2. * 3.14;

	if (x > 3.14) {
		x -= 3.14;
		m = -1.;
	}
	return m * (x - x * x * x / 6. + x * x * x * x * x / 120.
		- x * x * x * x * x * x * x / (120 * 6 * 7)
		+ x * x * x * x * x * x * x * x * x/ (120 * 6 * 7 * 8 * 9));
}

static int _sin_w = 100;
static int _sin_h = 30000;
static int sin_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	uint16_t *data;

	data = outputBuffer;

	for (int i = 0; i < framesPerBuffer; i++) {
		double x = 2 * 3.14 * (i % _sin_w) / _sin_w;
		*data++ = (uint16_t) ((1. + _sin(x)) * _sin_h); /* Left channel  */
		*data++ = (uint16_t) ((1. + _sin(x)) * _sin_h); /* Right channel */
	}

	return 0;
}

#if !MINIMAL_BUILD
static FILE *wav_fd = NULL;
static int _fchan = 2;
static int _fbuf_len = AUDIO_BUF_SIZE;
static uint8_t *_fbuf;

#if !AUDIO_SDRAM
static uint8_t static_audio_buffer[AUDIO_BUF_SIZE];
static void audio_buf_init(void) {
	_fbuf = static_audio_buffer;
}
#else
extern uint32_t sdram_start_address(void);
static void audio_buf_init(void) {
	_fbuf = (uint8_t*) sdram_start_address();
}
#endif /* AUDIO_SDRAM */

static int fd_callback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData) {
	static int _ptr = 0;
	int read_bytes;

	read_bytes = min(_fbuf_len - _ptr, framesPerBuffer * _fchan * 2); /* Stereo 16-bit */
	//printf("FP = %d, LEN - %d, read_bytes = %d\n", framesPerBuffer, framesPerBuffer * _fchan * 2, read_bytes);
	memcpy(outputBuffer, &_fbuf[_ptr], read_bytes);
	_ptr += read_bytes;

	printf("|");
	fflush(stdout);
	if (_ptr < _fbuf_len) {
		return paContinue;
	} else {
		printf("\n");
		_ptr = 0;
		return paComplete;
	}
}

static int read_wav_file(const char *filename, struct _wav_info *wi) {
	static uint8_t fmt_buf[128];
#if WAV_STORAGE == WAV_SDRAM
	audio_buf_init();
	memcpy(fmt_buf, _fbuf, 128);
#elif WAV_STORAGE == WAV_SDCARD
	int res, i;
	res = BSP_SD_ReadBlocks((uint32_t*) fmt_buf, 0, 512, 1);
	if (res != SD_OK) {
		printf("play: BSP_SD_ReadBlocks read 0 block failed\n");
		return -1;
	}
#elif WAV_STORAGE == WAV_FS
	if (NULL == (wav_fd = fopen(filename, "r"))) {
		printf("Can't open file %s\n", filename);
		return 0;
	}
	fread(fmt_buf, 1, 44, wav_fd);
#elif
 #error "Undefined WAV_STORAGE"
#endif

	if (raw_get_file_format(fmt_buf) != RIFF_FILE) {
		printf("%s is not a RIFF audio file\n", filename);
		return 0;
	}

	wi->format       = *((uint16_t*) &fmt_buf[20]);
	wi->chan_n          = *((uint16_t*) &fmt_buf[22]);
	wi->sample_rate     = *((uint32_t*) &fmt_buf[24]);
	wi->bits_per_sample = *((uint16_t*) &fmt_buf[34]);
	wi->fdata_len       = *((uint32_t*) &fmt_buf[40]);

	printf("File size:             %d bytes\n",
	       *((uint32_t*) &fmt_buf[4]));
	printf("File type header:      %c%c%c%c\n",
	        fmt_buf[8], fmt_buf[9], fmt_buf[10], fmt_buf[11]);
	printf("Length of format data: %d\n", *((uint32_t*) &fmt_buf[16]));
	printf("Type format:           %d\n", wi->format);
	printf("Number of channels:    %d\n", wi->chan_n);
	printf("Sample rate:           %d\n", wi->sample_rate);
	printf("Bits per sample:       %d\n", wi->bits_per_sample);
	printf("Size of data section:  %d\n", wi->fdata_len);

	if (wi->bits_per_sample * wi->sample_rate * wi->chan_n == 0) {
		printf("Check bps, sample rate and channel number, they should not be zero!\n");
		return -1;
	}

	printf("Progress:\n");

#if WAV_STORAGE == WAV_SDCARD
	for (i = 1; i < wi->fdata_len / 512; i++) {
		res = BSP_SD_ReadBlocks((uint32_t*) (_fbuf + i * 512), i * 512, 512, 1);
		while (res != SD_OK) {
			printf("play: BSP_SD_ReadBlocks read %d block failed\n", i);
			res = BSP_SD_ReadBlocks((uint32_t*) (_fbuf + i * 512), i * 512, 512, 1);
		}
	}
#elif WAV_STORAGE == WAV_FS
	_fbuf_len = min(fread(_fbuf, 1, 64 * 1024 * 1024, wav_fd), _fbuf_len);
#endif
	printf("play: File read completed successfully!\n");

	_fbuf_len = wi->fdata_len;

	_fchan = wi->chan_n;
	return 0;
}
#endif /* MINIMAL_BUILD */

int main(int argc, char **argv) {
	int opt;
	int err;
	int sleep_msec;
	struct _wav_info wi = {
		.chan_n = CHAN_N_DEFAULT,
		.sample_rate = 44100,
		.bits_per_sample = 16,
		.fdata_len = 0x100000
	};

	PaStreamCallback *callback = NULL;
	PaStream *stream = NULL;

	struct PaStreamParameters out_par;

#if WAV_STORAGE == WAV_FS
	if (argc < 2) {
		print_usage();
		return 0;
	}
#endif

	while (-1 != (opt = getopt(argc, argv, "nsh"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			callback = &sin_callback;
			break;
		default:
			printf("Unknown argument: %c", opt);
			return 0;
		}
	}

#if !MINIMAL_BUILD
	if (callback == NULL) {
		callback = &fd_callback;
	}
	if (callback == fd_callback) {
		err = read_wav_file(argv[argc - 1], &wi);
		if (err != 0) {
			goto err_clean;
		}
	}
#else
	if (callback == NULL) {
		print_usage();
		return 0;
	}
#endif

	/* Initialize PA */
	if (paNoError != (err = Pa_Initialize())) {
		printf("Portaudio error: could not initialize!\n");
		goto err_clean;
	}

	out_par = (PaStreamParameters) {
		.device                    = 0,
		.channelCount              = wi.chan_n,
		.sampleFormat              = paInt16,
		.suggestedLatency          = 10,
		.hostApiSpecificStreamInfo = NULL,
	};

	err = Pa_OpenStream(&stream,
			NULL,
			&out_par,
			wi.sample_rate,
			FRAMES_PER_BUFF,
			0,
			callback,
			NULL);

	if (err != paNoError) {
		printf("Portaudio error: could not open stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_StartStream(stream))) {
		printf("Portaudio error: could not start stream!\n");
		goto err_terminate_pa;
	}

	sleep_msec = 1000 * (wi.fdata_len /
	           (wi.bits_per_sample / 8 * wi.sample_rate * wi.chan_n));
	Pa_Sleep(sleep_msec);

	if (paNoError != (err = Pa_StopStream(stream))) {
		printf("Portaudio error: could not stop stream!\n");
		goto err_terminate_pa;
	}

	if (paNoError != (err = Pa_CloseStream(stream))) {
		printf("Portaudio error: could not close stream!\n");
		goto err_terminate_pa;
	}

err_terminate_pa:
	if (paNoError != (err = Pa_Terminate()))
		printf("Portaudio error: could not terminate!\n");

err_clean:
#if !MINIMAL_BUILD
	if (wav_fd)
		fclose(wav_fd);
#endif

	return 0;
}
