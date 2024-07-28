#include "espc.h"

moonPhaser moonP;

void ESPC::drawWatchFace()
{
    display.fillScreen(BG_COLOR);
    display.setTextColor(FG_COLOR);
    drawFiel ();
    drawTime();
    drawDate();
    drawSteps();
    drawBattery();
    // drawWeather();
    // drawEva();
    // drawLine();

    display.drawBitmap(118, 168, WIFI_CONFIGURED ? wifi : wifioff, 25, 18, FG_COLOR);
    // if(BLE_CONFIGURED)
    // {
    //    display.drawBitmap(100, 75, bluetooth, 13, 21, FG_COLOR);
    // }
    drawMoon();
    drawSun();
}

// Vibrate function
void ESPC::vibrate(int times, int buzz_duration, int delay_duration)
{
  pinMode(VIB_MOTOR_PIN, OUTPUT);
  for (int i = 0; i < (times); i++)
  {
    digitalWrite(VIB_MOTOR_PIN, true);
    delay(buzz_duration);
    digitalWrite(VIB_MOTOR_PIN, false);
    delay(delay_duration);
  }
}

// Tap out the time
void ESPC::vibrateTime()
{
  // Vibrate once per hour (am/pm)
  int twelveHr = currentTime.Hour % 12;
  vibrate(twelveHr, 300, 300);

  delay(1000);

  // Vibrate once per 15 min after the hr
  int quarterHour = currentTime.Minute / 15;
  vibrate(quarterHour, 200, 200);

  delay(1000);

  // Vibrate once per 5 min after the 15
  int fiveMins = (currentTime.Minute - (quarterHour * 15)) / 5;
  vibrate(fiveMins, 100, 100);
}

//----- OVERRIDES TO WATCHY FUNCS -----//
void ESPC::handleButtonPress()
{
    if (guiState == WATCHFACE_STATE)
    {
        uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();
        
        // Up Button: Vibrate time of day
        if (wakeupBit & UP_BTN_MASK) 
        {
            showState++;
            if (showState > 2) { showState = 0; }
            ; HOUR_SET = !HOUR_SET;
            RTC.read(currentTime);
            showWatchFace(true);
            vibrateTime(); 
            return;
        }

        // Down Button: Change 12/24
        if (wakeupBit & DOWN_BTN_MASK) 
        {
            showState--;
            if (showState < 0) { showState = 2; }
            HOUR_SET = !HOUR_SET;
            RTC.read(currentTime);
            showWatchFace(true);
            return;
        }

        // Back Button: Invert dark mode
        if (wakeupBit & BACK_BTN_MASK) 
        {
            DARKMODE = !DARKMODE;
            RTC.read(currentTime);
            showWatchFace(true);
            return;
        } 

        // Menu Button: Call menu 
        if (wakeupBit & MENU_BTN_MASK) 
        {
            Watchy::handleButtonPress();
            return;
        }
    }
    else {Watchy::handleButtonPress();}
    return;
}

void ESPC::showMenu(byte menuIndex, bool partialRefresh) {
  display.setFullWindow();
  display.fillScreen(BG_COLOR);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(FG_COLOR);

  int16_t x1, y1;
  uint16_t w, h;
  int16_t yPos;

  const char *menuItems[] = {
      "About Watchy", "Vibrate Motor", "Show Accelerometer",
      "Set Time",     "Setup WiFi",    "Update Firmware",
      "Sync NTP"};
  for (int i = 0; i < MENU_LENGTH; i++) {
    yPos = MENU_HEIGHT + (MENU_HEIGHT * i);
    display.setCursor(0, yPos);
    if (i == menuIndex) {
      display.getTextBounds(menuItems[i], 0, yPos, &x1, &y1, &w, &h);
      display.fillRect(x1 - 1, y1 - 10, 200, h + 15, FG_COLOR);
      display.setTextColor(BG_COLOR);
      display.println(menuItems[i]);
    } else {
      display.setTextColor(FG_COLOR);
      display.println(menuItems[i]);
    }
  }
}

void ESPC::showFastMenu(byte menuIndex) {
  display.setFullWindow();
  display.fillScreen(BG_COLOR);
  display.setFont(&FreeMonoBold9pt7b);

  int16_t x1, y1;
  uint16_t w, h;
  int16_t yPos;

  const char *menuItems[] = {
      "About Watchy", "Vibrate Motor", "Show Accelerometer",
      "Set Time",     "Setup WiFi",    "Update Firmware",
      "Sync NTP"};
  for (int i = 0; i < MENU_LENGTH; i++) {
    yPos = MENU_HEIGHT + (MENU_HEIGHT * i);
    display.setCursor(0, yPos);
    if (i == menuIndex) {
      display.getTextBounds(menuItems[i], 0, yPos, &x1, &y1, &w, &h);
      display.fillRect(x1 - 1, y1 - 10, 200, h + 15, FG_COLOR);
      display.setTextColor(BG_COLOR);
      display.println(menuItems[i]);
    } else {
      display.setTextColor(FG_COLOR);
      display.println(menuItems[i]);
    }
  }

  display.display(true);

  guiState = MAIN_MENU_STATE;
}

