#ifndef ENTITIES_H
#define ENTITIES_H

#include "GameEntity.h"

// template for creating entities
#define NEWENTITY(NAME, ...) \
class NAME final: public GameEntity { \
  private:\
	void __VA_ARGS__; \
  public:\
	NAME(Scene&, uint16_t); \
	~NAME(); \
};

#endif /* ENTITIES_H */
