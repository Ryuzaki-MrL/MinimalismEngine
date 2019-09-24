#include "Camera.h"

Camera::Camera() {}
Camera::~Camera() {}

void Camera::update(float px, float py) {
	float limx = lim.right - width;
	float limy = lim.bot - height;
	if (xspd || yspd) {
		translate(xspd, yspd);
		if (dim.left >= lim.left && dim.left <= limx && dim.top >= lim.top && dim.top <= limy) {
			xspd = yspd = 0;
		}
	}
	else if (!locked) {
		float newx = clamp(px - xorig, lim.left, limx);
		float newy = clamp(py - yorig, lim.top,  limy);
		setPosition(newx, newy);
	}
}
