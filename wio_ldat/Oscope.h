#ifndef MY_OSCOPE_H
#define MY_OSCOPE_H

#include <Arduino.h>
#include "TFT_eSPI.h"

#define SENSOR_READMAX  500 //1023
#define SENSOR_THRESHOLD 300

#define CH1_HEIGHT 10
#define TFT_LED_PULSE_H 15

class Oscope {

  private:
    Stream* stream;
    TFT_eSPI tft;
    int x0;
    int y0;
    int h;
    int w;

    int ch1_y0; int ch1_h;
    int ch2_y0; int ch2_h;

    int color_bg       = TFT_BLACK;
    int color_ch1      = TFT_CYAN;
    int color_ch2      = TFT_OLIVE;
    int color_thresh_2 = TFT_ORANGE;
    int color_scanline = TFT_YELLOW;

    int scanline_x = 0;
    int ch2_blip_height;

    void drawFastVLine(int16_t x, int16_t y, int16_t h, int16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, int16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);

  public:
    Oscope(int x, int y, int h, int w, HardwareSerial* serial);

    void init(TFT_eSPI tft);
    void setColors(int bg, int ch1, int ch2, int thresh_2, int scanline);

    void drawZeroLines();
    void drawNextLine(bool led, int sensor);

    void draw();
};

#endif