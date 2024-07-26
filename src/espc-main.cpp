// @ENTRY POINT
// Note: platformIO calls frameworks before this
#ifndef ESPC_MAIN
#define ESPC_MAIN

#include "Watchy_7_SEG.h"
#include "settings.h"

#endif // ESPC_MAIN

Watchy7SEG watchy(settings);


void setup() {
  watchy.init();
}

void loop() {}
