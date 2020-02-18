#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "Input.h"
#include "PollEvent.h"

#define MAX_CONTROLLERS 8

// TODO: support generic joypads (SDL_Joypad)
// TODO: gamepad deadzone

struct MousePoint { coord_t px, py; };
struct AxisPoint  { axis_t  dx, dy; };

struct ControllerState {
	SDL_GameController* handle = nullptr;
	SDL_JoystickID joy_id;

	bool bDown[SDL_CONTROLLER_BUTTON_MAX];
	bool bDownFlag[SDL_CONTROLLER_BUTTON_MAX];
	bool bUp[SDL_CONTROLLER_BUTTON_MAX];
	bool bUpFlag[SDL_CONTROLLER_BUTTON_MAX];
	bool bHeld[SDL_CONTROLLER_BUTTON_MAX];
	uint16_t bAny = 0;

	axis_t axis[SDL_CONTROLLER_AXIS_MAX];
};

static uint8_t kDown[0x200];
static uint8_t kDownFlag[0x200];
static uint8_t kUp[0x200];
static uint8_t kUpFlag[0x200];
static const uint8_t* kHeld = kDown;
static MousePoint mouse;

static kbkey_t kBinds[MAX_KEYBINDS];

static std::unordered_map<SDL_JoystickID, uint8_t> gp_devices;
static ControllerState gp_state[MAX_CONTROLLERS];
SDL_JoystickID gp_first = 0;


void searchFirstGamepad() {
	gp_first = 0;
	for (uint8_t i = 0; i < SDL_NumJoysticks() && i < MAX_CONTROLLERS; ++i) {
		if (Input::Gamepad::isConnected(i) || Input::Gamepad::init(i)) {
			gp_first = i;
			break;
		}
	}
}

void inputPollEvent(SDL_Event e) {
	kbkey_t sc = e.key.keysym.scancode;

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
		
		case SDL_CONTROLLERBUTTONDOWN: {
			uint8_t gp = gp_devices[e.cbutton.which];
			button_t btn = button_t(e.cbutton.button);
			if (!gp_state[gp].bDownFlag[btn]) {
				gp_state[gp].bDown[btn] = gp_state[gp].bDownFlag[btn] = true;
				gp_state[gp].bUpFlag[btn] = false;
			}
			gp_state[gp].bHeld[btn] = true;
			++gp_state[gp].bAny;
			break;
		}
		case SDL_CONTROLLERBUTTONUP: {
			uint8_t gp = gp_devices[e.cbutton.which];
			button_t btn = button_t(e.cbutton.button);
			if (!gp_state[gp].bUpFlag[btn]) {
				gp_state[gp].bUp[btn] = gp_state[gp].bUpFlag[btn] = true;
				gp_state[gp].bDownFlag[btn] = false;
			}
			gp_state[gp].bHeld[btn] = false;
			--gp_state[gp].bAny;
			break;
		}
		case SDL_CONTROLLERAXISMOTION: {
			uint8_t gp = gp_devices[e.caxis.which];
			gp_state[gp].axis[e.caxis.axis] = e.caxis.value;
			break;
		}
		
		case SDL_CONTROLLERDEVICEADDED: {
			Input::Gamepad::init(e.cdevice.which);
			printf("New device: %s\n", Input::Gamepad::getDeviceName(e.cdevice.which));
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED: {
			Input::Gamepad::fini(gp_devices[e.cdevice.which]);
			break;
		}
	}
}

void inputReset() {
	memset(kDown, 0, sizeof(kDown));
	memset(kUp, 0, sizeof(kUp));
	for (uint8_t i = 0; i < MAX_CONTROLLERS; ++i) {
		memset(gp_state[i].bDown, 0, sizeof(ControllerState::bDown));
		memset(gp_state[i].bUp, 0, sizeof(ControllerState::bUp));
	}
}


namespace Input {

bool init(uint16_t feats) {
	memset(kDownFlag, 0, sizeof(kDownFlag));
	memset(kUpFlag, 0, sizeof(kUpFlag));
	inputReset();

	if (feats & INPUT_GAMEPAD) {
		SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);
		searchFirstGamepad();
	}

	return true;
}

void fini() {
	for (uint8_t i = 0; i < MAX_CONTROLLERS; ++i) {
		Gamepad::fini(i);
	}
	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
}

