#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


const int analogPinLD = A2; // Пин для проверки напряжения
const int greenLed = 2;     // Зеленый светодиод
const int redLed = 3;       // Красный светодиод
const int relayPin = 4;
const int sensorPhotoBlue = 5;
const int relayPin2 = 6;
const int relayRisistorPin = 8;
const int sensorPhotoRed = 7;

const int analogPinResistor = A1;    // Пин для измерения
const float Vcc = 5.0;       // Напряжение питания Arduino (в Вольтах)
const float R1 = 4660.0;  



// 4 минуты в миллисекундах (4 * 60 * 1000)
const unsigned long waitTime2 = 130000; 
const unsigned long waitTime4 = 242000; 

// const unsigned long waitTime2 = 5000; 
// const unsigned long waitTime4 = 7000; 



bool checked2 = false;
bool checked4 = false;
bool bluecheckedLed = false;
bool redcheckedLed = false;
bool checkedV = false;
bool pinswitchcheck = false;
bool pinrccheck = false;

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
  pinMode(relayRisistorPin, OUTPUT);
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
  digitalWrite(relayRisistorPin, LOW);
   }

void loop() {
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
    lcd.clear();
    lcd.setCursor(0, 0);
    digitalWrite(relayPin, HIGH);
    delay(500);
    rawValueLD1 = analogRead(analogPinLD);
    rawValueLD2 = analogRead(analogPinLD);
    rawValueLD3 = analogRead(analogPinLD);
    rawValueLD = average(rawValueLD1, rawValueLD2, rawValueLD3);
    int ledred = digitalRead(sensorPhotoRed);
    Serial.print("Led red=");
    Serial.println(ledred);
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
      if (voltageLD >= 1.87){
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
  
  Serial.print("Checked");
  Serial.print(bluecheckedLed);
  Serial.print(redcheckedLed);
  Serial.println(checkedV);
  digitalWrite(relayRisistorPin, HIGH);
  delay(500);
  float RX = resistor(Vcc, R1);
  if (RX == 0.0) {
    pinswitchcheck = true;  
  }
  Serial.print("Rx=");
  Serial.println(RX);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RX=");
  lcd.print(RX);
  lcd.setCursor(0, 1);
  lcd.print(pinswitchcheck);
  delay(2000);

  digitalWrite(relayPin, LOW);
  digitalWrite(relayPin2, HIGH);
  delay(500);
  RX = resistor(Vcc, R1);
  if (RX == 0.0) {
    pinrccheck = true;  
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("RXSW=");
  lcd.print(RX);
  lcd.setCursor(0, 1);
  lcd.print(pinrccheck);
  

  if (bluecheckedLed && checkedV && redcheckedLed && pinswitchcheck && pinrccheck) {
    digitalWrite(greenLed, HIGH);
    Serial.println("PASSED");
  } else {
    digitalWrite(redLed, HIGH);
    Serial.println("FAILED");
  } 

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BD=");
  lcd.print(bluecheckedLed);
  lcd.print(",VCH=");
  lcd.print(checkedV);
  lcd.print(",");
  lcd.print(voltageLD);
  lcd.setCursor(0, 1);
  lcd.print("RD=");
  lcd.print(redcheckedLed);
  lcd.print(",SW=");
  lcd.print(pinswitchcheck);
  lcd.print(",RC=");
  lcd.print(pinrccheck);

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

float resistor(float Vcc, float R1) {
  int raw = analogRead(analogPinResistor); 
   // Проверка на короткое замыкание (КЗ)
  Serial.println(raw);
  if (raw < 5) { 
    Serial.println("Статус: КОРОТКОЕ ЗАМЫКАНИЕ! (R ~ 0 Ом)");
    return 0.0;
  } 
  // Проверка на обрыв цепи (нет резистора)
  else if (raw > 1020) {
    Serial.println("Статус: ОБРЫВ ЦЕПИ! (R -> бесконечность)");
    return 1000000000.0;
  } 
  // Расчет сопротивления
  else {
    // Вычисляем напряжение на выходе делителя
    float Vout = (raw * Vcc) / 1023.0; 
    // Формула делителя напряжения для поиска Rx
    float Rx = R1 * (Vout / (Vcc - Vout)); 
    
    Serial.print("Сопротивление: ");
    if (Rx >= 1000.0) {
      Serial.print(Rx / 1000.0, 2);
      Serial.println(" кОм");
    } else {
      Serial.print(Rx, 1);
      Serial.println(" Ом");
    }
    return Rx;
  }
}
