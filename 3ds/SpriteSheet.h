#ifndef SPRSHEET_H
#define SPRSHEET_H

#include <citro2d.h>
#include <stdio.h>
#include <stdint.h>

class SpriteSheet {
  public:
	uint16_t rows, cols;
  private:
	C2D_SpriteSheet sheet;
	bool tileset;

  public:
	SpriteSheet(): rows(0), cols(0), sheet(nullptr), tileset(false) {}
	~SpriteSheet() {
		if (sheet) C2D_SpriteSheetFree(sheet);
	}

	inline bool load(const char* sheetfile, const char* metafile) {
		sheet = C2D_SpriteSheetLoad(sheetfile);
		if (!sheet) return false;

		FILE* ts = fopen(metafile, "r");
		if (ts) {
			tileset = true;
			fscanf(ts, "%hu %hu", &rows, &cols); // dangerous
			fclose(ts);
		}

		return true;
	}

	inline C2D_SpriteSheet get() const { return sheet; }
	inline bool isTileset() const { return tileset; }
};

#endif /* SPRSHEET_H */
