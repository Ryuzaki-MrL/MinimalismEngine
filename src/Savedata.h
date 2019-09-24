#ifndef SAVEDATA_H
#define SAVEDATA_H

/* Game specific */
#include "SaveTypes.h"
#include "WorldData.h"

#define MAX_PROFILES	1

class SaveData {
  private:
	/* Game specific */
	SaveProfile profiles[MAX_PROFILES];
	SaveSystem sys;

  public:
	SaveData(const char* filename);
	~SaveData();

	inline SaveProfile& getProfile(uint16_t p) { return profiles[p]; }
	inline SaveSystem& getSystemData() { return sys; }

	bool commit(const char* filename) const;
	void clear();
};

SaveData& getGlobalSavedata();
bool commitGlobalSavedata();

const SaveProfile& getCurrentSaveProfile();
void selectSaveProfile(uint16_t slot); // discards current WorldData

WorldData& getCurrentWorldData();
bool commitCurrentWorldData();

void getCurrentWorldData(WorldData& out);
bool commitCurrentWorldData(const WorldData& world);

#endif /* SAVEDATA_H */
