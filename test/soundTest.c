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
#include "../src/sound/sound.h"
#include "../src/sound/instruments.h"

#define PI M_PI
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

		double env = exp(-duration * t);

		samples[i] = (int16_t)(s * env * 15000);
	}
	return (Sound){
		.type = SOUND_TYPE_SAMPLE,
		.source.sample = {
			.sampleNum = sampleCount,
			.sampleRate = SAMPLE_RATE,
			.samples = samples}};
}
SoundGeneratorResult generator(double time)
{
	return (SoundGeneratorResult){
		.val = laser(time), .isFinished = false};
}

int main()
{
	soundInit();
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
			.attackStep = 1.0f / (SAMPLE_RATE * 0.001f),

		};
		printf("[1] : %d\n", soundIsPlaying(1));
		if (terminalGetKeyState(KEY_A) == KEY_JUST_PRESSED)
		{
			soundPlay(&b, 1, NULL);
		};
		if (terminalGetKeyState(KEY_S) == KEY_JUST_PRESSED)
		{
			soundStart(&b, 1, 1, true, NULL);
		};
		if (terminalGetKeyState(KEY_D) == KEY_JUST_PRESSED)
		{
			soundStart(&b, 1, 1, false, NULL);
		};
		if (terminalGetKeyState(KEY_W) == KEY_JUST_PRESSED)
		{
			soundStop(1);
		};
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
				soundPlay(&sounds[i], i, &defaultSettings);
				// playSoundDuration(&sounds[i], i, 0.5f, &defaultSettings);
				// soundStart(&sounds[i], i, 1, false, &defaultSettings);
			};
		}
	}
}