#ifndef RADIO_ARDUINO_H_MOQQDS35
#define RADIO_ARDUINO_H_MOQQDS35

#ifdef __cplusplus
extern "C" {
#endif


#include <Arduino.h>

#include "radio.h"


template<const int pin>
void write(uint8_t high)
{
	if (high)
		TXLED1;
	else
		TXLED0;

	digitalWrite(pin, high ? HIGH : LOW);
}

void delay_microseconds(size_t microseconds)
{
	delayMicroseconds(microseconds);
}

radio_time microseconds()
{
	return micros();
}

template<Radio *& radio>
void on_receive()
{
	radio_on_receive(radio);
}

template<const int power_pin, const int data_pin>
void transmitter_power(uint8_t on)
{
	pinMode(power_pin, OUTPUT);
	pinMode(data_pin, OUTPUT);
	digitalWrite(power_pin, on ? HIGH : LOW);
}

template<Radio *& radio, const int power_pin, const int data_pin>
void receiver_power(uint8_t on)
{
	pinMode(power_pin, OUTPUT);
	digitalWrite(power_pin, on ? HIGH : LOW);

	if (on)
		attachInterrupt(data_pin, on_receive<radio>, CHANGE);
	else
		detachInterrupt(data_pin);
}

template<const int power_pin>
uint8_t powered()
{
	return digitalRead(power_pin) == HIGH ? 1 : 0;
}

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
