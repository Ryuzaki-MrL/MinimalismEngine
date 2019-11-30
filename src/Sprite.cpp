#include <algorithm>

#include "Sprite.h"
#include "rzdb.h"

static SpriteData* spritedata;

Sprite::Sprite(uint16_t id): id(id) {
	xoffs = spritedata[id].xorig;
	yoffs = spritedata[id].yorig;
}
Sprite::~Sprite() {}

const Rectangle Sprite::calcBoundingBox() const {
	const SpriteData& spr = spritedata[id];

	// Scale bbox around origin
	float left  = (((spr.bbox_left   - spr.xorig)     * abs(xscale)));
	float right = (((spr.bbox_right  - spr.xorig + 1) * abs(xscale)));
	float top   = (((spr.bbox_top    - spr.yorig)     * abs(yscale)));
	float bot   = (((spr.bbox_bottom - spr.yorig + 1) * abs(yscale)));

	// Rotate scaled bbox vertices
	Point lt(left, top);  lt.rotate(-rotation); lt.translate(spr.xorig + 0, spr.yorig + 0);
	Point rt(right, top); rt.rotate(-rotation); rt.translate(spr.xorig - 1, spr.yorig + 0);
	Point lb(left, bot);  lb.rotate(-rotation); lb.translate(spr.xorig + 0, spr.yorig - 1);
	Point rb(right, bot); rb.rotate(-rotation); rb.translate(spr.xorig - 1, spr.yorig - 1);

	// Pick up min and max points for the enclosing bbox
	left  = std::min(lt.x, std::min(rt.x, std::min(lb.x, rb.x)));
	right = std::max(lt.x, std::max(rt.x, std::max(lb.x, rb.x)));
	top   = std::min(lt.y, std::min(rt.y, std::min(lb.y, rb.y)));
	bot   = std::max(lt.y, std::max(rt.y, std::max(lb.y, rb.y)));

	return Rectangle(left, right, top, bot);
}

void spritesLoad(const char* filename) {
	spritedata = (SpriteData*)RZDB_ReadEntireFile(filename, sizeof(SpriteData));
}

const SpriteData& spriteGetData(uint16_t id) {
	return spritedata[id];
}
