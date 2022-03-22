#include "TheAnim.hpp"

TheAnim::TheAnim() {}

void TheAnim::setup(ThePanel &tp)
{
	thePanel = &tp;
}

void TheAnim::loop()
{
	theCanvas.setCursor(0, 0);
	theCanvas.print("Boris");
	// thePanel->dma_display->drawBitmap(0, 0, canvas.getBuffer(), 128, 32, blue, red);
	thePanel->dma_display->drawRGBBitmap(0, 0, theCanvas.getBuffer(), 128, 32);
}

void TheAnim::start()
{
	theCanvas.fillScreen(0);
}