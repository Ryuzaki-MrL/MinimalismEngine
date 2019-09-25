#include <3ds.h>

#include "Timer.h"

#define ELAPSED (seconds_t(svcGetSystemTick() - tstart) / SYSCLOCK_ARM11)

Timer::Timer(): tstart(0), tend(0), state(TIMER_STOPPED) {}
Timer::~Timer() {}

void Timer::start(seconds_t stop_after) {
	tstart = svcGetSystemTick();
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
		tstart = svcGetSystemTick();
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
	return (svcGetSystemTick() - tstart);
}
