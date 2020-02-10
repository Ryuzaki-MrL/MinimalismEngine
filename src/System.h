#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

// TODO: device info (screen size, etc)
// TODO: init features as needed

namespace System {
	bool init();
	void fini();

	bool loop(); // process events, etc
	bool hasFocus();

	uint64_t getTime(); // TODO: datetime
	uint16_t getLanguage();

	uint64_t getTicks();
	uint64_t getFrequency(); // Hz

	const char* getRootPath();
};

#endif /* SYSTEM_H */
