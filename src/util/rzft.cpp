/// Custom font format (RyuZaki FonT)

#include <unordered_map>

#include "rzft.h"
#include "mem.h"
#include "Graphics.h"

#pragma pack(1)

struct rzftHeader {
	char magic[4]; // RZFT
	uint32_t sheet;
	uint16_t lineHeight;
	uint16_t baseline;
	uint16_t npages;
	uint16_t spacing;
};

struct rzftChar {
	uint16_t glyph;
	int16_t xoffs;
	int16_t yoffs;
	int16_t advance;
};

struct rzftPage {
	rzftChar chr[0x100];
};

struct RZFT_Font {
	rzftHeader hdr;
	rzftPage pages[];
};

RZFT_Font* RZFT_LoadFont(const char* fname, uint32_t sheet) {
	FILE* file = fopen(fname, "rb");
	if (!file) return NULL;

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	rewind(file);

	RZFT_Font* font = (RZFT_Font*)memalloc(size);
	if (!font) return NULL;
	fread(font, 1, size, file);
	fclose(file);

	font->hdr.sheet = sheet;

	return font;
}

void RZFT_FreeFont(RZFT_Font* font) {
	if (font) memfree(font);
}

// source: https://github.com/smealum/ctrulib/blob/master/libctru/source/util/utf/decode_utf8.c
static int RZFT_DecodeUTF8(uint32_t* out, const uint8_t* in) {
	uint8_t code1, code2, code3, code4;
	code1 = *in++;
	if (code1 < 0x80) {
		/* 1-byte sequence */
		*out = code1;
		return 1;
	}
	else if (code1 < 0xC2) {
		return -1;
	}
	else if (code1 < 0xE0) {
		/* 2-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80) return -1;
		*out = (code1 << 6) + code2 - 0x3080;
		return 2;
	}
	else if (code1 < 0xF0) {
		/* 3-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80) return -1;
		if (code1 == 0xE0 && code2 < 0xA0) return -1;
		code3 = *in++;
		if ((code3 & 0xC0) != 0x80) return -1;
		*out = (code1 << 12) + (code2 << 6) + code3 - 0xE2080;
		return 3;
	}
	else if (code1 < 0xF5) {
		/* 4-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80) return -1;
		if (code1 == 0xF0 && code2 < 0x90) return -1;
		if (code1 == 0xF4 && code2 >= 0x90) return -1;
		code3 = *in++;
		if ((code3 & 0xC0) != 0x80) return -1;
		code4 = *in++;
		if ((code4 & 0xC0) != 0x80) return -1;
		*out = (code1 << 18) + (code2 << 12) + (code3 << 6) + code4 - 0x3C82080;
		return 4;
	}
	return -1;
}

float RZFT_GetTextWidth(const RZFT_Font* font, float scale, const char* text, char stop) {
	float w = 0, mw = 0;
	while(*text) {
		uint32_t c;
		int units = RZFT_DecodeUTF8(&c, (const uint8_t*)text);
		if (units == -1 || c == stop) break;
		text += units;
		if (c == '\n') {
			if (w > mw) mw = w;
			w = 0;
		} else {
			w += (font->hdr.spacing + font->pages[c >> 8].chr[c & 0xFF].advance) * scale;
		}
	}
	return ((w > mw) ? w : mw);
}

static void RZFT_WordWrap(const RZFT_Font* font, float scale, int wrap, char* text) {
	if (wrap <= 0) return;
	int len = 0, lastlen = 0;
	char* wpos = text;
	while(*text) {
		uint32_t c;
		int units = RZFT_DecodeUTF8(&c, (const uint8_t*)text);
		if (units == -1) break;
		if (c == ' ') {
			wpos = text;
			lastlen = len;
		} else if (c == '\n') {
			len = 0;
			text += units;
			continue;
		}
		text += units;
		rzftChar chr = font->pages[c >> 8].chr[c & 0xFF];
		len += (font->hdr.spacing + chr.advance) * scale;
		if (len > wrap) {
			*wpos = '\n';
			len -= lastlen;
		}
	}
}

static void RZFT_DrawTextInternal(const RZFT_Font* font, float x, float y, float scaleX, float scaleY, uint32_t color, uint8_t align, const char* text) {
	int xx = x - (RZFT_GetTextWidth(font, scaleX, text, '\n') * (align / 2.0));
	int yy = y;
	while(*text) {
		uint32_t c;
		int units = RZFT_DecodeUTF8(&c, (const uint8_t*)text);
		if (units == -1) break;
		text += units;
		if (c == '\n') {
			yy += font->hdr.lineHeight * scaleY;
			xx = x - (RZFT_GetTextWidth(font, scaleX, text, '\n') * (align / 2.0));
			continue;
		}
		rzftChar chr = font->pages[c >> 8].chr[c & 0xFF];
		Renderer::drawTextureExt(font->hdr.sheet, chr.glyph, xx+chr.xoffs*scaleX, yy+chr.yoffs*scaleY, 0, 0, scaleX, scaleY, 0, color);
		xx += (font->hdr.spacing + chr.advance) * scaleX;
	}
}

void RZFT_DrawText(const RZFT_Font* font, float x, float y, float scaleX, float scaleY, uint32_t color, int wrapX, uint8_t align, const char* text) {
	static char buffer[256] = "";
	if (wrapX > 0) {
		strncpy(buffer, text, sizeof(buffer) - 1);
		RZFT_WordWrap(font, scaleX, wrapX, buffer);
		RZFT_DrawTextInternal(font, x, y, scaleX, scaleY, color, align, buffer);
	} else {
		RZFT_DrawTextInternal(font, x, y, scaleX, scaleY, color, align, text);
	}
}

void RZFT_DrawTextFormat(const RZFT_Font* font, float x, float y, float scaleX, float scaleY, uint32_t color, int wrapX, uint8_t align, const char* text, ...) {
	static char buffer[256] = "";
	va_list args;
	va_start(args, text);
	vsnprintf(buffer, sizeof(buffer) - 1, text, args);
	if (wrapX > 0) RZFT_WordWrap(font, scaleX, wrapX, buffer);
	RZFT_DrawTextInternal(font, x, y, scaleX, scaleY, color, align, buffer);
	va_end(args);
}
