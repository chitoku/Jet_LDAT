#include "TFT_eSPI.h"
// #include "Free_Fonts.h" //include the header file

#define TFT_BG_COLOR TFT_BLACK

#define TFT_OSCOPE_X0 10
#define TFT_OSCOPE_X1 310
#define TFT_OSCOPE_Y0 40
#define TFT_OSCOPE_Y1 90
#define TFT_LED_PULSE_H 15
int oscope_scanline_x = TFT_OSCOPE_X0;
int oscope_blip_height;
int oscope_blip_height_old;

#define PIN_LED 1
bool led_left_on = false;
#define SENSOR_THRESHOLD 400
int oscope_threshold_y = map(SENSOR_THRESHOLD, 0, 1023, TFT_OSCOPE_Y1, TFT_OSCOPE_Y0);

 #include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

#define HW_TIMER_FREQ_IN_HZ                    10000L
#define HW_TIMER_INTERVAL_US            (1000000 / HW_TIMER_FREQ_IN_HZ) // 100 us

#define TIMER_INTERVAL_US_SCANUPDATE            5000L
#define INTERVAL_UNIT_SCANUPDATE        (TIMER_INTERVAL_US_SCANUPDATE / HW_TIMER_INTERVAL_US) // 50 unit

#define TIMER_INTERVAL_US_FLASHLED            500000L
#define INTERVAL_UNIT_FLASHLED          (TIMER_INTERVAL_US_FLASHLED / HW_TIMER_INTERVAL_US) // 5000 unit
#define DURATION_IN_US_LEDON                   50000L       // 50 ms --> 10 scan line
#define DURATION_IN_UNIT_LEDON          (DURATION_IN_US_LEDON / HW_TIMER_INTERVAL_US) // 100 unit

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
  static bool sensed = false;
  static bool oscopeUpdated = false;
  static unsigned long time1;
  static unsigned long time2;

  time1 = micros();

  // Every 1 run
  brightness = analogRead(WIO_LIGHT);
  if(brightness > SENSOR_THRESHOLD && !sensed){
    sensed = true;
    if(lastFlashIndex > index){
      latencyUnits = index + HW_TIMER_FREQ_IN_HZ - lastFlashIndex;
    }else{
      latencyUnits = index - lastFlashIndex;
    }
    Serial.print(latencyUnits/10);
    Serial.print(".");
    Serial.println(latencyUnits%10);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.drawString(String(latencyUnits), oscope_scanline_x, TFT_OSCOPE_Y1 + 10 );
  }

  if(index > lastFlashIndex + DURATION_IN_UNIT_LEDON){
    byInterruptTurnOffLED();    
  }  

  if (index % INTERVAL_UNIT_SCANUPDATE == 0 ){
    byInterruptUpdateOscope(led_left_on);
    oscopeUpdated = true;
  }
  
  if (index % INTERVAL_UNIT_FLASHLED == 0){
    lastFlashIndex = index;
    sensed = false;
    byInterruptTurnOnLED();
  }

  index++;
  if (index == HW_TIMER_FREQ_IN_HZ){
    index = 0;
  }

  time2 = micros();
  if(oscopeUpdated){
    Serial.println(time2 - time1);
    oscopeUpdated = false;
  }
}

void byInterruptUpdateOscope(bool led_left_on)
{
  tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_BG_COLOR);
  if(brightness>SENSOR_THRESHOLD){
    tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y1 - oscope_blip_height, oscope_blip_height, TFT_YELLOW);
  }else{
    tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y1 - oscope_blip_height, oscope_blip_height, TFT_OLIVE);
  }
  if(led_left_on){
    tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y0, TFT_LED_PULSE_H, TFT_CYAN);    
  }
  if(oscope_scanline_x % 4 > 2){
    tft.drawPixel(oscope_scanline_x, oscope_threshold_y, TFT_ORANGE);    
  }
  
  oscope_blip_height = map(brightness, 0, 1023, 0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0) + 1;
  oscope_scanline_x++;
  // tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y1 - oscope_blip_height, oscope_blip_height, TFT_YELLOW);

  if(oscope_scanline_x >= TFT_OSCOPE_X1){
    oscope_scanline_x = TFT_OSCOPE_X0;
    //tft.fillRect(TFT_OSCOPE_X0, TFT_OSCOPE_Y0, TFT_OSCOPE_X1-TFT_OSCOPE_X0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_BG_COLOR);
  }
  tft.drawFastVLine(oscope_scanline_x, TFT_OSCOPE_Y0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0, TFT_YELLOW);
}

void byInterruptTurnOnLED(){ 
  digitalWrite(PIN_LED, LOW);
  led_left_on = true;
}

void byInterruptTurnOffLED(){ 
  digitalWrite(PIN_LED, HIGH);
  led_left_on = false;
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
  tft.drawFastHLine(TFT_OSCOPE_X0, TFT_OSCOPE_Y0-1, TFT_OSCOPE_X1-TFT_OSCOPE_X0, TFT_CYAN);
  
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
  
}