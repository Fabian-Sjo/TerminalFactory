#define _USE_MATH_DEFINES
#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <process.h>
#include "sound.h"

#pragma comment(lib, "winmm.lib")

static HANDLE audioThread;
static CRITICAL_SECTION voiceMutex;

static volatile int audioRunning = 1;

#define VOICE_FRAMES 512
#define BUFFER_COUNT 8
#define RELEASE_SAMPLES 512
#define FADE_TIME 0.005f

#define FADE_STEP \
	(1.0f / (soundSampleRate * FADE_TIME))

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
	SoundDuration durationType;
	float duration;
	float envelope; // current volume multiplier

	double time;   // generator
	int samplePos; // sample
	int playing;
	Fade fade;
} Voice;

static Voice voices[SOUND_CHANNELS];
static Voice newVoices[SOUND_CHANNELS];
int soundSampleRate = 44100;

int soundGetSampleRate()
{
	return soundSampleRate;
}
void soundSetSampleRate(int sampleRate)
{
	soundSampleRate = sampleRate;
}
bool soundIsPlaying(int channel)
{
	return voices[channel].playing;
}
void soundStop(int channel)
{
	EnterCriticalSection(&voiceMutex);
	if (voices[channel].playing)
	{
		voices[channel].fade.fadeTarget = 0.0f;
		voices[channel].fade.fadeStep = FADE_STEP;
	}
	LeaveCriticalSection(&voiceMutex);
}
void playSoundDurationType(Sound *sound, int channel, float duration, SoundDuration durationType, SoundSettings *settings)
{
	EnterCriticalSection(&voiceMutex);

	SoundSettings defaultSettings = {
		.volume = 1.0f,
		.pan = 0.0f,
		.pitch = 1.0f,
		.attackStep = 1.0f / (soundSampleRate * 0.005f),
		.releaseStep = 1.0f / (soundSampleRate * 0.005f),
	};

	if (settings)
		defaultSettings = *settings;
	// printf("using volume%f\n", defaultSettings.volume);
	//  Move current voice into fading voice
	if (voices[channel].playing)
	{
		voices[channel].fade.fadeTarget = 0.0f;
		voices[channel].fade.fadeStep = FADE_STEP;
	}

	// Create replacement voice
	newVoices[channel] = (Voice){
		.sound = sound,
		.duration = duration,
		.durationType = durationType,
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
void soundStart(Sound *sound, int channel, float duration, bool loop, SoundSettings *settings)
{
	playSoundDurationType(sound, channel, duration, loop ? SOUND_DURATION_LOOP : SOUND_DURATION_SET_TIME, settings);
}
void soundPlay(Sound *sound, int channel, SoundSettings *settings)
{
	playSoundDurationType(sound, channel, 0, SOUND_DURATION_TILL_END, settings);
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
float getSoundSample(Voice *voice)
{
	Sound *sound = voice->sound;

	switch (sound->type)
	{
	case SOUND_TYPE_SAMPLE:
	{

		SoundSample *sample = &sound->source.sample;

		switch (voice->durationType)
		{
		case SOUND_DURATION_LOOP:
		{
			float timePlayed = voice->samplePos / ((float)soundSampleRate);
			bool shouldRestart = false;
			if (voice->duration > 0 && timePlayed >= voice->duration)
				shouldRestart = true;
			else if (voice->samplePos >= sample->sampleNum)
				shouldRestart = true;
			if (shouldRestart)
			{
				voice->samplePos = 0;
			}

			break;
		}

		case SOUND_DURATION_SET_TIME:
		{
			double time =
				voice->samplePos / (double)soundSampleRate;

			if (time >= voice->duration)
			{
				voice->playing = false;
				return 0;
			}

			break;
		}

		case SOUND_DURATION_TILL_END:
		{
			int samplesLeft =
				sample->sampleNum -
				voice->samplePos;

			if (samplesLeft < RELEASE_SAMPLES)
			{
				voice->fade.fadeVolume =
					samplesLeft /
					(float)RELEASE_SAMPLES;
			}

			if (voice->samplePos >= sample->sampleNum)
			{
				voice->playing = false;
				return 0;
			}

			break;
		}

		break;
		}

		float value = sample->samples[voice->samplePos] / 32768.0f;

		voice->samplePos += 1 * voice->settings.pitch;
		return value;
	}

	case SOUND_TYPE_GENERATOR:
	{
		SoundGeneratorResult result = sound->source.generator.generator(
			voice->time);
		switch (voice->durationType)
		{
		case SOUND_DURATION_LOOP:
		{
			bool shouldRestart = false;

			if (voice->duration > 0 &&
				voice->time >= voice->duration)
			{
				shouldRestart = true;
			}

			if (shouldRestart)
			{
				voice->time = 0.0;
			}

			break;
		}

		// TODO fade out
		case SOUND_DURATION_SET_TIME:
		{
			if (voice->time >= voice->duration)
			{
				voice->playing = false;
				return 0;
			}

			break;
		}
		// TODO fade out
		case SOUND_DURATION_TILL_END:
		{
			if (result.isFinished)
			{
				voice->playing = false;
				return 0;
			}
			break;
		}
		}

		float value = result.val;

		voice->time +=
			(1.0 / soundSampleRate) *
			voice->settings.pitch;

		return value *
			   voice->settings.volume *
			   voice->fade.fadeVolume;
	}
	}

	return 0;
}
float getVoiceSample(Voice *voice)
{
	if (!voice->playing)
		return 0;

	Sound *sound = voice->sound;

	updateFade(voice);
	float sample = getSoundSample(voice);
	// sound->samples[voice->samplePos] / 32768.0f;

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
	format.nSamplesPerSec = soundSampleRate;
	format.wBitsPerSample = 16;
	format.nBlockAlign = 4;
	format.nAvgBytesPerSec = soundSampleRate * 4;

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
void soundInit()
{
	InitializeCriticalSection(&voiceMutex);
	audioRunning = 1;
	audioThread = CreateThread(
		NULL,
		0,
		audioThreadFunc,
		NULL,
		0,
		NULL);
}
void soundKill()
{
	audioRunning = 0;

	WaitForSingleObject(
		audioThread,
		INFINITE);

	DeleteCriticalSection(&voiceMutex);
}
/*
int main()
{

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
		sounds[i] = getSamples(3, freq);
		// sounds[i] = getSamples(7, 10 * (10 + i));
	}

	terminalInit();
	Sound b = (Sound){
		.type = SOUND_TYPE_GENERATOR,
		.source.generator = {
			.generator = &generator}};
	while (1)
	{
		Sleep(16);
		poolInput();
		SoundSettings lower = (SoundSettings){
			.volume = 0.1f,
			.pan = (0),
			.pitch = 1.0f,
			.attackStep = 1.0f / (soundSampleRate * 0.001f),

		};
		printf("[1] : %d\n", soundIsPlayingChannel(1));
		if (terminalGetKeyState(KEY_A) == KEY_JUST_PRESSED)
Start			soundPlay(&b, 1, &lower);
		};
		if (terminalGetKeyState(KEY_D) == KEY_JUST_PRESSED)
		{
			soundStop(1);
		};
		for (size_t i = 0; i < 10; i++)
		{
			SoundSettings defaultSettings = (SoundSettings){
				.volume = 1.0f,
				.pan = (i / 10.f),
				.pitch = 1.0f,
				.attackStep = 1.0f / (soundSampleRate * 0.001f),

			};
			if (terminalGetKeyState(keys[i]) == KEY_JUST_PRESSED)
	Start				// soundPlay(&sounds[i], i, &defaultSettings);
				// soundPlay(&sounds[i], i, 0.5f, &defaultSettings);
				soundPlay(&sounds[i], i, 1, false, &defaultSettings);
			};
		}
	}

	audioRunning = 0;

	WaitForSingleObject(
		audioThread,
		INFINITE);

	DeleteCriticalSection(&voiceMutex);
}*/