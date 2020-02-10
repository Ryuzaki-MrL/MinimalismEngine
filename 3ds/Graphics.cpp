#include <unordered_map>
#include <stdio.h>

#include "Graphics.h"
#include "Geometry.h"
#include "SpriteSheet.h"
#include "rzft.h"

namespace Renderer {

static RenderContext ctx;

static C2D_TextBuf staticbuf;
static std::unordered_map<const char*, C2D_Text> txtready; // using a pointer as key is intended here
static C2D_ImageTint s_tint;
static C2D_DrawParams s_params;
static float s_3dside = 1.0;

static std::unordered_map<uint16_t, RZFT_Font*> s_fonts;
static std::unordered_map<uint16_t, SpriteSheet> s_sheets;

static void cacheTextForRendering(const char* str) {
	if (!staticbuf) {
		staticbuf = C2D_TextBufNew(768);
		C2D_TextBufClear(staticbuf);
	}
	C2D_TextParse(&txtready[str], staticbuf, str);
	C2D_TextOptimize(&txtready[str]);
}

bool init() {
	gfxInit(GSP_BGR8_OES, GSP_BGR8_OES, true);
	gfxSet3D(true);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// custom color modulation
	C3D_TexEnv* env = C3D_GetTexEnv(2);
	C3D_TexEnvInit(env);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PREVIOUS, GPU_PRIMARY_COLOR, GPU_TEXTURE3);
	C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

	ctx.top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	ctx.right = C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT);
	ctx.bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

	ctx.internalbuf = C2D_TextBufNew(256);

	return true;
}

void fini() {
	C2D_TextBufDelete(ctx.internalbuf);

	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

bool is3DEnabled() {
	return (osGet3DSliderState() != 0.f);
}

float get3D() {
	return osGet3DSliderState() * s_3dside;
}

void frameStart() {
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TextBufClear(ctx.internalbuf);
}

void frameEnd() {
	C3D_FrameEnd(0);
}

void createTargetTexture(uint16_t id, uint32_t w, uint32_t h) {
	// TODO
}

void destroyTargetTexture(uint16_t id) {
	// TODO
}

void targetTexture(uint16_t targ) {
	// TODO
}

void targetScreen(Screen screen) {
	if (screen.type == GFX_TOP) {
		C2D_SceneBegin(ctx.currtarg = ((screen.side==GFX_LEFT) ? ctx.top : ctx.right));
		s_3dside = (screen.side==GFX_LEFT) ? -1.0 : 1.0;
		ctx.tw = 400;
	} else {
		C2D_SceneBegin(ctx.currtarg = ctx.bot);
		ctx.tw = 320;
	}
	ctx.th = 240;
	C2D_TargetClear(ctx.currtarg, C_WHITE);
}

void targetClear(uint32_t color) {
	drawRectangle(0, 0, ctx.tw, ctx.th, color);
}

void targetBlend(uint32_t color) {
	C2D_Fade(color);
}

void targetResize(uint32_t w, uint32_t h) {
	if (ctx.tw == w && ctx.th == h) return;
	C2D_SceneSize(ctx.tw = h, ctx.th = w, ctx.currtarg->linked);
	C2D_TargetClear(ctx.currtarg, C_WHITE);
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
	C2D_DrawTriangle(
		x0 - ctx.tx, y0 - ctx.ty, color,
		x1 - ctx.tx, y1 - ctx.ty, color,
		x2 - ctx.tx, y2 - ctx.ty, color,
		depth
	);
}

void drawRectangle(float x, float y, float w, float h, uint32_t color, float depth) {
	C2D_DrawRectSolid(x - ctx.tx, y - ctx.ty, depth, w, h, color);
}

void drawEllipse(float x, float y, float w, float h, uint32_t color, float depth) {
	C2D_DrawEllipseSolid(x - ctx.tx, y - ctx.ty, depth, w, h, color);
}

void drawTexture(uint16_t sheet, uint16_t id, float x, float y, float depth) {
	C2D_PlainImageTint(&s_tint, C_WHITE, 0.0f);
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(s_sheets[sheet].get(), id), x - ctx.tx, y - ctx.ty, depth, &s_tint);
}

void drawTextureExt(uint16_t sheet, uint16_t id, float x, float y, float xorig, float yorig, float xscale, float yscale, float angle, uint32_t color, float depth) {
	C2D_PlainImageTint(&s_tint, color, 0.0f);
	C2D_Image img = C2D_SpriteSheetGetImage(s_sheets[sheet].get(), id);
	s_params = {
		{ floor(x+xorig - ctx.tx), floor(y+yorig - ctx.ty), xscale*img.subtex->width, yscale*img.subtex->height },
		{ xorig, yorig },
		depth, degtorad(-angle)
	};
	C2D_DrawImage(img, &s_params, &s_tint);
}

