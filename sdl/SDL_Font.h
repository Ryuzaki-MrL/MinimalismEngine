#ifndef SDLFONT_H
#define SDLFONT_H

#include <stdint.h>

#include "rzdb.h"
#include "rzft.h"

struct SDL_Font {
	RZFT_Font* rzft;

	SDL_Font(): rzft(nullptr) {}
	~SDL_Font() {
		if (rzft) RZFT_FreeFont(rzft);
	}

	inline bool load(const char* fontfile, uint16_t sheet) {
		rzft = RZFT_LoadFont(fontfile, sheet);
		return (rzft != nullptr);
	}
};

#endif /* SDLFONT_H */
