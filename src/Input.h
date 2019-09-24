#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define MAX_KEYBINDS	8

// TODO: mouse, keyboard, multiple controllers and touch points
// TODO: key, touch and axis int types per platform (int16, uint32, etc)
// TODO: bind multiple keys to the same slot

namespace Input {
	bool init();
	void fini();

	void poll();

	bool isKeyDown(uint32_t key);
	bool isKeyBindDown(uint16_t index);
	bool isKeyHeld(uint32_t key);
	bool isKeyBindHeld(uint16_t index);
	bool isKeyUp(uint32_t key);
	bool isKeyBindUp(uint16_t index);

	void keyBind(uint16_t index, uint32_t key);
	void keyUnbind(uint16_t index);

	uint16_t getTouchX();
	uint16_t getTouchY();

	int16_t getStickAxisX(uint8_t stick);
	int16_t getStickAxisY(uint8_t stick);

	int16_t getAccelX();
	int16_t getAccelY();
	int16_t getAccelZ();
	void setAccelState(bool state);

	int16_t getGyroX();
	int16_t getGyroY();
	int16_t getGyroZ();
	void setGyroState(bool state);

	bool getString(char* out, const char* htext, const char* def, int maxlength, bool password = false);
	bool getInteger(int& out, const char* htext, int def = 0);
};

#endif /* INPUT_H */
