#include "Tilemap.h"
#include "Graphics.h"

#define GET_SECTION(x, y) (((((y) / sec_height) * sec_cols) + ((x) / sec_width)) % (sec_cols * sec_rows))

TileMap::TileMap(): sections(nullptr) {}

TileMap::~TileMap() {
	Renderer::destroyTargetTexture(cached);
	if (sections) delete sections;
}

void TileMap::create(uint32_t mw, uint32_t mh, uint32_t sw, uint32_t sh) {
	sec_width = sw;
	sec_height = sh;
	sec_rows = ((mh / sh) + 1);
	sec_cols = ((mw / sw) + 1);
	if (sections) delete sections;
	sections = new TileMapSection[sec_rows * sec_cols];
}

void TileMap::tileAdd(const Tile& tile) {
	sections[GET_SECTION(int(tile.x), int(tile.y))].tiles.push_back(tile);
}

const TileMapSection& TileMap::getSection(uint32_t x, uint32_t y) const {
	return sections[GET_SECTION(x, y)];
}

const TileMapRegion& TileMap::getRegion(const Rectangle& region) const {
	static TileMapRegion tmp;
	tmp.clear();

	int sx = ROUND(int(region.left), sec_width, -1);
	if (sx < 0) sx = 0;
	int sy = ROUND(int(region.top), sec_height, -1);
	if (sy < 0) sy = 0;

	for (uint32_t j = sy; j <= ROUND(uint32_t(region.bot), sec_height, 1); j += sec_height) {
		for (uint32_t i = sx; i <= ROUND(uint32_t(region.right), sec_width, 1); i += sec_width) {
			tmp.push_back(&sections[GET_SECTION(i, j)]);
		}
	}

	return tmp;
}

void TileMap::cacheToTexture(uint16_t id) {
	cached = id;
	uint32_t w = sec_width * sec_cols;
	uint32_t h = sec_height * sec_rows;
	Renderer::createTargetTexture(id, w, h);
	Renderer::targetTexture(id);
	Renderer::targetResize(w, h);
	Renderer::targetClear(C_WHITE);

	for (int i = 0; i < sec_rows*sec_cols; ++i) {
		for (const Tile& t : sections[i].tiles) {
			Renderer::drawTile(t);
		}
	}
}
