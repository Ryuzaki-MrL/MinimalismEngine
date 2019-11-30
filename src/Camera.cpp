#include "Camera.h"

Camera::Camera(): xspd(0), yspd(0), locked(false), scrolling(false) {}
Camera::~Camera() {}

void Camera::update(float px, float py) {
	float limx = lim.right - width;
	float limy = lim.bot - height;
	if (abs(xspd) + abs(yspd) > 0.1) {
		translate(xspd, yspd);
		scrolling = true;
		if (dim.left >= lim.left && dim.left <= limx && dim.top >= lim.top && dim.top <= limy) {
			xspd = yspd = 0;
			scrolling = false;
		}
	}
	else if (!locked) {
		float newx = clamp(px - xorig, lim.left, limx);
		float newy = clamp(py - yorig, lim.top,  limy);
		setPosition(newx, newy);
	}
}
