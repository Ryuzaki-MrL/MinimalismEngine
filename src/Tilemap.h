#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>

#include "Geometry.h"

struct Tile {
	Rectangle dim;
	float x, y;
	float xscale;
	float yscale;
	uint16_t sheet;
	uint16_t texture;
	uint16_t flags;
};

struct TileMapSection {
	std::vector<Tile> tiles;
};

typedef std::vector<const TileMapSection*> TileMapRegion;

class TileMap {
  private:
	TileMapSection* sections;
	uint32_t sec_width;
	uint32_t sec_height;
	uint16_t cached;
	uint8_t sec_rows;
	uint8_t sec_cols;

  public:
	TileMap();
	~TileMap();

	void create(uint32_t mw, uint32_t mh, uint32_t sw, uint32_t sh);

	void tileAdd(const Tile& tile);
	const TileMapSection& getSection(uint32_t x, uint32_t y) const;
	const TileMapRegion& getRegion(const Rectangle& region) const;

	void cacheToTexture(uint16_t id);
	inline uint16_t getCachedTexture() const { return cached; }
};

#endif /* TILEMAP_H */
