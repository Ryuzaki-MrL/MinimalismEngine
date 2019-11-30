#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <unordered_map>
#include <queue>

#include "Audio.h"

namespace Audio {

static std::unordered_map<uint16_t, Mix_Chunk*> s_chunks;
static std::unordered_map<uint16_t, Mix_Music*> s_music;

static std::queue<QueuedSound> s_sfxqueue;

bool init() {
	SDL_InitSubSystem(SDL_INIT_AUDIO);
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, 2, AUDIO_SAMPLES);
	return true;
}

void fini() {
	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void playSound(uint16_t id, int channel, int loops) {
	Mix_PlayChannel(channel, s_chunks[id], loops);
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
	Mix_HaltChannel(channel);
}

void playMusic(uint16_t id, int loops) {
	Mix_PlayMusic(s_music[id], loops);
}

void pauseMusic() {
	Mix_PauseMusic();
}

void resumeMusic() {
	Mix_ResumeMusic();
}

void stopMusic() {
	Mix_HaltMusic();
}

bool isMusicPlaying() {
	return Mix_PlayingMusic();
}

bool isMusicPaused() {
	return Mix_PausedMusic();
}

void soundAdd(const char* name, uint16_t id) {
	s_chunks[id] = Mix_LoadWAV(name);
}

void soundUnload(uint16_t id) {
	if (s_chunks.count(id) && s_chunks[id]) {
		Mix_FreeChunk(s_chunks[id]);
		s_chunks.erase(id);
	}
}

void musicAdd(const char* name, uint16_t id) {
	s_music[id] = Mix_LoadMUS(name);
}

void musicUnload(uint16_t id) {
	if (s_music.count(id) && s_music[id]) {
		Mix_FreeMusic(s_music[id]);
		s_music.erase(id);
	}
}

}; // namespace Audio