void ESPC::drawTime()
{
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setCursor(6, 53+5);
    long ss = currentTime.Hour * 60 + currentTime.Minute;
    int sh = ss / 60;

    if (HOUR_SET == false && sh >= 12)
    {
        display.fillRect(7, 60, 25, 9, BG_COLOR);
        display.drawBitmap(7, 60, pm, 25, 9, FG_COLOR);
    }
    else if (HOUR_SET == false && sh < 12)
    {
        display.fillRect(7, 60, 25, 9, BG_COLOR);
        display.drawBitmap(7, 60, am, 25, 9, FG_COLOR);
    }

    if (HOUR_SET == false && sh > 12)
    {
        sh -= 12;
    }
    int sm = ss % 60;
    int a = sh >= 10 ? sh / 10 : 0;
    int b = sh % 10;
    int c = sm >= 10 ? sm / 10 : 0;
    int d = sm % 10;

    if (a == 0)
        display.drawBitmap(11, 5, fd_0, 33, 53, FG_COLOR);
    else if (a == 1)
        display.drawBitmap(11, 5, fd_1, 33, 53, FG_COLOR);
    else if (a == 2)
        display.drawBitmap(11, 5, fd_2, 33, 53, FG_COLOR);
    else if (a == 3)
        display.drawBitmap(11, 5, fd_3, 33, 53, FG_COLOR);
    else if (a == 4)
        display.drawBitmap(11, 5, fd_4, 33, 53, FG_COLOR);
    else if (a == 5)
        display.drawBitmap(11, 5, fd_5, 33, 53, FG_COLOR);
    else if (a == 6)
        display.drawBitmap(11, 5, fd_6, 33, 53, FG_COLOR);
    else if (a == 7)
        display.drawBitmap(11, 5, fd_7, 33, 53, FG_COLOR);
    else if (a == 8)
        display.drawBitmap(11, 5, fd_8, 33, 53, FG_COLOR);
    else if (a == 9)
        display.drawBitmap(11, 5, fd_9, 33, 53, FG_COLOR);

    if (b == 0)
        display.drawBitmap(55, 5, fd_0, 33, 53, FG_COLOR);
    else if (b == 1)
        display.drawBitmap(55, 5, fd_1, 33, 53, FG_COLOR);
    else if (b == 2)
        display.drawBitmap(55, 5, fd_2, 33, 53, FG_COLOR);
    else if (b == 3)
        display.drawBitmap(55, 5, fd_3, 33, 53, FG_COLOR);
    else if (b == 4)
        display.drawBitmap(55, 5, fd_4, 33, 53, FG_COLOR);
    else if (b == 5)
        display.drawBitmap(55, 5, fd_5, 33, 53, FG_COLOR);
    else if (b == 6)
        display.drawBitmap(55, 5, fd_6, 33, 53, FG_COLOR);
    else if (b == 7)
        display.drawBitmap(55, 5, fd_7, 33, 53, FG_COLOR);
    else if (b == 8)
        display.drawBitmap(55, 5, fd_8, 33, 53, FG_COLOR);
    else if (b == 9)
        display.drawBitmap(55, 5, fd_9, 33, 53, FG_COLOR);

    if (c == 0)
        display.drawBitmap(111, 5, fd_0, 33, 53, FG_COLOR);
    else if (c == 1)
        display.drawBitmap(111, 5, fd_1, 33, 53, FG_COLOR);
    else if (c == 2)
        display.drawBitmap(111, 5, fd_2, 33, 53, FG_COLOR);
    else if (c == 3)
        display.drawBitmap(111, 5, fd_3, 33, 53, FG_COLOR);
    else if (c == 4)
        display.drawBitmap(111, 5, fd_4, 33, 53, FG_COLOR);
    else if (c == 5)
        display.drawBitmap(111, 5, fd_5, 33, 53, FG_COLOR);
    else if (c == 6)
        display.drawBitmap(111, 5, fd_6, 33, 53, FG_COLOR);
    else if (c == 7)
        display.drawBitmap(111, 5, fd_7, 33, 53, FG_COLOR);
    else if (c == 8)
        display.drawBitmap(111, 5, fd_8, 33, 53, FG_COLOR);
    else if (c == 9)
        display.drawBitmap(111, 5, fd_9, 33, 53, FG_COLOR);

    if (d == 0)
        display.drawBitmap(155, 5, fd_0, 33, 53, FG_COLOR);
    else if (d == 1)
        display.drawBitmap(155, 5, fd_1, 33, 53, FG_COLOR);
    else if (d == 2)
        display.drawBitmap(155, 5, fd_2, 33, 53, FG_COLOR);
    else if (d == 3)
        display.drawBitmap(155, 5, fd_3, 33, 53, FG_COLOR);
    else if (d == 4)
        display.drawBitmap(155, 5, fd_4, 33, 53, FG_COLOR);
    else if (d == 5)
        display.drawBitmap(155, 5, fd_5, 33, 53, FG_COLOR);
    else if (d == 6)
        display.drawBitmap(155, 5, fd_6, 33, 53, FG_COLOR);
    else if (d == 7)
        display.drawBitmap(155, 5, fd_7, 33, 53, FG_COLOR);
    else if (d == 8)
        display.drawBitmap(155, 5, fd_8, 33, 53, FG_COLOR);
    else if (d == 9)
        display.drawBitmap(155, 5, fd_9, 33, 53, FG_COLOR);
}

