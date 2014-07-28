#ifndef RADIO_H_F8ULXMAI
#define RADIO_H_F8ULXMAI

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include <stdint.h>

#include "types.h"


struct Radio;

typedef struct Radio
{
	// status
	uint8_t transmitting;
	uint8_t receiving;

	// options
	radio_time pulse_length;
	radio_time tolerance;
	uint8_t transmit_repeat;
	uint8_t preamble;

	// receivement
	radio_time sync;
	radio_time delay;
	radio_time delay_tolerance;
	radio_time last_time;
	uint8_t * received_message;
	size_t received_bytes_count;
	size_t received_message_allocated_size;
	size_t buffer_size;
	uint8_t bit_count;
	uint8_t change_count;

	// abstracted methods
	void (*write)(uint8_t high);
	void (*delay_microseconds)(size_t microseconds);
	radio_time(*microseconds)();
	void (*transmitter_power)(uint8_t on);
	void (*receiver_power)(uint8_t on);
	uint8_t (*transmitter_powered)();
	uint8_t (*receiver_powered)();

	// callbacks
	void (*callback_receive)(struct Radio * radio, uint8_t * content,
	                         size_t content_size);
} Radio;

Radio * radio_create();
void radio_free(Radio * radio);

uint8_t radio_send(void * radio, const uint8_t * content, size_t content_size);
void radio_on_receive(Radio * radio);
uint8_t radio_resize_receiver_buffer(Radio * radio, size_t size);


#ifdef __cplusplus
}
#endif

#endif // end of include guard: RADIO_H_F8ULXMAI
