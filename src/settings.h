#ifndef SETTINGS_H
#define SETTINGS_H

//Weather Settings
#define CITY_ID "6094817" //Ottawa https://openweathermap.org/current#cityid
#define OPENWEATHERMAP_APIKEY "fac78a4b0d75aa574766d966dc147263" //era1112
#define OPENWEATHERMAP_URL "http://api.openweathermap.org/data/2.5/weather?id=" //open weather api
#define TEMP_UNIT "metric" //metric = Celsius , imperial = Fahrenheit
#define TEMP_LANG "en"
#define WEATHER_UPDATE_INTERVAL 30 //must be greater than 5, measured in minutes

//NTP Settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600 * -4 //Ottawa is UTC -4(DST)
#define DST_OFFSET_SEC 3600 

watchySettings settings{
  .cityID = CITY_ID,
  .weatherAPIKey = OPENWEATHERMAP_APIKEY,
  .weatherURL = OPENWEATHERMAP_URL,
  .weatherUnit = TEMP_UNIT,
  .weatherLang = TEMP_LANG,
  .weatherUpdateInterval = WEATHER_UPDATE_INTERVAL,
  .ntpServer = NTP_SERVER,
  .gmtOffset = GMT_OFFSET_SEC
};

#endif