void ESPC::drawDate()
{
    display.setFont(&Seven_Segment10pt7b);

    int16_t  x1, y1;
    uint16_t w, h;

    String dayOfWeek = dayStr(currentTime.Wday);
    dayOfWeek = dayOfWeek.substring(0, 2);
    display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
    if(currentTime.Wday == 4)
    {
        w = w - 5;
    }
    display.setCursor(76 - w, 86);
    display.println(dayOfWeek);

    String month = monthShortStr(currentTime.Month);
    display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
    display.setCursor(79 - w, 110);
    display.println(month);

    int da = currentTime.Day;
    int ye = currentTime.Year + 1970;

    int a = da / 10;
    int b = da % 10;
    int c = ye / 1000;
    ye = ye % 1000;
    int d = ye / 100;
    ye = ye % 100;
    int e = ye / 10;
    ye = ye % 10;
    int f = ye;

    if (a == 0)
        display.drawBitmap(8, 95, dd_0, 16, 25, FG_COLOR);
    else if (a == 1)
        display.drawBitmap(8, 95, dd_1, 16, 25, FG_COLOR);
    else if (a == 2)
        display.drawBitmap(8, 95, dd_2, 16, 25, FG_COLOR);
    else if (a == 3)
        display.drawBitmap(8, 95, dd_3, 16, 25, FG_COLOR);
    else if (a == 4)
        display.drawBitmap(8, 95, dd_4, 16, 25, FG_COLOR);
    else if (a == 5)
        display.drawBitmap(8, 95, dd_5, 16, 25, FG_COLOR);
    else if (a == 6)
        display.drawBitmap(8, 95, dd_6, 16, 25, FG_COLOR);
    else if (a == 7)
        display.drawBitmap(8, 95, dd_7, 16, 25, FG_COLOR);
    else if (a == 8)
        display.drawBitmap(8, 95, dd_8, 16, 25, FG_COLOR);
    else if (a == 9)
        display.drawBitmap(8, 95, dd_9, 16, 25, FG_COLOR);

    if (b == 0)
        display.drawBitmap(29, 95, dd_0, 16, 25, FG_COLOR);
    else if (b == 1)
        display.drawBitmap(29, 95, dd_1, 16, 25, FG_COLOR);
    else if (b == 2)
        display.drawBitmap(29, 95, dd_2, 16, 25, FG_COLOR);
    else if (b == 3)
        display.drawBitmap(29, 95, dd_3, 16, 25, FG_COLOR);
    else if (b == 4)
        display.drawBitmap(29, 95, dd_4, 16, 25, FG_COLOR);
    else if (b == 5)
        display.drawBitmap(29, 95, dd_5, 16, 25, FG_COLOR);
    else if (b == 6)
        display.drawBitmap(29, 95, dd_6, 16, 25, FG_COLOR);
    else if (b == 7)
        display.drawBitmap(29, 95, dd_7, 16, 25, FG_COLOR);
    else if (b == 8)
        display.drawBitmap(29, 95, dd_8, 16, 25, FG_COLOR);
    else if (b == 9)
        display.drawBitmap(29, 95, dd_9, 16, 25, FG_COLOR);

    if (c == 0)
        display.drawBitmap(8, 129, dd_0, 16, 25, FG_COLOR);
    else if (c == 1)
        display.drawBitmap(8, 129, dd_1, 16, 25, FG_COLOR);
    else if (c == 2)
        display.drawBitmap(8, 129, dd_2, 16, 25, FG_COLOR);
    else if (c == 3)
        display.drawBitmap(8, 129, dd_3, 16, 25, FG_COLOR);
    else if (c == 4)
        display.drawBitmap(8, 129, dd_4, 16, 25, FG_COLOR);
    else if (c == 5)
        display.drawBitmap(8, 129, dd_5, 16, 25, FG_COLOR);
    else if (c == 6)
        display.drawBitmap(8, 129, dd_6, 16, 25, FG_COLOR);
    else if (c == 7)
        display.drawBitmap(8, 129, dd_7, 16, 25, FG_COLOR);
    else if (c == 8)
        display.drawBitmap(8, 129, dd_8, 16, 25, FG_COLOR);
    else if (c == 9)
        display.drawBitmap(8, 129, dd_9, 16, 25, FG_COLOR);

    if (d == 0)
        display.drawBitmap(29, 129, dd_0, 16, 25, FG_COLOR);
    else if (d == 1)
        display.drawBitmap(29, 129, dd_1, 16, 25, FG_COLOR);
    else if (d == 2)
        display.drawBitmap(29, 129, dd_2, 16, 25, FG_COLOR);
    else if (d == 3)
        display.drawBitmap(29, 129, dd_3, 16, 25, FG_COLOR);
    else if (d == 4)
        display.drawBitmap(29, 129, dd_4, 16, 25, FG_COLOR);
    else if (d == 5)
        display.drawBitmap(29, 129, dd_5, 16, 25, FG_COLOR);
    else if (d == 6)
        display.drawBitmap(29, 129, dd_6, 16, 25, FG_COLOR);
    else if (d == 7)
        display.drawBitmap(29, 129, dd_7, 16, 25, FG_COLOR);
    else if (d == 8)
        display.drawBitmap(29, 129, dd_8, 16, 25, FG_COLOR);
    else if (d == 9)
        display.drawBitmap(29, 129, dd_9, 16, 25, FG_COLOR);

    if (e == 0)
        display.drawBitmap(50, 129, dd_0, 16, 25, FG_COLOR);
    else if (e == 1)
        display.drawBitmap(50, 129, dd_1, 16, 25, FG_COLOR);
    else if (e == 2)
        display.drawBitmap(50, 129, dd_2, 16, 25, FG_COLOR);
    else if (e == 3)
        display.drawBitmap(50, 129, dd_3, 16, 25, FG_COLOR);
    else if (e == 4)
        display.drawBitmap(50, 129, dd_4, 16, 25, FG_COLOR);
    else if (e == 5)
        display.drawBitmap(50, 129, dd_5, 16, 25, FG_COLOR);
    else if (e == 6)
        display.drawBitmap(50, 129, dd_6, 16, 25, FG_COLOR);
    else if (e == 7)
        display.drawBitmap(50, 129, dd_7, 16, 25, FG_COLOR);
    else if (e == 8)
        display.drawBitmap(50, 129, dd_8, 16, 25, FG_COLOR);
    else if (e == 9)
        display.drawBitmap(50, 129, dd_9, 16, 25, FG_COLOR);

    if (f == 0)
        display.drawBitmap(71, 129, dd_0, 16, 25, FG_COLOR);
    else if (f == 1)
        display.drawBitmap(71, 129, dd_1, 16, 25, FG_COLOR);
    else if (f == 2)
        display.drawBitmap(71, 129, dd_2, 16, 25, FG_COLOR);
    else if (f == 3)
        display.drawBitmap(71, 129, dd_3, 16, 25, FG_COLOR);
    else if (f == 4)
        display.drawBitmap(71, 129, dd_4, 16, 25, FG_COLOR);
    else if (f == 5)
        display.drawBitmap(71, 129, dd_5, 16, 25, FG_COLOR);
    else if (f == 6)
        display.drawBitmap(71, 129, dd_6, 16, 25, FG_COLOR);
    else if (f == 7)
        display.drawBitmap(71, 129, dd_7, 16, 25, FG_COLOR);
    else if (f == 8)
        display.drawBitmap(71, 129, dd_8, 16, 25, FG_COLOR);
    else if (f == 9)
        display.drawBitmap(71, 129, dd_9, 16, 25, FG_COLOR);
}

