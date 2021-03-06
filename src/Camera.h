#ifndef CAMERA_H
#define CAMERA_H

#include "Geometry.h"

class Camera {
  private:
	Rectangle dim;

	inline void updateView() { dim.right = dim.left + width; dim.bot = dim.top + height; }

  public:
	Rectangle lim;
	float width, height;
	float xspd, yspd;
	float xorig, yorig;
	bool locked;
	bool scrolling;

	Camera();
	~Camera();

	inline void setOrigin(float x, float y) { xorig = x; yorig = y; }
	inline void setSize(float w, float h) { width = w; height = h; updateView(); }
	inline void setPosition(float x, float y) { dim.left = x; dim.top = y; updateView(); }
	inline void translate(float x, float y) { dim.left += x; dim.top += y; updateView(); }
	inline float getX() const { return dim.left; }
	inline float getY() const { return dim.top; }

	inline void setSpeed(float x, float y) { xspd = x; yspd = y; }

	inline void setLimit(const Rectangle& r) { lim = r; }
	inline const Rectangle& getLimit() const { return lim; }
	inline const Rectangle& getViewport() const { return dim; }

	void update(float px, float py);
};

#endif /* CAMERA_H */
