#ifndef RADIO_ARDUINO_H_MOQQDS35
#define RADIO_ARDUINO_H_MOQQDS35

#ifdef __cplusplus
extern "C" {
#endif


#include "arduino.h"
#include "radio.h"

template<Radio *& radio, const int transmitter_power_pin, const int transmitter_data_pin, const int receiver_power_pin, const int receiver_data_pin>
void radio_create_arduino()
{
	radio = radio_create();

	if (!radio)
		return;

	radio->transmitter_power =
	    transmitter_power<transmitter_power_pin, transmitter_data_pin>;
	radio->receiver_power =
	    receiver_power<radio, receiver_power_pin, receiver_data_pin>;
	radio->transmitter_powered = powered<transmitter_power_pin>;
	radio->receiver_powered = powered<receiver_power_pin>;
	radio->write = write<transmitter_data_pin>;
	radio->delay_microseconds = delay_microseconds;
	radio->microseconds = microseconds;
}

#ifdef __cplusplus
}
#endif

#endif // end of include guard: RADIO_ARDUINO_H_MOQQDS35
