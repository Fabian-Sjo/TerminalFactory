#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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
// Returns the current global audio sample rate.
int soundGetSampleRate();
// Sets the global audio sample rate used by the sound system.
void soundSetSampleRate(int sampleRate);
// Returns whether the provided channel is currently playing audio.
bool soundIsPlaying(int channel);
// Stops playback on the supplied channel.
void soundStop(int channel);
// Starts playback of a sound on the given channel with optional looping and a duration. A duration of 0 with loop disabled means play until the sound ends.
void soundStart(Sound *sound, int channel, float duration, bool loop, SoundSettings *settings);
// Plays a sound immediately on the supplied channel using the provided settings. For generator sounds the playback continues until the generator reports completion.
void soundPlay(Sound *sound, int channel, SoundSettings *settings);
// Initializes the sound system.
void soundInit();
// Shuts down the sound system and releases resources.
void soundKill();
// Parses a WAV file and returns a newly allocated Sound object. The current implementation assumes a single-channel or multi-channel PCM WAV and mixes channels down to one.
Sound *parseWav(FILE *file);
#endif
