#include <stdio.h>
#include <stdlib.h>

#include "Scene.h"
#include "Graphics.h"

#ifndef OBJECT_CAPACITY
#define OBJECT_CAPACITY 1000
#endif

Scene::Scene(): pool(OBJECT_CAPACITY), flags(0) {}
Scene::~Scene() {}

GameEntity* Scene::instanceCreate(float x, float y, uint16_t id) {
	GameEntity* obj = pool.create(id, *this);
	if (obj) obj->resetPosition(x, y);
	return obj;
}

void Scene::instanceDestroy(GameEntity* obj) {
	if (obj) obj->kill();
	pool.garbageCollect();
}

size_t Scene::countActive(uint16_t type) const {
	size_t cnt = 0;
	for (size_t i : pool.getActiveList()) {
		cnt += (pool[i]->getType() == type);
	}
	return cnt;
}

void Scene::instanceActiveRegion(const Rectangle& region) {
	pool.buildActiveList([region](GameEntity& ent) -> bool {
		return ent.collideWith(region);
	});
}

bool Scene::checkTile(const Rectangle& bbox, uint16_t flags, bool flmode) const {
	return false;
	// TODO
}

bool Scene::checkCollision(const Rectangle& bbox, uint16_t flags, bool flmode) const {
	// TODO: optimize static collision checking
	for (const SceneCollision& col : colmap) {
		if ((
				(flmode && BIT_TEST(col.flags, flags)) ||
				(!flmode && BIT_ANY(col.flags, flags))
			)
			&& col.bbox.collideWith(bbox)
		) {
			return true;
		}
	}
	return false;
}

GameEntity* Scene::checkObject(const Rectangle& bbox, uint16_t except, EntityFinder entfn) {
	for (size_t i : pool.getActiveList()) {
		GameEntity* ent = pool[i];
		if (ent->getUID()!=except && ent->collideWith(bbox) && entfn(*ent)) {
			return ent;
		}
	}
	return nullptr;
}

bool Scene::checkPlace(const Rectangle& bbox, uint16_t except, uint16_t ef, uint16_t cf) {
	return (
		nullptr != checkObject(bbox, except, [ef](const GameEntity& ent) {
			return ent.hasFlag(ef);
		})
		|| checkCollision(bbox, cf, FLAG_ANY)
	);
}

void Scene::onCollision(const Rectangle& bbox, uint16_t except, EntityFn colfn) {
	for (size_t i : pool.getActiveList()) {
		if (pool[i]->getUID()!=except && pool[i]->collideWith(bbox)) {
			colfn(*pool[i]);
		}
	}
}

/// Checks ALL entities, not only active
GameEntity* Scene::find(EntityFinder entfn) {
	for (size_t i = 0; i < pool.getCount(); ++i) {
		GameEntity* ent = pool[i];
		if (!ent->isDead() && entfn(*ent)) {
			return ent;
		}
	}
	return nullptr;
}

void Scene::foreach(EntityFn entfn, int group) {
	for (size_t i : pool.getActiveList()) {
		GameEntity& ent = *pool[i];
		if (group < 0 || ent.getGroup() == group) {
			entfn(ent);
		}
	}
}

void Scene::setTrigger(uint16_t id, TriggerFn fn) {
	triggers[id] = fn;
}

void Scene::trigger(uint16_t id) {
	if (triggers.count(id)) triggers[id](flags);
}

void Scene::update() {
	for (size_t i : pool.getActiveList()) {
		pool[i]->update();
	}
	pool.garbageCollect();
}

void Scene::draw() {
	for (size_t i : pool.getActiveList()) {
		pool[i]->draw();
	}
}