void ESPC::drawSteps()
{
    // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0)
    {
        // st4 = st3;
        // st3 = st2;
        // st2 = st1;
        // st1 = stepCount;
        sensor.resetStepCounter();
        // stepCount = 0;
    }
    uint32_t stepCount = sensor.getCounter();

    // display.drawBitmap(10, 165, steps, 19, 23, FG_COLOR);
    // display.setCursor(6, 190);
    // if (stepCount >= 10000)
    //     ;
    // else if (stepCount >= 1000)
    //     display.print("0");
    // else if (stepCount >= 100)
    //     display.print("00");
    // else if (stepCount >= 10)
    //     display.print("000");
    // else if (stepCount >= 0)
    //     display.print("0000");
    // display.println(stepCount);

    // uint32_t l1 = 61 * st1 / 20000;
    // uint32_t l2 = 61 * st2 / 20000;
    // uint32_t l3 = 61 * st3 / 20000;
    // uint32_t l4 = 61 * st4 / 20000;
    uint32_t l5 = 61 * stepCount / 10000;

    if (l5 > 61)
    {
        l5 = 61;
    }

    // display.fillRect(125, 73 + 61 - l1, 9, l4, FG_COLOR);
    // display.fillRect(143, 73 + 61 - l1, 9, l3, FG_COLOR);
    // display.fillRect(161, 73 + 61 - l1, 9, l2, FG_COLOR);
    // display.fillRect(179, 73 + 61 - l1, 9, l1, FG_COLOR);
    display.fillRect(131, 148, l5, 9, FG_COLOR);
    // display.

    int a = stepCount / 10000;
    stepCount = stepCount % 10000;
    int b = stepCount / 1000;
    stepCount = stepCount % 1000;
    int c = stepCount / 100;
    stepCount = stepCount % 100;
    int d = stepCount / 10;
    int e = stepCount % 10;

    if (a == 0)
        display.drawBitmap(8, 165, dd_0, 16, 25, FG_COLOR);
    else if (a == 1)
        display.drawBitmap(8, 165, dd_1, 16, 25, FG_COLOR);
    else if (a == 2)
        display.drawBitmap(8, 165, dd_2, 16, 25, FG_COLOR);
    else if (a == 3)
        display.drawBitmap(8, 165, dd_3, 16, 25, FG_COLOR);
    else if (a == 4)
        display.drawBitmap(8, 165, dd_4, 16, 25, FG_COLOR);
    else if (a == 5)
        display.drawBitmap(8, 165, dd_5, 16, 25, FG_COLOR);
    else if (a == 6)
        display.drawBitmap(8, 165, dd_6, 16, 25, FG_COLOR);
    else if (a == 7)
        display.drawBitmap(8, 165, dd_7, 16, 25, FG_COLOR);
    else if (a == 8)
        display.drawBitmap(8, 165, dd_8, 16, 25, FG_COLOR);
    else if (a == 9)
        display.drawBitmap(8, 165, dd_9, 16, 25, FG_COLOR);

    if (b == 0)
        display.drawBitmap(29, 165, dd_0, 16, 25, FG_COLOR);
    else if (b == 1)
        display.drawBitmap(29, 165, dd_1, 16, 25, FG_COLOR);
    else if (b == 2)
        display.drawBitmap(29, 165, dd_2, 16, 25, FG_COLOR);
    else if (b == 3)
        display.drawBitmap(29, 165, dd_3, 16, 25, FG_COLOR);
    else if (b == 4)
        display.drawBitmap(29, 165, dd_4, 16, 25, FG_COLOR);
    else if (b == 5)
        display.drawBitmap(29, 165, dd_5, 16, 25, FG_COLOR);
    else if (b == 6)
        display.drawBitmap(29, 165, dd_6, 16, 25, FG_COLOR);
    else if (b == 7)
        display.drawBitmap(29, 165, dd_7, 16, 25, FG_COLOR);
    else if (b == 8)
        display.drawBitmap(29, 165, dd_8, 16, 25, FG_COLOR);
    else if (b == 9)
        display.drawBitmap(29, 165, dd_9, 16, 25, FG_COLOR);

    if (c == 0)
        display.drawBitmap(50, 165, dd_0, 16, 25, FG_COLOR);
    else if (c == 1)
        display.drawBitmap(50, 165, dd_1, 16, 25, FG_COLOR);
    else if (c == 2)
        display.drawBitmap(50, 165, dd_2, 16, 25, FG_COLOR);
    else if (c == 3)
        display.drawBitmap(50, 165, dd_3, 16, 25, FG_COLOR);
    else if (c == 4)
        display.drawBitmap(50, 165, dd_4, 16, 25, FG_COLOR);
    else if (c == 5)
        display.drawBitmap(50, 165, dd_5, 16, 25, FG_COLOR);
    else if (c == 6)
        display.drawBitmap(50, 165, dd_6, 16, 25, FG_COLOR);
    else if (c == 7)
        display.drawBitmap(50, 165, dd_7, 16, 25, FG_COLOR);
    else if (c == 8)
        display.drawBitmap(50, 165, dd_8, 16, 25, FG_COLOR);
    else if (c == 9)
        display.drawBitmap(50, 165, dd_9, 16, 25, FG_COLOR);

    if (d == 0)
        display.drawBitmap(71, 165, dd_0, 16, 25, FG_COLOR);
    else if (d == 1)
        display.drawBitmap(71, 165, dd_1, 16, 25, FG_COLOR);
    else if (d == 2)
        display.drawBitmap(71, 165, dd_2, 16, 25, FG_COLOR);
    else if (d == 3)
        display.drawBitmap(71, 165, dd_3, 16, 25, FG_COLOR);
    else if (d == 4)
        display.drawBitmap(71, 165, dd_4, 16, 25, FG_COLOR);
    else if (d == 5)
        display.drawBitmap(71, 165, dd_5, 16, 25, FG_COLOR);
    else if (d == 6)
        display.drawBitmap(71, 165, dd_6, 16, 25, FG_COLOR);
    else if (d == 7)
        display.drawBitmap(71, 165, dd_7, 16, 25, FG_COLOR);
    else if (d == 8)
        display.drawBitmap(71, 165, dd_8, 16, 25, FG_COLOR);
    else if (d == 9)
        display.drawBitmap(71, 165, dd_9, 16, 25, FG_COLOR);

    if (e == 0)
        display.drawBitmap(92, 165, dd_0, 16, 25, FG_COLOR);
    else if (e == 1)
        display.drawBitmap(92, 165, dd_1, 16, 25, FG_COLOR);
    else if (e == 2)
        display.drawBitmap(92, 165, dd_2, 16, 25, FG_COLOR);
    else if (e == 3)
        display.drawBitmap(92, 165, dd_3, 16, 25, FG_COLOR);
    else if (e == 4)
        display.drawBitmap(92, 165, dd_4, 16, 25, FG_COLOR);
    else if (e == 5)
        display.drawBitmap(92, 165, dd_5, 16, 25, FG_COLOR);
    else if (e == 6)
        display.drawBitmap(92, 165, dd_6, 16, 25, FG_COLOR);
    else if (e == 7)
        display.drawBitmap(92, 165, dd_7, 16, 25, FG_COLOR);
    else if (e == 8)
        display.drawBitmap(92, 165, dd_8, 16, 25, FG_COLOR);
    else if (e == 9)
        display.drawBitmap(92, 165, dd_9, 16, 25, FG_COLOR);
}

