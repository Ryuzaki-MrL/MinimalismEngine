#ifndef RENDERCTX_H
#define RENDERCTX_H

#include <SDL2/SDL.h>
#include <stdint.h>

#define RGBA8(r, g, b, a) (((r)&0xff)|(((g)&0xff)<<8)|(((b)&0xff)<<16)|(((a)&0xff)<<24))

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define SUBSCREEN_WIDTH 1280
#define SUBSCREEN_HEIGHT 720

#define DEFAULT_SCREEN nullptr

typedef SDL_Window* Screen;

struct RenderContext {
	float tx, ty;
	SDL_Window* window;
	SDL_Renderer* renderer;
};

#endif /* RENDERCTX_H */
