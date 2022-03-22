#pragma once
#ifndef CLOCK_ANALOG_HPP
#define CLOCK_ANALOG_HPP

#include "Arduino.h"
#include "ThePanel.hpp"
#include "ezTime.h"

#define MAX_CLOCK_ANALOG_DURATION 10000

#define CLOCK_CENTER_X 28
#define CLOCK_CENTER_Y 15
#define CLOCK_RAY 15
#define CLOCK_RAY_MIN 13
#define CLOCK_RAY_HOUR 8
#define CLOCK_RAY_MINUTE 11
#define CLOCK_RAY_SECOND 13
#define CLOCK_SPACE_TEXT 14

class ClockAnalog
{
public:
	ClockAnalog();
	void setup(ThePanel &tp);
	uint8_t loop(Timezone timeZone);
	uint16_t start();

private:
	ThePanel *thePanel;

	/*CRGB white;
	CRGB red;
	CRGB green;
	CRGB blue;*/
	uint16_t black;
	uint16_t white;
	uint16_t red;
	uint16_t green;
	uint16_t blue;

	uint8_t oldSecond;
	uint8_t oldMinute;
	uint8_t oldHour;
};

#endif