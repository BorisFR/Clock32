#pragma once
#ifndef THE_ANM_HPP
#define THE_ANM_HPP

#include "Arduino.h"
#include "ThePanel.hpp"
//#include "Roboto_Black_32.h"
#include "Roboto_Mono_Medium_30.h"

class TheAnim
{
public:
	TheAnim();
	void setup(ThePanel &tp);
	void loop();
	void setPosition(String text);
	void start(String text);

private:
	ThePanel *thePanel;
	GFXcanvas1 canvas = GFXcanvas1(128, 32);
	GFXcanvas16 theCanvas = GFXcanvas16(128, 32);
	CRGB wheelColors[256];
	uint16_t darkColor;
	byte deltaWheel;
	String oldText;
	byte posX;
	byte posY;
};

#endif