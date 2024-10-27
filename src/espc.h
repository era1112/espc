#ifndef ESPC_H
#define ESPC_H

#include <math.h>
#include <Watchy.h>
#include "Dusk2Dawn.h"
#include "moonPhaser.h"
#include "Font_S7_10pt7b.h"
#include "Font_S7_Classic_Regular_15.h"
#include "Font_S7_Classic_Bold_25.h"
#include "Font_S7_Classic_Regular_39.h"
#include "icons.h"

//Weather Settings
#define CITY_ID "6094817" //Ottawa https://openweathermap.org/current#cityid // Not needed, collides with lat/lon
#define OPENWEATHERMAP_URL "http://api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}&lang={lang}&units={units}&appid={apiKey}" //open weather api using lat lon
#define OPENWEATHERMAP_APIKEY "xxx" //era1112
#define TEMP_UNIT "metric" //metric = Celsius , imperial = Fahrenheit
#define TEMP_LANG "en"
#define WEATHER_UPDATE_INTERVAL 10 //must be greater than 5, measured in minutes

//NTP Settings
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC 3600 * TIMEZONE
#define DST_OFFSET_SEC 3600   // @TODO: This should fix DST but it doesn't

// latitude, longitude, timezone 
#define LAT 45.145805 // 416 @ Roger Stevens
#define LON -75.680771
#define TIMEZONE 4  //EDT is UTC -4(DST) vs -5 (EST)

// Use these macros during display calls to maintain light/dark theme
#define BG_COLOR (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define FG_COLOR (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)

RTC_DATA_ATTR bool DARKMODE = false;
RTC_DATA_ATTR bool HOUR_SET = false; // 12 hour mode
RTC_DATA_ATTR int showState = 0;

// @TODO: Some display calls in Watchy.cpp still need to get overridden to use dark/light mode properly
class ESPC : public Watchy{
    using Watchy::Watchy;
    public:
        void drawWatchFace();
        void drawTime();
        void drawDate();
        void drawSteps();
        void drawBattery();
        void drawFiel();
        void drawMoon();
        void drawSun();
        void handleButtonPress();
        void showMenu(byte, bool);
        void showFastMenu(byte);

        void vibrate(int, int, int);
        void vibrateTime();

        // @TODO: Experiment with these
       void drawWeather();
       void drawEva();
       void drawLine();
};

#endif  // ESPC_H
