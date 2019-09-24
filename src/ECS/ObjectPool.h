#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H

#include <vector>
#include <functional>

#include "ClassBuffer.h"
#include "GameEntity.h"

typedef std::function<bool(GameEntity&)> EntityActiveFn;
typedef bool(*EntityFactory)(GameEntity* out, uint16_t id, Scene& scene); // plain function pointer

class ObjectPool {
  private:
	ClassBuffer<GameEntity>* obj;
	std::vector<size_t> activelist;
	EntityFactory factory;
	size_t size, count;

  public:
	ObjectPool(size_t n);
	~ObjectPool();

	inline GameEntity* get(size_t pos) { return (pos < count) ? obj[pos].get() : nullptr; }
	inline const GameEntity* get(size_t pos) const { return (pos < count) ? (obj[pos].get()) : nullptr; }
	inline GameEntity* operator[](size_t pos) { return obj[pos]; }
	inline const GameEntity* operator[](size_t pos) const { return obj[pos]; }
	inline size_t getCount() const { return count; }
	inline size_t getSize() const { return size; }

	inline void setFactory(EntityFactory f) { factory = f; }
	GameEntity* create(uint16_t id, Scene& scene);
	void destroy(size_t pos);
	void garbageCollect();
	void clear();

	void buildActiveList(EntityActiveFn);
	inline void clearActiveList() { activelist.clear(); }
	inline const std::vector<size_t>& getActiveList() const { return activelist; }
};

#endif /* OBJECTPOOL_H */
