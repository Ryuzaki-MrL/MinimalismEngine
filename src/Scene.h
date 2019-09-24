#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <functional>

#include "Camera.h"
#include "ObjectPool.h"
#include "Tilemap.h"

typedef std::function<void(GameEntity&)> EntityFn;

class Scene {
  public:
	ObjectPool pool;
	TileMap tilemap;
	Camera camera;
	uint32_t width;
	uint32_t height;
	uint32_t flags;

	Scene();
	~Scene();

	GameEntity* instanceCreate(float x, float y, uint16_t id);
	void instanceDestroy(GameEntity*);
	inline size_t instanceCount() const { return pool.getCount(); }
	inline size_t countActive() const { return pool.getActiveList().size(); }
	inline size_t instanceCount(uint16_t type) const { return GameEntity::getCount(type); }
	size_t countActive(uint16_t type) const;
	void instanceActiveRegion(const Rectangle&);

	bool checkTile(const Rectangle& bbox, uint16_t flags) const;
	bool checkSolid(const Rectangle& bbox, uint16_t except) const;
	const GameEntity* checkObject(const Rectangle& bbox, uint16_t except, uint8_t flags = 0xFF) const;
	void collisionHandle(const Rectangle& bbox, uint16_t except, EntityFn colfn);
	void foreach(EntityFn, int group = -1);

	void update();
	void draw();
};

#endif /* SCENE_H */
