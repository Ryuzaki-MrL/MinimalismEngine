#include "Fading.h"
#include "Graphics.h"
#include "types.h"

struct FadeCtx {
	Timer timer;
	Color color;
	float tend;
	bool halfdone = false;
	bool done = true;
};

static FadeCtx ctx;

void fadeStart(float time, uint32_t color) {
	ctx.color.rgba = color;
	ctx.timer.start(time);
	ctx.tend = time;
	ctx.done = ctx.halfdone = false;
}

void fadeUpdate() {
	if (ctx.done) return;
	if (ctx.halfdone) {
		if (ctx.timer.complete()) {
			ctx.done = true;
		}
	} else {
		if (ctx.timer.complete()) {
			ctx.halfdone = true;
			ctx.timer.restart();
		}
	}
	ctx.color.c.a = (ctx.timer.elapsed() * 255) / ctx.tend;
	Renderer::targetBlend(ctx.color.rgba);
}

int fadeStatus() {
	if (ctx.counter >= ctx.steps) return FADE_HALFDONE;
	if (ctx.done) return FADE_DONE;
	return FADE_RUNNING;
}
