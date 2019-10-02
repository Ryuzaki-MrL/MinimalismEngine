#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_STOPPED	0
#define TIMER_RUNNING	1
#define TIMER_PAUSED	2

typedef float seconds_t;
typedef uint64_t ticks_t;

class Timer {
  public:
	seconds_t tend;
  private:
	ticks_t tstart;
	uint8_t state;

  public:
	Timer();
	~Timer();

	void start(seconds_t stop = 0);
	void pause();
	void reset();
	void stop();
	bool complete();

	inline void restart() { start(tend); }

	bool elapsed(seconds_t);
	seconds_t elapsed();
	ticks_t elapsedTicks();
};

#endif /* TIMER_H */
