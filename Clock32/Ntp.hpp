#pragma once
#ifndef NTP_HPP
#define NTP_HPP

#include "Arduino.h"

#include <ezTime.h>

#define NTP_SERVER_EU "europe.pool.ntp.org"
#define NTP_REFRESH 3 * 60 * 60
#define NTP_SYNC_TIMEOUT 10 * 1000

class Ntp
{
public:
	Ntp();
	void setup();
	void loop();
	Timezone timeZone;
};

#endif