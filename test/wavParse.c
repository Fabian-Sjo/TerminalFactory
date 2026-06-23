#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../src/sound/sound.h"

#define ONE_BYTE 1
#define HEADER_LENGTH 44
#define SECTION_RIFF 4
#define SECTION_FILE_SIZE 4
#define SECTION_WAVE 4
#define SECTION_FMT 4
#define SECTION_FORMAT_LENGTH 4
#define SECTION_FORMAT_TYPE 2
#define SECTION_NR_OF_CHANNELS 2
#define SECTION_SAMPLE_RATE 4
#define SECTION_BYTE_RATE 4
#define SEC16ON_BLOCK_ALIGHN 2
#define SECTION_BLOCK_ALIGN 2
#define SECTION_BITS_PER_SAMPLE 2
#define SECTION_DATA_CHUNK_START 4
#define SECTION_SIZE_OF_DATA 4

typedef struct
{
	char riff[SECTION_RIFF + 1];
	int32_t fileSize;
	char wave[SECTION_WAVE + 1];
	char fmt[SECTION_FMT + 1];
	int32_t formatLength;
	int16_t formatType;
	int16_t nrOfChannels;
	int32_t sampleRate;
	int32_t byteRate;
	int16_t blockAlign;
	int16_t bitsPerSample;
	int nrOfSamples;
	char dataStart[SECTION_DATA_CHUNK_START + 1];
	int32_t dataSize;
	int32_t *data;
} FileWav;
FileWav *parseWav(FILE *file)
{
	FileWav *wavFile = malloc(sizeof(FileWav));
	int currentByte = 0;

	printf("\n---SECTION_RIFF---(%d - %d)\n", currentByte, currentByte + SECTION_RIFF);
	currentByte += SECTION_RIFF;
	fread(wavFile->riff, ONE_BYTE, SECTION_RIFF, file);
	printf(wavFile->riff);

	printf("\n---SECTION_FILE_SIZE---(%d - %d)\n", currentByte, currentByte + SECTION_FILE_SIZE);
	currentByte += SECTION_FILE_SIZE;
	fread(&wavFile->fileSize, ONE_BYTE, SECTION_FILE_SIZE, file);
	printf("%d", wavFile->fileSize);

	printf("\n---SECTION_WAVE---(%d - %d)\n", currentByte, currentByte + SECTION_WAVE);
	currentByte += SECTION_WAVE;
	fread(wavFile->wave, ONE_BYTE, SECTION_WAVE, file);
	printf(wavFile->wave);

	printf("\n---SECTION_FMT---(%d - %d)\n", currentByte, currentByte + SECTION_FMT);
	currentByte += SECTION_FMT;
	fread(wavFile->fmt, ONE_BYTE, SECTION_FMT, file);
	printf(wavFile->fmt);

	printf("\n---SECTION_FORMAT_LENGTH---(%d - %d)\n", currentByte, currentByte + SECTION_FORMAT_LENGTH);
	currentByte += SECTION_FORMAT_LENGTH;
	fread(&wavFile->formatLength, ONE_BYTE, SECTION_FORMAT_LENGTH, file);
	printf("%d", wavFile->formatLength);

	printf("\n---SECTION_FORMAT_TYPE---(%d - %d)\n", currentByte, currentByte + SECTION_FORMAT_TYPE);
	currentByte += SECTION_FORMAT_TYPE;
	fread(&wavFile->formatType, ONE_BYTE, SECTION_FORMAT_TYPE, file);
	printf("%d", wavFile->formatType);

	printf("\n---SECTION_NR_OF_CHANNELS---(%d - %d)\n", currentByte, currentByte + SECTION_NR_OF_CHANNELS);
	currentByte += SECTION_NR_OF_CHANNELS;
	fread(&wavFile->nrOfChannels, ONE_BYTE, SECTION_NR_OF_CHANNELS, file);
	printf("%d", wavFile->nrOfChannels);

	printf("\n---SECTION_SAMPLE_RATE---(%d - %d)\n", currentByte, currentByte + SECTION_SAMPLE_RATE);
	currentByte += SECTION_SAMPLE_RATE;
	fread(&wavFile->sampleRate, ONE_BYTE, SECTION_SAMPLE_RATE, file);
	printf("%d", wavFile->sampleRate);

	printf("\n---SECTION_BYTE_RATE---(%d - %d)\n", currentByte, currentByte + SECTION_BYTE_RATE);
	currentByte += SECTION_BYTE_RATE;
	fread(&wavFile->byteRate, ONE_BYTE, SECTION_BYTE_RATE, file);
	printf("%d", wavFile->byteRate);

	printf("\n---SECTION_BLOCK_ALIGN---(%d - %d)\n", currentByte, currentByte + SECTION_BLOCK_ALIGN);
	currentByte += SECTION_BLOCK_ALIGN;
	fread(&wavFile->blockAlign, ONE_BYTE, SECTION_BLOCK_ALIGN, file);
	printf("%d", wavFile->blockAlign);

	printf("\n---SECTION_BITS_PER_SAMPLE---(%d - %d)\n", currentByte, currentByte + SECTION_BITS_PER_SAMPLE);
	currentByte += SECTION_BITS_PER_SAMPLE;
	fread(&wavFile->bitsPerSample, ONE_BYTE, SECTION_BITS_PER_SAMPLE, file);
	printf("%d", wavFile->bitsPerSample);

	printf("\n---SECTION_DATA_CHUNK_START---(%d - %d)\n", currentByte, currentByte + SECTION_DATA_CHUNK_START);
	currentByte += SECTION_DATA_CHUNK_START;
	fread(wavFile->dataStart, ONE_BYTE, SECTION_DATA_CHUNK_START, file);
	printf(wavFile->dataStart);

	printf("\n---SECTION_SIZE_OF_DATA---(%d - %d)\n", currentByte, currentByte + SECTION_SIZE_OF_DATA);
	currentByte += SECTION_SIZE_OF_DATA;
	fread(&wavFile->dataSize, ONE_BYTE, SECTION_SIZE_OF_DATA, file);
	printf("%d", wavFile->dataSize);

	int nrOfFrames = wavFile->dataSize / wavFile->blockAlign;
	void *currentFrame = malloc(wavFile->blockAlign);
	size_t bytesPerSample = wavFile->bitsPerSample / 8;
	void *currentSample = malloc(bytesPerSample);

	size_t savedData = 0;
	int16_t *singleChannelData = malloc(nrOfFrames * sizeof(int16_t));

	for (size_t frame = 0; frame < nrOfFrames; frame++)
	{
		fread(currentFrame, 1, wavFile->blockAlign, file);

		long combinedSample = 0;
		for (size_t channel = 0; channel < wavFile->nrOfChannels; channel++)
		{

			int16_t sample;
			unsigned char *samplePtr =
				(unsigned char *)currentFrame + channel * bytesPerSample;
			combinedSample += sample;
			memcpy(&sample, samplePtr, sizeof(int16_t));
		}
		combinedSample /= wavFile->nrOfChannels;
		singleChannelData[savedData] = combinedSample;
		savedData++;
	}
	wavFile->nrOfSamples = savedData;
	free(currentFrame);
	free(currentSample);
	wavFile->data = singleChannelData;
	return wavFile;
};
int main()
{

	// Character that store the read
	// character
	char ch;

	// Opening file in reading mode
	FILE *fptr = fopen("sample-12s.wav", "rb");
	if (!fptr)
	{

		printf("failed to open file");
		return 0;
	}
	FileWav *wavFile = parseWav(fptr);

	Sound sound = (Sound){
		.type = SOUND_TYPE_SAMPLE,
		.source.sample = {
			.sampleNum = wavFile->nrOfSamples,
			.sampleRate = wavFile->sampleRate,
			.samples = wavFile->data}};

	soundInit();
	SoundSettings settings = (SoundSettings){
		.volume = 0.098f,
		.pan = (0),
		.pitch = 1.0f,
		.attackStep = 1.0f / (wavFile->nrOfSamples * 0.001f),

	};
	soundPlay(&sound, 0, &settings);
	while (1)
	{
		/* code */
	}

	// Closing the file
	fclose(fptr);
	return 0;
}