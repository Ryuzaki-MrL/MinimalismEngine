#ifndef SDLFONT_H
#define SDLFONT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>

#include "SDL_FontCache.h"
#include "rzdb.h"

struct SDL_Font {
	TTF_Font* ttf;
	FC_Font* fc;

	SDL_Font(): ttf(nullptr), fc(nullptr) {}
	~SDL_Font() {
		if (fc) FC_FreeFont(fc);
		if (ttf) TTF_CloseFont(ttf);
	}

	inline bool load(SDL_Renderer* renderer, const char* fontfile, size_t size) {
		ttf = TTF_OpenFont(fontfile, size);
		FC_LoadFontFromTTF(fc, renderer, ttf, FC_MakeColor(255, 255, 255, 255));
		return (ttf && fc);
	}
};

#endif /* SDLFONT_H */
