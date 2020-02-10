#ifndef FADING_H
#define FADING_H

#include <stdint.h>

enum { FADE_RUNNING, FADE_HALFDONE, FADE_DONE };

void fadeStart(float time, uint32_t color);
void fadeUpdate();
void fadeApply();
int fadeStatus();

#endif /* FADING_H */
