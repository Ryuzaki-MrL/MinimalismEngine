/// Custom database format (RyuZaki DataBase)

#include <stdint.h>

#include "rzdb.h"
#include "mem.h"

#pragma pack(1)

struct RZDB_File {
	FILE* handle;
};

struct RZDB_Chunk {
	char* data;
	uint16_t cnt;
	size_t chkidx[];
};

RZDB_File* RZDB_OpenFile(const char* fname) {
	FILE* handle = fopen(fname, "rb");
	if (!handle) return nullptr;

	uint32_t magic = 0;
	fread(&magic, 1, sizeof(magic), handle);

	RZDB_File* file = (RZDB_File*)memalloc(sizeof(RZDB_File));
	if (!file) return nullptr;

	file->handle = handle;

	return file;
}

void RZDB_CloseFile(RZDB_File* file) {
	if (!file) return;
	fclose(file->handle);
	memfree(file);
}

size_t RZDB_ReadChunk(RZDB_File* file, char* out, size_t chksize, size_t cnt) {
	return fread(out, chksize, cnt, file->handle);
}

size_t RZDB_ReadString(RZDB_File* file, char* out) {
	return RZDB_ReadChunk(file, out, sizeof(char), RZDB_ReadSize(file));
}

size_t RZDB_ReadSize(RZDB_File* file) {
	size_t sz = 0;
	size_t shift = 0;
	u8 tmp = 0;
	do {
		fread(&tmp, 1, sizeof(tmp), file->handle);
		sz |= (tmp & 0x7F) << shift;
		shift += 7;
	} while(tmp > 127);
	return sz;
}

RZDB_Chunk* RZDB_ReadChunkIndexed(RZDB_File* file, size_t chksize, size_t cnt) {
	int pos = ftell(file->handle);
	RZDB_Chunk* chk = (RZDB_Chunk*)memalloc(sizeof(RZDB_Chunk) + sizeof(size_t) * cnt);
	if (!chk) return nullptr;
	chk->cnt = cnt;
	chk->chkidx[0] = 0;
	for (uint16_t i = 1; i <= cnt; ++i) {
		chk->chkidx[i] = RZDB_ReadSize(file) * chksize;
		fseek(file->handle, chk->chkidx[i], SEEK_CUR);
		chk->chkidx[i] += chk->chkidx[i-1] + 1;
	}
	fseek(file->handle, pos, SEEK_SET);
	chk->data = (char*)memalloc(chk->chkidx[cnt]);
	if (!chk->data) {
		memfree(chk);
		return nullptr;
	}
	char* out = chk->data;
	for (uint16_t i = 0; i < cnt; ++i) {
		out += RZDB_ReadChunk(file, out, chksize, RZDB_ReadSize(file));
		*out = 0; ++out;
	}
	return chk;
}

const char* RZDB_ChunkGetDataAt(RZDB_Chunk* chunk, size_t idx) {
	return chunk->data + chunk->chkidx[idx];
}

size_t RZDB_ChunkGetCount(RZDB_Chunk* chunk) {
	return chunk->cnt;
}

void RZDB_FreeChunk(RZDB_Chunk* chunk) {
	if (!chunk) return;
	memfree(chunk->data);
	memfree(chunk);
}

char* RZDB_ReadEntireFile(const char* fname, size_t datasize) {
	RZDB_File* rf = RZDB_OpenFile(fname);
	if (!rf) return nullptr;

	size_t cnt = RZDB_ReadSize(rf);
	char* data = new char[cnt * datasize];
	if (!data) {
		RZDB_CloseFile(rf);
		return nullptr;
	}

	RZDB_ReadChunk(rf, data, datasize, cnt);
	RZDB_CloseFile(rf);

	return data;
}
