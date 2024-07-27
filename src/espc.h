#ifndef ESPC_FACE_H
#define ESPC_FACE_H

// latitude, longitude, timezone 
#define LAT 45.145805 // 416 @ Roger Stevens
#define LON -75.680771
#define TIMEZONE 4  //EDT is UTC -4(DST) vs -5 (EST)

#include <math.h>
#include <Watchy.h>
#include "Dusk2Dawn.h"
#include "moonPhaser.h"
#include "Font_S7_10pt7b.h"
#include "Font_S7_Classic_Regular_15.h"
#include "Font_S7_Classic_Bold_25.h"
#include "Font_S7_Classic_Regular_39.h"
#include "icons.h"

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

        void vibrate(int, int, int);
        void vibrateTime();
        virtual void handleButtonPress();

        // @TODO: Experiment with these
       void drawWeather();
       void drawEva();
       void drawLine();
};

#endif
