#include "Timer.h"
#include "System.h"

#define ELAPSED (seconds_t(System::getTicks() - tstart) / System::getFrequency())

Timer::Timer(): tstart(0), tend(0), ts(0), state(TIMER_STOPPED) {}
Timer::~Timer() {}

void Timer::start(seconds_t stop_after) {
	tstart = System::getTicks();
	tend = stop_after;
	state = TIMER_RUNNING;
}

void Timer::pause() {
	if (state == TIMER_RUNNING) {
		tend -= ELAPSED;
		ts += ELAPSED;
		state = TIMER_PAUSED;
	}
}

void Timer::reset() {
	if (state != TIMER_STOPPED) {
		tstart = System::getTicks();
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
	return (elapsed() >= t);
}

seconds_t Timer::elapsed() {
	return (state == TIMER_PAUSED) ? (ts) : (ELAPSED + ts);
}

ticks_t Timer::elapsedTicks() {
	return (System::getTicks() - tstart);
}
