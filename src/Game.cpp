#include "Game.h"
#include "Input.h"
#include "Fading.h"
#include "System.h"
#include "Graphics.h"

Game::Game(): framecnt(0), state(nullptr), running(true), transition(false) {
	frametimer.start(1.0);
}

Game::~Game() {
	delete state;
}

// TODO: apply delta time on update
void Game::update() {
	if (transition) {
		switch(fadeStatus()) {
			case FADE_HALFDONE: setState(tmpstate); break;
			case FADE_DONE: transition = false; break;
		}
	} else {
		Input::update();
		state->update();
	}
}

void Game::setState(GameState* st) {
	delete state;
	state = st;
}

void Game::setStateWithFade(GameState* st, uint16_t steps) {
	transition = true;
	tmpstate = st;
	fadeStart(steps, C_BLACK);
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
