#pragma once
#ifndef THE_ANM_HPP
#define THE_ANM_HPP

#include "Arduino.h"
#include "ThePanel.hpp"

class TheAnim
{
public:
	TheAnim();
	void setup(ThePanel &tp);
	void loop();
	void start();

private:
	ThePanel *thePanel;
	GFXcanvas1 canvas = GFXcanvas1(128, 32);
	GFXcanvas16 theCanvas = GFXcanvas16(128, 32);
};

#endif