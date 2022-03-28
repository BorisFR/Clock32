#include "TheAnim.hpp"

TheAnim::TheAnim() {}

void TheAnim::setup(ThePanel &tp)
{
	thePanel = &tp;
	// fill_gradient_RGB(wheelColors, 0, CRGB(0, 255, 0), 16, CRGB(255, 255, 0));
	CHSV hsv;
	hsv.hue = 0;
	hsv.val = 255;
	hsv.sat = 240;
	CRGB rgb;
	for (int i = 0; i < 256; ++i)
	{
		hsv2rgb_rainbow(hsv, rgb);
		wheelColors[i] = rgb;
		// wheelColors[i] = thePanel->dma_display->color565(rgb.r, rgb.g, rgb.b);
		hsv.hue += 1;
	}
	darkColor = thePanel->dma_display->color565(20, 20, 20);
}

void TheAnim::loop()
{
	// thePanel->dma_display->drawBitmap(0, 0, canvas.getBuffer(), 128, 32, blue, red);
	// thePanel->dma_display->drawRGBBitmap(0, 0, theCanvas.getBuffer(), 128, 32);

	int16_t x = 0, y = 0;
	uint16_t *bitmap = theCanvas.getBuffer();
	// thePanel->dma_display->startWrite();
	for (int16_t j = 0; j < MATRIX_HEIGHT; j++, y++)
	{
		byte wheelIndex = j + deltaWheel;
		for (int16_t i = 0; i < MATRIX_WIDTH; i++)
		{
			switch (bitmap[j * MATRIX_WIDTH + i])
			{
			case 1:
				thePanel->dma_display->drawPixelRGB888(x + i, y, 40, 40, 40);
				// thePanel->dma_display->writePixel(x + i, y, darkColor);
				break;
			case 2:
				CRGB rgb = wheelColors[wheelIndex];
				thePanel->dma_display->drawPixelRGB888(x + i, y, rgb.r, rgb.g, rgb.b);
				// thePanel->dma_display->writePixel(x + i, y, wheelColors[wheelIndex]);
				break;
			}
			wheelIndex++;
		}
	}

	// thePanel->dma_display->endWrite();
	deltaWheel++;
}

void TheAnim::setPosition(String text)
{
	int16_t x1, y1;
	uint16_t width, height;
	theCanvas.setTextWrap(false);
	theCanvas.setFont(&Roboto_Mono_Medium_26);
	theCanvas.setTextSize(1);
	theCanvas.getTextBounds(text, 0, 32, &x1, &y1, &width, &height);
	Serial.println("W: " + String(width) + ", H: " + String(height));
	posX = (MATRIX_WIDTH - width) / 2;					   // 20;
	posY = MATRIX_HEIGHT - ((MATRIX_HEIGHT - height) / 2); // 27;
}

void TheAnim::start(String text)
{
	// String text = "Boris";
	if (oldText.equals(text))
		return;
	oldText = text;
	theCanvas.fillScreen(0);
	theCanvas.setTextWrap(false);
	theCanvas.setFont(&Roboto_Mono_Medium_26);
	theCanvas.setTextSize(1);
	theCanvas.setTextColor(1);
	for (byte x = posX - 1; x <= posX + 1; x++)
	{
		for (byte y = posY - 1; y <= posY + 1; y++)
		{
			theCanvas.setCursor(x, y);
			theCanvas.print(text);
		}
	}
	theCanvas.setCursor(posX, posY);
	theCanvas.setTextColor(2);
	theCanvas.print(text);
}