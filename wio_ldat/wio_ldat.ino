#include "TFT_eSPI.h"
// #include "Free_Fonts.h" //include the header file

#define TFT_BG_COLOR TFT_BLACK

#define TFT_OSCOPE_X0 10
#define TFT_OSCOPE_X1 310
#define TFT_OSCOPE_Y0 40
#define TFT_OSCOPE_Y1 90
int oscope_scanline_x = TFT_OSCOPE_X0;
int oscope_blip_height;
int oscope_blip_height_old;

#define PIN_LED 1
bool led_left_on = false;
#define SENSOR_THRESHOLD 500

#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

#define HW_TIMER_FREQ_IN_HZ                    10000L
#define HW_TIMER_INTERVAL_US            (1000000 / HW_TIMER_FREQ_IN_HZ) // 100 us

#define TIMER_INTERVAL_US_SCANUPDATE            5000L
#define INTERVAL_UNIT_SCANUPDATE        (TIMER_INTERVAL_US_SCANUPDATE / HW_TIMER_INTERVAL_US) // 50 unit

#define TIMER_INTERVAL_US_FLASHLED            500000L
#define INTERVAL_UNIT_FLASHLED          (TIMER_INTERVAL_US_FLASHLED / HW_TIMER_INTERVAL_US) // 5000 unit

// Init selected SAMD timer
#define SELECTED_TIMER      TIMER_TC3
SAMDTimer ITimer(SELECTED_TIMER);

// Init SAMD_ISR_Timer
// Each SAMD_ISR_Timer can service 16 different ISR-based timers
SAMD_ISR_Timer ISR_Timer;

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite

int brightness;
int latencyUnits;

void TimerHandler(void)
{
  ISR_Timer.run();

  static int index  = 0; 
  static int lastFlashIndex = 0;
  static bool flashed = false;

  // Every 1 run
  brightness = analogRead(WIO_LIGHT);
  if(brightness > SENSOR_THRESHOLD && flashed){
    flashed = false;
    if(lastFlashIndex > index){
      latencyUnits = index + HW_TIMER_FREQ_IN_HZ - lastFlashIndex;
    }else{
      latencyUnits = index - lastFlashIndex;
    }
    Serial.print("Latency (unit): ");
    Serial.println(latencyUnits);
  }

  if (index % INTERVAL_UNIT_SCANUPDATE == 0 ){
    byInterruptUpdateOscope();
  }
  
  if (index % INTERVAL_UNIT_FLASHLED == 0){
    lastFlashIndex = index;
    flashed = true;
    byInterruptFlashLED();
  }

  index++;
  if (index == HW_TIMER_FREQ_IN_HZ){
    index = 0;
  }
}

void byInterruptUpdateOscope()
{
  tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_BG_COLOR);
  tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y1 - oscope_blip_height, oscope_blip_height, TFT_GREENYELLOW);
  
  oscope_blip_height = map(brightness, 0, 1023, 0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0) + 1;
  oscope_scanline_x++;
  // tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y1 - oscope_blip_height, oscope_blip_height, TFT_YELLOW);

  if (led_left_on){
    digitalWrite(PIN_LED, HIGH); // LED is left on for TIMER_INTERVAL_SCANUPDATE (5ms)
  }

  if(oscope_scanline_x >= TFT_OSCOPE_X1){
    oscope_scanline_x = TFT_OSCOPE_X0;
    //tft.fillRect(TFT_OSCOPE_X0, TFT_OSCOPE_Y0, TFT_OSCOPE_X1-TFT_OSCOPE_X0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_BG_COLOR);
  }
  tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_YELLOW);
}

void byInterruptFlashLED(){ 
  digitalWrite(PIN_LED, LOW);
  led_left_on = true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  pinMode(WIO_LIGHT, INPUT);
  pinMode(PIN_LED, OUTPUT);  
  digitalWrite(PIN_LED, HIGH);

  tft.begin();
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);
  spr.setRotation(1);
  tft.setRotation(1);
  digitalWrite(LCD_BACKLIGHT, HIGH);

  tft.fillScreen(TFT_BG_COLOR);
  drawHeader();
  
  // Interval in millisecs
  // if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler))
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

}

void drawHeader(){
  tft.setTextSize(2);
  tft.drawString("Header text 0123",10,10);
}

void loop() {
  // spr.fillSprite(TFT_WHITE);
  // brightness = analogRead(WIO_LIGHT);

  // if (data.size() == max_size) {
  //   data.pop();  //this is used to remove the first read variable
  // }
  // data.push(brightness);  //read variables and store in data

  // //Settings for the line graph title
  // auto header = text(0, 0)
  //                 .value("Light Sensor Readings")
  //                 .align(center)
  //                 .valign(vcenter)
  //                 .width(tft.width())
  //                 .thickness(2);

  // header.height(header.font_height() * 2);
  // header.draw();  //Header height is the twice the height of the font

  // //Settings for the line graph
  // auto content = line_chart(20, header.height());  //(x,y) where the line graph begins
  // content
  //   .height(tft.height() - header.height() * 1.5)  //actual height of the line chart
  //   .width(tft.width() - content.x() * 2)          //actual width of the line chart
  //   .based_on(0.0)                                 //Starting point of y-axis, must be a float
  //   .show_circle(false)                            //drawing a cirle at each point, default is on.
  //   .value(data)                                   //passing through the data to line graph
  //   .color(TFT_RED)                                //Setting the color for the line
  //   .draw();
  // spr.pushSprite(0, 0);
  // delay(10);
}