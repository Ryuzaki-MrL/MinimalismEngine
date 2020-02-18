#include <SDL2/SDL.h>
#include <windows.h>
#include <shlobj.h>
#include <sys/stat.h>
#include <stdio.h>
#include <chrono>

#include "System.h"
#include "PollEvent.h"

// TODO: keyboard text mode (SDL_StartTextInput)

namespace System {

static char userbasepath[260] = "";
static bool focus;

bool init() {
	SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, userbasepath);
	sprintf(userbasepath, "%s/%s", userbasepath, APP_TITLE);
	CreateDirectoryA(userbasepath, NULL);
	return true;
}

void fini() {
	
}

bool loop() {
	inputReset();

	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
			case SDL_QUIT:
				return false;
			
			case SDL_KEYDOWN:
			case SDL_KEYUP:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
			case SDL_CONTROLLERBUTTONDOWN:
			case SDL_CONTROLLERBUTTONUP:
			case SDL_CONTROLLERAXISMOTION:
			case SDL_CONTROLLERDEVICEADDED:
			case SDL_CONTROLLERDEVICEREMOVED:
				inputPollEvent(event);
				break;
			
			case SDL_WINDOWEVENT: {
				switch(event.window.event) {
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						focus = true;
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						focus = false;
						break;
				}
			}
		}
	}
	return true;
}

bool hasFocus() {
	return focus;
}

uint64_t getTime() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

uint16_t getLanguage() {
	return 0;
}

uint64_t getTicks() {
	return SDL_GetPerformanceCounter();
}

uint64_t getFrequency() {
	return SDL_GetPerformanceFrequency();
}

const char* getRootPath() {
	return "./romfs/";
}

const char* getUserPath() {
	return userbasepath;
}

}; // namespace System
