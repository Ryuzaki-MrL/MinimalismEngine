#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>

#include "Input.h"

namespace Input {

static button_t kDown;
static button_t kHeld;
static button_t kUp;
static touchPosition touch;
static circlePosition cpad[2];
static accelVector accel;
static angularRate gyro;
static bool isNew3DS;

static button_t kBinds[MAX_KEYBINDS];

bool init(uint16_t feats) {
	if (feats & INPUT_MOTION) {
		Motion::setAccelState(true);
		Motion::setGyroState(true);
	}
	
	APT_CheckNew3DS(&isNew3DS);

	return R_SUCCEEDED(hidInit());
}

void fini() {
	hidExit();
}

void poll() {
	hidScanInput();

	kDown = hidKeysDown();
	kHeld = hidKeysHeld();
	kUp = hidKeysUp();

	hidCircleRead(&cpad[STICK_LEFT]);
	irrstCstickRead(&cpad[STICK_RIGHT]);
	hidTouchRead(&touch);
	hidAccelRead(&accel);
	hidGyroRead(&gyro);
}


namespace Keyboard
{
	bool isKeyDown(kbkey_t key) { return (kDown & key); }
	bool isKeyHeld(kbkey_t key) { return (kHeld & key); }
	bool isKeyUp(kbkey_t key)   { return (kUp & key);   }

	bool isKeyBindDown(uint16_t index) { return (kDown & kBinds[index]); }
	bool isKeyBindHeld(uint16_t index) { return (kHeld & kBinds[index]); }
	bool isKeyBindUp(uint16_t index)   { return (kUp & kBinds[index]);   }

	void keyBind(uint16_t index, kbkey_t key) {
		kBinds[index] = key;
	}

	void keyUnbind(uint16_t index, kbkey_t) {
		kBinds[index] = 0;
	}

	const char* getKeyName(kbkey_t key) {
		return Gamepad::getButtonName(key);
	}
};


namespace Mouse
{
	bool isDown(mbutton_t) { return (kDown & KEY_TOUCH); }
	bool isHeld(mbutton_t) { return (kHeld & KEY_TOUCH); }
	bool isUp(mbutton_t)   { return (kUp & KEY_TOUCH);   }

	coord_t getX() { return touch.px; }
	coord_t getY() { return touch.py; }
};


namespace Touch
{
	coord_t getX(uint8_t) { return touch.px; }
	coord_t getY(uint8_t) { return touch.py; }
};


namespace Gamepad
{
	bool init(uint8_t) {}
	void fini(uint8_t) {}
	bool isConnected(uint8_t) { return true; }

	void poll(uint8_t) {}

	bool isButtonDown(button_t key, uint8_t) { return (kDown & key); }
	bool isButtonHeld(button_t key, uint8_t) { return (kHeld & key); }
	bool isButtonUp(button_t key, uint8_t)   { return (kUp & key);   }

	bool buttonAny(uint8_t) {
		return (kDown != 0);
	}

	const char* getDeviceName(uint8_t) {
		return (isNew3DS) ? "New Nintendo 3DS" : "Nintendo 3DS";
	}

	const char* getButtonName(button_t key, uint8_t) {
		static const char* names_[] = {
			"None", "A", "B", "Select", "Start",
			"D-Right", "D-Left", "D-Up", "D-Down",
			"R", "L", "X", "Y", "", "", "ZL", "ZR",
			"", "", "", "", "", "", "", "",
			"S-Right", "S-Left", "S-Up", "S-Down",
			"C-Right", "C-Left", "C-up", "C-Down"
		};
		int index = 0;
		while(key) { ++index; key >>= 1; }
		return names_[index];
	}

	axis_t getStickAxisX(uint8_t stick, uint8_t) {
		return cpad[stick].dx;
	}

	axis_t getStickAxisY(uint8_t stick, uint8_t) {
		return cpad[stick].dy;
	}
};


namespace Motion
{
	axis_t getAccelX() { return accel.x; }
	axis_t getAccelY() { return accel.y; }
	axis_t getAccelZ() { return accel.z; }

	void setAccelState(bool state) {
		if (state) {
			HIDUSER_EnableAccelerometer();
		} else {
			HIDUSER_DisableAccelerometer();
		}
	}

	axis_t getGyroX() { return gyro.x; }
	axis_t getGyroY() { return gyro.y; }
	axis_t getGyroZ() { return gyro.z; }

	void setGyroState(bool state) {
		if (state) {
			HIDUSER_EnableGyroscope();
		} else {
			HIDUSER_DisableGyroscope();
		}
	}
};


bool swkbd() {
	return true;
}

bool getString(char* out, const char* htext, const char* def, size_t maxlength, bool password) {
	SwkbdState kb;

	swkbdInit(&kb, SWKBD_TYPE_QWERTY, 2, maxlength);
	swkbdSetInitialText(&kb, def);
	swkbdSetHintText(&kb, htext);
	swkbdSetButton(&kb, SWKBD_BUTTON_LEFT, "Cancel", false);
	swkbdSetButton(&kb, SWKBD_BUTTON_RIGHT, "Confirm", true);
	swkbdSetValidation(&kb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
	if (password) swkbdSetPasswordMode(&kb, SWKBD_PASSWORD_HIDE);

	swkbdInputText(&kb, out, maxlength);

	SwkbdResult result = swkbdGetResult(&kb);
	return (result==SWKBD_D1_CLICK1);
}

bool getInteger(int& out, const char* htext, int def) {
	static char buffer[11] = "";
	SwkbdState kb;

	swkbdInit(&kb, SWKBD_TYPE_NUMPAD, 2, sizeof(buffer) - 1);
	snprintf(buffer, sizeof(buffer)-1, "%d", def);
	swkbdSetInitialText(&kb, buffer);
	swkbdSetHintText(&kb, htext);
	swkbdSetButton(&kb, SWKBD_BUTTON_LEFT, "Cancel", false); // TODO: get button name based on language
	swkbdSetButton(&kb, SWKBD_BUTTON_RIGHT, "Confirm", true);
	swkbdSetFeatures(&kb, SWKBD_FIXED_WIDTH);
	swkbdSetValidation(&kb, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);

	swkbdInputText(&kb, buffer, sizeof(buffer));

	SwkbdResult result = swkbdGetResult(&kb);
	out = atoi(buffer);
	return (result==SWKBD_D1_CLICK1);
}

}; // namespace Input
