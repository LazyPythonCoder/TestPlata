#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


const int analogPinLD = A2; // Пин для проверки напряжения
const int greenLed = 2;     // Зеленый светодиод
const int redLed = 3;       // Красный светодиод
const int relayPin = 4; // Пин реле 1 для соедениния концевого выключателя
const int sensorPhotoBlue = 5; // Пин для подключения фоторезистора синего
const int relayPin2 = 6; // Пин реле 2 соединяет 3.3 вольта ардуино с разъемом rc платы
const int relayRisistorPin = 8; // Пин реле 3 соединяет  минус выхода взрывателя платы c минусом резистивного делителя она общая точка А1 
const int sensorPhotoRed = 7; // Пин для подключение фоторезистора красного
const int relayGraundResistor = 9; // Пин реле 4 соединяет землю через резистор 20кОм со входом A2 ардуино. Необходимо для точного показания напряжения при обрыва соеденения. 
const int BUTTON_PIN = 10; // Пин кнопки для переключения режимов работы

const int analogPinResistor = A1;    // Пин для измерения
const float Vcc = 5.0;       // Напряжение питания Arduino (в Вольтах)
const float R1 = 4660.0;  // Сопротивление резисторого известного резистивного делителя



// 4 минуты в миллисекундах (4 * 60 * 1000)
int unsigned long waitTime2 = 130000; 
int unsigned long waitTime4 = 240000; 

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
  pinMode(relayGraundResistor, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); 
  delay(50);
  
  lcd.init();           // Инициализация дисплея
  lcd.backlight();      // Включаем подсветку
  
  lcd.clear();
  lcd.setCursor(0, 0);  // Устанавливаем курсор (столбец 0, строка 0)
  lcd.print("TEST START");
  lcd.setCursor(0, 1);  // Переходим на вторую строку 
  
  if (digitalRead(BUTTON_PIN) == LOW) { //Кнопка нажата
    lcd.print("M2, Time=");
    waitTime2 = 130000/2; 
    waitTime4 = 240000/2;
    lcd.print(waitTime4/60000);
    lcd.print(" min");
  } else {                            //Кнопка не нажата
    lcd.print("M1, Time=");
    lcd.print(waitTime4/60000);
    lcd.print(" min");
  }
     
  // Сначала выключаем оба светодиода и выключаем все реле(переводим их в разрыв)
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  digitalWrite(relayPin, LOW);
  digitalWrite(relayPin2, LOW);
  digitalWrite(relayRisistorPin, LOW);
  digitalWrite(relayGraundResistor, LOW);
   }

void loop() {
  // Проверяем, прошло ли 2 минуты и не делали ли мы проверку ранее
  if (!checked2 && millis() >= waitTime2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    int led = digitalRead(sensorPhotoBlue); // Проверяем горит ли синий светодиод
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

  if (!checked4 && millis() >= waitTime4) {  // Если прошло 4 мин и не делали мы ранее то 
    lcd.clear();
    lcd.setCursor(0, 0);
    digitalWrite(relayPin, HIGH);
    digitalWrite(relayGraundResistor, HIGH);
    delay(500);
    // Проверяем напряжение 3 раза и выбираем среднее значение
    rawValueLD1 = analogRead(analogPinLD);
    rawValueLD2 = analogRead(analogPinLD);
    rawValueLD3 = analogRead(analogPinLD);
    rawValueLD = average(rawValueLD1, rawValueLD2, rawValueLD3);
    int ledred = digitalRead(sensorPhotoRed); // считываем данные фоторезитора (0-включен, 1-выключен)
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
    voltageLD = rawValueLD * (5.0 / 1023.0); // Переводим значение в вольты
    lcd.setCursor(0, 1);
    lcd.print("voltageLD=");
    lcd.print(voltageLD);
    lcd.print("V"); 
    Serial.print("VoltageLD=");
    Serial.println(voltageLD);
    checked4 = true; 
      if (voltageLD >= 1.92){ // Проверяем что значение напржения ны выходе взрывателя платы больше 1.92 вольта
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
  float RX = resistor(Vcc, R1);// Находим сопротивление на пине А1 ардуино. Необходимо для проверки того факта, что при замыкании концевика (реле1) или подаче 3.3 В минусовой разъем выхода взрывателя подключен к земле
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

  digitalWrite(relayPin, LOW); // размыкаем концевик
  digitalWrite(relayPin2, HIGH); // Подаем 3.3 В на разъем rc разъем 
  delay(500);
  RX = resistor(Vcc, R1); // Проверяем что при подаче 3.3 вольта сопротивление между минусом разъема взрывателя и землей имеет значение близкое к 0. В резистивном делителя это сопротивлепние есть неизвестное сопротивление
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
