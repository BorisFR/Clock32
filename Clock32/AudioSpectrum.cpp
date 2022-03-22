#include "AudioSpectrum.hpp"

AudioSpectrum::AudioSpectrum() {}

void AudioSpectrum::setup(ThePanel &tp)
{
	thePanel = &tp;
	white = CRGB(255, 255, 255);
	red = CRGB(255, 0, 0);
	yellow = CRGB(255, 255, 0);
	green = CRGB(0, 255, 0);
	cyan = CRGB(0, 255, 255);
	blue = CRGB(0, 0, 255);
	purple = CRGB(255, 0, 255);
	black = CRGB(0, 0, 0);
	fill_gradient_RGB(bandsColor, 0, green, 16, yellow);
	fill_gradient_RGB(bandsColor, 16, yellow, 31, red);

	fill_gradient_RGB(bandsColorPeak, 1, black, 5, yellow);
	fill_gradient_RGB(bandsColorPeak, 6, red, 10, cyan);
	fill_gradient_RGB(bandsColorPeak, 11, green, 31, purple);

	fft = arduinoFFT();
	sampling_period_us = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); // => 25
	for (byte i = 0; i < NUMBER_BANDS; i++)
		peak[i] = 0;
	/*#if NUMBER_BANDS == 32
		for (byte i = 0; i < NUMBER_BANDS; i++)
			divider[i] = 100;
	#endif*/
	isNoise = true;
	adcFilter = ExponentialFilter();
	adcFilter.SetWeight(5);
}

uint16_t AudioSpectrum::bucketFrequency(uint16_t iBucket)
{
	if (iBucket <= 1)
		return 0;
	uint16_t iOffset = iBucket - 2;
	uint16_t temp = iOffset * (SAMPLING_FREQUENCY / 2) / (NUMBER_SAMPLES / 2);
	return temp;
}

bool AudioSpectrum::isNoisy()
{
	uint16_t value = analogRead(AUDIO_PIN);
	// remove the MX9614 bias of 1.25VDC
	value = abs(1023 - value);
	// hard limit noise/hum
	value = (value <= NOISE) ? 0 : abs(value - NOISE);
	// apply the exponential filter to smooth the raw signal
	adcFilter.Filter(value);
	level = adcFilter.Current();
	// Serial.println(level);
	if (level < 30)
	{
		if (isNoise)
		{
			isNoise = false;
			lastNoAudio = millis();
		}
		return false;
	}
	else
	{
		if (!isNoise)
		{
			isNoise = true;
			lastAudioNoise = millis();
		}
	}
	return true;
}

