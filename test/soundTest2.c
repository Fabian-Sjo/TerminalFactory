#include "../src/sound/sound.h"

SoundGeneratorResult walkSound(double time)
{

	float freq =
		1200.0f -
		time * 1500.0f;

	float env =
		exp(-5 * time);

	return (SoundGeneratorResult){
		.val = sin(2 * 3.14 * freq * time) * env, .isFinished = false};
}

int main(int argc, char const *argv[])
{
	soundInit();
	
	Sound b = (Sound){
		.type = SOUND_TYPE_GENERATOR,
		.source.generator = {
			.generator = &walkSound}};
	soundPlay(&b, 1, NULL);
	getch();
	return 0;
}
