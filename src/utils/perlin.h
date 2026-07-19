#ifndef PERLIN_H
#define PERLIN_H

// Returns a 2D Perlin-noise value for the given coordinates and frequency/depth settings.
extern double perlin_Get2d(double x, double y, double freq, int depth);

#endif  // PERLIN_H