void AudioSpectrum::loop()
{
	for (uint16_t i = 0; i < NUMBER_SAMPLES; i++)
	{
		newTime = micros() - oldTime;
		oldTime = newTime;
		vReal[i] = analogRead(AUDIO_PIN); // A conversion takes about 1uS on an ESP32
		vImag[i] = 0;
		while (micros() < (newTime + sampling_period_us))
		{ /* do nothing to wait */
		}
	}

	fft.Windowing(vReal, NUMBER_SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
	fft.Compute(vReal, vImag, NUMBER_SAMPLES, FFT_FORWARD);
	fft.ComplexToMagnitude(vReal, vImag, NUMBER_SAMPLES);

	double sum = 0;
	for (uint16_t i = 2; i < (NUMBER_SAMPLES / 2); i++)
	{ // Don't use sample 0 and only first SAMPLES/2 are usable
		if (vReal[i] > NOISE_FILTER)
		{
			uint16_t freq = bucketFrequency(i);
			uint8_t iBand = 0;
			while (iBand < NUMBER_BANDS)
			{
				if (freq < bandCutoffTable[iBand])
					break;
				iBand++;
			}
			if (iBand >= NUMBER_BANDS)
				iBand = NUMBER_BANDS - 1;
			sum += vReal[i];
			bandValue[iBand] += vReal[i];
		}
	}
}

void AudioSpectrum::display()
{
#if NUMBER_BANDS == 8
	bandValue[6] = (bandValue[5] + bandValue[7]) / 2;
	divider[6] = (divider[5] + divider[7]) >> 1;
#endif
#if NUMBER_BANDS == 16
	bandValue[1] = (bandValue[0] + bandValue[2]) / 2;
	bandValue[3] = (bandValue[2] + bandValue[4]) / 2;
	divider[1] = (divider[0] + divider[2]) >> 1;
	divider[3] = (divider[2] + divider[4]) >> 1;
#endif
#if NUMBER_BANDS == 24
	bandValue[1] = (bandValue[0] + bandValue[2]) / 2;
	bandValue[5] = (bandValue[4] + bandValue[6]) / 2;
	divider[1] = (divider[0] + divider[2]) >> 1;
	divider[5] = (divider[4] + divider[6]) >> 1;
#endif
#if NUMBER_BANDS == 32
	bandValue[2] = (bandValue[1] + bandValue[3]) / 2;
	bandValue[4] = (bandValue[3] + bandValue[5]) / 2;
	bandValue[6] = (bandValue[5] + bandValue[7]) / 2;
	bandValue[8] = (bandValue[7] + bandValue[9]) / 2;
	bandValue[10] = (bandValue[9] + bandValue[11]) / 2;
	divider[2] = (divider[1] + divider[3]) >> 1;
	divider[4] = (divider[3] + divider[5]) >> 1;
	divider[6] = (divider[5] + divider[7]) >> 1;
	divider[8] = (divider[7] + divider[9]) >> 1;
	divider[10] = (divider[9] + divider[11]) >> 1;
#endif
	// if ((millis() - lastDisplay) <= AUDIO_DISPLAY_REFRESH)
	//	return;

	byte maxValue = 1;
	for (byte band = 0; band < NUMBER_BANDS; band++)
	{
		uint64_t v = bandValue[band] / divider[band];
		if (v > 31)
		{
			while (v > 31)
			{
				divider[band] += 5;
				v = bandValue[band] / divider[band];
			}
			Serial.print("{");
			for (byte b = 0; b < NUMBER_BANDS; b++)
				Serial.print(String(divider[b]) + ",");
			Serial.println("}");
		}
		if (v > peak[band])
		{
			peak[band] = v;
		}
		if (v > maxValue)
			maxValue = v;
	}
	coeff = 30.0 / maxValue;
	if (coeff < 1.0)
		coeff = 1.0;

	thePanel->dma_display->fillScreenRGB888(0, 0, 0);
	for (byte band = 0; band < NUMBER_BANDS; band++)
		displayBand(band);
	for (byte b = 0; b < NUMBER_BANDS; b++)
		bandValue[b] = 0;
	// if (millis() % 2 == 0)
	{
		for (byte band = 0; band < NUMBER_BANDS; band++)
		{
			if (peak[band] > 0)
				peak[band] -= 1;
		}
	}
	// lastDisplay = millis();
}

void AudioSpectrum::displayBand(uint8_t band)
{
	uint64_t v = coeff * (float)bandValue[band] / (float)divider[band];
	/*if (v > 31)
	{
		while (v > 31)
		{
			divider[band] += 5;
			v = bandValue[band] / divider[band];
		}
		Serial.print("{");
		for (byte b = 0; b < NUMBER_BANDS; b++)
			Serial.print(String(divider[b]) + ",");
		Serial.println("}");
	}
	if (v > peak[band])
	{
		peak[band] = v;
	}*/
	for (byte y = 0; y < v; y++)
	{
		thePanel->dma_display->drawFastHLine(band * BAND_WIDTH, 32 - y, BAND_WIDTH - 1, bandsColor[y].red, bandsColor[y].green, bandsColor[y].blue);
	}

	/*
	for (byte l = 0; l < (BAND_WIDTH - 1); l++)
	{
		if (v < 21)
		{
			thePanel->dma_display->drawFastVLine(band * BAND_WIDTH + l, 32 - v, v, 0, 255, 0);
		}
		else
		{
			if (v < 26)
			{
				thePanel->dma_display->drawFastVLine(band * BAND_WIDTH + l, 32 - 20, 20, 0, 255, 0);
				thePanel->dma_display->drawFastVLine(band * BAND_WIDTH + l, 32 - v, v - 20, 255, 255, 0);
			}
			else
			{
				thePanel->dma_display->drawFastVLine(band * BAND_WIDTH + l, 32 - 20, 20, 0, 255, 0);
				thePanel->dma_display->drawFastVLine(band * BAND_WIDTH + l, 32 - v, v - 20, 255, 255, 0);
				thePanel->dma_display->drawFastVLine(band * BAND_WIDTH + l, 32 - v, v - 20 - 6, 255, 0, 0);
			}
		}
	}
	*/

	thePanel->dma_display->drawFastHLine(band * BAND_WIDTH, 32 - peak[band], BAND_WIDTH - 1, white.red, white.green, white.blue);

	thePanel->dma_display->drawFastHLine(band * BAND_WIDTH, 0, BAND_WIDTH - 1, bandsColorPeak[peak[band]].red, bandsColorPeak[peak[band]].green, bandsColorPeak[peak[band]].blue);
	thePanel->dma_display->drawFastHLine(band * BAND_WIDTH, 1, BAND_WIDTH - 1, bandsColorPeak[peak[band]].red, bandsColorPeak[peak[band]].green, bandsColorPeak[peak[band]].blue);
}