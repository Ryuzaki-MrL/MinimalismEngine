#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>

#define PI 3.14159265

#define ROUND(x, y, b) ((((x) / (y)) * (y)) + b*y)

/// Helper functions
inline float degtorad(float deg) {
	return deg * PI / 180.0;
}

inline float radtodeg(float rad) {
	return rad * 180.0 / PI;
}

inline float dsin(float deg) {
	return sin(degtorad(deg));
}

inline float dcos(float deg) {
	return cos(degtorad(deg));
}

inline float pointDistance(float x1, float y1, float x2, float y2) {
	return ((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

inline float pointDirection(float x1, float y1, float x2, float y2) {
	return 180 - (atan2(y2-y1, x2-x1) * 180.0 / PI);
}

template <typename T>
inline int signum(const T& val) {
	return (0 < val) - (val < 0);
}

template <typename T>
inline T clamp(const T& val, const T& low, const T& hi) {
	return ((val < low) ? low : ((hi < val) ? hi : val));
}


/// Point
struct Point {
	float x, y;

	Point(float x, float y): x(x), y(y) {}
	~Point() {}

	inline void translate(float xx, float yy) {
		x += xx; y += yy;
	}

	inline void rotate(float angle) {
		float a = degtorad(angle);
		float nx = (x * cos(a) - y * sin(a));
		float ny = (y * cos(a) + x * sin(a));
		x = nx;
		y = ny;
	}
};


/// Rectangle
struct Rectangle {
	float left, right, top, bot;

	Rectangle() {}
	Rectangle(float l, float r, float t, float b): left(l), right(r), top(t), bot(b) {}
	~Rectangle() {}

	inline bool collideWith(const Rectangle& other) const {
		return !((other.left > right) || (other.right < left) || (other.top > bot) || (other.bot < top));
	}

	inline bool isInside(float x, float y) const {
		return (x > left && x < right && y > top && y < bot);
	}

	inline const Rectangle operator+(const Rectangle other) const {
		return Rectangle((other.left + left), (other.right + right), (other.top + top), (other.bot + bot));
	}

	inline const Rectangle operator*(const Rectangle other) const {
		return Rectangle((other.left * left), (other.right * right), (other.top * top), (other.bot * bot));
	}
};


/// 2D Vector
struct Vector2D {
	float mag, dir;

	Vector2D() {}
	Vector2D(float m, float d): mag(m), dir(d) {}
	~Vector2D() {}

	inline void rotate(float d) { dir += d; }
	inline void scale(float m) { mag *= m; }

	inline float getX() const { return mag * +dcos(dir); }
	inline float getY() const { return mag * -dsin(dir); }

	inline const Vector2D operator+(const Vector2D other) {
		float nx = getX() + other.getX();
		float ny = getY() + other.getY();
		float m = sqrt((nx*nx) + (ny*ny));
		float d = pointDirection(0.0, 0.0, nx, ny);
		return Vector2D(m, d);
	}
};

#endif /* GEOMETRY_H */
