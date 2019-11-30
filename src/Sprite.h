#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

#include "Geometry.h"
#include "types.h"

// TODO: animations

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
	uint16_t padding;
};

void spritesLoad(const char* filename);
const SpriteData& spriteGetData(uint16_t id);

struct Sprite {
	Color color = Color(0xFFFFFFFF);
	float xscale = 1;
	float yscale = 1;
	float xoffs = 0;
	float yoffs = 0;
	float rotation = 0;
	float speed = 0; // spf
	uint16_t id = 0;
	uint16_t frame = 0;

	Sprite() {}
	Sprite(uint16_t id);
	~Sprite();

	const Rectangle calcBoundingBox() const;
	const SpriteData& data() const { return spriteGetData(id); }
};

#endif /* SPRITE_H */
