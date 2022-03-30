#include "Ntp.hpp"

// ***********************************************************************************************

Ntp ::Ntp() {}

// ***********************************************************************************************

void Ntp::setup()
{
	// setDebug(DEBUG, Serial);
	setServer(NTP_SERVER_EU);
	setInterval(NTP_REFRESH);
	waitForSync(NTP_SYNC_TIMEOUT);
	Serial.println("UTC: " + UTC.dateTime());
	if(!timeZone.setLocation("Europe/Paris")) {
		timeZone.setPosix("CEST-02:00");
	}	Serial.println("Local time: " + timeZone.dateTime());
	timeZone.setDefault();
}

// ***********************************************************************************************

void Ntp::loop()
{
	events();
}