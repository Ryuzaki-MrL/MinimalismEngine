#include "GameEntity.h"
#include "Scene.h"
#include "Graphics.h"

#define GROUP_COUNT 256

#ifndef NUM_ENTITIES
#define NUM_ENTITIES 256
#endif

template<int N> struct _{ operator char() { return N + 256; } };

static uint16_t s_nextuid = 0;
static uint16_t s_entcount[NUM_ENTITIES] = { 0 };
static uint16_t s_entgp[GROUP_COUNT] = { 0 };

GameEntity::GameEntity(Scene& scene, uint16_t type):
  scene(scene), data(entityGetData(type)), uid(++s_nextuid), type(type), group(0), dead(false) {
	char(_<sizeof(GameEntity)>()); // report size as a compiler warning
	++s_entcount[type];
	++s_entgp[group];
	spr = Sprite(data.sprite);
	bbox = spr.calcBoundingBox();
	v = Vector2D(0, 0);
	flags = data.flags;
#ifdef USE_COMPONENTS
	comps.init();
#endif
}
GameEntity::~GameEntity() {
	--s_entcount[type];
	--s_entgp[group];
}

uint16_t GameEntity::getCount(uint16_t type) {
	return s_entcount[type];
}

uint16_t GameEntity::getCountGroup(uint8_t group) {
	return s_entgp[group];
}

void GameEntity::setGroup(uint8_t gp) {
	--s_entgp[group];
	group = gp;
	++s_entgp[group];
}

void GameEntity::setScale(float xx, float yy, uint8_t flags) {
	spr.xscale = xx;
	spr.yscale = yy;
	if (BIT_TEST(flags, TR_BBOX)) {
		bbox = spr.calcBoundingBox();
	}
}

void GameEntity::setAngle(float deg, uint8_t flags) {
	spr.rotation = deg;
	if (BIT_TEST(flags, TR_BBOX)) {
		bbox = spr.calcBoundingBox();
	}
}

bool GameEntity::moveCheck(float x, float y, uint16_t ef, uint16_t cf) {
	const Rectangle tmpbox = getBoundingBox(this->x + x, this->y + y);
	if (!scene.checkPlace(tmpbox, uid, ef, cf)) {
		translate(x, y);
		return true;
	}
	return false;
}

bool GameEntity::applyVector(bool checksolid, uint16_t ef, uint16_t cf) {
	if (checksolid)
		return moveCheck(v.getX(), v.getY(), ef, cf);
	translate(v.getX(), v.getY());
	return true;
}

void GameEntity::update() {
	if (checkFlag(EFLAG_NOUPDATE)) return;

	for (uint8_t i = 0; i < ENTITY_TIMERS; ++i) {
		if (timers[i].update(g_deltatime) && timers[i].complete()) {
			timers[i].stop();
			onTimeOut(i);
		}
	}

	if (animtimer.update(g_deltatime) && animtimer.complete()) {
		animtimer.start(spr.speed);
		++spr.frame;
		if (spr.frame >= spr.data().imgcount) {
			spr.frame = 0;
			onAnimEnd();
		}
	}
	if (spr.speed > 0.0001) {
		if (!animtimer.isRunning()) animtimer.start(spr.speed);
	}
	else {
		animtimer.stop();
	}

#ifdef USE_COMPONENTS
	comps.update(*this);
#endif

	onUpdate();
}

void GameEntity::draw() {
#ifdef USE_COMPONENTS
	comps.draw(*this); // pre-draw
#endif
	if (checkFlag(EFLAG_VISIBLE)) {
		Renderer::drawSprite(spr, x, y);
	}
	onDraw();
}
