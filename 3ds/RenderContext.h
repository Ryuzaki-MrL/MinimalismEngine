#ifndef RENDERCTX_H
#define RENDERCTX_H

#include <citro3d.h>
#include <citro2d.h>
#include <stdint.h>

#define RGBA8(r, g, b, a) C2D_Color32(r, g, b, a)

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 240
#define SUBSCREEN_WIDTH 320
#define SUBSCREEN_HEIGHT 240

typedef C3D_RenderTarget RenderTarget;

struct Screen {
	gfxScreen_t type;
	gfx3dSide_t side;
};

struct RenderContext {
	float tx, ty, tw, th;
	C3D_RenderTarget* top;
	C3D_RenderTarget* right;
	C3D_RenderTarget* bot;
	C3D_RenderTarget* currtarg;
	C2D_TextBuf internalbuf;
};

#endif /* RENDERCTX_H */
