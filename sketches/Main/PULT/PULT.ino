
#include <SPI.h>          
#include "nRF24L01.h"     
#include "RF24.h"         
#include <EncButton.h>

RF24 radio(9, 10); 

int X_state, Y_state;
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; 
byte defolt = 0;            // Отправляем по умолчанию на машинку, активирует функцию стоп на машинке
byte forward = 1;           // Отправляем при нажатии 1 кнопки, активирует функцию  езды вперед на машинке
byte right = 2;             // Отправляем при нажатии 2 кнопки, активирует функцию поворота направо на машинке
byte left = 3;              // Отправляем при нажатии 3 кнопки, активирует функцию поворота налево на машинке
byte backward = 4;          // Отправляем при нажатии 4 кнопки, активирует функцию езды назад на машинке
byte pegim = 5;             // Отправляем при нажатии 5 кнопки, меняет режим на машинке
byte transmit_data[4] = {}; // Массив с данными, отправляемыми на машинку
Button btn(8);              // Инилизация "Улучшеной кнопки" для закрытия манипулятора
Button btn_mode(6);         // Инилизация "Улучшеной кнопки" для смены режима

void setup() {
  Serial.begin(9600);         // открываем порт для связи с ПК

  radio.begin();              // активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openWritingPipe(address[0]);  // мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);             // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS


  radio.powerUp();        
  radio.stopListening();  
  pinMode(2, INPUT_PULLUP); 
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

}
bool send = 1;
int mode = 1;
int func_state = 10; 
void loop() {      
  int buttonForward = digitalRead(2);         // Чтение состояния кнопки
  int buttonRight = digitalRead(3);         // Чтение состояния кнопки
  int buttonLeft = digitalRead(4);         // Чтение состояния кнопки
  int buttonBackward = digitalRead(5);         // Чтение состояния кнопки
  X_state = analogRead(A0);                   //Ось Х
  Y_state = analogRead(A1);                   //Ось У
  //Улучшенное считывание значения с кнопки
  btn_mode.tick();
    if (btn_mode.click()) mode = !mode;   // Присваивание значения действия машинки (смена режима)
  btn.tick();
    if (btn.click()) send = !send;          // Присваивание значения закрытия(открытия) манипулятора
  //Присваивание значения перпеменной в зависимости от нажатой кнопки
  if (mode){
    if (buttonForward == LOW) {
        func_state = 11;
    }
    else if (buttonRight == LOW) {

        func_state = 12;
    }
    else if (buttonLeft == LOW) {

        func_state = 13;
    }
    else if (buttonBackward == LOW) {
        func_state = 14;
    }
    else func_state = 10;
  }
  else func_state = 15;
  //Заполнение массива
  Serial.println(mode);
  Serial.println(func_state);
  transmit_data[0] = func_state;                      // Добавление в массив действия машинки (стоп, вперед, налево, направо, смена режима)
  transmit_data[1] = map(X_state, 0, 1024, 0, 255);   // X ось джойстика
  transmit_data[2] = map(Y_state, 0, 1024, 0, 255);   // У ось джойстика
  transmit_data[3] = send;                            // Закрытие(открытие) манипулятора
  radio.write(&transmit_data, sizeof(transmit_data)); // Отправка
  delay(50);
}
