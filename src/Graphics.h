#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "RenderContext.h"
#include "Tilemap.h"
#include "Sprite.h"

// TODO: universal color function
#define C_BLACK RGBA8(0,0,0,0xFF)
#define C_WHITE RGBA8(0xFF,0xFF,0xFF,0xFF)

#define ALIGN_LEFT		0
#define ALIGN_CENTER 	1
#define ALIGN_RIGHT 	2

namespace Renderer {
	bool init();
	void fini();

	/// For 3DS or any other supported platform
	float get3D();

	void frameStart();
	void frameEnd();

	void createTargetTexture(uint16_t id, uint32_t w, uint32_t h);
	void destroyTargetTexture(uint16_t id);

	void targetTexture(uint16_t targ);
	void targetScreen(Screen);
	void targetClear(uint32_t color);
	void targetBlend(uint32_t color);
	void targetResize(uint32_t w, uint32_t h);
	void targetTranslate(float x, float y);

	void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, uint32_t color, float depth = 0.0f);
	void drawRectangle(float x, float y, float w, float h, uint32_t color, float depth = 0.0f);
	void drawEllipse(float x, float y, float w, float h, uint32_t color, float depth = 0.0f);

	void drawTexture(uint16_t sheet, uint16_t id, float x, float y, float depth = 0.0f);
	void drawTextureExt(uint16_t sheet, uint16_t id, float x, float y, float xorig, float yorig, float xscale, float yscale, float angle, uint32_t color, float depth = 0.0f);
	void drawTextureFill(uint16_t sheet, uint16_t id, float depth = 0.0f);
	void drawTargetTexture(uint16_t targ, float x, float y, const Rectangle& region, float depth = 0.0f);
	void drawSprite(const Sprite& sprite, float x, float y, float depth = 0.0f);
	void drawTileMap(const TileMap& tmap, const Rectangle& region, float depth = 0.0f);
	void drawTile(const Tile& tile, float depth = 0.0f);

	void drawText(uint16_t font, float x, float y, float z, float size, uint32_t color, uint8_t align, const char* str);
	void drawTextFormat(uint16_t font, float x, float y, float z, float size, uint32_t color, uint8_t align, const char* str, ...);

	/* Resources */
	void texsheetAdd(const char* name, uint16_t index);
	void texsheetUnload(uint16_t index);
	void fontAdd(const char* name, uint16_t index, size_t size);
	void fontUnload(uint16_t index);
};

#endif /* GRAPHICS_H */
