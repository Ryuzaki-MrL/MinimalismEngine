#include "Fading.h"
#include "Graphics.h"
#include "Timer.h"
#include "types.h"

struct FadeCtx {
	Timer timer;
	Color color;
	bool halfdone = false;
	bool done = true;
};

static FadeCtx ctx;

void fadeStart(float time, uint32_t color) {
	ctx.color.rgba = color;
	ctx.timer.start(time / 2);
	ctx.done = ctx.halfdone = false;
}

void fadeUpdate() {
	if (ctx.done) return;

	uint8_t progress = (ctx.timer.elapsed() * 255) / ctx.timer.tend;

	if (ctx.halfdone) {
		if (ctx.timer.complete()) {
			ctx.done = true;
		} else {
			ctx.color.c.a = 255 - progress;
		}
	}
	else {
		if (ctx.timer.complete()) {
			ctx.halfdone = true;
			ctx.timer.restart();
		} else {
			ctx.color.c.a = progress;
		}
	}
}

void fadeApply() {
	Renderer::targetBlend(ctx.color.rgba);
}

int fadeStatus() {
	if (ctx.done) return FADE_DONE;
	if (ctx.halfdone) return FADE_HALFDONE;
	return FADE_RUNNING;
}