void drawTextureFill(uint16_t sheet, uint16_t id, float depth) {
	C2D_PlainImageTint(&s_tint, C_WHITE, 0.0f);
	C2D_Image img = C2D_SpriteSheetGetImage(s_sheets[sheet].get(), id);
	int txi = ctx.tx;
	int tyi = ctx.ty;
	for (uint32_t x = 0; x <= ctx.tw; x += img.subtex->width) {
		for (uint32_t y = 0; y <= ctx.th; y += img.subtex->height) {
			C2D_DrawImageAt(img, x - (txi % img.subtex->width), y - (tyi % img.subtex->height), depth, &s_tint);
		}
	}
}

void drawTargetTexture(uint16_t targ, float x, float y, const Rectangle& region, float depth) {
	// TODO
}

void drawSprite(const Sprite& spr, float x, float y, float depth) {
	const SpriteData& data = spr.data();
	uint16_t frame = data.texture + (spr.frame % data.imgcount);
	drawTextureExt(data.sheet, frame, x, y, spr.xoffs, spr.yoffs, spr.xscale, spr.yscale, spr.rotation, spr.color.rgba, depth);
}

void drawTileMap(const TileMap& tmap, const Rectangle& region, float depth) {
	const TileMapRegion& visibles_tiles = tmap.getRegion(region);
	for (const TileMapSection* ts : visibles_tiles) {
		for (const Tile& tt : ts->tiles) {
			drawTile(tt, depth);
		}
	}
}

void drawTile(const Tile& tile, float depth) {
	const SpriteSheet& ssheet = s_sheets[tile.sheet + tile.texture];
	C2D_SpriteSheet tsheet = ssheet.get();
	C2D_Image img = C2D_SpriteSheetGetImage(tsheet, 0); // use first tile as reference

	int isw = img.subtex->width;
	int ish = img.subtex->height;
	int sx = tile.dim.left / isw;
	int sy = tile.dim.top / ish;
	int sw = tile.dim.right / isw;
	int sh = tile.dim.bot / ish;
	float tdx = tile.x;
	float tdy = tile.y;
	float xs = tile.xscale*isw;
	float ys = tile.yscale*ish;
	if (tile.xscale < 0) tdx += xs;
	if (tile.yscale < 0) tdy += ys;

	C2D_PlainImageTint(&s_tint, C_WHITE, 0.0f);

	for (int i = 0; i < sw; ++i) {
		for (int j = 0; j < sh; ++j) {
			img = C2D_SpriteSheetGetImage(tsheet, (sy+j)*ssheet.cols + (sx+i));
			s_params = {
				{ floor(tdx+i*xs - ctx.tx), floor(tdy+j*ys - ctx.ty), xs, ys },
				{ 0, 0 }, depth, 0
			};
			C2D_DrawImage(img, &s_params, &s_tint);
		}
	}
}

void drawText(uint16_t font, float x, float y, float z, float size, uint32_t color, uint8_t align, const char* str) {
	RZFT_Font* fnt = s_fonts[font];
	if (!fnt) {
		float w, xx;
		if (!txtready.count(str)) {
			cacheTextForRendering(str);
		}
		C2D_TextGetDimensions(&txtready[str], size, size, &w, nullptr);
		xx = x - w * (align / 2.0);
		C2D_DrawText(&txtready[str], C2D_WithColor, xx - ctx.tx, y - ctx.ty, z, size, size, color);
	}
	else {
		RZFT_DrawText(fnt, x, y, size, size, color, -1, align, str);
	}
}

void drawTextFormat(uint16_t font, float x, float y, float z, float size, uint32_t color, uint8_t align, const char* str, ...) {
	static char buffer[256] = "";
	va_list args;
	va_start(args, str);
	vsnprintf(buffer, sizeof(buffer) - 1, str, args);

	RZFT_Font* fnt = s_fonts[font];
	if (!fnt) {
		float w, xx;
		C2D_Text tmp;
		C2D_TextParse(&tmp, ctx.internalbuf, buffer);
		C2D_TextGetDimensions(&tmp, size, size, &w, nullptr);
		xx = x - w * (align / 2.0);
		C2D_DrawText(&tmp, C2D_WithColor, xx - ctx.tx, y - ctx.ty, z, size, size, color);
	}
	else {
		RZFT_DrawText(fnt, x, y, size, size, color, -1, align, buffer);
	}

	va_end(args);
}


void texsheetAdd(const char* name, uint16_t index) {
	static char metapath[256] = "";
	sprintf(metapath, "%s.t", name);
	s_sheets[index].load(name, metapath);
}

void texsheetUnload(uint16_t index) {
	if (s_sheets.count(index)) {
		s_sheets.erase(index);
	}
}

void fontAdd(const char* name, uint16_t index, uint16_t sheet) {
	s_fonts[index] = RZFT_LoadFont(name, sheet);
}

void fontUnload(uint16_t index) {
	if (s_fonts.count(index)) {
		RZFT_FreeFont(s_fonts[index]);
		s_fonts.erase(index);
	}
}

}; // namespace Renderer
