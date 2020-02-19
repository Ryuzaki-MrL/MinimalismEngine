#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdlib.h>

#include "typedefs.h"

#ifndef MAX_KEYBINDS
#define MAX_KEYBINDS	8
#endif

#define INPUT_KEYBOARD 	(1 << 0)
#define INPUT_MOUSE		(1 << 1)
#define INPUT_GAMEPAD	(1 << 2)
#define INPUT_MOTION 	(1 << 3)
#define INPUT_RUMBLE	(1 << 4)

enum { GP0, GP1, GP2, GP3 };
enum { STICK_LEFT, STICK_RIGHT };

// TODO: bind multiple keys to the same slot
// TODO: rumble devices support
// TODO: init features as needed

namespace Input
{
	bool init(uint16_t feats = 0);
	void fini();

	void poll();

	namespace Keyboard
	{
		bool isKeyDown(kbkey_t);
		bool isKeyBindDown(uint16_t index);
		bool isKeyHeld(kbkey_t);
		bool isKeyBindHeld(uint16_t index);
		bool isKeyUp(kbkey_t);
		bool isKeyBindUp(uint16_t index);

		void keyBind(uint16_t index, kbkey_t);
		void keyUnbind(uint16_t index, kbkey_t);

		const char* getKeyName(kbkey_t);
	};

	namespace Mouse
	{
		bool isDown(mbutton_t);
		bool isHeld(mbutton_t);
		bool isUp(mbutton_t);
		
		coord_t getX();
		coord_t getY();
	};

	namespace Touch
	{
		coord_t getX(uint8_t tpoint = 0);
		coord_t getY(uint8_t tpoint = 0);
	};

	namespace Gamepad
	{
		bool init(uint8_t gpad = GPD);
		void fini(uint8_t gpad = GPD);
		bool isConnected(uint8_t gpad = GPD);

		void poll(uint8_t gpad = GPD);

		bool isButtonDown(button_t, uint8_t gpad = GPD);
		bool isButtonHeld(button_t, uint8_t gpad = GPD);
		bool isButtonUp(button_t, 	uint8_t gpad = GPD);
		bool buttonAny(uint8_t gpad = GPD);

		const char* getDeviceName(uint8_t gpad = GPD);
		const char* getButtonName(button_t, uint8_t gpad = GPD);

		axis_t getStickAxisX(uint8_t stick, uint8_t gpad = GPD);
		axis_t getStickAxisY(uint8_t stick, uint8_t gpad = GPD);
		//axis_t getTrigger(uint8_t trigger, uint8_t gpad = GPD);
	};

	namespace Motion
	{
		axis_t getAccelX();
		axis_t getAccelY();
		axis_t getAccelZ();
		void setAccelState(bool state);

		axis_t getGyroX();
		axis_t getGyroY();
		axis_t getGyroZ();
		void setGyroState(bool state);
	};

	bool swkbd(); // check for system keyboard support
	bool getString(char* out, const char* htext, const char* def, size_t maxlength, bool password = false);
	bool getInteger(int& out, const char* htext, int def = 0);
};

#endif /* INPUT_H */
