#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>
#include <SDL2/SDL.h>

typedef SDL_GameControllerButton button_t;
typedef SDL_Scancode kbkey_t;
typedef uint8_t mbutton_t;
typedef int32_t coord_t;
typedef int16_t axis_t;

extern SDL_JoystickID gp_first;
#define GPD gp_first

#endif /* TYPEDEFS_H */
