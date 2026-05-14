
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


const int analogPinLD = A2; // Пин для проверки напряжения
const int greenLed = 2;     // Зеленый светодиод
const int redLed = 3;       // Красный светодиод
const int relayPin = 4;
const int sensorPhotoBlue = 5;
const int relayPin2 = 6;
const int sensorPhotoRed = 7;


// 4 минуты в миллисекундах (4 * 60 * 1000)
const unsigned long waitTime2 = 130000; 
const unsigned long waitTime4 = 242000; 

bool checked2 = false;
bool checked4 = false;
bool bluecheckedLed = false;
bool redcheckedLed = false;
bool checkedV = false;

int rawValueLD = 0;
int rawValueLD1 = 0;
int rawValueLD2 = 0;
int rawValueLD3 = 0;

float voltageLD = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600);
  Serial.println("TEST START");
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(sensorPhotoBlue, INPUT); 
  pinMode(sensorPhotoRed, INPUT); 

  lcd.init();           // Инициализация дисплея
  lcd.backlight();      // Включаем подсветку
  
  lcd.clear();
  lcd.setCursor(0, 0);  // Устанавливаем курсор (столбец 0, строка 0)
  lcd.print("TEST START"); 
  
  lcd.setCursor(0, 1);  // Переходим на вторую строку
  lcd.print("Arduino I2C LCD");
    
  // Сначала выключаем оба светодиода
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  digitalWrite(relayPin, LOW);
  digitalWrite(relayPin2, LOW);
  }

void loop() {
  // int checkedLed = digitalRead(sensorPhoto);
  // Serial.println(checkedLed);
  // delay(2000);
  // Проверяем, прошло ли 2 минуты и не делали ли мы проверку ранее
  if (!checked2 && millis() >= waitTime2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    int led = digitalRead(sensorPhotoBlue);
    Serial.println(led);
    
    if (led == 0) {
      lcd.print("Blue led checked");
      Serial.println("Blue led checked");
      bluecheckedLed = true;
    } else {
      lcd.print("Blue led failed");
      Serial.println("Blue led failed");
    }
    checked2 = true;
  }

  if (!checked4 && millis() >= waitTime4) {
    // lcd.setCursor(0, 1);
    digitalWrite(relayPin, HIGH);
    digitalWrite(relayPin2, HIGH);
    delay(700);
    rawValueLD1 = analogRead(analogPinLD);
    rawValueLD2 = analogRead(analogPinLD);
    rawValueLD3 = analogRead(analogPinLD);
    rawValueLD = average(rawValueLD1, rawValueLD2, rawValueLD3);
    int ledred = digitalRead(sensorPhotoRed);
    Serial.print("Led red=");
    Serial.println(ledred);
    lcd.setCursor(0, 0);
    if (ledred == 0) {
      Serial.println("Led red passed");
      redcheckedLed = true;
      lcd.print("Red led checked ");
    } else {
      Serial.println("Led red failed");
      lcd.print("Red led failed  ");
    }

    Serial.println(rawValueLD1 * (5.0 / 1023.0));
    Serial.println(rawValueLD2 * (5.0 / 1023.0));
    Serial.println(rawValueLD3 * (5.0 / 1023.0));
    voltageLD = rawValueLD * (5.0 / 1023.0);
    lcd.setCursor(0, 1);
    lcd.print("voltageLD=");
    lcd.print(voltageLD);
    lcd.print("V"); 
    Serial.print("VoltageLD=");
    Serial.println(voltageLD);
    checked4 = true; 
      if (voltageLD >= 1.9){
        checkedV = true;
      } else {
        checkedV = false;
      }
  Serial.print("checkedV=");
  if (checkedV) {
    Serial.println("true");
  } else {
    Serial.println("false");
  }
  
  Serial.println("Finish");
  Serial.print("Checked");
  Serial.print(bluecheckedLed);
  Serial.print(redcheckedLed);
  Serial.println(checkedV);
  if (bluecheckedLed && checkedV && redcheckedLed) {
    digitalWrite(greenLed, HIGH);
    Serial.println("PASSED");
  } else {
    digitalWrite(redLed, HIGH);
    Serial.println("FAILED");
  } 
    }
}

float average(int a, int b, int c) {
  float avg = (a + b + c) / 3.0;
  float diffA = abs(a - avg);
  float diffB = abs(b - avg);
  float diffC = abs(c - avg);
  int closest;
  if (diffA <= diffB && diffA <= diffC) {
    closest = a;
  } else if (diffB <= diffA && diffB <= diffC) {
    closest = b;
  } else {
    closest = c;
  }
  return closest;
}
