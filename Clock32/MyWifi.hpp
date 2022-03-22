#pragma once
#ifndef MY_WIFI_HPP
#define MY_WIFI_HPP

#include "Arduino.h"
//#include <WiFi.h>
//#include <WiFiUdp.h>
#include "WiFiConnect.h"
//#include <WiFiClient.h>
#include <HTTPClient.h>

class MyWifi
{
public:
	MyWifi();
	void setup();
	void loop();
	void startWiFi(boolean showParams = false);

private:
	WiFiConnect wc;
};

#endif