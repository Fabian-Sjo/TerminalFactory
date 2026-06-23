#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>
#include <stdbool.h>

#define SOUND_CHANNELS 16

typedef struct SoundSettings
{
	float volume;
	float pan;
	float pitch;

	float attackStep; // fade-in speed
	float releaseStep;
} SoundSettings;
typedef enum
{
	SOUND_TYPE_SAMPLE,
	SOUND_TYPE_GENERATOR
} SoundType;
typedef enum
{
	SOUND_DURATION_LOOP,
	SOUND_DURATION_TILL_END,
	SOUND_DURATION_SET_TIME,
} SoundDuration;

typedef struct
{
	int sampleNum;
	int16_t *samples;

} SoundSample;
typedef struct
{
	float val;
	bool isFinished;
} SoundGeneratorResult;
typedef struct
{
	SoundGeneratorResult (*generator)(double time);
} SoundGenerator;

typedef struct Sound
{
	SoundType type;

	union
	{
		SoundSample sample;
		SoundGenerator generator;

	} source;

} Sound;
int soundGetSampleRate();
void soundSetSampleRate(int sampleRate);
bool soundIsPlaying(int channel);
void soundStop(int channel);
void soundStart(Sound *sound, int channel, float duration, bool loop, SoundSettings *settings);
void soundPlay(Sound *sound, int channel, SoundSettings *settings);
void soundInit();
void soundKill();
#endif
