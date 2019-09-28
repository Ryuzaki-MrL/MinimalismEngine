#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#include "typedefs.h"

#ifndef MAX_KEYBINDS
#define MAX_KEYBINDS	8
#endif

// TODO: mouse, keyboard, multiple controllers and touch points
// TODO: key, touch and axis int types per platform (int16, uint32, etc)
// TODO: bind multiple keys to the same slot

namespace Input {
	bool init();
	void fini();

	void poll();

	bool isKeyDown(key_t key);
	bool isKeyBindDown(uint16_t index);
	bool isKeyHeld(key_t key);
	bool isKeyBindHeld(uint16_t index);
	bool isKeyUp(key_t key);
	bool isKeyBindUp(uint16_t index);

	void keyBind(uint16_t index, key_t key);
	void keyUnbind(uint16_t index);

	uint16_t getTouchX();
	uint16_t getTouchY();

	axis_t getStickAxisX(uint8_t stick);
	axis_t getStickAxisY(uint8_t stick);

	axis_t getAccelX();
	axis_t getAccelY();
	axis_t getAccelZ();
	void setAccelState(bool state);

	axis_t getGyroX();
	axis_t getGyroY();
	axis_t getGyroZ();
	void setGyroState(bool state);

	bool getString(char* out, const char* htext, const char* def, size_t maxlength, bool password = false);
	bool getInteger(int& out, const char* htext, int def = 0);
};

#endif /* INPUT_H */
