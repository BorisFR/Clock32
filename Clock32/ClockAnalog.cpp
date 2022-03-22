#include <ClockAnalog.hpp>

// ***********************************************************************************************

ClockAnalog::ClockAnalog() {}

// ***********************************************************************************************

void ClockAnalog::setup(ThePanel &tp)
{
	thePanel = &tp;
	/*white = CRGB(255, 255, 255);
	red = CRGB(255, 0, 0);
	green = CRGB(0, 255, 0);
	blue = CRGB(0, 0, 255);*/
	white = thePanel->dma_display->color565(255, 255, 255);
	red = thePanel->dma_display->color565(255, 0, 0);
	green = thePanel->dma_display->color565(0, 255, 0);
	blue = thePanel->dma_display->color565(0, 0, 255);
	black = CRGB().Black; // thePanel->dma_display->color565(0, 0, 0);
}

// ***********************************************************************************************

uint8_t ClockAnalog::loop(Timezone timeZone)
{
	uint8_t hour = timeZone.hour();
	uint8_t minute = timeZone.minute();
	uint8_t second = timeZone.second();
	if (oldSecond == second)
		return BUSY;
	byte y = (CLOCK_RAY_SECOND * cos(PI - (2 * PI) / 60 * oldSecond)) + CLOCK_CENTER_Y;
	byte x = (CLOCK_RAY_SECOND * sin(PI - (2 * PI) / 60 * oldSecond)) + CLOCK_CENTER_X;
	thePanel->dma_display->drawCircle(x, y, 1, black);
	thePanel->dma_display->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, black);
	oldSecond = second;

	y = (CLOCK_RAY_MINUTE * cos(PI - (2 * PI) / 60 * oldMinute)) + CLOCK_CENTER_Y;
	x = (CLOCK_RAY_MINUTE * sin(PI - (2 * PI) / 60 * oldMinute)) + CLOCK_CENTER_X;
	thePanel->dma_display->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, black);

	y = (CLOCK_RAY_HOUR * cos(PI - (2 * PI) / 12 * oldHour - (2 * PI) / 720 * oldMinute)) + CLOCK_CENTER_Y;
	x = (CLOCK_RAY_HOUR * sin(PI - (2 * PI) / 12 * oldHour - (2 * PI) / 720 * oldMinute)) + CLOCK_CENTER_X;
	thePanel->dma_display->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, black);
	oldHour = hour;
	oldMinute = minute;

	thePanel->dma_display->drawCircle(CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RAY, white);
	for (byte i = 0; i < 12; i++)
	{
		y = (CLOCK_RAY * cos(PI - (2 * PI) / 12 * i)) + CLOCK_CENTER_Y;
		x = (CLOCK_RAY * sin(PI - (2 * PI) / 12 * i)) + CLOCK_CENTER_X;
		byte y1 = (CLOCK_RAY_MIN * cos(PI - (2 * PI) / 12 * i)) + CLOCK_CENTER_Y;
		byte x1 = (CLOCK_RAY_MIN * sin(PI - (2 * PI) / 12 * i)) + CLOCK_CENTER_X;
		thePanel->dma_display->drawLine(x1, y1, x, y, white);
	}

	y = (CLOCK_RAY_SECOND * cos(PI - (2 * PI) / 60 * second)) + CLOCK_CENTER_Y;
	x = (CLOCK_RAY_SECOND * sin(PI - (2 * PI) / 60 * second)) + CLOCK_CENTER_X;
	thePanel->dma_display->drawCircle(x, y, 1, blue);
	thePanel->dma_display->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, blue);

	y = (CLOCK_RAY_MINUTE * cos(PI - (2 * PI) / 60 * minute)) + CLOCK_CENTER_Y;
	x = (CLOCK_RAY_MINUTE * sin(PI - (2 * PI) / 60 * minute)) + CLOCK_CENTER_X;
	thePanel->dma_display->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, green);

	y = (CLOCK_RAY_HOUR * cos(PI - (2 * PI) / 12 * hour - (2 * PI) / 720 * minute)) + CLOCK_CENTER_Y;
	x = (CLOCK_RAY_HOUR * sin(PI - (2 * PI) / 12 * hour - (2 * PI) / 720 * minute)) + CLOCK_CENTER_X;
	// byte y1 = sin(PI - (2 * PI) / 12 * hour - (2 * PI) / 720 * minute);
	// byte x1 = cos(PI - (2 * PI) / 12 * hour - (2 * PI) / 720 * minute);
	// byte y1 = (CLOCK_RAY_HOUR * cos(PI - (2 * PI) / 12 * hour - (2 * PI) / 720 * minute)) + CLOCK_CENTER_Y + 1;
	// byte x1 = (CLOCK_RAY_HOUR * sin(PI - (2 * PI) / 12 * hour - (2 * PI) / 720 * minute)) + CLOCK_CENTER_X + 1;
	thePanel->dma_display->drawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, x, y, red);
	// thePanel->dma_display->drawLine(CLOCK_CENTER_X + 1, CLOCK_CENTER_Y + 1, x1, y1, red);
	// thePanel->dma_display->drawLine(CLOCK_CENTER_X + x1, CLOCK_CENTER_Y + y1, x, y, red);
	// thePanel->dma_display->drawLine(CLOCK_CENTER_X - x1, CLOCK_CENTER_Y - y1, x, y, red);

	thePanel->dma_display->setTextColor(thePanel->dma_display->color565(255, 255, 255));
	// thePanel->dma_display->setCursor(34, 24);
	//  String toDisplay = on2(timeZone.hour()) + ":" + on2(timeZone.minute()) + ":" + on2(timeZone.second());
	String toDisplay = timeZone.dateTime("l");
	thePanel->dma_display->setCursor(CLOCK_CENTER_X + CLOCK_RAY + CLOCK_SPACE_TEXT, 2);
	thePanel->dma_display->print(toDisplay);
	toDisplay = timeZone.dateTime("j F");
	thePanel->dma_display->setCursor(CLOCK_CENTER_X + CLOCK_RAY + CLOCK_SPACE_TEXT, 13);
	thePanel->dma_display->print(toDisplay);
	toDisplay = timeZone.dateTime("Y");
	thePanel->dma_display->setCursor(CLOCK_CENTER_X + CLOCK_RAY + CLOCK_SPACE_TEXT, 24);
	thePanel->dma_display->print(toDisplay);
	// thePanel->swapDisplayBuffers();
	return BUSY;
}

// ***********************************************************************************************

uint16_t ClockAnalog::start()
{
	oldSecond = 100;
	oldMinute = 0;
	oldHour = 0;
	thePanel->dma_display->setFont(NULL);
	thePanel->dma_display->setTextSize(1);
	thePanel->dma_display->fillScreenRGB888(0, 0, 0);

	return MAX_CLOCK_ANALOG_DURATION;
}