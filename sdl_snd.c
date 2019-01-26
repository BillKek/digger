#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "def.h"
#include "device.h"
#include "hardware.h"
#include "digger_math.h"
#include "digger_log.h"
#include "newsnd.h"

static void fill_audio(void *udata, uint8_t *stream, int len);

bool wave_device_available = false;

bool initsounddevice(void)
{
	return(true);
}

struct sudata {
    SDL_AudioSpec obtained;
    int16_t *buf;
    unsigned int bsize;
    struct bqd_filter *lp_fltr;
    struct bqd_filter *hp_fltr;
};

bool setsounddevice(uint16_t samprate, uint16_t bufsize)
{
	SDL_AudioSpec wanted;
        struct sudata *sud;
	bool result = false;
	
        sud = (struct sudata*)malloc(sizeof(*sud));
        if (sud == NULL) {
                fprintf(digger_log, "setsounddevice: malloc(3) failed\n");

                return (false);
        }
        memset(sud, '\0', sizeof(*sud));
        SDL_zero(wanted);
        SDL_zero(sud->obtained);
	wanted.freq = samprate;
	wanted.samples = bufsize;
	wanted.channels = 1;
	wanted.format = AUDIO_S16;
	wanted.userdata = sud;
	wanted.callback = fill_audio;

	if ((SDL_Init(SDL_INIT_AUDIO)) >= 0)
		if ((SDL_OpenAudio(&wanted, &sud->obtained)) >= 0)
			result = true;
	if (result == false) {
		fprintf(digger_log, "Couldn't open audio: %s\n", SDL_GetError());
                free(sud);
                return (false);
        }
        sud->bsize = sud->obtained.size;
	sud->buf = (int16_t*)malloc(sud->bsize);
        if (sud->buf == NULL) {
                fprintf(digger_log, "setsounddevice: malloc(3) failed\n");
                SDL_CloseAudio();
                free(sud);
                return (false);
        }
        sud->lp_fltr = bqd_lp_init(sud->obtained.freq, 4000);
        sud->hp_fltr = bqd_hp_init(sud->obtained.freq, 1000);
	wave_device_available = true;

	return(result);
}

static void fill_audio(void *udata, uint8_t *stream, int len)
{
	int i;
        struct sudata *sud;
#if !defined(NO_SND_FILTER)
        double sample;
#endif

        sud = (struct sudata *)udata;
        SDL_memset(stream, sud->obtained.silence, len);
	if (len > sud->bsize) {
                fprintf(digger_log, "fill_audio: OUCH, len > bsize!\n");
		len = sud->bsize;
        }
	for (i = 0; i < len / sizeof(int16_t); i++) {
#if !defined(NO_SND_FILTER)
		sample = getsample();
		sample = bqd_apply(sud->hp_fltr, (sample - 127.0) * 128.0);
                sud->buf[i] = round(bqd_apply(sud->lp_fltr, sample));
#else
		sud->buf[i] = getsample();
#endif
        }

	SDL_MixAudioFormat(stream, (uint8_t *)sud->buf, sud->obtained.format, len,
            SDL_MIX_MAXVOLUME);
}


void killsounddevice(void)
{
	SDL_PauseAudio(1);
}

