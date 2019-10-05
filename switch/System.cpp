#include <chrono>
#include <switch.h>

#include "System.h"

namespace System {

static uint64_t lang;

bool init() {
	// TODO: init other services if requested
	romfsInit();
}

void fini() {
	romfsExit();
}

bool loop() {
	return appletMainLoop();
}

uint64_t getTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

uint16_t getLanguage() {
	return lang;
}

const char* getRootPath() {
	return "romfs:/";
}

}; // namespace System
