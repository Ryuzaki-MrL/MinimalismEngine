#include "Sprite.h"
#include "rzdb.h"

static SpriteData* spritedata;

void spritesLoad(const char* filename) {
	spritedata = (SpriteData*)RZDB_ReadEntireFile(filename, sizeof(SpriteData));
}

const SpriteData& spriteGetData(uint16_t id) {
	return spritedata[id];
}
