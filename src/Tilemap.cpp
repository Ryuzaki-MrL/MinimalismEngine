#include "Tilemap.h"
#include "Graphics.h"

#define GET_SECTION(x, y) (((((y) / sec_height) * sec_cols) + ((x) / sec_width)) % (sec_cols * sec_rows))

Tilemap::Tilemap(): sections(nullptr) {}

Tilemap::~Tilemap() {
	Renderer::destroyTargetTexture(cached);
	if (sections) delete sections;
}

void Tilemap::create(uint32_t mw, uint32_t mh, uint32_t sw, uint32_t sh) {
	sec_width = sw;
	sec_height = sh;
	sec_rows = ((mh / sh) + 1);
	sec_cols = ((mw / sw) + 1);
	sections = new TileMapSection[sec_rows * sec_cols];
}

void Tilemap::tileAdd(const Tile& tile) {
	sections[GET_SECTION(int(tile.x), int(tile.y))].tiles.push_back(tile);
}

const TileMapSection& Tilemap::getSection(uint32_t x, uint32_t y) const {
	return sections[GET_SECTION(x, y)];
}

const TileMapRegion& Tilemap::getRegion(const Rectangle& region) const {
	static TileMapRegion tmp;
	tmp.clear();

	int sx = ROUND(int(region.top), sec_height, -1);
	if (sx < 0) sx = 0;
	int sy = ROUND(int(region.left), sec_width, -1);
	if (sy < 0) sy = 0;

	for (int j = sy; j <= ROUND(int(region.bot), sec_height, 1); j += sec_height) {
		for (int i = sx; i <= ROUND(int(region.right), sec_width, 1); i += sec_width) {
			tmp.push_back(&sections[GET_SECTION(i, j)]);
		}
	}

	return tmp;
}

void Tilemap::cacheToTexture(uint16_t id) {
	cached = id;
	Renderer::createTargetTexture(id, sec_width * sec_cols, sec_height * sec_rows);
	Renderer::targetTexture(id);

	for (int i = 0; i < sec_rows*sec_cols; ++i) {
		for (const Tile& t : sections[i].tiles) {
			Renderer::drawTile(t);
		}
	}
}
