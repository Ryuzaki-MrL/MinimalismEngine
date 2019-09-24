#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

// TODO: separate SoundQueue and threaded implementation for each platform

struct QueuedSound {
	uint16_t id;
	int channel;
	int loops;
};

namespace Audio {
	bool init();
	void fini();

	void playSound(uint16_t id, int channel, int loops);
	void queueSound(uint16_t id, int channel, int loops);
	void stopSound(int channel);

	void processQueue();

	void playMusic(uint16_t id, int loops);
	void pauseMusic();
	void resumeMusic();
	void stopMusic();
	bool isMusicPlaying();
	bool isMusicPaused();

	/* Resources */
	void soundAdd(const char* name, uint16_t id);
	void soundUnload(uint16_t id);
	void musicAdd(const char* name, uint16_t id);
	void musicUnload(uint16_t id);
};

#endif /* AUDIO_H */
