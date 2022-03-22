#pragma once
#ifndef SOLAR_HPP
#define SOLAR_HPP

#include "math.h"
#include "Arduino.h"

#include "ezTime.h"

// taken here : https://github.com/jpb10/SolarCalculator
// (c) jpb10, thanks

const double SUNRISESET_STD_ALTITUDE = -0.8333;
const double CIVIL_DAWNDUSK_STD_ALTITUDE = -6.0;
const double NAUTICAL_DAWNDUSK_STD_ALTITUDE = -12.0;
const double ASTRONOMICAL_DAWNDUSK_STD_ALTITUDE = -18.0;

//#define COMPUTE_EVERY 10000

class SunTime
{
public:
	SunTime(){};
	SunTime(uint8_t hour, uint8_t minute)
	{
		this->hour = hour;
		this->minute = minute;
		time = String(hour) + ":" + zeropad(minute, 2);
	}
	String getTime() { return time; };
	uint8_t hour;
	uint8_t minute;

private:
	String time;
};

class SunTimes
{
public:
	SunTimes(){};
	SunTimes(uint16_t year, uint8_t month, uint8_t day, SunTime sunDawn, SunTime sunDusk, SunTime sunRise, SunTime sunSet)
	{
		this->year = year;
		this->month = month;
		this->day = day;
		this->sunDawn = sunDawn;
		this->sunDusk = sunDusk;
		this->sunRise = sunRise;
		this->sunSet = sunSet;
	};
	uint16_t year;
	uint8_t month;
	uint8_t day;
	SunTime sunDawn;
	SunTime sunDusk;
	SunTime sunRise;
	SunTime sunSet;
	int16_t getSunDuration() { return (sunSet.hour * 60 + sunSet.minute) - (sunRise.hour * 60 + sunRise.minute); };
	int16_t sunDurationDelta;
	SunTime sunRiseNext;
	SunTime sunSetNext;
	int16_t getSunDurationNext() { return (sunSetNext.hour * 60 + sunSetNext.minute) - (sunRiseNext.hour * 60 + sunRiseNext.minute); };
	int16_t sunDurationDeltaNext;
};

class Solar
{
public:
	Solar();
	void setup(double latitude, double longitude, Timezone &timeZone);
	void loop();

	SunTimes today;

private:
	//======================================================================================================================
	// Intermediate calculations
	//
	// Time T is measured in Julian Centuries (36525 ephemeris days from the epoch J2000.0)
	//======================================================================================================================

	double wrapTo360(double angle);
	double wrapTo180(double angle);
	double between0And1(double n);
	double interpolateCoordinates(double n, double y1, double y2, double y3);
	double fractionalDay(int hour, int minute, int second);
	double calcJulianDay(int year, int month, int day);
	double calcJulianCent(double JD);
	double calcJulianCentSplit(double JD, double m);
	double calcGeomMeanLongSun(double T);
	double calcGeomMeanAnomalySun(double T);
	double calcEccentricityEarthOrbit(double T);
	double calcSunEqOfCenter(double T);
	double calcSunTrueLong(double T);
	double calcSunTrueAnomaly(double T);
	double calcSunRadVector(double T);
	double calcSunApparentLong(double T);
	double calcMeanObliquityOfEcliptic(double T);
	double calcNutationLongitude(double T);
	double calcNutationObliquity(double T);
	double calcObliquityCorrection(double T);
	double calcSunRtAscension(double T);
	double calcSunDeclination(double T);
	double calcNutationRtAscension(double T);
	double calcGrMeanSiderealTime(double JD);
	double calcGrApparentSiderealTime(double JD);
	double calcGrSiderealTimeInstant(double GAST, double m);
	double calcSunAzimuth(double HA, double decl, double lat);
	double calcSunElevation(double HA, double decl, double lat);
	double calcRefraction(double elev);
	double equationOfTimeSmart(double T);
	double equationOfTimeHughes(double T);
	double equationOfTimeMeeus(double T);
	double calcDeltaT(double year, double month);
	double calcDeltaTPoly(double year, double month);
	double degreesToHours(double deg);
	void printCoordHours(double d);
	void printCoordDegrees(double d);

	//======================================================================================================================
	// Solar calculator
	//
	// All calculations assume time inputs in Coordinated Universal Time (UTC)
	//
	// Results are passed by reference
	//======================================================================================================================

	// Calculate the equation of (ephemeris) time, in minutes of time
	//
	void calcEquationOfTime(int year, int month, int day, int hour, int minute, int second,
							double &E);

	// Calculate the Sun's right ascension and declination, in degrees
	//
	void calcEquatorialCoordinates(int year, int month, int day, int hour, int minute, int second,
								   double &rt_ascension, double &declination);

	// Calculate the Sun's azimuth and elevation (altitude), in degrees
	//
	void calcHorizontalCoordinates(int year, int month, int day, int hour, int minute, int second,
								   double latitude, double longitude,
								   double &azimuth, double &elevation);

	// Calculate the Sun's radius vector (distance), in AUs
	//
	void calcSunRadiusVector(int year, int month, int day, int hour, int minute, int second,
							 double &radius_vector);

	// Calculate the Sun's times of rising, transit and setting, in hours
	//
	void calcSunriseSunset(int year, int month, int day, double latitude, double longitude,
						   double &transit, double &sunrise, double &sunset,
						   double altitude = SUNRISESET_STD_ALTITUDE);

	// Calculate the times of civil, nautical and astronomical dawn and dusk, in hours
	//
	void calcCivilDawnDusk(int year, int month, int day, double latitude, double longitude,
						   double &transit, double &dawn, double &dusk);

	void calcNauticalDawnDusk(int year, int month, int day, double latitude, double longitude,
							  double &transit, double &dawn, double &dusk);

	void calcAstronomicalDawnDusk(int year, int month, int day, double latitude, double longitude,
								  double &transit, double &dawn, double &dusk);

	/*

	*/
	SunTime toLocalTZ(int16_t timeZone, double value);
	SunTimes calculate(Timezone timeZone);
	String on2(uint8_t value);

	// unsigned long lastCompute;
	Timezone *timeZone;
	double latitude;
	double longitude;
};

#endif