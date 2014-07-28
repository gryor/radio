#ifndef BIT_H_CLCIX6XP
#define BIT_H_CLCIX6XP

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

static inline void bit_on(uint8_t * byte, uint8_t n)
{
	*byte |= 1 << n;
}

static inline void bit_off(uint8_t * byte, uint8_t n)
{
	*byte &= ~(1 << n);
}

static inline void bit_toggle(uint8_t * byte, uint8_t n)
{
	*byte ^= 1 << n;
}

static inline uint8_t bit_test(const uint8_t byte, uint8_t n)
{
	return byte & (1 << n);
}

static inline uint8_t byte_odd(const uint8_t byte)
{
	return byte & 1;
}

static inline uint8_t byte_even(const uint8_t byte)
{
	return ~byte & 1;
}

#ifdef __cplusplus
}
#endif

#endif // end of include guard: BIT_H_CLCIX6XP

