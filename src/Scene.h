#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <functional>

#include "Camera.h"
#include "ObjectPool.h"
#include "Tilemap.h"

#define FLAG_ANY false
#define FLAG_ALL true

/// Generic bbox collision
struct SceneCollision {
	Rectangle bbox;
	uint16_t flags;
};

typedef std::function<void(GameEntity&)> EntityFn;
typedef std::function<bool(const GameEntity&)> EntityFinder;
typedef std::vector<SceneCollision> CollisionMap;

class Scene {
  public:
	ObjectPool pool;
	TileMap tilemap;
	CollisionMap colmap;
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

	bool checkTile(const Rectangle&, uint16_t flags, bool flmode = FLAG_ALL) const;
	bool checkCollision(const Rectangle&, uint16_t flags, bool flmode = FLAG_ALL) const;
	GameEntity* checkObject(const Rectangle&, uint16_t except, EntityFinder);
	bool checkPlace(const Rectangle&, uint16_t except, uint16_t ef, uint16_t cf);
	void onCollision(const Rectangle&, uint16_t except, EntityFn);

	GameEntity* find(EntityFinder);
	void foreach(EntityFn, int group = -1);

	void update();
	void draw();
};

#endif /* SCENE_H */
