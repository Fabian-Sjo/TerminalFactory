#include <math.h>
#include <stdint.h>

#define PI 3.14159265358979323846

float sine(double time)
{
	float freq = 440.0f;

	return sin(2.0 * PI * freq * time);
}

float synthLead(double time)
{
	float freq = 220.0f;

	float s =
		sin(2 * PI * freq * time) * 1.0f +
		sin(2 * PI * freq * 2 * time) * 0.5f +
		sin(2 * PI * freq * 3 * time) * 0.25f +
		sin(2 * PI * freq * 4 * time) * 0.1f;

	return s * 0.5f;
}

float squareWave(double time)
{
	float freq = 220.0f;

	float s = sin(2 * PI * freq * time);

	return s > 0 ? 1.0f : -1.0f;
}

float sawWave(double time)
{
	float freq = 220.0f;

	float phase = fmod(time * freq, 1.0);

	return phase * 2.0f - 1.0f;
}

float triangleWave(double time)
{
	float freq = 220.0f;

	float phase =
		fmod(time * freq, 1.0);

	return 4.0f *
			   fabs(phase - 0.5f) -
		   1.0f;
}

float kick(double time)
{
	float length = 0.5f;

	if (time > length)
		return 0;

	float freq =
		120.0f *
		exp(-20.0f * time);

	float env =
		exp(-8.0f * time);

	return sin(2 * PI * freq * time) * env;
}

static uint32_t seed = 123456;

float noise()
{
	seed = seed * 1664525 + 1013904223;

	return ((seed >> 16) & 0x7fff) / 16384.0f - 1.0f;
}

float snare(double time)
{
	if (time > 0.3)
		return 0;

	float noisePart =
		noise() *
		exp(-18 * time);

	float body =
		sin(2 * PI * 180 * time) * exp(-12 * time);

	return noisePart * 0.7f + body * 0.3f;
}

float laser(double time)
{
	if (time > 0.7)
		return 0;

	float freq =
		1200.0f -
		time * 1500.0f;

	float env =
		exp(-5 * time);

	return sin(2 * PI * freq * time) * env;
}
float coin(double time)
{
	float freq =
		600.0f + time * 800.0f;

	float env =
		exp(-8 * time);

	return sin(2 * PI * freq * time) * env;
}
float engine(double time)
{
	float base =
		70.0f +
		sin(2 * PI * 5 * time) * 10;

	float wave =
		sin(2 * PI * base * time);

	return wave * 0.5f;
}
float wind(double time)
{
	float low =
		sin(2 * PI * 80 * time);

	float n =
		noise();

	return (low * 0.2f + n * 0.8f) * 0.5f;
}
float fmSynth(double time)
{
	float carrier = 220.0f;
	float modulator = 110.0f;

	float mod =
		sin(2 * PI * modulator * time) * 200;

	return sin(
		2 * PI *
		(carrier + mod) *
		time);
}