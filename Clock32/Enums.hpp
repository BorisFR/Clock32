#pragma once
#ifndef ENUMS_HPP
#define ENUMS_HPP

#include "Arduino.h"

enum class GLOBAL_STATE : uint8_t
{
	STATE_CLOCK = 1,
	STATE_WEATHER = 2,
	STATE_ANIMATEDGIF = 3,
	STATE_AUDIO_SPECTRUM = 4
};

#endif