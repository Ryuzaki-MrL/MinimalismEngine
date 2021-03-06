#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER_STOPPED	0
#define TIMER_RUNNING	1
#define TIMER_PAUSED	2

#define ONE_SECOND 1.0
#define HALF_SECOND 0.5

typedef float seconds_t;
typedef uint64_t ticks_t;

class Timer {
  private:
	ticks_t tstart;
  public:
	seconds_t tend;
  private:
	seconds_t ts;
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


class DeltaTimer {
  public:
	seconds_t tend;
  private:
	seconds_t tcurrent;
	bool running;

  public:
	DeltaTimer(): tend(0), tcurrent(0), running(false) {}
	~DeltaTimer() {}

	inline void start(seconds_t stop = 0) {
		tcurrent = 0;
		tend = stop;
		running = true;
	}

	inline bool update(seconds_t dt) {
		if (running) tcurrent += dt;
		return running;
	}

	inline void restart() { start(tend); }
	inline void stop() { running = false; }

	inline seconds_t elapsed() const { return tcurrent; }
	inline bool complete() const { return (running && tcurrent >= tend); }
	inline bool isRunning() const { return running; }
};

#endif /* TIMER_H */