void ESPC::drawBattery()
{
    // display.drawBitmap(154, 73, battery, 37, 21, FG_COLOR);
    // display.fillRect(159, 78, 27, BATTERY_SEGMENT_HEIGHT, BG_COLOR);//clear battery segments
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.1)
    {
        batteryLevel = 37;
    }
    else if(VBAT > 4.05 && VBAT <= 4.1)
    {
        batteryLevel = 33;
    }
    else if(VBAT > 4 && VBAT <= 4.05)
    {
        batteryLevel = 28;
    }

    else if(VBAT > 3.95 && VBAT <= 4)
    {
        batteryLevel = 23;
    }
    else if(VBAT > 3.9 && VBAT <= 3.95)
    {
        batteryLevel = 18;
    }
    else if(VBAT > 3.85 && VBAT <= 3.9)
    {
        batteryLevel = 13;
    }
    else if(VBAT > 3.8 && VBAT <= 3.85)
    {
        batteryLevel = 8;
    }
    else if(VBAT > 3.75 && VBAT <= 3.8)
    {
        batteryLevel = 4;
    }
    else if(VBAT <= 3.75)
    {
        batteryLevel = 0;
    }

    // for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++)
    // {
        display.fillRect(155, 169, batteryLevel, 15, FG_COLOR);
    // }
}

