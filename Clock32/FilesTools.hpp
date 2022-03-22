#pragma once
#ifndef FILES_TOOLS_HPP
#define FILES_TOOLS_HPP

#include "Arduino.h"

#include "SdFat.h"
#include "sdios.h"

// For HSPI, to be compatible with HUB 75 Matrix
#define SD_SS 22
#define SD_SCK 33
#define SD_MISO 32
#define SD_MOSI 21

extern SdFs theSd;
// extern SPIClass spiSD;

class FilesTools
{
public:
	FilesTools();
	uint8_t setup();
	uint8_t populateGif();
	uint16_t getNumberOfGifs();
	void chooseNextRandomFile();

	FsFile theFileSD;

	String fullPathName;

private:
	std::vector<String> GifDirectories;
	std::vector<uint16_t> GifCount;
	std::vector<String> GifFiles;
};

#endif