#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>

#include "Input.h"

namespace Input {

static uint32_t kDown;
static uint32_t kHeld;
static uint32_t kUp;
static touchPosition touch;
static circlePosition cpad[2];
static accelVector accel;
static angularRate gyro;

static uint32_t kBinds[MAX_KEYBINDS];

bool init() {
	hidInit();
}

void fini() {
	hidExit();
}

void poll() {
	hidScanInput();

	kDown = hidKeysDown();
	kHeld = hidKeysHeld();
	kUp = hidKeysUp();

	hidCircleRead(&cpad[0]);
	irrstCstickRead(&cpad[1]);
	hidTouchRead(&touch);
	hidAccelRead(&accel);
	hidGyroRead(&gyro);
}

bool isKeyDown(uint32_t key) {
	return (kDown & key);
}

bool isKeyBindDown(uint16_t index) {
	return (kDown & kBinds[index]);
}

bool isKeyHeld(uint32_t key) {
	return (kHeld & key);
}

bool isKeyBindHeld(uint16_t index) {
	return (kHeld & kBinds[index]);
}

bool isKeyUp(uint32_t key) {
	return (kUp & key);
}

bool isKeyBindUp(uint16_t index) {
	return (kUp & kBinds[index]);
}

void keyBind(uint16_t index, uint32_t key) {
	kBinds[index] = key;
}

void keyUnbind(uint16_t index) {
	kBinds[index] = 0;
}

uint16_t getTouchX() {
	return touch.px;
}

uint16_t getTouchY() {
	return touch.py;
}

int16_t getStickAxisX(uint8_t stick) {
	return cpad[stick].dx;
}

int16_t getCirclePadY(uint8_t stick) {
	return cpad[stick].dy;
}

int16_t getAccelX() {
	return accel.x;
}

int16_t getAccelY() {
	return accel.y;
}

int16_t getAccelZ() {
	return accel.z;
}

void setAccelState(bool state) {
	if (state) {
		HIDUSER_EnableAccelerometer();
	} else {
		HIDUSER_DisableAccelerometer();
	}
}

int16_t getGyroX() {
	return gyro.x;
}

int16_t getGyroY() {
	return gyro.y;
}

int16_t getGyroZ() {
	return gyro.z;
}

void setGyroState(bool state) {
	if (state) {
		HIDUSER_EnableGyroscope();
	} else {
		HIDUSER_DisableGyroscope();
	}
}

bool getString(char* out, const char* htext, const char* def, int maxlength, bool password) {
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