void ESPC::drawFiel()
{
    display.drawBitmap(0, 0, field, 200, 200, FG_COLOR);
}

void ESPC::drawMoon() {
    moonData_t moon;
    
    // January 31st, 2020 @ 1:30PM UTC
    int year = currentTime.Year + 1970;
    int32_t month = currentTime.Month;
    int32_t day = currentTime.Day;
    double hour = currentTime.Hour + 0.1;

    moon = moonP.getPhase(year, month, day, hour);

    int ag = moon.angle;
    double lt = moon.percentLit;

    // Waxing: 0-180
    // Waning: 180-360
    // display.setCursor(100, 74);

    if (ag <= 180)
    {
        if (lt < 0.1)
            display.drawBitmap(131, 74, luna1, 61, 61, FG_COLOR);
        else if (lt < 0.25)
            display.drawBitmap(131, 74, luna12, 61, 61, FG_COLOR);
        else if (lt < 0.4)
            display.drawBitmap(131, 74, luna11, 61, 61, FG_COLOR);
        else if (lt < 0.6)
            display.drawBitmap(131, 74, luna10, 61, 61, FG_COLOR);
        else if (lt < 0.75)
            display.drawBitmap(131, 74, luna9, 61, 61, FG_COLOR);
        else if (lt < 0.9)
            display.drawBitmap(131, 74, luna8, 61, 61, FG_COLOR);
        else 
            display.drawBitmap(131, 74, luna7, 61, 61, FG_COLOR);
    }
    else
    {
        if (lt < 0.1)
            display.drawBitmap(131, 74, luna1, 61, 61, FG_COLOR);
        else if (lt < 0.25)
            display.drawBitmap(131, 74, luna2, 61, 61, FG_COLOR);
        else if (lt < 0.4)
            display.drawBitmap(131, 74, luna3, 61, 61, FG_COLOR);
        else if (lt < 0.6)
            display.drawBitmap(131, 74, luna4, 61, 61, FG_COLOR);
        else if (lt < 0.75)
            display.drawBitmap(131, 74, luna5, 61, 61, FG_COLOR);
        else if (lt < 0.9)
            display.drawBitmap(131, 74, luna6, 61, 61, FG_COLOR);
        else 
            display.drawBitmap(131, 74, luna7, 61, 61, FG_COLOR);
    }

    // display.print(moon.angle);
    // display.setCursor(100, 100);
    // display.print(moon.percentLit);
    // display.print(month);

    // display.setCursor(100, 130);
    // display.print(day);
    //  Serial.print( "Moon phase angle: " );
    //  Serial.print( moon.angle );             // angle is a integer between 0-360
    //  Serial.println( " degrees." );
    //  Serial.print( "Moon surface lit: " );
    //  Serial.println( moon.percentLit * 100 );  // percentLit is a real between 0-1
}

