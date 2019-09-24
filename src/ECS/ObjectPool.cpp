#include <algorithm>

#include "ObjectPool.h"

ObjectPool::ObjectPool(size_t n): obj(new ClassBuffer<GameEntity>[n]), factory(nullptr), size(n), count(0) {
	activelist.reserve(n);
}
ObjectPool::~ObjectPool() {
	for (size_t i = 0; i < count; ++i) {
		obj[i].get()->~GameEntity();
	}
	delete obj;
}

GameEntity* ObjectPool::create(uint16_t id, Scene& scene) {
	if (count < size && factory && factory(obj[count], id, scene)) {
		activelist.push_back(count);
		return obj[count++];
	}
	return nullptr;
}

void ObjectPool::destroy(size_t pos) {
	obj[pos].get()->~GameEntity();
	std::swap(obj[pos], obj[--count]);
}

void ObjectPool::garbageCollect() {
	for (size_t i = 0; i < activelist.size();) {
		if (obj[activelist[i]].get()->isDead()) {
			std::swap(activelist[i], activelist.back());
			activelist.pop_back();
		} else {
			++i;
		}
	}
}

void ObjectPool::clear() {
	count = 0;
	clearActiveList();
}

void ObjectPool::buildActiveList(EntityActiveFn isActive) {
	clearActiveList();
	for (size_t i = 0; i < count;) {
		GameEntity& inst = *obj[i];
		if (inst.isDead()) {
			destroy(i);
		} else {
			if (isActive(inst)) {
				inst.onLoad();
				activelist.push_back(i);
			}
			++i;
		}
	}
}
