const int analogPin = A2;    // Пин для измерения
const float Vcc = 5.0;       // Напряжение питания Arduino (в Вольтах)
const float R1 = 4660.0;    // Точное сопротивление известного резистора (10 кОм)

void setup() {
  Serial.begin(9600);
}

void loop() {
  float RX = resistor(Vcc, R1);
  Serial.print("RX=");
  Serial.println(RX);  
  delay(2000); // Опрос раз в  2 секунды
}

float resistor(float Vcc, float R1) {
  int raw = analogRead(analogPin); 
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
