#ifndef GAME_H
#define GAME_H

#define UNITS_PER_FRAME(x, d) ((x) * (d))

class GameState;
class Game {
  private:
	uint64_t lasttime;
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
	void setStateWithFade(GameState*, uint16_t);

	void update();
	void draw();
	void end();
};

#endif /* GAME_H */
