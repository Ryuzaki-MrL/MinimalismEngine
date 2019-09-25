#include "GameEntity.h"
#include "Scene.h"
#include "Graphics.h"

#define GROUP_COUNT 256

template<int N> struct _{ operator char() { return N + 256; } };

static uint16_t s_nextuid = 0;
static uint16_t s_entcount[NUM_ENTITIES] = { 0 };
static uint16_t s_entgp[GROUP_COUNT] = { 0 };

GameEntity::GameEntity(Scene& scene, uint16_t type):
  scene(scene), data(entityGetData(type)), uid(++s_nextuid), type(type), group(0), flags(data.flags) {
	char(_<sizeof(GameEntity)>()); // report size as a compiler warning
	++s_entcount[type];
	++s_entgp[group];
	spr.id = data.sprite;
	comps.init(data);
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

bool GameEntity::moveCheck(float x, float y) {
	if (!scene.checkSolid(getBoundingBox(this->x + x, this->y + y), uid)) {
		translate(x, y);
		onMove();
		return true;
	}
	return false;
}

bool GameEntity::applyVector(bool checksolid) {
	if (checksolid)
		return moveCheck(v.getX(), v.getY());
	translate(v.getX(), v.getY());
	onMove();
	return true;
}

void GameEntity::pauseTimers() {
	for (uint8_t i = 0; i < ENTITY_TIMERS; ++i) {
		timers[i].pause();
	}
}

void GameEntity::resetTimers() {
	for (uint8_t i = 0; i < ENTITY_TIMERS; ++i) {
		timers[i].reset();
	}
}

void GameEntity::update() {
	for (uint8_t i = 0; i < ENTITY_TIMERS; ++i) {
		if (timers[i].complete()) onTimeOut(i);
	}
	comps.update();
	onUpdate();
}

void GameEntity::draw() {
	if (checkFlag(EFLAG_VISIBLE)) {
		Renderer::drawSprite(spr, x, y);
	}
	comps.draw();
	onDraw();
}
