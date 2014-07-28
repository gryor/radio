Radio
=====
A cross-platform and cross-radio c library for transmitting and receiving raw
data using radio modules.

Currently there's implemented functionality only for arduino and for those cheap radio modules
found on ebay with separate transmitter and receiver (cost of both about $1).

How to add a new radio module
======
Create radio_<type>.h and radio_<type>.c similar to radio.h and radio.c
Create a function named radio_<type>_send that's similar to radio_send.
Create radio_<type>_<platform>.h for each platform similar to radio_arduino.h

How to add a new platform
======
Create <platform>.h for platform specific functions.
Create radio_<type_<platform>.h and .c for each radio module.
