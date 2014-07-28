#include "radio.h"
#include <string.h>
#include "bit.h"

Radio * radio_create()
{
	Radio * radio = malloc(sizeof(Radio));

	if(!radio)
		return NULL;

	memset(radio, 0, sizeof(Radio));
	radio->pulse_length = 470;
	radio->transmit_repeat = 1;
	radio->tolerance = 60;
	radio->buffer_size = 10;
	radio->received_message = malloc(radio->buffer_size);
	return radio;
}

void radio_free(Radio * radio)
{
	if(radio->received_message)
		free(radio->received_message);

	free(radio);
}

static inline uint8_t valid_message(uint8_t * content, size_t content_size)
{
	int i;

	if(!content_size || content_size == 1)
		return 0;

	for(i = 0; i < content_size; i++) {
		if(content[i])
			return 1;
	}

	return 0;
}

static inline void reset_receiver(Radio * radio)
{
	if(radio->received_bytes_count
	   && valid_message(radio->received_message, radio->received_bytes_count))
		radio->callback_receive(radio, radio->received_message,
		                        radio->received_bytes_count);

	radio->receiving = 0;
	radio->change_count = 0;
	radio->bit_count = 0;
	radio->received_bytes_count = 0;
}

static void radio_transmit_pulse(Radio * radio, size_t high, size_t low)
{
	radio->write(1);
	radio->delay_microseconds(radio->pulse_length * high);
	radio->write(0);
	radio->delay_microseconds(radio->pulse_length * low);
}

static void radio_transmit_byte(Radio * radio, uint8_t byte)
{
	int i;

	for(i = 7; i >= 0; i--) {
		//if(byte & (1 << i))
		if(bit_test(byte, i))
			radio_transmit_pulse(radio, 3, 1);
		else
			radio_transmit_pulse(radio, 1, 3);
	}
}

static void radio_transmit_byte_repeat(Radio * radio, uint8_t byte,
                                       size_t count)
{
	int i;

	for(i = 0; i < count; i++)
		radio_transmit_byte(radio, byte);
}

static inline void radio_transmit_preamble(Radio * radio)
{
	radio_transmit_byte_repeat(radio, 0xaa, 8);
	radio_transmit_sync(radio);
}

static void radio_transmit_sync(Radio * radio)
{
	radio_transmit_pulse(radio, 1, 31);
}

static void radio_transmit_bytes(Radio * radio, uint8_t * bytes, size_t count)
{
	int b, r;

	if(radio->preamble)
		radio_transmit_preamble(radio);

	for(r = 0; r <= radio->transmit_repeat; r++) {
		for(b = 0; b < count; b++)
			radio_transmit_byte(radio, bytes[b]);

		radio_transmit_sync(radio);
	}
}

uint8_t radio_send(void * radio_pointer, const uint8_t * content,
                   size_t content_size)
{
	Radio * radio = (Radio *)radio_pointer;
	uint8_t receiver_powered = radio->receiver_powered();

	if(receiver_powered) {
		if(radio->received_bytes_count)
			return 1;

		radio->receiver_power(0);
		radio->received_bytes_count = 0;
		reset_receiver(radio);
	}

	radio->transmitting = 1;
	radio->transmitter_power(1);
	radio_transmit_bytes(radio, content, content_size);
	radio->transmitter_power(0);
	radio->transmitting = 0;

	if(receiver_powered)
		radio->receiver_power(1);

	return 0;
}

static inline uint8_t tolerated(radio_time value, radio_time center,
                                radio_time tolerance)
{
	return (value >= center - tolerance) && (value <= center + tolerance);
}

static inline uint8_t between(radio_time value, radio_time low, radio_time high,
                              radio_time tolerance)
{
	return (value >= low - tolerance) && (value <= high + tolerance);
}

static inline uint8_t received_one(Radio * radio, radio_time duration)
{
	return tolerated(duration, 3 * radio->delay, radio->delay_tolerance);
}

static inline uint8_t received_zero(Radio * radio, radio_time duration)
{
	return tolerated(duration, radio->delay, radio->delay_tolerance);
}

uint8_t radio_resize_receiver_buffer(Radio * radio, size_t size)
{
	uint8_t powered = radio->receiver_powered();

	if(powered && radio->received_bytes_count)
		return 1;

	radio->receiver_power(0);
	radio->received_bytes_count = 0;

	if(radio->received_message) {
		free(radio->received_message);
		radio->received_message = 0;
	}

	radio->received_message = malloc(size);

	if(!radio->received_message)
		return 1;

	radio->buffer_size = size;
	reset_receiver(radio);
	radio->receiver_power(powered);
	return 0;
}

void radio_on_receive(Radio * radio)
{
	if(radio->transmitting || !radio->callback_receive)
		return;

	radio_time time = radio->microseconds();
	radio_time duration = time - radio->last_time;
	radio->last_time = time;

	if(time < radio->last_time) {
		reset_receiver(radio);
		return;
	}

	if(!radio->receiving && radio->change_count) {
		radio->receiving = 1;
		radio->sync = duration;
		radio->delay = radio->sync / 31;
		radio->delay_tolerance = radio->delay * radio->tolerance * 0.01;
		radio->bit_count = 0;
		radio->received_bytes_count = 0;
	}

	if(radio->receiving) {
		if(between(duration, radio->delay, 3 * radio->delay, radio->delay_tolerance)) {
			if(byte_even(radio->change_count)) {
				if(radio->received_bytes_count >= radio->buffer_size) {
					// Overflow
					// Realloc on simple uc would result in memory fragmentation
					// and freeze
					radio->received_bytes_count = 0;
					reset_receiver(radio);
					return;
				}

				if(received_one(radio, duration))
					bit_on(&radio->received_message[radio->received_bytes_count],
					       7 - radio->bit_count++);
				else if(received_zero(radio, duration))
					bit_off(&radio->received_message[radio->received_bytes_count],
					        7 - radio->bit_count++);
				else {
					reset_receiver(radio);
					return;
				}

				if(radio->bit_count == 8) {
					radio->bit_count = 0;
					radio->received_bytes_count++;
				}
			}
		} else if(radio->change_count > 1) {
			reset_receiver(radio);
			return;
		}
	}

	radio->change_count++;
}
