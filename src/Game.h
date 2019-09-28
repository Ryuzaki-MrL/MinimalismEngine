#ifndef GAME_H
#define GAME_H

#include "Timer.h"

#define UNITS_PER_FRAME(x, t) ((x) * (t))

class GameState;
class Game {
  private:
	Timer frametimer;
	uint16_t framecnt;
	float fps;
	GameState* state;
	GameState* tmpstate;
	bool running;
	bool transition;

  public:
	Game();
	~Game();

	inline bool isRunning() const { return (running && state); }
	inline float getFPS() const { return fps; }

	void setState(GameState*);
	void setStateWithFade(GameState*, seconds_t);

	void update();
	void draw();
	void end();
};

#endif /* GAME_H */
