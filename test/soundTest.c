#define _USE_MATH_DEFINES
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

#include <process.h>
#include "../src/terminal/terminal.h"

static HANDLE audioThread;
static CRITICAL_SECTION voiceMutex;

static volatile int audioRunning = 1;

#define SAMPLE_RATE 44100
#define SOUND_CHANNELS 16
#define VOICE_FRAMES 512
#define BUFFER_COUNT 8
typedef struct SoundSettings
{
	float volume;
	float pan;
	float pitch;

	float attackStep; // fade-in speed
	float releaseStep;
} SoundSettings;
typedef struct Sound
{
	int sampleNum;
	int sampleRate;
	int16_t *samples;
} Sound;

#define FADE_TIME 0.005f

#define FADE_STEP \
	(1.0f / (SAMPLE_RATE * FADE_TIME))
typedef struct Fade
{
	float fadeVolume; // current gain
	float fadeTarget; // target gain
	float fadeStep;	  // per sample
} Fade;
typedef struct Voice
{
	Sound *sound;
	SoundSettings settings;
	float envelope; // current volume multiplier

	int samplePos;
	int playing;
	Fade fade;
} Voice;

static Voice voices[SOUND_CHANNELS];
static Voice newVoices[SOUND_CHANNELS];

Sound getSamples(double duration, double freq)
{
	int sampleCount = duration * SAMPLE_RATE;
	int16_t *samples = malloc(sampleCount * sizeof(int16_t));

	for (int i = 0; i < sampleCount; i++)
	{
		double t = (double)i / SAMPLE_RATE;

		double s =
			1.00 * sin(2 * M_PI * freq * t) +
			0.50 * sin(2 * M_PI * freq * 2 * t) +
			0.25 * sin(2 * M_PI * freq * 3 * t);

		double env = exp(-1.0 * t);

		samples[i] = (int16_t)(s * env * 15000);
	}

	return (Sound){
		.sampleNum = sampleCount,
		.sampleRate = SAMPLE_RATE,
		.samples = samples};
}


