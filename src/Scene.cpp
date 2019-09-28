#include <stdio.h>
#include <stdlib.h>

#include "Scene.h"
#include "Graphics.h"

#define OBJECT_CAPACITY 1000

Scene::Scene(): pool(OBJECT_CAPACITY), flags(0) {}
Scene::~Scene() {}

GameEntity* Scene::instanceCreate(float x, float y, uint16_t id) {
	GameEntity* obj = pool.create(id, *this);
	if (obj) obj->setPosition(x, y);
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
		return region.isInside(ent.getBoundingBox()) || region.collideWith(ent.getBoundingBox());
	});
}

bool Scene::checkTile(const Rectangle& bbox, uint16_t flags) const {
	// TODO
}

bool Scene::checkCollision(const Rectangle& bbox, uint16_t flags) const {
	for (const SceneCollision& col : colmap) {
		if (BIT_TEST(col.flags, flags) && col.bbox.collideWith(bbox)) {
			return true;
		}
	}
	return false;
}

const GameEntity* Scene::checkObject(const Rectangle& bbox, uint16_t except, uint8_t flags) const {
	for (size_t i : pool.getActiveList()) {
		if (pool[i]->getUID()!=except && pool[i]->collideWith(bbox) && pool[i]->checkFlag(flags)) {
			return pool[i];
		}
	}
	return nullptr;
}

void Scene::collisionHandle(const Rectangle& bbox, uint16_t except, EntityFn colfn) {
	for (size_t i : pool.getActiveList()) {
		if (pool[i]->getUID()!=except && pool[i]->collideWith(bbox)) {
			colfn(*pool[i]);
		}
	}
}

void Scene::foreach(EntityFn entfn, int group) {
	for (size_t i : pool.getActiveList()) {
		GameEntity& ent = *pool[i];
		if (group < 0 || ent.getGroup() == group) {
			entfn(ent);
		}
	}
}

void Scene::update() {
	for (size_t i : pool.getActiveList()) {
		pool[i]->update();
	}
	pool.garbageCollect();
}

void Scene::draw() {
	Renderer::targetClear(C_WHITE);
	Renderer::targetTranslate(camera.getX(), camera.getY());

	Renderer::drawTileMap(tilemap, camera.getViewport());

	for (size_t i : pool.getActiveList()) {
		pool[i]->draw();
	}

	Renderer::targetTranslate(-camera.getX(), -camera.getY());
}
