#pragma once

// bit set/clear/toggle
#define SetBit(var, mask)	((var) |= (uint8_t)  _BV(mask))
#define ClrBit(var, mask)	((var) &= (uint8_t) ~_BV(mask))
#define TogBit(var, mask)	((var) ^= (uint8_t)  _BV(mask))

// several bits set/clear/toggle
#define SetVal(var, mask)	((var) |= (uint8_t)  mask)
#define ClrVal(var, mask)	((var) &= (uint8_t) ~mask)
#define TogVal(var, mask)	((var) ^= (uint8_t)  mask)
