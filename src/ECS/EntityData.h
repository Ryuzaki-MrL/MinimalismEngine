#ifndef ENTITYDATA_H
#define ENTITYDATA_H

#include <stdint.h>

struct EntityData {
	uint16_t comps;
	uint16_t props;
	uint16_t sprite;
	uint8_t flags;
};

void entitiesLoad(const char* fname);
const EntityData& entityGetData(uint16_t ent);

#endif /* ENTITYDATA_H */
