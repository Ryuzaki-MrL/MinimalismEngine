#include <stdio.h>
#include <string.h>

#include "Savedata.h"

#ifndef SAVEDATA_PATH
#define SAVEDATA_PATH   "savedata.bin"
#endif
#ifndef PROFILE_PATH
#define PROFILE_PATH	"profiles.bin"
#endif

#ifdef USE_SAVEDATA
static SaveData g_save(PROFILE_PATH);
#endif
static WorldData g_wdata[MAX_PROFILES];
static WorldData g_current_wdata;
static bool g_saveinit = false;
static uint16_t g_slot = 0;

#ifdef USE_SAVEDATA
SaveData::SaveData(const char* filename): profiles(), sys() {
	FILE* file = fopen(filename, "rb");
	if (!file) return;
	fread(this, 1, sizeof(SaveData), file);
	fclose(file);
}
SaveData::~SaveData() {}

bool SaveData::commit(const char* filename) const {
	FILE* file = fopen(filename, "wb");
	if (!file) return false;
	fwrite(this, 1, sizeof(SaveData), file);
	fclose(file);
	return true;
}

void SaveData::clear() {
	memset(this, 0, sizeof(SaveData));
}

static void loadGlobalWorldData(const char* filename) {
	FILE* file = fopen(filename, "rb");
	if (!file) return;
	fread(g_wdata, 1, sizeof(g_wdata), file);
	fclose(file);
}

static bool saveGlobalWorldData(const char* filename) {
	FILE* file = fopen(filename, "wb");
	if (!file) return false;
	fwrite(g_wdata, 1, sizeof(g_wdata), file);
	fclose(file);
	return true;
}

SaveData& getGlobalSavedata() {
	return g_save;
}

bool commitGlobalSavedata() {
	return g_save.commit(PROFILE_PATH);
}

const SaveProfile& getCurrentSaveProfile() {
	return g_save.getProfile(g_slot);
}
#endif

void selectSaveSlot(uint16_t slot) {
	if (!g_saveinit) {
		loadGlobalWorldData(SAVEDATA_PATH);
		g_saveinit = true;
	}
	g_slot = slot % MAX_PROFILES;
	g_current_wdata = g_wdata[g_slot];
}

WorldData& getCurrentWorldData() {
	return g_current_wdata;
}

bool commitCurrentWorldData() {
	g_wdata[g_slot] = g_current_wdata;
	return saveGlobalWorldData(SAVEDATA_PATH);
}

void getCurrentWorldData(WorldData& out) {
	out = g_wdata[g_slot];
}

bool commitCurrentWorldData(const WorldData& world) {
	g_wdata[g_slot] = world;
	return saveGlobalWorldData(SAVEDATA_PATH);
}
