#include <ThePanel.hpp>

// ***********************************************************************************************

// instanciate the object
ThePanel::ThePanel() {}

// ***********************************************************************************************

// initialize the DMD & gif animator
uint8_t ThePanel::setup(DISPLAY_COLOR_ORDER order)
{
	HUB75_I2S_CFG::i2s_pins _pins;
	switch (order)
	{
	case DISPLAY_COLOR_ORDER::RGB:
		_pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
		break;
	case DISPLAY_COLOR_ORDER::RBG:
		_pins = {R1_PIN, B1_PIN, G1_PIN, R2_PIN, B2_PIN, G2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
		break;
	}
	// RGB
	// HUB75_I2S_CFG::i2s_pins _pins = {R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
	// RBG
	// HUB75_I2S_CFG::i2s_pins _pins = {R1_PIN, B1_PIN, G1_PIN, R2_PIN, B2_PIN, G2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
	HUB75_I2S_CFG mxconfig(
		PANEL_RES_X, // width
		PANEL_RES_Y, // height
		NUM_COLS,	 // chain length
		_pins		 // pin mapping
	);
	// mxconfig.double_buff = true; // Turn of double buffer
	mxconfig.clkphase = false;
	dma_display = new MatrixPanel_I2S_DMA(mxconfig);

	dma_display->setBrightness8(120); // 192 range is 0-255, 0 - 0%, 255 - 100%

	// Allocate memory and start DMA display
	if (not dma_display->begin())
	{
		Serial.println("****** !KABOOM! I2S memory allocation failed ***********");
		return FAIL;
	}

	// well, hope we are OK, let's draw some colors first :)
	dma_display->setFont(NULL);
	dma_display->setTextSize(3); // 18x24
	dma_display->setTextColor(dma_display->color565(0, 0, 0));

	Serial.println("Fill screen: RED");
	dma_display->fillScreenRGB888(255, 0, 0);
	dma_display->setCursor((128 - 5 * 18) / 2, 4);
	dma_display->print("ROUGE");
	// swapDisplayBuffers();
	delay(1000);
	Serial.println("Fill screen: GREEN");
	dma_display->fillScreenRGB888(0, 255, 0);
	dma_display->setCursor((128 - 4 * 18) / 2, 4);
	dma_display->print("VERT");
	// swapDisplayBuffers();
	delay(1000);
	Serial.println("Fill screen: BLUE");
	dma_display->fillScreenRGB888(0, 0, 255);
	dma_display->setCursor((128 - 4 * 18) / 2, 4);
	dma_display->print("BLEU");
	// swapDisplayBuffers();
	delay(1000);
	Serial.println("Fill screen: Neutral White");
	dma_display->fillScreenRGB888(64, 64, 64);
	dma_display->setCursor((128 - 4 * 18) / 2, 4);
	dma_display->print("GRIS");
	// swapDisplayBuffers();
	delay(1000);
	Serial.println("Fill screen: black");
	dma_display->fillScreenRGB888(0, 0, 0);
	dma_display->setTextColor(dma_display->color565(255, 255, 255));
	dma_display->setCursor((128 - 4 * 18) / 2, 4);
	dma_display->print("NOIR");
	// swapDisplayBuffers();
	delay(1000);
	return OK;
}

// ***********************************************************************************************

void ThePanel::loop()
{
}

// ***********************************************************************************************

/*void ThePanel::swapDisplayBuffers()
{
	dma_display->flipDMABuffer();
}*/