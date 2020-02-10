#include <3ds.h>

#include "System.h"

namespace System {

static bool isNew3DS;
static uint8_t lang;

bool init() {
	// TODO: init other services if requested
	Result res = romfsInit();

	APT_CheckNew3DS(&isNew3DS);
	if (isNew3DS)
		osSetSpeedupEnable(true);

	cfguInit();
	CFGU_GetSystemLanguage(&lang);
	cfguExit();

	return R_SUCCEEDED(res);
}

void fini() {
	romfsExit();
}

bool loop() {
	return aptMainLoop();
}

bool hasFocus() {
	return !aptIsHomePressed();
}

uint64_t getTime() {
	return osGetTime();
}

uint16_t getLanguage() {
	return lang;
}

uint64_t getTicks() {
	return svcGetSystemTick();
}

uint64_t getFrequency() {
	return SYSCLOCK_ARM11;
}

const char* getRootPath() {
	return "romfs:/";
}

}; // namespace System
