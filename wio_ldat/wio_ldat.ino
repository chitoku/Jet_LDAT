#include "TFT_eSPI.h"
// #include "Free_Fonts.h" //include the header file
#include "Oscope.h"
#include "Topmenu.h"

#define PIN_LED 1
bool led_left_on = false;
// #define SENSOR_THRESHOLD 400
// int oscope_threshold_y = map(SENSOR_THRESHOLD, 0, 1023, TFT_OSCOPE_Y1, TFT_OSCOPE_Y0);

#include "SAMDTimerInterrupt.h"
#include "SAMD_ISR_Timer.h"

#define HW_TIMER_FREQ_IN_HZ                    10000L
#define HW_TIMER_INTERVAL_IN_US            (1000000 / HW_TIMER_FREQ_IN_HZ) // 100 us

// Init selected SAMD timer
#define SELECTED_TIMER      TIMER_TC3
SAMDTimer ITimer(SELECTED_TIMER);

// Init SAMD_ISR_Timer
// Each SAMD_ISR_Timer can service 16 different ISR-based timers
SAMD_ISR_Timer ISR_Timer;

#define INTERVAL_IN_US_SCANUPDATE            5000L // 5ms
int interval_in_us_scanUpdate = INTERVAL_IN_US_SCANUPDATE;
int interval_in_unit_scanUpdate = interval_in_us_scanUpdate / HW_TIMER_INTERVAL_IN_US; // (50 unit)

#define INTERVAL_IN_US_FLASHLED            500000L // 500ms
int interval_in_us_flashLED = INTERVAL_IN_US_FLASHLED;
int interval_in_unit_flashLED = interval_in_us_flashLED / HW_TIMER_INTERVAL_IN_US; // (5000 unit)

#define DURATION_IN_US_LEDON                50000L // 50 ms --> 10 scan line
int duration_in_us_ledOn = DURATION_IN_US_LEDON;
int duration_in_unit_ledOn = duration_in_us_ledOn / HW_TIMER_INTERVAL_IN_US; // (500 unit)

TFT_eSPI tft;
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite

#define TFT_TOPMENU_X0 20
#define TFT_TOPMENU_X1 320
#define TFT_TOPMENU_Y0 0
#define TFT_TOPMENU_Y1 30
Topmenu topmenu(TFT_TOPMENU_X0, TFT_TOPMENU_Y0, 
  TFT_TOPMENU_X1-TFT_TOPMENU_X0, TFT_TOPMENU_Y1-TFT_TOPMENU_Y0,
  (HardwareSerial*)&Serial);

#define TFT_OSCOPE_X0 20
#define TFT_OSCOPE_X1 320
#define TFT_OSCOPE_Y0 40
#define TFT_OSCOPE_Y1 100
Oscope oscope(TFT_OSCOPE_X0, TFT_OSCOPE_Y0, 
  TFT_OSCOPE_X1-TFT_OSCOPE_X0, TFT_OSCOPE_Y1-TFT_OSCOPE_Y0,
  (HardwareSerial*)&Serial);

int brightness;
int latency_in_unit;
bool latency_updated = false;

bool oscope_updated = false;

bool led_onoff = false;
bool led_updated = false;

bool interrupt_time_violation = false;
int violation_count = 0;
int violation_indexes[256];
int violation_time_in_us[256];

void TimerHandler(void)
{
  static unsigned long time1;
  static unsigned long time2;

  time1 = micros();

  ISR_Timer.run();

  static int index  = 0; 
  static int lastFlashIndex = 0;
  static bool triggerReady = false;

  // Every 1 run
  brightness = analogRead(WIO_LIGHT);
  if(brightness > SENSOR_THRESHOLD && triggerReady){
    triggerReady = false;
    if(lastFlashIndex > index){
      latency_in_unit = index + HW_TIMER_FREQ_IN_HZ - lastFlashIndex;
    }else{
      latency_in_unit = index - lastFlashIndex;
    }
    // ### Un-comment the following 5 lines to test 
    // ### if the sketch can detect the ISR timing violation by itself.
    // Serial.print("|         index: "); Serial.println(index);
    // Serial.print("|lastFlashIndex: "); Serial.println(lastFlashIndex);
    // Serial.print(latency_in_unit/10);
    // Serial.print(".");
    // Serial.println(latency_in_unit%10);
    latency_updated = true;
  }

  // For every (50) unit, update the oscope with 1 new scanline
  if (index % interval_in_unit_scanUpdate == 0 ){
    oscope_updated = true;
  }
  
  // For every (5000) unit, flash the LED
  if (index % interval_in_unit_flashLED == 0){
    triggerReady = true; // enable lightsensor to generate trigger
    lastFlashIndex = index;
    led_onoff = true;
    digitalWrite(PIN_LED, !led_onoff);
    led_updated = true;
  }

  // (500) unit after LED flash, turn off the LED
  if (index > lastFlashIndex + duration_in_unit_ledOn && led_onoff){
    led_onoff = false;
    digitalWrite(PIN_LED, !led_onoff);
    led_updated = true;
  }

  index++;
  if (index >= HW_TIMER_FREQ_IN_HZ){
    index = 0;
  }

  time2 = micros();

  if (time2 - time1 > HW_TIMER_INTERVAL_IN_US){
    interrupt_time_violation = true;
    violation_indexes[violation_count] = index;
    violation_time_in_us[violation_count] = time2 - time1;
    violation_count++;
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial && millis() < 5000);

  pinMode(WIO_LIGHT, INPUT);
  pinMode(PIN_LED, OUTPUT);  
  digitalWrite(PIN_LED, HIGH);

  tft.begin();
  spr.createSprite(TFT_HEIGHT, TFT_WIDTH);
  spr.setRotation(1);
  tft.setRotation(1);
  digitalWrite(LCD_BACKLIGHT, HIGH);

  tft.fillScreen(TFT_BLACK);
  topmenu.init(tft);
  topmenu.drawFull();

  oscope.init(tft);
  oscope.drawZeroLines();
  
  // Interval in millisecs
  // if (ITimer.attachInterruptInterval_MS(HW_TIMER_INTERVAL_MS, TimerHandler))
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_IN_US, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

}

void loop() {
  if(latency_updated){
    Serial.print(latency_in_unit/10);
    Serial.print(".");
    Serial.println(latency_in_unit%10);
    latency_updated = false;
  }

  if(oscope_updated){
    oscope_updated = false;
    oscope.drawNextLine(led_onoff, brightness);
  }
  // if(led_updated){
  //   digitalWrite(PIN_LED, !led_onoff);
  // }

  if(interrupt_time_violation){
    Serial.print("[WARN] ISR taking too long: [");
    Serial.print(violation_count);
    Serial.println("] times");
    for(int i=0; i<violation_count; i++){
      Serial.print(violation_time_in_us[i]);
      Serial.print("us(#");
      Serial.print(violation_indexes[i]);
      Serial.print("), ");
    }
    Serial.print(" --> total violoation: ");
    Serial.println(violation_count);
    violation_count = 0;
    interrupt_time_violation = false;
  }
}