void ESPC::drawSun() {
    Dusk2Dawn location(LAT, LON, TIMEZONE);
    int year = currentTime.Year + 1970;
    int32_t month = currentTime.Month;
    int32_t day = currentTime.Day;
    int sr = location.sunrise(year, month, day, false);
    int ss = location.sunset(year, month, day, false);

    long k = currentTime.Hour * 60 + currentTime.Minute;
    int tk = (k - sr) * 60 / (ss - sr);
    if (k > ss)
        tk = 60;
    else if (k < sr)
        tk = 0;
    display.drawBitmap(110, 132 - tk, arr, 3, 5, FG_COLOR);

    int rh = sr / 60;
    int rm = sr % 60;
    int sh = ss / 60;
    int sm = ss % 60;

    if (HOUR_SET == false && rh > 12)
    {
        rh -= 12;
    }

    if (HOUR_SET == false && sh >12)
    {
        sh -= 12;
    }

    int a = sh >= 10 ? sh / 10 : 0;
    int b = sh % 10;
    int c = sm >= 10 ? sm / 10 : 0;
    int d = sm % 10;
    int e = rh >= 10 ? rh / 10 : 0;
    int f = rh % 10;
    int g = rm >= 10 ? rm / 10 : 0;
    int h = rm % 10;

    if (a == 0)
        display.drawBitmap(116, 67, num_0, 3, 5, FG_COLOR);
    else if (a == 1)
        display.drawBitmap(116, 67, num_1, 3, 5, FG_COLOR);
    else if (a == 2)
        display.drawBitmap(116, 67, num_2, 3, 5, FG_COLOR);
    else if (a == 3)
        display.drawBitmap(116, 67, num_3, 3, 5, FG_COLOR);
    else if (a == 4)
        display.drawBitmap(116, 67, num_4, 3, 5, FG_COLOR);
    else if (a == 5)
        display.drawBitmap(116, 67, num_5, 3, 5, FG_COLOR);
    else if (a == 6)
        display.drawBitmap(116, 67, num_6, 3, 5, FG_COLOR);
    else if (a == 7)
        display.drawBitmap(116, 67, num_7, 3, 5, FG_COLOR);
    else if (a == 8)
        display.drawBitmap(116, 67, num_8, 3, 5, FG_COLOR);
    else if (a == 9)
        display.drawBitmap(116, 67, num_9, 3, 5, FG_COLOR);

    if (b == 0)
        display.drawBitmap(120, 67, num_0, 3, 5, FG_COLOR);
    else if (b == 1)
        display.drawBitmap(120, 67, num_1, 3, 5, FG_COLOR);
    else if (b == 2)
        display.drawBitmap(120, 67, num_2, 3, 5, FG_COLOR);
    else if (b == 3)
        display.drawBitmap(120, 67, num_3, 3, 5, FG_COLOR);
    else if (b == 4)
        display.drawBitmap(120, 67, num_4, 3, 5, FG_COLOR);
    else if (b == 5)
        display.drawBitmap(120, 67, num_5, 3, 5, FG_COLOR);
    else if (b == 6)
        display.drawBitmap(120, 67, num_6, 3, 5, FG_COLOR);
    else if (b == 7)
        display.drawBitmap(120, 67, num_7, 3, 5, FG_COLOR);
    else if (b == 8)
        display.drawBitmap(120, 67, num_8, 3, 5, FG_COLOR);
    else if (b == 9)
        display.drawBitmap(120, 67, num_9, 3, 5, FG_COLOR);

    if (c == 0)
        display.drawBitmap(128, 67, num_0, 3, 5, FG_COLOR);
    else if (c == 1)
        display.drawBitmap(128, 67, num_1, 3, 5, FG_COLOR);
    else if (c == 2)
        display.drawBitmap(128, 67, num_2, 3, 5, FG_COLOR);
    else if (c == 3)
        display.drawBitmap(128, 67, num_3, 3, 5, FG_COLOR);
    else if (c == 4)
        display.drawBitmap(128, 67, num_4, 3, 5, FG_COLOR);
    else if (c == 5)
        display.drawBitmap(128, 67, num_5, 3, 5, FG_COLOR);
    else if (c == 6)
        display.drawBitmap(128, 67, num_6, 3, 5, FG_COLOR);
    else if (c == 7)
        display.drawBitmap(128, 67, num_7, 3, 5, FG_COLOR);
    else if (c == 8)
        display.drawBitmap(128, 67, num_8, 3, 5, FG_COLOR);
    else if (c == 9)
        display.drawBitmap(128, 67, num_9, 3, 5, FG_COLOR);

    if (d == 0)
        display.drawBitmap(132, 67, num_0, 3, 5, FG_COLOR);
    else if (d == 1)
        display.drawBitmap(132, 67, num_1, 3, 5, FG_COLOR);
    else if (d == 2)
        display.drawBitmap(132, 67, num_2, 3, 5, FG_COLOR);
    else if (d == 3)
        display.drawBitmap(132, 67, num_3, 3, 5, FG_COLOR);
    else if (d == 4)
        display.drawBitmap(132, 67, num_4, 3, 5, FG_COLOR);
    else if (d == 5)
        display.drawBitmap(132, 67, num_5, 3, 5, FG_COLOR);
    else if (d == 6)
        display.drawBitmap(132, 67, num_6, 3, 5, FG_COLOR);
    else if (d == 7)
        display.drawBitmap(132, 67, num_7, 3, 5, FG_COLOR);
    else if (d == 8)
        display.drawBitmap(132, 67, num_8, 3, 5, FG_COLOR);
    else if (d == 9)
        display.drawBitmap(132, 67, num_9, 3, 5, FG_COLOR);

    if (e == 0)
        display.drawBitmap(116, 137, num_0, 3, 5, FG_COLOR);
    else if (e == 1)
        display.drawBitmap(116, 137, num_1, 3, 5, FG_COLOR);
    else if (e == 2)
        display.drawBitmap(116, 137, num_2, 3, 5, FG_COLOR);
    else if (e == 3)
        display.drawBitmap(116, 137, num_3, 3, 5, FG_COLOR);
    else if (e == 4)
        display.drawBitmap(116, 137, num_4, 3, 5, FG_COLOR);
    else if (e == 5)
        display.drawBitmap(116, 137, num_5, 3, 5, FG_COLOR);
    else if (e == 6)
        display.drawBitmap(116, 137, num_6, 3, 5, FG_COLOR);
    else if (e == 7)
        display.drawBitmap(116, 137, num_7, 3, 5, FG_COLOR);
    else if (e == 8)
        display.drawBitmap(116, 137, num_8, 3, 5, FG_COLOR);
    else if (e == 9)
        display.drawBitmap(116, 137, num_9, 3, 5, FG_COLOR);

    if (f == 0)
        display.drawBitmap(120, 137, num_0, 3, 5, FG_COLOR);
    else if (f == 1)
        display.drawBitmap(120, 137, num_1, 3, 5, FG_COLOR);
    else if (f == 2)
        display.drawBitmap(120, 137, num_2, 3, 5, FG_COLOR);
    else if (f == 3)
        display.drawBitmap(120, 137, num_3, 3, 5, FG_COLOR);
    else if (f == 4)
        display.drawBitmap(120, 137, num_4, 3, 5, FG_COLOR);
    else if (f == 5)
        display.drawBitmap(120, 137, num_5, 3, 5, FG_COLOR);
    else if (f == 6)
        display.drawBitmap(120, 137, num_6, 3, 5, FG_COLOR);
    else if (f == 7)
        display.drawBitmap(120, 137, num_7, 3, 5, FG_COLOR);
    else if (f == 8)
        display.drawBitmap(120, 137, num_8, 3, 5, FG_COLOR);
    else if (f == 9)
        display.drawBitmap(120, 137, num_9, 3, 5, FG_COLOR);

    if (g == 0)
        display.drawBitmap(128, 137, num_0, 3, 5, FG_COLOR);
    else if (g == 1)
        display.drawBitmap(128, 137, num_1, 3, 5, FG_COLOR);
    else if (g == 2)
        display.drawBitmap(128, 137, num_2, 3, 5, FG_COLOR);
    else if (g == 3)
        display.drawBitmap(128, 137, num_3, 3, 5, FG_COLOR);
    else if (g == 4)
        display.drawBitmap(128, 137, num_4, 3, 5, FG_COLOR);
    else if (g == 5)
        display.drawBitmap(128, 137, num_5, 3, 5, FG_COLOR);
    else if (g == 6)
        display.drawBitmap(128, 137, num_6, 3, 5, FG_COLOR);
    else if (g == 7)
        display.drawBitmap(128, 137, num_7, 3, 5, FG_COLOR);
    else if (g == 8)
        display.drawBitmap(128, 137, num_8, 3, 5, FG_COLOR);
    else if (g == 9)
        display.drawBitmap(128, 137, num_9, 3, 5, FG_COLOR);

    if (h == 0)
        display.drawBitmap(132, 137, num_0, 3, 5, FG_COLOR);
    else if (h == 1)
        display.drawBitmap(132, 137, num_1, 3, 5, FG_COLOR);
    else if (h == 2)
        display.drawBitmap(132, 137, num_2, 3, 5, FG_COLOR);
    else if (h == 3)
        display.drawBitmap(132, 137, num_3, 3, 5, FG_COLOR);
    else if (h == 4)
        display.drawBitmap(132, 137, num_4, 3, 5, FG_COLOR);
    else if (h == 5)
        display.drawBitmap(132, 137, num_5, 3, 5, FG_COLOR);
    else if (h == 6)
        display.drawBitmap(132, 137, num_6, 3, 5, FG_COLOR);
    else if (h == 7)
        display.drawBitmap(132, 137, num_7, 3, 5, FG_COLOR);
    else if (h == 8)
        display.drawBitmap(132, 137, num_8, 3, 5, FG_COLOR);
    else if (h == 9)
        display.drawBitmap(132, 137, num_9, 3, 5, FG_COLOR);
}

