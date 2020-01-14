#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>

#include "Input.h"
#include "PollEvent.h"

struct MousePoint { int px, py; };

static uint8_t kDown[0x200];
static uint8_t kDownFlag[0x200];
static uint8_t kUp[0x200];
static uint8_t kUpFlag[0x200];
static const uint8_t* kHeld = kDown;
static MousePoint mouse;

static button_t kBinds[MAX_KEYBINDS];

void inputPollEvent(SDL_Event e) {
	SDL_Scancode sc = e.key.keysym.scancode;
	switch(e.type) {
		case SDL_KEYDOWN: {
			if (!kDownFlag[sc]) {
				kDown[sc] = kDownFlag[sc] = true;
				kUpFlag[sc] = false;
			}
			break;
		}
		case SDL_KEYUP: {
			if (!kUpFlag[sc]) {
				kUp[sc] = kUpFlag[sc] = true;
				kDownFlag[sc] = false;
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			break;
	}
}

void inputReset() {
	memset(kDown, 0, sizeof(kDown));
	memset(kUp, 0, sizeof(kUp));
}


namespace Input {

bool init() {
	memset(kDownFlag, 0, sizeof(kDownFlag));
	memset(kUpFlag, 0, sizeof(kUpFlag));
	inputReset();
	return true;
}

void fini() {
	
}

void poll() {
	SDL_PumpEvents();
	kHeld = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mouse.px, &mouse.py);
}

bool isKeyDown(button_t key) {
	return kDown[key];
}

bool isKeyBindDown(uint16_t index) {
	return kDown[kBinds[index]];
}

bool isKeyHeld(button_t key) {
	return kHeld[key];
}

bool isKeyBindHeld(uint16_t index) {
	return kHeld[kBinds[index]];
}

bool isKeyUp(button_t key) {
	return kUp[key];
}

bool isKeyBindUp(uint16_t index) {
	return kUp[kBinds[index]];
}

void keyBind(uint16_t index, button_t key) {
	kBinds[index] = key;
}

void keyUnbind(uint16_t index) {
	kBinds[index] = 0;
}

uint16_t getTouchX() {
	return 0;
}

uint16_t getTouchY() {
	return 0;
}

axis_t getStickAxisX(uint8_t stick) {
	return 0;
}

axis_t getStickAxisY(uint8_t stick) {
	return 0;
}

axis_t getAccelX() {
	return 0;
}

axis_t getAccelY() {
	return 0;
}

axis_t getAccelZ() {
	return 0;
}

void setAccelState(bool state) {
	
}

axis_t getGyroX() {
	return 0;
}

axis_t getGyroY() {
	return 0;
}

axis_t getGyroZ() {
	return 0;
}

void setGyroState(bool state) {
	
}

bool getString(char* out, const char* htext, const char* def, size_t maxlength, bool password) {
	return false;
}

bool getInteger(int& out, const char* htext, int def) {
	return false;
}

}; // namespace Input
