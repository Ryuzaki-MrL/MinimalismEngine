#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

#include "Input.h"

#define MAX_CONTROLLERS 8

namespace Input {

struct ControllerState {
	button_t kDown;
	button_t kHeld;
	button_t kUp;
	JoystickPosition joypad[2];
	SixAxisSensorValues sas;

	inline void poll(uint8_t gpad) {
		HidControllerID id = HidControllerID(gpad);
		kDown = hidKeysDown(id);
		kHeld = hidKeysHeld(id);
		kUp   = hidKeysUp(id);

		hidJoystickRead(&joypad[STICK_LEFT], id, JOYSTICK_LEFT);
		hidJoystickRead(&joypad[STICK_RIGHT], id, JOYSTICK_RIGHT);

		hidSixAxisSensorValuesRead(&sas, id, 1);
	}
};

struct MouseState {
	mbutton_t mDown;
	mbutton_t mHeld;
	mbutton_t mUp;
	MousePosition pos;

	inline void poll() {
		mDown = hidMouseButtonsDown();
		mHeld = hidMouseButtonsHeld();
		mUp   = hidMouseButtonsUp();
		hidMouseRead(&pos);
	}
};

static ControllerState ctrl[MAX_CONTROLLERS];
static touchPosition touch[16];
static MouseState mouse;

static kbkey_t kBinds[MAX_KEYBINDS];

bool init(uint16_t feats) {
	hidInitialize();
	if (feats & INPUT_MOTION)
		hidInitializeSevenSixAxisSensor();
	return true;
}

void fini() {
	//hidFinalizeSevenSixAxisSensor();
	hidExit();
}

void poll() {
	hidScanInput();

	ctrl[GP0].poll(CONTROLLER_P1_AUTO);

	for (uint32_t i = 0; i < hidTouchCount(); ++i) {
		hidTouchRead(&touch[i], i);
	}

	mouse.poll();
}


namespace Keyboard
{
	bool isKeyDown(kbkey_t key) { return hidKeyboardDown(key); }
	bool isKeyHeld(kbkey_t key) { return hidKeyboardHeld(key); }
	bool isKeyUp(kbkey_t key)   { return hidKeyboardUp(key);   }

	bool isKeyBindDown(uint16_t index) { return hidKeyboardDown(kBinds[index]); }
	bool isKeyBindHeld(uint16_t index) { return hidKeyboardHeld(kBinds[index]); }
	bool isKeyBindUp(uint16_t index)   { return hidKeyboardUp(kBinds[index]);   }

	void keyBind(uint16_t index, kbkey_t key) {
		kBinds[index] = key;
	}

	void keyUnbind(uint16_t index, kbkey_t) {
		kBinds[index] = KBD_NONE;
	}
};


namespace Mouse
{
	bool isDown(mbutton_t mb) { return (mouse.mDown & mb); }
	bool isHeld(mbutton_t mb) { return (mouse.mHeld & mb); }
	bool isUp(mbutton_t mb)   { return (mouse.mUp & mb);   }

	coord_t getX() { return (mouse.pos.x); }
	coord_t getY() { return (mouse.pos.y); }
};


namespace Touch
{
	coord_t getX(uint8_t tpoint) { return touch[tpoint].px; }
	coord_t getY(uint8_t tpoint) { return touch[tpoint].py; }
};


namespace Gamepad
{
	bool init(uint8_t) {
		return true;
	}

	void fini(uint8_t) {}

	bool isConnected(uint8_t gpad) {
		return hidIsControllerConnected(HidControllerID(gpad));
	}

	void poll(uint8_t gpad) {
		ctrl[gpad].poll(gpad);
	}

	bool isButtonDown(button_t key, uint8_t gpad) {
		return (ctrl[gpad].kDown & key);
	}

	bool isButtonHeld(button_t key, uint8_t gpad) {
		return (ctrl[gpad].kHeld & key);
	}

	bool isButtonUp(button_t key, uint8_t gpad) {
		return (ctrl[gpad].kUp & key);
	}

	bool buttonAny(uint8_t gpad) {
		return (ctrl[gpad].kHeld != 0);
	}

	const char* getDeviceName(uint8_t gpad) {
		switch(hidGetControllerType(HidControllerID(gpad))) {
			case TYPE_PROCONTROLLER: return "Nintendo Switch Pro Controller";
			case TYPE_HANDHELD: return "Nintendo Switch (Handheld Mode)";
			case TYPE_JOYCON_PAIR: return "Joycon Controller";
			case TYPE_JOYCON_LEFT: return "Joycon (Left)";
			case TYPE_JOYCON_RIGHT: return "Joycon (Right)";
			default: return "Unknown";
		}
	}

	const char* getButtonName(button_t, uint8_t) {
		return "";
	}

	axis_t getStickAxisX(uint8_t stick, uint8_t gpad) {
		return ctrl[gpad].joypad[stick].dx;
	}

	axis_t getStickAxisY(uint8_t stick, uint8_t gpad) {
		return ctrl[gpad].joypad[stick].dy;
	}
};


namespace Motion
{
	axis_t getAccelX() { return ctrl[GP0].sas.accelerometer.x; }
	axis_t getAccelY() { return ctrl[GP0].sas.accelerometer.y; }
	axis_t getAccelZ() { return ctrl[GP0].sas.accelerometer.z; }

	void setAccelState(bool state) {
		if (state) {
			hidStartSevenSixAxisSensor();
		} else {
			hidStopSevenSixAxisSensor();
		}
	}

	axis_t getGyroX() { return ctrl[GP0].sas.gyroscope.x; }
	axis_t getGyroY() { return ctrl[GP0].sas.gyroscope.y; }
	axis_t getGyroZ() { return ctrl[GP0].sas.gyroscope.z; }

	void setGyroState(bool state) {
		if (state) {
			hidStartSevenSixAxisSensor();
		} else {
			hidStopSevenSixAxisSensor();
		}
	}
};


bool swkbd() {
	return true;
}

bool getString(char* out, const char* htext, const char* def, size_t maxlength, bool password) {
	SwkbdConfig* kb = nullptr;

	Result res = swkbdCreate(kb, 0);
	if (R_FAILED(res)) return false;

	if (password) {
		swkbdConfigMakePresetPassword(kb);
	} else {
		swkbdConfigMakePresetDefault(kb);
	}

	swkbdConfigSetInitialText(kb, def);
	swkbdConfigSetHeaderText(kb, htext);
	swkbdConfigSetGuideText(kb, htext);
	swkbdConfigSetOkButtonText(kb, "OK");
	swkbdConfigSetReturnButtonFlag(kb, true);

	res = swkbdShow(kb, out, maxlength);
	swkbdClose(kb);
	return (R_SUCCEEDED(res));
}

bool getInteger(int& out, const char* htext, int def) {
	static char buffer[12] = "";
	SwkbdConfig* kb = nullptr;

	Result res = swkbdCreate(kb, 0);
	if (R_FAILED(res)) return false;

	sprintf(buffer, "%d", def);

	swkbdConfigSetType(kb, SwkbdType_NumPad);
	swkbdConfigSetBlurBackground(kb, true);
	swkbdConfigSetInitialText(kb, buffer);
	swkbdConfigSetHeaderText(kb, htext);
	swkbdConfigSetGuideText(kb, htext);
	swkbdConfigSetOkButtonText(kb, "OK");
	swkbdConfigSetReturnButtonFlag(kb, true);

	res = swkbdShow(kb, buffer, sizeof(buffer));
	swkbdClose(kb);
	out = atoi(buffer);
	return (R_SUCCEEDED(res));
}

}; // namespace Input
