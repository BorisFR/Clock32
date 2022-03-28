#include "TheGif.hpp"

// ***********************************************************************************************
// All stuff to do callback for an object

// instance of the object
TheGif *theGifInstance;

void GifDraw(GIFDRAW *pDraw)
{
	theGifInstance->GIFDraw(pDraw);
}

void *GifOpenFile(char const *fname, int32_t *pSize)
{
	return theGifInstance->GIFOpenFile(fname, pSize);
}

void GifCloseFile(void *pHandle)
{
	theGifInstance->GIFCloseFile(pHandle);
}

int32_t GifReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
	return theGifInstance->GIFReadFile(pFile, pBuf, iLen);
}

int32_t GifSeekFile(GIFFILE *pFile, int32_t iPosition)
{
	return theGifInstance->GIFSeekFile(pFile, iPosition);
}

// ***********************************************************************************************

// instanciate the object
TheGif::TheGif()
{
	theGifInstance = this;
}

// ***********************************************************************************************

void TheGif::setup(FilesTools &ft, ThePanel &tp)
{
	filesTools = &ft;
	thePanel = &tp;
	gif.begin(LITTLE_ENDIAN_PIXELS);
}

// ***********************************************************************************************

// if it's time, display the next frame of gif
uint8_t TheGif::loop()
{
	if ((micros() - gifAnimatedFrameStart) >= gifAnimatedFrameDelay)
	{
		// play current animated gif
		int delayToWait;
		int ret = gif.playFrame(false, &delayToWait);
		if (ret > 0)
		{
			// swapBuffers();
			delayToWait -= gifAnimatedFrameTimeToDraw;
			if (delayToWait < 0)
			{
				Serial.println("oups... delay: " + String(delayToWait));
				delayToWait = 0;
			}
			gifAnimatedFrameDelay = delayToWait * 1000; // convert to microseconds
			gifAnimatedFrameTimeToDraw = 0;
			gifAnimatedFrameStart = micros();
			return BUSY;
		}
		else
		{
			if (ret < 0)
			{
				Serial.println("ERROR gif.playframe");
			}
			// no more animation, end of gif
			return OK;
		}
	}
	return BUSY;
}

// ***********************************************************************************************

// play a new animated gif
uint16_t TheGif::start()
{
	filesTools->chooseNextRandomFile();
	uint8_t strLen = filesTools->fullPathName.length() + 1;
	char char_array[strLen];
	filesTools->fullPathName.toCharArray(char_array, strLen);
	thePanel->dma_display->fillScreenRGB888(0, 0, 0);
	if (gif.open(char_array, GifOpenFile, GifCloseFile, GifReadFile, GifSeekFile, GifDraw))
	{
		gifAnimatedFrameStart = 0;
		gifAnimatedFrameDelay = 0;
		// Serial.println("Playing: " + filesTools->fullPathName);
		return MAX_DISPLAY_GIF_TIME_SECONDS * 1000;
	}
	else
	{
		// Houston? We've got a problem...
		Serial.println("*** Error playing: " + filesTools->fullPathName);
		return 0;
	}
}

// ***********************************************************************************************

