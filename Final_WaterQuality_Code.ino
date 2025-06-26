#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define WIFI_SSID "wifi-ssid"
#define WIFI_PASSWORD "password"

#define API_KEY "API_KEY"
#define DATABASE_URL "https://your-database-url.firebaseio.com/"

#define TdsSensorPin 34
#define VREF 3.3
#define SCOUNT 30
#define MAX_TDS_DISPLAY 4

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
int copyIndex = 0;

const int pin_pH = 35;
float Po = 0;
float pH_step;
double voltage;

// pH calibration
float PH4 = 3.22;
float PH7 = 2.69;

float tdsValue = 0;

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lasttime = 0;
unsigned long timerDelay = 2000;

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
  {
    bTab[i] = bArray[i];
  }
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
  {
    bTemp = bTab[(iFilterLen - 1) / 2];
  }
  else
  {
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  }
  return bTemp;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.print("\nConnected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if(Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Udah oke nih");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Wire.begin(22, 23);
  lcd.begin(16, 2);
  lcd.setBacklight(5);
  lcd.print("Initializing...");
  pinMode(TdsSensorPin, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println();
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.println("Temperature: " + String(temperatureC) + "C");

  static unsigned long analogSampleTime = millis();
  if (millis() - analogSampleTime > 40U)
  {
    analogSampleTime = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
    analogBufferIndex = (analogBufferIndex + 1) % SCOUNT;
  }

  static unsigned long printTimePoint = millis();
  if (millis() - printTimePoint > 800U)
  {
    printTimePoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
    {
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    }
    float averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4095.0;
    float compensationCoefficient = 1.0 + 0.02 * (temperatureC - 25.0);
    float compensationVoltage = averageVoltage / compensationCoefficient;
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;

    Serial.println("TDS Value: " + String(tdsValue, 0) + " ppm");

    // Always update the display without checking tdsDisplayCount
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(tdsValue, 0);
    lcd.print("ppm|pH:");
    lcd.print(Po, 2);
  }

  int pH_analog = analogRead(pin_pH);
  voltage = pH_analog * (VREF / 4095.0); // Adjusted to use VREF directly
  pH_step = (PH7 - PH4) / 3;             // Corrected the pH_step calculation
  Po = 7 - ((voltage - PH4) / pH_step); // Adjusted the calculation

  Serial.println("pH Value: " + String(Po, 2));
  Serial.println("ADC Value (pH): " + String(pH_analog));

  lcd.setCursor(0, 1);
  lcd.print("Temp: " + String(temperatureC) + " \xDF""C");
  delay(2000);

  if(Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    if(Firebase.RTDB.setFloat(&fbdo, "database/temperature", temperatureC)){
      Serial.println("TEMPERATURE PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else{
      Serial.println("TEMPERATURE FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    // count++;
    if (Firebase.RTDB.setFloat(&fbdo, "database/tds", tdsValue)){
      Serial.println("TDS PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("TDS FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setFloat(&fbdo, "database/ph", (Po, 2))){
      Serial.println("PH PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("PH FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    if (Firebase.RTDB.setFloat(&fbdo, "database/adc", pH_analog)){
      Serial.println("ADC PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("ADC FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}