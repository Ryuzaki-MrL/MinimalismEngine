#include <stdio.h>
#include <stdlib.h>
#include <switch.h>

#include "Input.h"

namespace Input {

static uint64_t kDown;
static uint64_t kHeld;
static uint64_t kUp;
static touchPosition touch;
static JoystickPosition joypad[2];
static SixAxisSensorValues sas;

static uint64_t kBinds[MAX_KEYBINDS];

bool init() {
	hidInitialize();
	hidInitializeSevenSixAxisSensor();
}

void fini() {
	hidFinalizeSevenSixAxisSensor();
	hidExit();
}

void poll() {
	hidScanInput();

	kDown = hidKeysDown(CONTROLLER_P1_AUTO);
	kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
	kUp = hidKeysUp(CONTROLLER_P1_AUTO);

	hidJoystickRead(&joypad[0], CONTROLLER_P1_AUTO, 0);
	hidJoystickRead(&joypad[1], CONTROLLER_P1_AUTO, 1);
	hidTouchRead(&touch, 0);
	hidSixAxisSensorValuesRead(&sas, CONTROLLER_P1_AUTO, 1);
}

bool isKeyDown(uint64_t key) {
	return (kDown & key);
}

bool isKeyBindDown(uint16_t index) {
	return (kDown & kBinds[index]);
}

bool isKeyHeld(uint64_t key) {
	return (kHeld & key);
}

bool isKeyBindHeld(uint16_t index) {
	return (kHeld & kBinds[index]);
}

bool isKeyUp(uint64_t key) {
	return (kUp & key);
}

bool isKeyBindUp(uint16_t index) {
	return (kUp & kBinds[index]);
}

void keyBind(uint16_t index, uint64_t key) {
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
	return sas.accelerometer.x;
}

int16_t getAccelY() {
	return sas.accelerometer.y;
}

int16_t getAccelZ() {
	return sas.accelerometer.z;
}

void setAccelState(bool state) {
	if (state) {
		hidStartSevenSixAxisSensor();
	} else {
		hidStopSevenSixAxisSensor();
	}
}

int16_t getGyroX() {
	return sas.gyroscope.x;
}

int16_t getGyroY() {
	return sas.gyroscope.y;
}

int16_t getGyroZ() {
	return sas.gyroscope.z;
}

void setGyroState(bool state) {
	if (state) {
		hidStartSevenSixAxisSensor();
	} else {
		hidStopSevenSixAxisSensor();
	}
}

bool getString(char* out, const char* htext, const char* def, int maxlength, bool password) {
	SwkbdConfig* kb;

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
	SwkbdConfig* kb;

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
