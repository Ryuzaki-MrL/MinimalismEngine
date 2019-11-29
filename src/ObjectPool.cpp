#include <algorithm>

#include "ObjectPool.h"

ObjectPool::ObjectPool(size_t n): obj(new ClassBuffer<GameEntity>[n]), factory(nullptr), size(n), count(0) {
	activelist.reserve(n);
}
ObjectPool::~ObjectPool() {
	clear();
	delete obj;
}

GameEntity* ObjectPool::create(uint16_t id, Scene& scene) {
	if (count < size && factory && factory(obj[count], id, scene)) {
		activelist.push_back(count);
		return obj[count++];
	}
	if (count == size && factory && !deadqueue.empty()) {
		size_t idx = deadqueue.front(); deadqueue.pop_front();
		if (factory(obj[idx], id, scene)) return obj[idx];
	}
	return nullptr;
}

void ObjectPool::destroy(size_t pos) {
	obj[pos].get()->~GameEntity();
	std::swap(obj[pos], obj[--count]);
}

void ObjectPool::garbageCollect() {
	for (size_t i = 0; i < activelist.size();) {
		size_t idx = activelist[i];
		if (obj[idx].get()->isDead()) {
			obj[idx].get()->~GameEntity();
			deadqueue.push_back(idx);
			std::swap(activelist[i], activelist.back());
			activelist.pop_back();
		} else {
			++i;
		}
	}
}

void ObjectPool::clear() {
	clearActiveList();
	for (size_t i = 0; i < count; ++i) {
		if (!obj[i].get()->isDead()) {
			obj[i].get()->~GameEntity();
		}
	}
	count = 0;
}

void ObjectPool::clearActiveList() {
	garbageCollect();
	activelist.clear();
	deadqueue.clear();
}

void ObjectPool::buildActiveList(EntityActiveFn isActive) {
	clearActiveList();
	for (size_t i = 0; i < count;) {
		GameEntity& inst = *obj[i];
		if (inst.isDead()) {
			std::swap(obj[i], obj[--count]);
			//destroy(i);
		} else {
			if (isActive(inst)) {
				inst.onLoad();
				activelist.push_back(i);
			}
			++i;
		}
	}
}
