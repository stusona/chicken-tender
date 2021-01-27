// Don't cook the chickens

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h> // LCD

// LCD shield button values
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define DHT_TYPE DHT22
#define LED_PIN 13    // heartbeat LED
#define NPN_E_PIN A4  // NPN EMITTER
#define NPN_B_PIN A5  // NPN BASE (Turning relay on/off)
#define TEMP_PIN A1  // TEMP SENSOR

#define BLINK_PERIOD 1000

// Initialize LCD
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // sainsmart LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float setTemp = 23; // Celsius
unsigned long blinkTime = 0; // time in millis of last blink transition
bool blinkFlag = false;       // flag for toggling LED on/off
bool relayFlag = false;       // flag for toggling relay on/off
bool errorFlag = false;
int lcd_key = btnNONE;
unsigned long blinkCounter = 0;  // number of times blink has run
unsigned long relayCounter = 0;  // number of times relay has been turned on (including when it's already on)
DHT_Unified dht(TEMP_PIN, DHT_TYPE);

/*****************************************************************************
void setup()

/*****************************************************************************/
void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(NPN_E_PIN, OUTPUT);
  pinMode(NPN_B_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT);

  digitalWrite(NPN_E_PIN, LOW);
  digitalWrite(NPN_B_PIN, LOW);

  // start temp sensor
  dht.begin();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Coop-Control    ");

  lcd.setCursor(0,1);
  lcd.print("Set: ");
  lcd.print(setTemp,0);
  lcd.print(" C");

  delay(3000);
  lcd.clear();
}

/*****************************************************************************
void loop()

/*****************************************************************************/
void loop() {
  // get current time
  unsigned long timeNow = millis();

  // read the buttons
  lcd_key = read_LCD_buttons();

  if(lcd_key == btnUP)
    {
      while(lcd_key == btnUP) lcd_key = read_LCD_buttons();
      setTemp++;
    }
  if(lcd_key == btnDOWN)
    {
      while(lcd_key == btnDOWN) lcd_key = read_LCD_buttons();
      setTemp--;
    }

  // blink
  if(timeNow-blinkTime > BLINK_PERIOD)
  {
    if(blinkFlag)
    {
      digitalWrite(LED_PIN, LOW);
      lcd.setCursor(15,0);
      lcd.print("*");
    }
    else
    {
      digitalWrite(LED_PIN, HIGH);
      lcd.setCursor(15,0);
      lcd.print(" ");
    }
    blinkFlag = !blinkFlag;
    blinkTime = timeNow;

    // Print temp info
    sensors_event_t temp_event, hum_event;
    dht.temperature().getEvent(&temp_event);
    dht.humidity().getEvent(&hum_event);
    if (isnan(temp_event.temperature)) {
      errorFlag = true;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Temp Error!"));
      lcd.setCursor(0,1);
      lcd.print(F("Check wires!"));
    }
    else {
      if(errorFlag)
      {
        errorFlag = false;
        lcd.clear();
      }
      lcd.setCursor(0,0);
      lcd.print("T:");
      lcd.print(temp_event.temperature,0);
      lcd.print("/");
      lcd.print(setTemp,0);
      lcd.print("C");
      lcd.print(" H:");
      lcd.print(hum_event.relative_humidity,0);
      lcd.print("%");
    }

    if(temp_event.temperature < setTemp-0.2)
    {
      relayOn();
      relayCounter++;
    }
    else if(temp_event.temperature > setTemp+0.2) relayOff();
    blinkCounter++;

    // print duty cycle of heater
    lcd.setCursor(0,1);
    lcd.print("Dooty: ");
    lcd.print(((float)relayCounter/blinkCounter)*100.0,0);
    lcd.print("%");
  } // end blink

}

void relayOn()
{
  digitalWrite(NPN_B_PIN, HIGH);
  lcd.setCursor(13,1);
  lcd.print(" ON");
}
void relayOff()
{
  digitalWrite(NPN_B_PIN, LOW);
  lcd.setCursor(13,1);
  lcd.print("OFF");
}

int read_LCD_buttons()
{
 int adc_key_in;
 adc_key_in = analogRead(0); // read the value from the sensor
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 250)  return btnUP;
 if (adc_key_in < 450)  return btnDOWN;
 if (adc_key_in < 650)  return btnLEFT;
 if (adc_key_in < 850)  return btnSELECT;
 return btnNONE;  // when all others fail, return this...
}
