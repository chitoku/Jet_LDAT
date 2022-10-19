#include "Oscope.h"

/*******************************************
 * public methods
********************************************/
Oscope::Oscope(int x, int y, int w, int h, HardwareSerial* serial){
  this->x0 = x;
  this->y0 = y;
  this->w = w;
  this->h = h;
  stream = serial;

  ch1_y0 = CH1_HEIGHT; ch1_h = CH1_HEIGHT;
  ch2_y0 = h;          ch2_h = h - CH1_HEIGHT;
  // stream->print("ch1_y0: "); stream->print(ch1_y0); 
  // stream->print(", ch1_h: "); stream->println(ch1_h); 
  // stream->print("ch2_y0: "); stream->print(ch2_y0); 
  // stream->print(", ch2_h: "); stream->println(ch2_h); 
}

void Oscope::init(TFT_eSPI tft){
  this->tft = tft;
  stream->println("OScope::init() done");
}

void Oscope::drawZeroLines(){
  drawFastHLine(0, ch1_y0, w, color_ch1);
  drawFastHLine(0, ch2_y0, w, color_ch2);
}

void Oscope::drawNextLine(bool led, int sensor){

  drawFastVLine(scanline_x, 0, h, color_bg); // current scanline initialize back to background color

  // stream->print("drawFastVLine( "); stream->print(scanline_x); 
  // stream->print(", "); stream->print(ch1_y0 - ch1_h); 
  // stream->print(", "); stream->print(ch1_h); 
  // stream->print(", "); stream->print(color_ch1); 
  // stream->println(");");
  if(led){
    drawFastVLine(scanline_x, ch1_y0 - ch1_h, ch1_h+1, color_ch1);
  }else{
    drawFastVLine(scanline_x, ch1_y0, 1, color_ch1);
  }

  // stream->print("sensor: "); stream->println(sensor); 
  ch2_blip_height = map(sensor, 0, SENSOR_READMAX, 0, ch2_h);
//  stream->print("ch2_blip_height: "); stream->println(ch2_blip_height); 
  int newLineColor;
  if(sensor>SENSOR_THRESHOLD){
    newLineColor = color_thresh_2;
  }else{
    newLineColor = color_ch2;
  }
  drawFastVLine(scanline_x, ch2_y0 - ch2_blip_height, ch2_blip_height, newLineColor);

  // stream->print("scanline_x: "); stream->println(scanline_x); 
  if(scanline_x + 1>= w){
    scanline_x = 0;
    //tft.fillRect(TFT_OSCOPE_X0, TFT_OSCOPE_Y0, TFT_OSCOPE_X1-TFT_OSCOPE_X0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_BG_COLOR);
  }else{
    scanline_x++;
  }
  drawFastVLine(scanline_x, 0, h, TFT_YELLOW); // Redraw scanline front
}

/*******************************************
 * private methods
********************************************/

void Oscope::drawFastVLine(int16_t x, int16_t y, int16_t h, int16_t color){
  tft.drawFastVLine(x + x0, y + y0, h, color);
}

void Oscope::drawFastHLine(int16_t x, int16_t y, int16_t w, int16_t color){
  tft.drawFastHLine(x + x0, y + y0, w, color);
}

void Oscope::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color){
  tft.drawLine(x0 + this->x0, y0 + this->y0, x1 + this->x0, y1 + this->y0, color);
}

void Oscope::drawPixel(int16_t x, int16_t y, uint16_t color){
  tft.drawPixel(x + x0, y + y0, color);
}
