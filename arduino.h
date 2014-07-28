#ifndef ARDUINO_H_NQZUPSUV
#define ARDUINO_H_NQZUPSUV


#include <Arduino.h>

#include "types.h"

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

template<typename T, T *& radio>
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

template<typename T, T *& radio, const int power_pin, const int data_pin>
void receiver_power(uint8_t on)
{
	pinMode(power_pin, OUTPUT);
	digitalWrite(power_pin, on ? HIGH : LOW);

	if (on)
		attachInterrupt(data_pin, on_receive<T, radio>, CHANGE);
	else
		detachInterrupt(data_pin);
}

template<const int power_pin>
uint8_t powered()
{
	return digitalRead(power_pin) == HIGH ? 1 : 0;
}


#endif // end of include guard: ARDUINO_H_NQZUPSUV
