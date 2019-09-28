#ifndef SAVEDATA_H
#define SAVEDATA_H

/* Game specific */
#ifdef USE_SAVEDATA
#include "SaveTypes.h"
#endif
#include "WorldData.h"

#ifndef MAX_PROFILES
#define MAX_PROFILES	1
#endif

#ifdef USE_SAVEDATA
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
#endif

void selectSaveSlot(uint16_t slot); // discards current WorldData

WorldData& getCurrentWorldData();
bool commitCurrentWorldData();

void getCurrentWorldData(WorldData& out);
bool commitCurrentWorldData(const WorldData& world);

#endif /* SAVEDATA_H */
