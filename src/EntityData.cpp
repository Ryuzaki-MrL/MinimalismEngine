#include "EntityData.h"
#include "rzdb.h"

static EntityData* entities;

void entitiesLoad(const char* fname) {
	entities = (EntityData*)RZDB_ReadEntireFile(fname, sizeof(EntityData));
}

const EntityData& entityGetData(uint16_t ent) {
	return entities[ent];
}
