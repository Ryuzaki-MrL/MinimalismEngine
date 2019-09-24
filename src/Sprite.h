#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

#include "types.h"

// TODO: animations

struct Sprite {
	Color color;
	float xscale;
	float yscale;
	float rotation;
	uint16_t id;
	uint16_t frame;
	int8_t steps;
	uint8_t counter;
};

struct SpriteData {
	uint16_t width;
	uint16_t height;
	int16_t xorig;
	int16_t yorig;
	int16_t bbox_left;
	int16_t bbox_right;
	int16_t bbox_top;
	int16_t bbox_bottom;
	uint16_t sheet;
	uint16_t texture;
	uint16_t imgcount;
};

void spritesLoad(const char* filename);

const SpriteData& spriteGetData(uint16_t id);

#endif /* SPRITE_H */
