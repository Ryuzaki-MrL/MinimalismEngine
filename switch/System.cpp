#include <chrono>
#include <switch.h>

#include "System.h"

namespace System {

static uint64_t lang;

bool init() {
	// TODO: init other services if requested
	return R_SUCCEEDED(romfsInit());
}

void fini() {
	romfsExit();
}

bool loop() {
	return appletMainLoop();
}

bool hasFocus() {
	return true;
}

uint64_t getTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

uint16_t getLanguage() {
	return lang;
}

uint64_t getTicks() {
	return armGetSystemTick();
}

uint64_t getFrequency() {
	return armGetSystemTickFreq();
}

const char* getRootPath() {
	return "romfs:/";
}

}; // namespace System
