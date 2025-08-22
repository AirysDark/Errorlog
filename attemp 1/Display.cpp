#include "Display.h"
#include <TFT_eSPI.h>

// Fallbacks if not defined by other headers (prevents “not declared” errors)
#ifndef HEIGHT_1
#define HEIGHT_1 240
#endif
#ifndef TEXT_HEIGHT
#define TEXT_HEIGHT 16
#endif
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 320
#endif
#ifndef BANNER_TEXT_SIZE
#define BANNER_TEXT_SIZE 2
#endif
#ifndef STATUS_BAR_WIDTH
#define STATUS_BAR_WIDTH 16
#endif
#ifndef YMAX
#define YMAX 240
#endif
#ifndef BOT_FIXED_AREA
#define BOT_FIXED_AREA 16
#endif
#ifndef TOP_FIXED_AREA
#define TOP_FIXED_AREA 16
#endif

// If your original code uses a 'key' array, keep a harmless stub
struct KeyStub { void setLabelDatum(int,int,int){} };
static KeyStub key[10];

void Display::twoPartDisplay(String msg) {
  tft.fillRect(0, 16, HEIGHT_1, 144, TFT_YELLOW);
  tft.setFreeFont(LOAD_FONT2);  // if not present, change to a font you use
  tft.drawString(msg, 10, 30);
}

void Display::touchToExit() {
  tft.fillRect(0, 32, HEIGHT_1, 16, TFT_LIGHTGREY);
  tft.drawString("Touch to Exit", 10, 40);
}

void Display::showCenterText(String text, int y) {
  tft.setCursor((SCREEN_WIDTH - (text.length() * (6 * BANNER_TEXT_SIZE))) / 2, y);
  tft.setTextSize(BANNER_TEXT_SIZE);
  tft.println(text);
}

void Display::buildBanner(String msg, int y) {
  int h = TEXT_HEIGHT; (void)h;     // avoid unused warning under -Werror
  tft.fillRect(0, STATUS_BAR_WIDTH, SCREEN_WIDTH, TEXT_HEIGHT, TFT_BLACK);
  tft.setTextSize(BANNER_TEXT_SIZE);
  tft.drawString(msg, 5, y);
}