void playSound(Sound *sound, int channel, SoundSettings *settings)
{
	EnterCriticalSection(&voiceMutex);

	SoundSettings defaultSettings = {
		.volume = 1.0f,
		.pan = 0.0f,
		.pitch = 1.0f,
		.attackStep = 1.0f / (SAMPLE_RATE * 0.005f),
		.releaseStep = 1.0f / (SAMPLE_RATE * 0.005f),
	};

	if (settings)
		defaultSettings = *settings;

	// Move current voice into fading voice
	if (voices[channel].playing)
	{
		voices[channel].fade.fadeTarget = 0.0f;
		voices[channel].fade.fadeStep = FADE_STEP;
	}

	// Create replacement voice
	newVoices[channel] = (Voice){
		.sound = sound,
		.settings = defaultSettings,
		.samplePos = 0,
		.playing = 1,

		.envelope = 1.0f,

		.fade = {
			.fadeVolume = 0.0f,
			.fadeTarget = 1.0f,
			.fadeStep = FADE_STEP}};

	LeaveCriticalSection(&voiceMutex);
}
void updateFade(Voice *voice)
{
	if (voice->fade.fadeVolume < voice->fade.fadeTarget)
	{
		voice->fade.fadeVolume += voice->fade.fadeStep;

		if (voice->fade.fadeVolume > voice->fade.fadeTarget)
			voice->fade.fadeVolume = voice->fade.fadeTarget;
	}
	else if (voice->fade.fadeVolume > voice->fade.fadeTarget)
	{
		voice->fade.fadeVolume -= voice->fade.fadeStep;

		if (voice->fade.fadeVolume < voice->fade.fadeTarget)
			voice->fade.fadeVolume = voice->fade.fadeTarget;
	}

	if (voice->fade.fadeVolume <= 0.0f)
		voice->playing = 0;
}
float getVoiceSample(Voice *voice)
{
	if (!voice->playing)
		return 0;

	Sound *sound = voice->sound;

	if (voice->samplePos >= sound->sampleNum)
	{
		voice->playing = 0;
		return 0;
	}

	updateFade(voice);
	float sample =
		sound->samples[voice->samplePos] / 32768.0f;

	voice->samplePos += 1;

	return sample *
		   voice->settings.volume * voice->fade.fadeVolume;
}
void mixVoices(int16_t *output, int frames)
{
	float mix[VOICE_FRAMES * 2] = {0};

	EnterCriticalSection(&voiceMutex);

	for (int c = 0; c < SOUND_CHANNELS; c++)
	{
		Voice *old = &voices[c];
		Voice *next = &newVoices[c];

		for (int i = 0; i < frames; i++)
		{
			float sample = 0;

			sample += getVoiceSample(old);
			sample += getVoiceSample(next);

			float pan = 0;

			if (old->playing)
				pan = old->settings.pan;

			else if (next->playing)
				pan = next->settings.pan;

			float left =
				sample * ((1.0f - pan) * 0.5f);

			float right =
				sample * ((1.0f + pan) * 0.5f);

			mix[i * 2] += left;
			mix[i * 2 + 1] += right;
		}

		// Promote new voice after old one faded out
		if (!old->playing && next->playing)
		{
			*old = *next;
			next->playing = 0;
		}
	}

	LeaveCriticalSection(&voiceMutex);

	for (int i = 0; i < frames * 2; i++)
	{
		float sample = mix[i];

		if (sample > 1.0f)
			sample = 1.0f;

		if (sample < -1.0f)
			sample = -1.0f;

		output[i] = sample * 32767;
	}
}
void playVoices()
{
	WAVEFORMATEX format = {0};

	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 2;
	format.nSamplesPerSec = SAMPLE_RATE;
	format.wBitsPerSample = 16;
	format.nBlockAlign = 4;
	format.nAvgBytesPerSec = SAMPLE_RATE * 4;

	HWAVEOUT wave;

	if (waveOutOpen(
			&wave,
			WAVE_MAPPER,
			&format,
			0,
			0,
			CALLBACK_NULL) != MMSYSERR_NOERROR)
	{
		return;
	}

	int16_t buffers[BUFFER_COUNT][VOICE_FRAMES * 2];
	WAVEHDR headers[BUFFER_COUNT] = {0};

	// Fill and submit all buffers initially
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		mixVoices(buffers[i], VOICE_FRAMES);

		headers[i].lpData =
			(LPSTR)buffers[i];

		headers[i].dwBufferLength =
			sizeof(buffers[i]);

		waveOutPrepareHeader(
			wave,
			&headers[i],
			sizeof(WAVEHDR));

		waveOutWrite(
			wave,
			&headers[i],
			sizeof(WAVEHDR));
	}

	int currentBuffer = 0;

	while (1)
	{
		// wait until this buffer finishes
		while (!(headers[currentBuffer].dwFlags & WHDR_DONE))
		{
			Sleep(1);
		}

		// recycle finished buffer
		waveOutUnprepareHeader(
			wave,
			&headers[currentBuffer],
			sizeof(WAVEHDR));

		mixVoices(
			buffers[currentBuffer],
			VOICE_FRAMES);

		headers[currentBuffer].lpData =
			(LPSTR)buffers[currentBuffer];

		headers[currentBuffer].dwBufferLength =
			sizeof(buffers[currentBuffer]);

		headers[currentBuffer].dwFlags = 0;

		waveOutPrepareHeader(
			wave,
			&headers[currentBuffer],
			sizeof(WAVEHDR));

		waveOutWrite(
			wave,
			&headers[currentBuffer],
			sizeof(WAVEHDR));

		// swap buffers
		currentBuffer++;
		if (currentBuffer >= BUFFER_COUNT)
			currentBuffer = 0;
	}

	// cleanup
	waveOutReset(wave);

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		waveOutUnprepareHeader(
			wave,
			&headers[i],
			sizeof(WAVEHDR));
	}

	waveOutClose(wave);
}
DWORD WINAPI audioThreadFunc(void *data)
{
	playVoices();
	printf("thread died");
	return 0;
}
int main()
{
	InitializeCriticalSection(&voiceMutex);
	Sound sounds[10];
	Key keys[10] = {
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
	};
	for (size_t i = 0; i < 10; i++)
	{
		float freq = pow(i, (i) / 12.0f) * 440;
		sounds[i] = getSamples(2, freq);
		// sounds[i] = getSamples(7, 10 * (10 + i));
	}

	Sound b = getSamples(2, 523);

	audioThread = CreateThread(
		NULL,
		0,
		audioThreadFunc,
		NULL,
		0,
		NULL);
	terminalInit();

	while (1)
	{

		poolInput();
		for (size_t i = 0; i < 10; i++)
		{
			SoundSettings defaultSettings = (SoundSettings){
				.volume = 1.0f,
				.pan = (i / 10.f),
				.pitch = 1.0f,
				.attackStep = 1.0f / (SAMPLE_RATE * 0.001f),

			};
			if (terminalGetKeyState(keys[i]) == KEY_JUST_PRESSED)
			{
				playSound(&sounds[i], i, &defaultSettings);
			};
		}
	}

	audioRunning = 0;

	WaitForSingleObject(
		audioThread,
		INFINITE);

	DeleteCriticalSection(&voiceMutex);
}