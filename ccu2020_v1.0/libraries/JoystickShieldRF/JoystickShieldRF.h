#ifndef JoystickShieldRF_h
#define JoystickShieldRF_h

#include <inttypes.h>

typedef struct Packet {
  uint32_t x:10;
  uint32_t y:10;
  uint32_t a:1;
  uint32_t b:1;
  uint32_t c:1;
  uint32_t d:1;
  uint32_t e:1;
  uint32_t f:1;
  uint32_t down:1;
} Packet;

#endif