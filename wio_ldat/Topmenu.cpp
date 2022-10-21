#include "Topmenu.h"

/*******************************************
 * public methods
********************************************/
Topmenu::Topmenu(int x, int y, int w, int h, HardwareSerial* serial){
  this->x0 = x;
  this->y0 = y;
  this->w = w;
  this->h = h;
  stream = serial;
}

void Topmenu::init(TFT_eSPI tft){
  this->tft = tft;
  stream->println("Topmenu::init() done");
}

void Topmenu::moveCursorUp(){

}
void Topmenu::moveCursorDown(){
  
}
void Topmenu::moveCursorRight(){
  curr_cursor_index++;
  if(curr_cursor_index > CURSOR_INDEX_MAX){
    curr_cursor_index = CURSOR_INDEX_MAX;
  }
}
void Topmenu::moveCursorLeft(){
  curr_cursor_index--;
  if(curr_cursor_index < CURSOR_INDEX_MIN){
    curr_cursor_index = CURSOR_INDEX_MIN;
  }
}

void Topmenu::setMode(int mode){
  this->mode = mode;
}
int Topmenu::getMode(){
  return mode;
}

void Topmenu::drawFull(){
  tft.setTextColor(TFT_BLACK);  
  tft.setTextSize(1);
  fillRoundRect(0,0,BTN_WIDTH+10,h,8,COLOR_BTN_CURSOR);
  fillRoundRect(0+3,0+3,BTN_WIDTH+10-6,h-6,5,COLOR_BTN1_ACTIVE_BG);
  drawString("Cont",5,8);
  fillRoundRect(BTN_WIDTH*2-10,0,BTN_WIDTH+10,h,8,COLOR_BTN3_INACTIVE_BG);
  drawString("Auto",BTN_WIDTH*2+5,8);
  fillRect(BTN_WIDTH,0,BTN_WIDTH,h,COLOR_BTN2_INACTIVE_BG);
  drawString("Man.",BTN_WIDTH+5,8);
}

/*******************************************
 * private methods
********************************************/

void Topmenu::drawFastVLine(int16_t x, int16_t y, int16_t h, int16_t color){
  tft.drawFastVLine(x + x0, y + y0, h, color);
}

void Topmenu::drawFastHLine(int16_t x, int16_t y, int16_t w, int16_t color){
  tft.drawFastHLine(x + x0, y + y0, w, color);
}

void Topmenu::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color){
  tft.drawLine(x0 + this->x0, y0 + this->y0, x1 + this->x0, y1 + this->y0, color);
}

void Topmenu::drawPixel(int16_t x, int16_t y, uint16_t color){
  tft.drawPixel(x + x0, y + y0, color);
}

void Topmenu::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color){
  tft.drawRect( x + x0, y + y0,  w,  h,  color);
}

void Topmenu::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color){
  tft.fillRect( x + x0, y + y0,  w,  h,  color);
}

void Topmenu::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color){
  tft.drawRoundRect( x + x0, y + y0,  w,  h,  r,  color);
}

void Topmenu::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color){
  tft.fillRoundRect( x + x0, y + y0,  w,  h,  r,  color);
}

void Topmenu::drawString(String str,int16_t x, int16_t y){
  tft.drawString(str, x + x0, y + y0);
}
