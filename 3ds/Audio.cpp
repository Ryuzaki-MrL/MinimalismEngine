#include <3ds.h>
#include <unordered_map>
#include <queue>

#include "Audio.h"

namespace Audio {

//static std::unordered_map<uint16_t, ndspWaveBuf*> s_chunks;
//static std::unordered_map<uint16_t, ndspWaveBuf*> s_music;

static std::queue<QueuedSound> s_sfxqueue;

bool init() {
	return true;
}

void fini() {
	
}

void playSound(uint16_t id, int channel, int loops) {
	
}

void queueSound(uint16_t id, int channel, int loops) {
	s_sfxqueue.push({ id, channel, loops });
}

void processQueue() {
	if (!s_sfxqueue.empty()) {
		QueuedSound qs = s_sfxqueue.front(); s_sfxqueue.pop();
		playSound(qs.id, qs.channel, qs.loops);
	}
}

void stopSound(int channel) {
	
}

void playMusic(uint16_t id, int loops) {
	
}

void pauseMusic() {
	
}

void resumeMusic() {
	
}

void stopMusic() {
	
}

bool isMusicPlaying() {
	
}

bool isMusicPaused() {
	
}

void soundAdd(const char* name, uint16_t id) {
	
}

void soundUnload(uint16_t id) {
	
}

void musicAdd(const char* name, uint16_t id) {
	
}

void musicUnload(uint16_t id) {
	
}

}; // namespace Audio
