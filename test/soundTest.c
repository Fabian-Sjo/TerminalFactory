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
typedef struct Sound
{
	int sampleNum;
	int sampleRate;
	int16_t *samples;
} Sound;

typedef struct Voice
{
	Sound *sound;

	int samplePos;
	float volume;
	float pan;
	float pitch;

	float envelope;	  // current volume multiplier
	float attackStep; // fade-in speed
	float releaseStep;

	int playing;
} Voice;

static Voice voices[SOUND_CHANNELS];

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

		double env = exp(-3.0 * t);

		samples[i] = (int16_t)(s * env * 15000);
	}

	return (Sound){
		.sampleNum = sampleCount,
		.sampleRate = SAMPLE_RATE,
		.samples = samples};
}

void playSound(Sound *sound, int channel)
{
	EnterCriticalSection(&voiceMutex);

	voices[channel] = (Voice){
		.sound = sound,
		.samplePos = 0,
		.volume = 1.0f,
		.pan = 0.0f,
		.pitch = 1.0f,
		.envelope = 0.0f,
		.attackStep = 1.0f / (SAMPLE_RATE * 0.001f),

		.playing = 1};

	LeaveCriticalSection(&voiceMutex);
}

void mixVoices(int16_t *output, int frames)
{
	for (int i = 0; i < frames * 2; i++)
		output[i] = 0;

	EnterCriticalSection(&voiceMutex);

	for (int v = 0; v < SOUND_CHANNELS; v++)
	{
		Voice *voice = &voices[v];

		if (!voice->playing)
			continue;

		Sound *sound = voice->sound;

		for (int i = 0; i < frames; i++)
		{
			if (voice->samplePos >= sound->sampleNum)
			{
				voice->playing = 0;
				break;
			}

			float sample =
				sound->samples[voice->samplePos] / 32768.0f;
			if (voice->envelope < 1.0f)
			{
				voice->envelope += voice->attackStep;

				if (voice->envelope > 1.0f)
					voice->envelope = 1.0f;
			}

			sample *= voice->volume * voice->envelope;

			int index = i * 2;

			int left =
				output[index] +
				sample * 32767;

			int right =
				output[index + 1] +
				sample * 32767;

			if (left > 32767)
				left = 32767;
			if (left < -32768)
				left = -32768;

			if (right > 32767)
				right = 32767;
			if (right < -32768)
				right = -32768;

			output[index] = left;
			output[index + 1] = right;

			voice->samplePos++;
		}
	}

	LeaveCriticalSection(&voiceMutex);
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

	// Fill and submit both buffers initially
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

		// check if anything is still playing
		int active = 0;

		for (int i = 0; i < SOUND_CHANNELS; i++)
		{
			if (voices[i].playing)
			{
				active = 1;
				break;
			}
		}

		if (!active)
			continue;

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
			currentBuffer = 1;
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

	Sound a = getSamples(2, 440);
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
		if (terminalGetKeyState(KEY_Q) == KEY_JUST_PRESSED)
		{
			playSound(&a, 1);
		};
	}

	audioRunning = 0;

	WaitForSingleObject(
		audioThread,
		INFINITE);

	DeleteCriticalSection(&voiceMutex);

	free(a.samples);
	free(b.samples);
}