#include <MyWifi.hpp>

void configModeCallback(WiFiConnect *mWiFiConnect)
{
	Serial.println("*** Entering Access Point ***");
	// afficher à l'écran qu'il faut se connecter sur ESP_3400229412 (192.168.4.1)
}

// ***********************************************************************************************

MyWifi::MyWifi() {}

// ***********************************************************************************************

void MyWifi::setup()
{
	startWiFi(true);
}

// ***********************************************************************************************

void MyWifi::loop()
{
}

// ***********************************************************************************************

void MyWifi::startWiFi(boolean showParams)
{
	// showBoot(10);
	WiFi.enableIpV6();
	wc.setDebug(true);
	/* Set our callbacks */
	wc.setAPCallback(configModeCallback);
	// wc.resetSettings(); // helper to remove the stored wifi connection, comment out after first upload and re upload
	/*
		 AP_NONE = Continue executing code
		 AP_LOOP = Trap in a continuous loop - Device is useless
		 AP_RESET = Restart the chip
		 AP_WAIT  = Trap in a continuous loop with captive portal until we have a working WiFi connection
	  */
	if (!wc.autoConnect())
	{ // try to connect to wifi
		// showBoot(15);
		/* We could also use button etc. to trigger the portal on demand within main loop */
		wc.startConfigurationPortal(AP_WAIT); // if not connected show the configuration portal
	}

	/*if (WiFi.status() != WL_CONNECTED)
	{
	  if (!wc.autoConnect())
		wc.startConfigurationPortal(AP_WAIT);
	}*/
	// showBoot(20);

	if (WiFi.status() == WL_CONNECTED)
		Serial.println("@IPv4: " + wc.toStringIp(WiFi.localIP()));
	else
	{
		Serial.println("@IPv4: " + String(wc.statusToString(WiFi.status())));
		Serial.println("@IPv4: " + wc.toStringIp(WiFi.localIP()));
	}
}