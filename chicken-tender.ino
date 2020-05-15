// Don't cook the chickens

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal.h> // LCD

#define DHT_TYPE DHT22
#define LED_PIN 13    // heartbeat LED
#define RELAY_PIN A5  // TEMP SENSOR
#define TEMP_PIN 2  // TEMP SENSOR

#define BLINK_PERIOD 1000

// Initialize LCD
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7; // sainsmart LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float setTemp = 30; // Celsius
unsigned long blinkTime=0; // time in millis of last blink transition
bool blinkFlag = false;       // flag for toggling LED on/off
bool relayFlag = false;       // flag for toggling relay on/off

DHT_Unified dht(TEMP_PIN, DHT_TYPE);
/*****************************************************************************
void setup()

/*****************************************************************************/
void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TEMP_PIN, INPUT);

  digitalWrite(RELAY_PIN, LOW);

  // start temp sensor
  dht.begin();

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.print("Coop-Control    ");
  lcd.setCursor(0,1);
  lcd.print("Set Temp: ");
  lcd.print(setTemp);
  delay(3000);
  lcd.clear();
}

/*****************************************************************************
void loop()

/*****************************************************************************/
void loop() {
  // get current time
  unsigned long timeNow = millis();

  // blink
  if(timeNow-blinkTime > BLINK_PERIOD)
  {
    if(blinkFlag)
    {
      digitalWrite(LED_PIN, LOW);
      lcd.setCursor(15,1);
      lcd.print("*");
    }
    else
    {
      digitalWrite(LED_PIN, HIGH);
      lcd.setCursor(15,1);
      lcd.print(" ");
    }
    blinkFlag = !blinkFlag;
    blinkTime = timeNow;

    // Print temp info
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(F("Temp Error!"));
      lcd.setCursor(0,1);
      lcd.print(F("Check wires!"));
    }
    else {
      lcd.setCursor(0,0);
      lcd.print("T: ");
      lcd.print(event.temperature);
      lcd.print(" C");
    }

    if(event.temperature < setTemp)
    {
      relayOn();
    }
    else
    {
      relayOff();
    }
  } // end blink

}

void relayOn()
{
  digitalWrite(RELAY_PIN, HIGH);
  lcd.setCursor(0,1);
  lcd.print("Relay on");
}
void relayOff()
{
  digitalWrite(RELAY_PIN, LOW);
  lcd.setCursor(0,1);
  lcd.print("Relay off");
}
