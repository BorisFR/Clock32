#pragma once
#ifndef AUDIO_SPECTRUM_HPP
#define AUDIO_SPECTRUM_HPP

#include "Arduino.h"
#include "ThePanel.hpp"
#include "arduinoFFT.h"
#include "ExponentialFilter.hpp"

#define AUDIO_PIN 35
#define NOISE 780

#define NUMBER_SAMPLES 512		 // Must be a power of 2
#define SAMPLING_FREQUENCY 44100 // Hz, must be 40000 or less due to ADC conversion time. Determines maximum frequency that can be analysed by the FFT Fmax=sampleF/2.
#define AMPLITUDE 1				 // Depending on your audio source level, you may need to increase this value
#define NUMBER_BANDS 32
#define BAND_WIDTH (uint8_t)(MATRIX_WIDTH / NUMBER_BANDS)
#define NOISE_FILTER 2500
#define AUDIO_DISPLAY_REFRESH 150

class AudioSpectrum
{
public:
	AudioSpectrum();
	void setup(ThePanel &tp);
	void loop();
	void display();
	bool isNoisy();

private:
	//  define the variables needed for the audio levels
	long level;
	ExponentialFilter adcFilter;
	ThePanel *thePanel;
	CRGB black;
	CRGB white;
	CRGB red;
	CRGB yellow;
	CRGB green;
	CRGB cyan;
	CRGB blue;
	CRGB purple;
	CRGB bandsColor[MATRIX_HEIGHT];
	CRGB bandsColorPeak[MATRIX_HEIGHT];
	float coeff;
	void displayBand(uint8_t band);
	uint16_t bucketFrequency(uint16_t iBucket);
	// taken here : https://github.com/donnersm/FFT_ESP32_Analyzer/blob/main/Main%20Sketch/V1.0/FFT.h
#if NUMBER_BANDS == 8
	int bandCutoffTable[NUMBER_BANDS] = {20, 150, 400, 550, 675, 780, 900, 1200};
#endif
#if NUMBER_BANDS == 16
	int bandCutoffTable[NUMBER_BANDS] = {40, 60, 100, 150, 250, 400, 650, 1000, 1600, 2500, 4000, 6250, 12000, 14000, 16000, 17000};
#endif
#if NUMBER_BANDS == 24
	int bandCutoffTable[NUMBER_BANDS] = {
		40, 80, 150, 220, 270, 320, 380, 440, 540, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150,
		3800, 4200, 4800, 5400, 6200, 7400, 12500};
#endif
#if NUMBER_BANDS == 32
	int bandCutoffTable[NUMBER_BANDS] = {
		45, 90, 130, 180, 220, 260, 310, 350,
		390, 440, 480, 525, 650, 825, 1000, 1300,
		1600, 2050, 2500, 3000, 4000, 5125, 6250, 9125,
		12000, 13000, 14000, 15000, 16000, 16500, 17000, 17500};
#endif
	arduinoFFT fft;
	unsigned int sampling_period_us;
	unsigned long microseconds;
	int16_t peak[NUMBER_BANDS];
	double vReal[NUMBER_SAMPLES];
	double vImag[NUMBER_SAMPLES];
	unsigned long newTime, oldTime;
	double bandValue[NUMBER_BANDS];
#if NUMBER_BANDS == 8
	uint16_t divider[NUMBER_BANDS] = {6810, 7875, 12340, 14320, 16450, 30755, 47640, 64525};
#endif
#if NUMBER_BANDS == 16
	uint16_t divider[NUMBER_BANDS] = {6450, 6700, 6980, 7058, 7136, 8575, 8380, 8241,
									  14951, 16185, 11495, 10175, 18440, 4240, 4255, 8731};
#endif
#if NUMBER_BANDS == 24
	uint16_t divider[NUMBER_BANDS] = {6530, 6700, 6870, 7320, 6160, 5557, 4955, 5960,
									  4400, 3790, 5955, 7730, 7985, 12805, 11835, 5895,
									  8480, 5565, 4680, 6185, 5265, 4585, 5010, 23910};
#endif
#if NUMBER_BANDS == 32
	uint16_t divider[NUMBER_BANDS] = {6545, 6860, 7025, 7190, 6672, 6155, 5635, 5115,
									  5547, 5980, 5310, 4640, 3165, 5700, 4165, 11305,
									  11605, 15005, 7325, 8675, 10075, 9380, 6070, 14870,
									  12935, 3075, 3525, 4150, 2610, 1385, 1205, 5325};
#endif

	unsigned long lastDisplay;
	unsigned long lastAudioNoise;
	unsigned long lastNoAudio;
	bool isNoise;
};

#endif