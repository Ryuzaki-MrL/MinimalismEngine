#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

// TODO: device info (screen size, etc)

namespace System {
	bool init();
	void fini();

	bool loop(); // process events, etc

	uint64_t getTime(); // TODO: datetime
	uint16_t getLanguage();

	const char* getRootPath();
};

#endif /* SYSTEM_H */
