#include <3ds.h>

#include "System.h"

namespace System {

static bool isNew3DS;
static uint8_t lang;

bool init() {
	// TODO: init other services if requested
	romfsInit();

	APT_CheckNew3DS(&isNew3DS);
	if (isNew3DS)
		osSetSpeedupEnable(true);

	cfguInit();
	CFGU_GetSystemLanguage(&lang);
	cfguExit();
}

void fini() {
	romfsExit();
}

uint64_t getTime() {
	return osGetTime();
}

uint16_t getLanguage() {
	return lang;
}

}; // namespace System
