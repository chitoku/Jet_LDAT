#include "Oscope.h"

Oscope::Oscope(int x, int y, int w, int h){
  this->tft = tft;
  this->x0 = x;
  this->y0 = y;
  this->w = w;
  this->h = h;

  ch1_y0 = y0 + CH1_HEIGHT; ch1_h = CH1_HEIGHT;
  ch2_y0 = y0 + h;          ch2_h = h - CH1_HEIGHT;
}

void Oscope::init(TFT_eSPI tft){
  this->tft = tft;
}

void Oscope::drawZeroLines(){
  tft.drawFastHLine(x0, ch1_y0, w, color_ch1);
  tft.drawFastHLine(x0, ch2_y0, w, color_ch2);
}

void Oscope::drawNextLine(bool led, int sensor){
}
