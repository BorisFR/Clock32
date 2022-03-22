#pragma once
#ifndef THE_GIF_HPP
#define THE_GIF_HPP

#include "Arduino.h"
#include <AnimatedGIF.h>
#include <FilesTools.hpp>
#include <ThePanel.hpp>

#define MAX_WEATHER_DURATION 0
#define MAX_DISPLAY_GIF_TIME_SECONDS 6 * 10

class TheGif
{
public:
	TheGif();
	void setup(FilesTools &ft, ThePanel &tp);
	uint8_t loop();
	uint16_t start();

	void GIFDraw(GIFDRAW *pDraw);
	void *GIFOpenFile(char const *fname, int32_t *pSize);
	void GIFCloseFile(void *pHandle);
	int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
	int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);

private:
	AnimatedGIF gif;

	FilesTools *filesTools;
	ThePanel *thePanel;

	unsigned long timeTakenToDraw;

	unsigned long gifAnimatedFrameTimeToDraw;
	unsigned long gifAnimatedFrameStart;
	unsigned long gifAnimatedFrameDelay;
};

#endif