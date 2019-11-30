#include "GameState.h"
#include "Input.h"
#include "Fading.h"
#include "System.h"
#include "Graphics.h"

float g_deltatime;

Game::Game(): framecnt(0), state(nullptr), tmpstate(nullptr), running(true), transition(false) {
	frametimer.start();
}

Game::~Game() {
	delete state;
}

// The game decides if timestep is fixed or variable
void Game::update(seconds_t delta) {
	if (transition) {
		switch(fadeStatus()) {
			case FADE_HALFDONE: setState(tmpstate); break;
			case FADE_DONE: transition = false; break;
		}
	} else {
		g_deltatime = delta;
		Input::poll();
		state->update();
	}
}

void Game::setState(GameState* st) {
	delete state;
	state = st;
}

void Game::setStateWithFade(GameState* st, seconds_t time) {
	transition = true;
	tmpstate = st;
	fadeStart(time, C_BLACK);
}

void Game::draw() {
	Renderer::frameStart();
	fadeUpdate();
	state->draw();
	Renderer::frameEnd();

	// fps
	++framecnt;
	seconds_t delta = frametimer.elapsed();
	if (delta >= 1.0) {
		fps = framecnt / delta;
		framecnt = 0;
		frametimer.reset();
	}
}

void Game::end() {
	running = false;
}