// Draw a line of image directly on the LED Matrix
void TheGif::GIFDraw(GIFDRAW *pDraw)
{
	unsigned long timeStart = millis();
	uint8_t *s;
	uint16_t *d, *usPalette, usTemp[320];
	int x, y, iWidth;

	// thePanel->dma_display->fillScreenRGB888(0, 0, 0);
	iWidth = pDraw->iWidth;
	if (iWidth > MATRIX_WIDTH)
		iWidth = MATRIX_WIDTH;

	usPalette = pDraw->pPalette;
	y = pDraw->iY + pDraw->y; // current line

	s = pDraw->pPixels;
	if (pDraw->ucDisposalMethod == 2) // restore to background color
	{
		for (x = 0; x < iWidth; x++)
		{
			if (s[x] == pDraw->ucTransparent)
				s[x] = pDraw->ucBackground;
		}
		pDraw->ucHasTransparency = 0;
	}
	// Apply the new pixels to the main image
	if (pDraw->ucHasTransparency) // if transparency used
	{
		uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
		int x, iCount;
		pEnd = s + pDraw->iWidth;
		x = 0;
		iCount = 0; // count non-transparent pixels
		while (x < pDraw->iWidth)
		{
			c = ucTransparent - 1;
			d = usTemp;
			while (c != ucTransparent && s < pEnd)
			{
				c = *s++;
				if (c == ucTransparent) // done, stop
				{
					s--; // back up to treat it like transparent
				}
				else // opaque
				{
					*d++ = usPalette[c];
					iCount++;
				}
			}			// while looking for opaque pixels
			if (iCount) // any opaque pixels?
			{
				for (int xOffset = 0; xOffset < iCount; xOffset++)
				{
					// display.drawPixelRGB565(x + xOffset, y, usTemp[xOffset]);
					thePanel->dma_display->drawPixel(x + xOffset, y, usTemp[xOffset]);
				}
				x += iCount;
				iCount = 0;
			}
			// no, look for a run of transparent pixels
			c = ucTransparent;
			while (c == ucTransparent && s < pEnd)
			{
				c = *s++;
				if (c == ucTransparent)
					iCount++;
				else
					s--;
			}
			if (iCount)
			{
				x += iCount; // skip these
				iCount = 0;
			}
		}
	}
	else // does not have transparency
	{
		s = pDraw->pPixels;
		// Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
		for (x = 0; x < pDraw->iWidth; x++)
		{
			// display.drawPixelRGB565(x, y, usPalette[*s++]);
			thePanel->dma_display->drawPixel(x, y, usPalette[*s++]);
		}
	}
	timeTakenToDraw = millis() - timeStart;
	gifAnimatedFrameTimeToDraw += timeTakenToDraw;
	// thePanel->swapDisplayBuffers();
} // GIFDraw()

// ***********************************************************************************************

void *TheGif::GIFOpenFile(char const *fname, int32_t *pSize)
{
	filesTools->theFileSD = theSd.open(filesTools->fullPathName.c_str(), O_RDONLY);
	if (!filesTools->theFileSD)
	{
		Serial.println("Error opening GIF file " + filesTools->fullPathName);
		return NULL;
	}
	*pSize = filesTools->theFileSD.fileSize();
	return (void *)&filesTools->theFileSD;
} // GIFOpenFile()

// ***********************************************************************************************

void TheGif::GIFCloseFile(void *pHandle)
{
	if (filesTools->theFileSD)
		filesTools->theFileSD.close();
} // GIFCloseFile()

// ***********************************************************************************************

int32_t TheGif::GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
	int32_t iBytesRead;
	iBytesRead = iLen;
	// SdFile *f = static_cast<SdFile *>(pFile->fHandle);
	//  Note: If you read a file all the way to the last byte, seek() stops working
	if ((pFile->iSize - pFile->iPos) < iLen)
		iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
	if (iBytesRead <= 0)
		return 0;
	iBytesRead = (int32_t)filesTools->theFileSD.read(pBuf, iBytesRead);
	pFile->iPos = filesTools->theFileSD.curPosition();
	// pFile->iPos = filesTools->theFileSD.position();
	return iBytesRead;
} // GIFReadFile()

// ***********************************************************************************************

int32_t TheGif::GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
	// int i = micros();
	// SdFile *f = static_cast<SdFile *>(pFile->fHandle);
	filesTools->theFileSD.seekSet(iPosition);
	// f->seek(iPosition);
	pFile->iPos = (int32_t)filesTools->theFileSD.curPosition();
	// pFile->iPos = (int32_t)f->position();
	// i = micros() - i;
	//   Serial.printf("Seek time = %d us\n", i);
	return pFile->iPos;
} // GIFSeekFile()
