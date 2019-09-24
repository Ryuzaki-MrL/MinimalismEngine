#include "Game.h"
#include "Input.h"
#include "Fading.h"
#include "System.h"
#include "Graphics.h"

Game::Game(): framecnt(0), lasttime(System::getTime()), state(nullptr), running(true), transition(false) {}

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

	// calculate fps (debug)
	++framecnt;
	uint64_t delta = System::getTime() - lasttime;
	if (delta > 1000) {
		fps = framecnt / (delta / 1000.0f);
		framecnt = 0;
		lasttime = System::getTime();
	}
}

void Game::end() {
	running = false;
}
