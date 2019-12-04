#ifndef SDLSHEET_H
#define SDLSHEET_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>

#include "rzdb.h"

class SDL_SpriteSheet {
  private:
	SDL_Rect* tex;
	SDL_Texture* sheet;

  public:
	SDL_SpriteSheet(): tex(nullptr), sheet(nullptr) {}
	~SDL_SpriteSheet() {
		if (tex) delete tex;
		if (sheet) SDL_DestroyTexture(sheet);
	}

	inline bool load(SDL_Renderer* renderer, const char* sheetfile, const char* metafile) {
		SDL_Surface* img = IMG_Load(sheetfile);
		if (!img) return false;

		sheet = SDL_CreateTextureFromSurface(renderer, img);
		SDL_FreeSurface(img);
		if (!sheet) return false;

		SDL_SetTextureBlendMode(sheet, SDL_BLENDMODE_BLEND);

		tex = (SDL_Rect*)RZDB_ReadEntireFile(metafile, sizeof(SDL_Rect));
		if (!tex) return false;

		return true;
	}

	inline SDL_Texture* getTextureSheet() const { return sheet; }
	inline const SDL_Rect* getTexturePart(uint16_t id) const { return (tex+id); }
};

#endif /* SDLSHEET_H */
