#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "EntityData.h"
#include "Components.h"
#include "Properties.h"
#include "Geometry.h"
#include "Sprite.h"
#include "Timer.h"

#ifndef ENTITY_TIMERS
#define ENTITY_TIMERS	1
#endif

#define ANIM_FPS(x)		(1.0 / (x))

#define TR_SPRITE		0b01
#define TR_BBOX			0b10
#define TR_ALL			0b11

#define CHECK_SOLID		true

/// Game defined flags should start from bit 3
enum {
	EFLAG_VISIBLE	= (1 << 0),
	EFLAG_SOLID		= (1 << 1),
	EFLAG_NOUPDATE	= (1 << 2),
};

class Scene;
class GameEntity {
  public:
#ifdef USE_COMPONENTS
	Components comps;
#endif
	DeltaTimer timers[ENTITY_TIMERS];
	Sprite spr;
	Rectangle bbox;
	Vector2D v;
  protected:
	DeltaTimer animtimer;
	float x, y, xs, ys;
	Scene& scene;
	const EntityData& data;
	const uint16_t uid;
	const uint16_t type;
	uint16_t flags;
  private:
	uint8_t group;
	bool dead;

  private:
	virtual void onUpdate() {}
	virtual void onDraw() {}

	virtual void onKill() {}

	virtual void onMove() {}
	virtual void onInteract(GameEntity& /* other */) {}
	virtual void onTimeOut(uint8_t /* timer */) {}
	virtual void onAnimEnd() {}

  public:
	GameEntity(Scene& scene, uint16_t type);
	virtual ~GameEntity();

	inline uint16_t getType() const { return type; }
	inline uint16_t getUID() const { return uid; }
	inline uint8_t getGroup() const { return group; }
	void setGroup(uint8_t);

	inline float getX() const { return x; }
	inline float getY() const { return y; }
	inline void translate(float x, float y) { this->x += x; this->y += y; }
	inline void setPosition(float x, float y) { this->x = x; this->y = y; }
	inline void resetPosition(float x, float y) { this->x = xs = x; this->y = ys = y; }
	inline void resetPosition() { x = xs; y = ys; }
	bool moveCheck(float x, float y);
	bool applyVector(bool checksolid = false);

	inline const Rectangle getBoundingBox(float xx, float yy) const { return bbox + Rectangle(xx, xx, yy, yy); }
	inline const Rectangle getBoundingBox() const { return getBoundingBox(pos.x, pos.y); }

	virtual void onLoad() {}

	inline bool hasComponent(uint16_t comp) const { return data.comps & comp; }
	inline bool hasProperty(uint16_t prop) const { return data.props & prop; }

	inline void kill() { dead = true; onKill(); }
	inline bool isDead() const { return dead; }

	inline void setFlag(uint16_t mask) { BIT_SET(flags, mask); }
	inline void clearFlag(uint16_t mask) { BIT_CLEAR(flags, mask); }
	inline bool checkFlag(uint16_t mask) const { return BIT_TEST(flags, mask); }
	inline bool hasFlag(uint16_t mask) const { return BIT_ANY(flags, mask); }

	inline bool collideWith(const Rectangle& r) const { return r.collideWith(getBoundingBox()); }
	inline bool collideWith(const GameEntity& obj) const { return obj.collideWith(getBoundingBox()); }

	void update();
	void draw();

	static uint16_t getCount(uint16_t);
	static uint16_t getCountGroup(uint8_t);
};

#endif /* GAMEENTITY_H */
