#include <EEPROM.h>
#include "SPI.h"

//#define WITHOUT_WIFI 1

#include "Enums.hpp"
#include "FilesTools.hpp"
#include "ThePanel.hpp"
#ifndef WITHOUT_WIFI
#include "MyWifi.hpp"
#include "Ntp.hpp"
#endif
#include "TheGif.hpp"
#include "ClockAnalog.hpp"
#include "Solar.hpp"
#include "AudioSpectrum.hpp"
#include "TheAnim.hpp"

GLOBAL_STATE globalState;
FilesTools filesTools = FilesTools();
ThePanel thePanel = ThePanel();
#ifndef WITHOUT_WIFI
MyWifi myWifi = MyWifi();
Ntp ntp = Ntp();
#endif
TheGif theGif = TheGif();
ClockAnalog clockAnalog = ClockAnalog();
Solar solar = Solar();
AudioSpectrum audioSpectrum = AudioSpectrum();
TheAnim theAnim = TheAnim();

unsigned long stateStart;
unsigned long stateDelayToChange;

void setup()
{
	Serial.begin(115200);
	delay(2000);
	Serial.println("*** PIXEL 32 ***");

#ifndef WITHOUT_WIFI
	showBoot(10);
	myWifi.setup();
	showBoot(20);

	ntp.setup();
#endif

	showBoot(105);

	while (filesTools.setup() != OK)
	{
		showBoot(40);
		delay(2000);
		showBoot(30);
	}
	showBoot(50);

	if (filesTools.populateGif() == FAIL)
	{
		showBoot(60);
		while (true)
			yield();
	}
	showBoot(70);
	// filesTools.openFileSD(26);
	// showBoot(80);

	if (thePanel.setup(DISPLAY_COLOR_ORDER::RGB) == FAIL)
	{
		showBoot(90);
		while (true)
			yield();
	}

	showBoot(100);

	randomSeed(micros());

	theGif.setup(filesTools, thePanel);

	clockAnalog.setup(thePanel);
#ifndef WITHOUT_WIFI
	solar.setup(50.633954, 3.12954217, ntp.timeZone); // https://gml.noaa.gov/grad/solcalc/
	// solar.setup(45.55, -73.633);
#endif
	audioSpectrum.setup(thePanel);

	theAnim.setup(thePanel);
	theAnim.setPosition("88:88:88");
	theAnim.start("");

	launchNewGif();
	// launchNewClock();
	// launchNewAudioSpectrum();
}

uint16_t countFps;
uint16_t lastFps;
unsigned long startFps;

void loop()
{
	yield();
	thePanel.loop();
#ifndef WITHOUT_WIFI
	ntp.loop();
#endif
	thePanel.dma_display->startWrite();
	switch (globalState)
	{
	case GLOBAL_STATE::STATE_CLOCK:
#ifndef WITHOUT_WIFI
		if (clockAnalog.loop(ntp.timeZone) == OK)
		{
			stateDelayToChange = 0;
		}
#endif
		break;
	case GLOBAL_STATE::STATE_WEATHER:
		break;
	case GLOBAL_STATE::STATE_ANIMATEDGIF:
		if (theGif.loop() == OK)
		{
			// Serial.println("GIF done");
			if ((millis() - stateStart) < 30 * 1000)
			{
				stateDelayToChange += theGif.start();
			}
			else
			{
				stateDelayToChange = 0;
			}
		}
		break;
	case GLOBAL_STATE::STATE_AUDIO_SPECTRUM:
		audioSpectrum.loop();
		audioSpectrum.display();
		// stateStart = millis();
		break;
	}
	// no change: DEBUG!
	// stateStart = millis();

#ifndef WITHOUT_WIFI
	solar.loop();
#endif
	/*
	String toDisplay = zeropad(ntp.timeZone.hour(), 2) + ":" + zeropad(ntp.timeZone.minute(), 2) + ":" + zeropad(ntp.timeZone.second(), 2);
	theAnim.start(toDisplay);
	theAnim.loop();
	*/

	// audioSpectrum.isNoisy();
	/*
	if (audioSpectrum.isNoisy())
		thePanel.dma_display->fillRect(0, 0, 23, 7, 120, 0, 0);
	else
		thePanel.dma_display->fillRect(0, 0, 23, 7, 0, 120, 0);
	thePanel.dma_display->setFont(NULL);
	thePanel.dma_display->setTextSize(1); // 6x7
	thePanel.dma_display->setTextColor(thePanel.dma_display->color565(255, 255, 255));
	thePanel.dma_display->setCursor(0, 0);
	thePanel.dma_display->print(lastFps);
	*/
	//thePanel.dma_display->endWrite();

	countFps++;
	if ((millis() - startFps) > 1000)
	{
		lastFps = countFps;
		countFps = 0;
		startFps = millis();
	}

	// timeout, change display
	if ((millis() - stateStart) >= stateDelayToChange)
	{
		// Serial.print("Time to change... ");
		switch (globalState)
		{
		case GLOBAL_STATE::STATE_CLOCK:
			launchNewWeather();
			break;
		case GLOBAL_STATE::STATE_WEATHER:
			launchNewGif();
			break;
		case GLOBAL_STATE::STATE_ANIMATEDGIF:
			launchNewAudioSpectrum();
			break;
		case GLOBAL_STATE::STATE_AUDIO_SPECTRUM:
			launchNewClock();
			break;
		}
	}

	// int sensorValue = analogRead(36);
	// Serial.print(" " + String(sensorValue));
}

void showBoot(byte state)
{
	Serial.println(state);
}

void launchNewGif()
{
	// showBoot(110);
	stateDelayToChange = theGif.start();
	globalState = GLOBAL_STATE::STATE_ANIMATEDGIF;
	stateStart = millis();
}

void launchNewClock()
{
	// showBoot(120);
	globalState = GLOBAL_STATE::STATE_CLOCK;
	stateStart = millis();
#ifndef WITHOUT_WIFI
	stateDelayToChange = clockAnalog.start();
#else
	stateDelayToChange = 0;
#endif
}

void launchNewWeather()
{
	// showBoot(130);
	stateDelayToChange = 0;
	globalState = GLOBAL_STATE::STATE_WEATHER;
	stateStart = millis();
}

void launchNewAudioSpectrum()
{
	// showBoot(140);
	stateDelayToChange = 20000;
	globalState = GLOBAL_STATE::STATE_AUDIO_SPECTRUM;
	stateStart = millis();
}
// ***********************************************************************************************
