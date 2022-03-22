#include <FilesTools.hpp>

SPIClass spiSD;
SdFs theSd;

// ***********************************************************************************************

// instanciate the object
FilesTools::FilesTools() {}

// ***********************************************************************************************

// initialize the SD card and retrieve basic informations
uint8_t FilesTools::setup()
{
	Serial.println(" * Loading FILESYSTEM");
	pinMode(SD_SS, OUTPUT);
	digitalWrite(SD_SS, HIGH);
	spiSD = SPIClass(HSPI);
	spiSD.begin(SD_SCK, SD_MISO, SD_MOSI, SD_SS);
	// spiSD = SPIClass(VSPI);
	// spiSD.begin(18, 19, 23, 5);
	SdSpiConfig sdConfig = SdSpiConfig(SD_SS, DEDICATED_SPI, SD_SCK_MHZ(16), &spiSD);
	if (!theSd.cardBegin(sdConfig))
	{
		Serial.println("SD Mount Failed");
		return BUSY;
	}

	cid_t m_cid;
	csd_t m_csd;
	uint32_t m_ocr;

	if (!theSd.card()->readCID(&m_cid) ||
		!theSd.card()->readCSD(&m_csd) ||
		!theSd.card()->readOCR(&m_ocr))
	{
		Serial.println("*5");
		return FAIL;
	}
	// ******* card type
	Serial.print(F("Card type: "));
	switch (theSd.card()->type())
	{
	case SD_CARD_TYPE_SD1:
		Serial.println(F("SD1"));
		break;
	case SD_CARD_TYPE_SD2:
		Serial.println(F("SD2"));
		break;
	case SD_CARD_TYPE_SDHC:
		if (sdCardCapacity(&m_csd) < 70000000)
		{
			Serial.println(F("SDHC"));
		}
		else
		{
			Serial.println(F("SDXC"));
		}
		break;
	default:
		Serial.println(F("Unknown"));
	}
	// ********** detail
	Serial.print(F("Manufacturer ID: 0x"));
	Serial.println(int(m_cid.mid), HEX);
	Serial.println("OEM ID: " + String(m_cid.oid[0]) + String(m_cid.oid[1]));
	Serial.print(F("Product: "));
	for (uint8_t i = 0; i < 5; i++)
	{
		Serial.print(m_cid.pnm[i]);
	}
	Serial.print(F("\nVersion: "));
	Serial.println(String(int(m_cid.prv_n)) + "." + String(int(m_cid.prv_m)));
	Serial.print("Serial number: 0x");
	Serial.println(m_cid.psn, HEX);
	Serial.print("Manufacturing date: " + String(int(m_cid.mdt_month)) + "/");
	Serial.println((2000 + m_cid.mdt_year_low + 10 * m_cid.mdt_year_high));
	// ******* volume
	if (!theSd.volumeBegin())
	{
		Serial.println(F("volumeBegin failed. Is the card formatted?"));
		return FAIL;
	}
	if (theSd.fatType() <= 32)
	{
		Serial.println("Volume is FAT" + String(int(theSd.fatType())));
	}
	else
	{
		Serial.println(F("Volume is exFAT"));
	}

	return OK;
}

// ***********************************************************************************************

// choose the new gif from available
void FilesTools::chooseNextRandomFile()
{
	uint16_t index = random(getNumberOfGifs());
	uint16_t baseDir = 0;
	uint16_t baseCountNext = 0;
	baseCountNext = GifCount.at(baseDir);
	while (baseCountNext <= index)
	{
		baseDir++;
		baseCountNext += GifCount.at(baseDir);
	}
	fullPathName = "/gif/" + GifDirectories.at(baseDir) + "/" + GifFiles.at(index) + ".GIF";
	Serial.println("#" + String(index) + ": " + fullPathName);
}

// ***********************************************************************************************

// search and memorize all gif files available on the SD card
uint8_t FilesTools::populateGif()
{
	FsFile directorySD;
	std::vector<String> directoryToParse;
	directoryToParse.push_back("/gif");
	while (directoryToParse.size() > 0)
	{
		String currentDirectory = directoryToParse.back();
		directoryToParse.pop_back();
		if (!directorySD.open(currentDirectory.c_str(), O_RDONLY))
		{
			Serial.println("*** peut pas ouvrir: " + currentDirectory);
			break;
		}
		FsFile fileSD;
		int countFiles = 0;
		char tempChar[100];
		while (fileSD.openNext(&directorySD, O_RDONLY))
		{
			if (countFiles > 700) // pas assez de mémoire !!!!!!!
				break;
			fileSD.getName(tempChar, 100);
			String filename = String(tempChar);
			if (fileSD.isDir())
			{
				directoryToParse.push_back(currentDirectory + "/" + filename);
			}
			else
			{
				String filenameString(filename);
				filename.toUpperCase();
				if (filename.endsWith(".GIF") == 1)
				{
					if (filenameString.length() > 3)
					{
						filename = filenameString.substring(0, filenameString.length() - 4);
						GifFiles.push_back(filename);
						countFiles++;
					}
					else
					{
						Serial.print("**BUG***" + filenameString + "***");
					}
				}
			}
			fileSD.close();
		}
		directorySD.rewindDirectory();
		if (countFiles > 0)
		{
			GifDirectories.push_back(currentDirectory.substring(5));
			GifCount.push_back(countFiles);
		}
	}
	directorySD.close();
	directoryToParse.clear();
	if (GifCount.size() == 0)
		return FAIL;
	Serial.println("Total GIFs: " + String(GifFiles.size()));
	return OK;
}

// ***********************************************************************************************

// get the number of gif files available
uint16_t FilesTools::getNumberOfGifs()
{
	return GifFiles.size();
}