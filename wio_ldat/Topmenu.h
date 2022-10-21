#ifndef MY_TOPMENU_H
#define MY_TOPMENU_H

#include <Arduino.h>
#include "TFT_eSPI.h"

#define COLOR_BTN_BG           TFT_CYAN
#define COLOR_BTN1_INACTIVE_BG TFT_DARKCYAN
#define COLOR_BTN1_ACTIVE_BG   TFT_CYAN
#define COLOR_BTN2_INACTIVE_BG TFT_DARKGREY
#define COLOR_BTN2_ACTIVE_BG   TFT_LIGHTGREY
#define COLOR_BTN3_INACTIVE_BG TFT_DARKGREEN
#define COLOR_BTN3_ACTIVE_BG   TFT_GREEN
#define COLOR_BTN_CURSOR       TFT_WHITE

#define CURSOR_INDEX_MIN  0
#define CURSOR_INDEX_MAX  4
enum cursor { c_btn_cont=0, c_btn_man=1, c_btn_auto=2,
              c_fld_intvl=3, c_fld_maxcount=4};

#define BTN_WIDTH 60

class Topmenu {

  private:
    Stream* stream;
    TFT_eSPI tft;
    int x0;
    int y0;
    int h;
    int w;

    int curr_cursor_index;
    int mode;

    void drawFastVLine(int16_t x, int16_t y, int16_t h, int16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, int16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    
    void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);
    void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color);

    void drawString(String str,int16_t x, int16_t y);

  public:
    Topmenu(int x, int y, int h, int w, HardwareSerial* serial);

    void init(TFT_eSPI tft);
    void moveCursorUp();
    void moveCursorDown();
    void moveCursorRight();
    void moveCursorLeft();
    
    void setMode(int mode);
    int getMode();

    void drawFull();
};

#endif