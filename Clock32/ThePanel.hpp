#pragma once
#ifndef THE_PANEL_HPP
#define THE_PANEL_HPP

#include "Arduino.h"
#include "Wire.h"
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "FastLED.h"

#define R1_PIN 25
#define G1_PIN 26
#define B1_PIN 27
#define R2_PIN 14
#define G2_PIN 12
#define B2_PIN 13
#define A_PIN 23
#define B_PIN 19
#define C_PIN 5
#define D_PIN 17
#define E_PIN -1 // required for 1/32 scan panels, like 64x64. Any available pin would do, i.e. IO32
#define CLK_PIN 16
#define LAT_PIN 4
#define OE_PIN 15

#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module.
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.
#define NUM_ROWS 1	   // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 2	   // Number of INDIVIDUAL PANELS per ROW

class ThePanel
{
public:
	ThePanel();
	uint8_t setup();
	void loop();
	// void swapDisplayBuffers();
	MatrixPanel_I2S_DMA *dma_display = nullptr;

private:
};

#endif