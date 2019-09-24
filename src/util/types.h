#ifndef TYPES_H
#define TYPES_H

#define BIT_SET(x, b) 	((x) |= (b))
#define BIT_TEST(x, b) 	(((x) & (b)) == (b))
#define BIT_CLEAR(x, b) ((x) &= ~(b))

union Color {
	uint32_t rgba;
	struct c {
		uint8_t r,g,b,a;
	} c;

	Color() {}
	Color(uint32_t rgba): rgba(rgba) {}
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { c = {r,g,b,a}; }
};

#endif /* TYPES_H */