void ESPC::drawWeather()
{

   weatherData currentWeather = getWeatherData();

   int8_t temperature = currentWeather.temperature;
   int16_t weatherConditionCode = currentWeather.weatherConditionCode;

   display.setFont(&DSEG7_Classic_Regular_39);
   int16_t  x1, y1;
   uint16_t w, h;
   display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
   if(159 - w - x1 > 87)
   {
       display.setCursor(159 - w - x1, 150);
   }
   else
   {
       display.setFont(&DSEG7_Classic_Bold_25);
       display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
       display.setCursor(159 - w - x1, 136);
   }
   display.println(temperature);
   display.drawBitmap(165, 110, currentWeather.isMetric ? celsius : fahrenheit, 26, 20, FG_COLOR);
   const unsigned char* weatherIcon;

   //https://openweathermap.org/weather-conditions
   if(weatherConditionCode > 801)
   {
       //Cloudy
       weatherIcon = cloudy;
   }
   else if(weatherConditionCode == 801)
   {
       //Few Clouds
       weatherIcon = cloudsun;
   }
   else if(weatherConditionCode == 800)
   {
       //Clear
       weatherIcon = sunny;
   }
   else if(weatherConditionCode >=700)
   {
       //Atmosphere
       weatherIcon = atmosphere;
   }
   else if(weatherConditionCode >=600)
   {
       //Snow
       weatherIcon = snow;
   }
   else if(weatherConditionCode >=500)
   {
       //Rain
       weatherIcon = rain;
   }
   else if(weatherConditionCode >=300)
   {
       //Drizzle
       weatherIcon = drizzle;
   }
   else if(weatherConditionCode >=200)
   {
       //Thunderstorm
       weatherIcon = thunderstorm;
   }
   else
       return;
   display.drawBitmap(145, 158, weatherIcon, 26, 20, FG_COLOR);
}

void ESPC::drawEva()
{
    display.drawBitmap(105, 100, eva, 100, 100, FG_COLOR);
}

void ESPC::drawLine()
{
    display.drawBitmap(100, 72, line, 1, 77, FG_COLOR);
}

// @main
// Entry point for execution
// Note: platformIO LDF will call frameworks before this
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