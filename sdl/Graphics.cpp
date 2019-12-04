#include <SDL2/SDL2_gfxPrimitives.h>
#include <unordered_map>
#include <stdio.h>

#include "SDL_SpriteSheet.h"
#include "SDL_Font.h"
#include "Graphics.h"
#include "Geometry.h"
#include "types.h"

#define MAKE_COLOR(x) { x.c.r, x.c.g, x.c.b, x.c.a };

namespace Renderer {

typedef std::unordered_map<const char*, SDL_Texture*> SDL_TextCache;

static RenderContext ctx;

static Color s_color;
static SDL_Color s_sdlcol;
static SDL_Point s_points[4];
static SDL_Rect s_srcrect;
static SDL_Rect s_dstrect;

static std::unordered_map<uint16_t, SDL_Texture*> s_txcache;
static std::unordered_map<uint16_t, SDL_TextCache> s_ftcache;

static std::unordered_map<uint16_t, SDL_Font> s_fonts;
static std::unordered_map<uint16_t, SDL_SpriteSheet> s_sheets;

bool init() {
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	ctx.window = SDL_CreateWindow("sdl2_gles2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (!ctx.window) return false;

	uint32_t flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE;
	ctx.renderer = SDL_CreateRenderer(ctx.window, 0, flags);
	if (!ctx.renderer) return false;

	SDL_SetRenderDrawBlendMode(ctx.renderer, SDL_BLENDMODE_BLEND);

	return true;
}

void fini() {
	for (auto& i : s_txcache)
		SDL_DestroyTexture(i.second);

	SDL_DestroyRenderer(ctx.renderer);
	SDL_DestroyWindow(ctx.window);

	TTF_Quit();
	IMG_Quit();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

float get3D() {
	return 0.f;
}

void frameStart() {
	// TODO
}

void frameEnd() {
	SDL_RenderPresent(ctx.renderer);
}

void createTargetTexture(uint16_t id, uint32_t w, uint32_t h) {
	destroyTargetTexture(id);
	s_txcache[id] = SDL_CreateTexture(ctx.renderer, SDL_PIXELFORMAT_UNKNOWN, SDL_TEXTUREACCESS_TARGET, w, h);
}

void destroyTargetTexture(uint16_t id) {
	if (s_txcache.count(id) && s_txcache[id] != nullptr) {
		SDL_DestroyTexture(s_txcache[id]);
	}
}

void targetTexture(uint16_t targ) {
	if (s_txcache.count(targ) && s_txcache[targ] != nullptr) {
		SDL_SetRenderTarget(ctx.renderer, s_txcache[targ]);
	}
}

void targetScreen(Screen) {
	SDL_SetRenderTarget(ctx.renderer, NULL);
	SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
	SDL_RenderClear(ctx.renderer);
}

void targetClear(uint32_t color) {
	s_color = Color(color);
	SDL_SetRenderDrawColor(ctx.renderer, s_color.c.r, s_color.c.g, s_color.c.b, s_color.c.a);
	SDL_RenderClear(ctx.renderer);
}

void targetBlend(uint32_t color) {
	s_color = Color(color);
	SDL_SetRenderDrawColor(ctx.renderer, s_color.c.r, s_color.c.g, s_color.c.b, s_color.c.a);
	s_srcrect = { 0, 0, 0, 0 };
	SDL_GetWindowSize(ctx.window, &s_srcrect.w, &s_srcrect.h);
	SDL_RenderFillRect(ctx.renderer, &s_srcrect);
}

void targetResize(uint32_t w, uint32_t h) {
	SDL_RenderSetLogicalSize(ctx.renderer, w, h);
	//SDL_SetWindowSize(ctx.window, w, h);
	SDL_SetRenderDrawColor(ctx.renderer, 0, 0, 0, 255);
	SDL_RenderClear(ctx.renderer);
}

void targetPosition(float x, float y) {
	ctx.tx = x;
	ctx.ty = y;
}

void targetTranslate(float x, float y) {
	ctx.tx += x;
	ctx.ty += y;
}

void drawTriangle(float x0, float y0, float x1, float y1, float x2, float y2, uint32_t color, float depth) {
	filledTrigonColor(ctx.renderer, x0, y0, x1, y1, x2, y2, color);
}

void drawRectangle(float x, float y, float w, float h, uint32_t color, float depth) {
	s_color = Color(color);
	int xx = x - ctx.tx;
	int yy = y - ctx.ty;
	s_srcrect = { xx, yy, int(w), int(h) };
	SDL_SetRenderDrawColor(ctx.renderer, s_color.c.r, s_color.c.g, s_color.c.b, s_color.c.a);
	SDL_RenderFillRect(ctx.renderer, &s_srcrect);
}

void drawEllipse(float x, float y, float w, float h, uint32_t color, float depth) {
	filledEllipseColor(ctx.renderer, x, y, w/2, h/2, color);
}

void drawTexture(uint16_t sheet, uint16_t id, float x, float y, float depth) {
	const SDL_Rect* src = s_sheets[sheet].getTexturePart(id);
	int xx = x - ctx.tx;
	int yy = y - ctx.ty;
	s_dstrect = { xx, yy, src->w, src->h };
	SDL_Texture* tex = s_sheets[sheet].getTextureSheet();
	SDL_SetTextureAlphaMod(tex, 0xFF);
	SDL_SetTextureColorMod(tex, 0xFF, 0xFF, 0xFF);
	SDL_RenderCopy(ctx.renderer, tex, src, &s_dstrect);
}

void drawTextureExt(uint16_t sheet, uint16_t id, float x, float y, float xorig, float yorig, float xscale, float yscale, float angle, uint32_t color, float depth) {
	const SDL_Rect* src = s_sheets[sheet].getTexturePart(id);
	if (!src) src = &s_srcrect;
	SDL_Texture* tex = s_sheets[sheet].getTextureSheet();
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (xscale < 0) flip = SDL_RendererFlip(flip | SDL_FLIP_HORIZONTAL);
	if (yscale < 0) flip = SDL_RendererFlip(flip | SDL_FLIP_VERTICAL);
	int xx = x - ctx.tx;
	int yy = y - ctx.ty;
	int ww = abs(src->w * xscale);
	int hh = abs(src->h * yscale);
	s_dstrect = { xx, yy, ww, hh };
	s_points[0] = { int(xorig), int(yorig) };
	s_color = Color(color);
	SDL_SetTextureAlphaMod(tex, s_color.c.a);
	SDL_SetTextureColorMod(tex, s_color.c.r, s_color.c.g, s_color.c.b);
	SDL_RenderCopyEx(ctx.renderer, tex, src, &s_dstrect, -angle, &s_points[0], flip);
}

void drawTextureFill(uint16_t sheet, uint16_t id, float depth) {
	// TODO
}

void drawTargetTexture(uint16_t targ, float x, float y, const Rectangle& region, float depth) {
	int w = region.right - region.left;
	int h = region.bot - region.top;
	int xx = x - ctx.tx;
	int yy = y - ctx.ty;
	s_srcrect = { int(region.left), int(region.top), w, h };
	s_dstrect = { xx, yy, w, h };
	SDL_Texture* tex = s_txcache[targ];
	SDL_SetTextureAlphaMod(tex, 0xFF);
	SDL_SetTextureColorMod(tex, 0xFF, 0xFF, 0xFF);
	SDL_RenderCopy(ctx.renderer, tex, &s_srcrect, &s_dstrect);
}

void drawSprite(const Sprite& spr, float x, float y, float depth) {
	const SpriteData& data = spr.data();
	uint16_t frame = data.texture + (spr.frame % data.imgcount);
	drawTextureExt(data.sheet, frame, x, y, spr.xoffs, spr.yoffs, spr.xscale, spr.yscale, spr.rotation, spr.color.rgba, depth);
}

void drawTileMap(const TileMap& tmap, const Rectangle& region, float depth) {
	drawTargetTexture(tmap.getCachedTexture(), region.left, region.top, region, depth);
}

void drawTile(const Tile& tile, float depth) {
	const SDL_Rect* src = s_sheets[tile.sheet].getTexturePart(tile.texture);
	SDL_Texture* tex = s_sheets[tile.sheet].getTextureSheet();
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	int tdx = tile.x;
	int tdy = tile.y;
	if (tile.xscale < 0) {
		flip = SDL_RendererFlip(flip | SDL_FLIP_HORIZONTAL);
		tdx += tile.dim.right * tile.xscale;
	}
	if (tile.yscale < 0) {
		flip = SDL_RendererFlip(flip | SDL_FLIP_VERTICAL);
		tdy += tile.dim.bot * tile.yscale;
	}
	s_srcrect = { int(src->x + tile.dim.left), int(src->y + tile.dim.top), int(tile.dim.right), int(tile.dim.bot) };
	s_dstrect = { int(tdx - ctx.tx), int(tdy - ctx.ty), abs(int(s_srcrect.w * tile.xscale)), abs(int(s_srcrect.h * tile.yscale)) };
	s_points[0] = { 0, 0 };
	SDL_SetTextureAlphaMod(tex, 0xFF);
	SDL_SetTextureColorMod(tex, 0xFF, 0xFF, 0xFF);
	SDL_RenderCopyEx(ctx.renderer, tex, &s_srcrect, &s_dstrect, 0.f, &s_points[0], flip);
}

void drawText(uint16_t font, float x, float y, float z, float size, uint32_t color, uint8_t align, const char* str) {
	// TODO: cache rendered text as texture
	RZFT_DrawText(s_fonts[font].rzft, x, y, size, size, color, -1, align, str);
}

void drawTextFormat(uint16_t font, float x, float y, float z, float size, uint32_t color, uint8_t align, const char* str, ...) {
	static char buffer[256] = "";
	va_list args;
	va_start(args, str);
	vsnprintf(buffer, sizeof(buffer) - 1, str, args);

	s_color = Color(color);
	s_sdlcol = MAKE_COLOR(s_color);
	FC_DrawEffect(s_fonts[font].fc, ctx.renderer, x - tx, y - ty, FC_MakeEffect(align, FC_MakeScale(size, size), s_sdlcol), buffer);

	va_end(args);
}


void texsheetAdd(const char* name, uint16_t index) {
	static char sheetpath[256] = "";
	sprintf(sheetpath, "%s.png", name);
	s_sheets[index].load(ctx.renderer, sheetpath, name);
}

void texsheetUnload(uint16_t index) {
	if (s_sheets.count(index)) {
		s_sheets.erase(index);
	}
}

void fontAdd(const char* name, uint16_t index, size_t size) {
	s_fonts[index].load(ctx.renderer, name, size);
}

void fontUnload(uint16_t index) {
	if (s_fonts.count(index)) {
		s_fonts.erase(index);
	}
}

}; // namespace Renderer
