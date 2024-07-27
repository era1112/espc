// @ENTRY POINT
// Note: platformIO calls frameworks before this
#ifndef ESPC_MAIN
#define ESPC_MAIN

//Weather Settings
#define CITY_ID "6094817" //Ottawa https://openweathermap.org/current#cityid // Not needed, collides with lat/lon
#define OPENWEATHERMAP_URL "http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&lang={lang}&units={units}&appid={apiKey}" //open weather api using lat lon
#define OPENWEATHERMAP_APIKEY "fac78a4b0d75aa574766d966dc147263" //era1112
#define TEMP_UNIT "metric" //metric = Celsius , imperial = Fahrenheit
#define TEMP_LANG "en"
#define WEATHER_UPDATE_INTERVAL 10 //must be greater than 5, measured in minutes

//NTP Settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600 * TIMEZONE
#define DST_OFFSET_SEC 3600   // @TODO: This should fix DST but it doesn't

#include "espc.h"

#endif // ESPC_MAIN

watchySettings theSettings{
  .cityID = CITY_ID,
  .weatherAPIKey = OPENWEATHERMAP_APIKEY,
  .weatherURL = OPENWEATHERMAP_URL,
  .weatherUnit = TEMP_UNIT,
  .weatherLang = TEMP_LANG,
  .weatherUpdateInterval = WEATHER_UPDATE_INTERVAL,
  .ntpServer = NTP_SERVER,
  .gmtOffset = GMT_OFFSET_SEC,
  .vibrateOClock = true
};

ESPC theESPC(theSettings);

void setup() { theESPC.init(); }

void loop() {}