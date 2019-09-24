#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

// TODO: device info (screen size, etc)

namespace System {
	bool init();
	void fini();

	uint64_t getTime();
	uint16_t getLanguage();
};

#endif /* SYSTEM_H */