void poll() {
	SDL_PumpEvents();
	kHeld = SDL_GetKeyboardState(NULL);
	SDL_GetMouseState(&mouse.px, &mouse.py);
}

namespace Keyboard
{
	bool isKeyDown(kbkey_t key) { return kDown[key]; }
	bool isKeyHeld(kbkey_t key) { return kHeld[key]; }
	bool isKeyUp(kbkey_t key)   { return kUp[key];   }

	bool isKeyBindDown(uint16_t index) { return kDown[kBinds[index]]; }
	bool isKeyBindHeld(uint16_t index) { return kHeld[kBinds[index]]; }
	bool isKeyBindUp(uint16_t index)   { return kUp[kBinds[index]];   }

	void keyBind(uint16_t index, kbkey_t key) {
		kBinds[index] = key;
	}

	void keyUnbind(uint16_t index, kbkey_t) {
		kBinds[index] = SDL_SCANCODE_UNKNOWN;
	}
};


namespace Mouse
{
	bool isDown(mbutton_t) { return false; }
	bool isHeld(mbutton_t) { return false; }
	bool isUp(mbutton_t)   { return false; }

	coord_t getX() { return mouse.px; }
	coord_t getY() { return mouse.py; }
};


namespace Touch
{
	coord_t getX(uint8_t) { return 0; }
	coord_t getY(uint8_t) { return 0; }
};


namespace Gamepad
{
	bool init(uint8_t gpad) {
		if (SDL_IsGameController(gpad)) {
			SDL_GameController* ctrl = SDL_GameControllerOpen(gpad);
			if (ctrl) {
				SDL_JoystickID id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(ctrl));
				gp_state[gpad].handle = ctrl;
				gp_state[gpad].joy_id = id;
				gp_devices[id] = gpad;
				return true;
			}
		}
		return false;
	}

	void fini(uint8_t gpad) {
		if (gp_state[gpad].handle) {
			SDL_GameControllerClose(gp_state[gpad].handle);
			gp_devices.erase(gp_state[gpad].joy_id);
			memset(&gp_state[gpad], 0, sizeof(ControllerState));
			if (gp_first == gpad) {
				searchFirstGamepad();
			}
		}
	}

	bool isConnected(uint8_t gpad) {
		return (gp_state[gpad].handle != nullptr);
	}

	void poll(uint8_t) {}

	bool isButtonDown(button_t key, uint8_t gpad) {
		return gp_state[gpad].bDown[key];
	}

	bool isButtonHeld(button_t key, uint8_t gpad) {
		return gp_state[gpad].bHeld[key];
	}

	bool isButtonUp(button_t key, uint8_t gpad) {
		return gp_state[gpad].bUp[key];
	}

	bool buttonAny(uint8_t gpad) {
		return (gp_state[gpad].bAny > 0);
	}

	const char* getDeviceName(uint8_t gpad) {
		if (gp_state[gpad].handle) {
			return SDL_GameControllerName(gp_state[gpad].handle);
		}
		return "";
	}

	const char* getButtonName(button_t key, uint8_t) {
		return SDL_GameControllerGetStringForButton(key);
	}

	axis_t getStickAxisX(uint8_t stick, uint8_t gpad) {
		uint8_t axis_id = (stick == STICK_LEFT) ? SDL_CONTROLLER_AXIS_LEFTX : SDL_CONTROLLER_AXIS_RIGHTX;
		return gp_state[gpad].axis[axis_id];
	}

	axis_t getStickAxisY(uint8_t stick, uint8_t gpad) {
		uint8_t axis_id = (stick == STICK_LEFT) ? SDL_CONTROLLER_AXIS_LEFTY : SDL_CONTROLLER_AXIS_RIGHTY;
		return gp_state[gpad].axis[axis_id];
	}
};


namespace Motion
{
	axis_t getAccelX() { return 0; }
	axis_t getAccelY() { return 0; }
	axis_t getAccelZ() { return 0; }

	void setAccelState(bool state) {}

	axis_t getGyroX() { return 0; }
	axis_t getGyroY() { return 0; }
	axis_t getGyroZ() { return 0; }

	void setGyroState(bool state) {}
};


bool swkbd() {
	return false;
}

bool getString(char* out, const char* htext, const char* def, size_t maxlength, bool password) {
	return false;
}

bool getInteger(int& out, const char* htext, int def) {
	return false;
}

}; // namespace Input
