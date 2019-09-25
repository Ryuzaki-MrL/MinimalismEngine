#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#include "Timer.h"

#define ELAPSED (seconds_t(SDL_GetPerformanceCounter() - tstart) / SDL_GetPerformanceFrequency())

Timer::Timer(): tstart(0), tend(0), state(TIMER_STOPPED) {
	SDL_InitSubSystem(SDL_INIT_TIMER);
}

Timer::~Timer() {
	SDL_QuitSubSystem(SDL_INIT_TIMER);
}

void Timer::start(seconds_t stop_after) {
	tstart = SDL_GetPerformanceCounter();
	tend = stop_after;
	state = TIMER_RUNNING;
}

void Timer::pause() {
	if (state == TIMER_RUNNING) {
		tend -= ELAPSED;
		state = TIMER_PAUSED;
	}
}

void Timer::reset() {
	if (state != TIMER_STOPPED) {
		tstart = SDL_GetPerformanceCounter();
		state = TIMER_RUNNING;
	}
}

void Timer::stop() {
	tend = 0;
	state = TIMER_STOPPED;
}

bool Timer::complete() {
	if ((state == TIMER_RUNNING) && (ELAPSED >= tend)) {
		state = TIMER_STOPPED;
		return true;
	}
	return false;
}

bool Timer::elapsed(seconds_t t) {
	return (ELAPSED >= t);
}

seconds_t Timer::elapsed() {
	return ELAPSED;
}

ticks_t Timer::elapsedTicks() {
	return (SDL_GetPerformanceCounter() - tstart);
}
