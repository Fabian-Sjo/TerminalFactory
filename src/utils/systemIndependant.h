#ifndef SYSTEMINDEPENDANT_H
#define SYSTEMINDEPENDANT_H
#include "vector2.h"

// Pauses execution for the requested number of milliseconds.
void msSleep(int milliseconds);

// Returns the current terminal size in cells.
Vector2Int getTermSize();

// Copies the provided string to the system clipboard.
void pasteToClipboard(char *string);
#endif
