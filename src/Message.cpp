#include <unordered_map>

#include "Message.h"
#include "rzdb.h"

typedef std::unordered_map<std::string, uint16_t> MsgDict;

static uint16_t g_lang = 0;
static RZDB_Chunk* langs;
static RZDB_Chunk* msgentries;
static RZDB_Chunk* messages;
static MsgDict langmap;
static MsgDict* msgmap;

bool messagesLoad(const char* filename) {
	RZDB_File* rf = RZDB_OpenFile(filename);
	if (!rf) return false;

	uint16_t langcnt = RZDB_ReadSize(rf);
	langs = RZDB_ReadChunkIndexed(rf, sizeof(char), langcnt);

	uint16_t entrycnt = RZDB_ReadSize(rf);
	msgentries = RZDB_ReadChunkIndexed(rf, sizeof(char), entrycnt);
	messages = RZDB_ReadChunkIndexed(rf, sizeof(char), entrycnt * langcnt);

	msgmap = new MsgDict[langcnt];
	for (uint16_t i = 0; i < langcnt; ++i) {
		langmap[std::string(RZDB_ChunkGetDataAt(langs, i))] = i;
		for (uint16_t j = 0; j < entrycnt; ++j) {
			msgmap[i][std::string(RZDB_ChunkGetDataAt(msgentries, j))] = i*entrycnt + j;
		}
	}

	RZDB_CloseFile(rf);
	return true;
}

void messageSetLang(uint16_t id) {
	g_lang = id % RZDB_ChunkGetCount(langs);
}

void messageSetLang(const std::string& id) {
	g_lang = langmap[id];
}

uint16_t messageGetLang() {
	return g_lang;
}

const char* messageGetLangStr() {
	return RZDB_ChunkGetDataAt(langs, g_lang);
}

const char* messageGet(uint16_t id) {
	return RZDB_ChunkGetDataAt(messages, g_lang * RZDB_ChunkGetCount(msgentries) + id);
}

const char* messageGet(const std::string& id) {
	const char* msg = RZDB_ChunkGetDataAt(messages, msgmap[g_lang][id]);
	return msg ? msg : "NULL";
}

uint16_t messageGetIndex(const std::string& id) {
	return msgmap[0][id];
}
