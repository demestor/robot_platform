/*   Данный скетч делает следующее: передатчик (TX) отправляет массив
     данных, который генерируется согласно показаниям с кнопки и с
     двух потенциомтеров. Приёмник (RX) получает массив, и записывает
     данные на реле, сервомашинку и генерирует ШИМ сигнал на транзистор.
    by AlexGyver 2016
*/

#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля
#include <EncButton.h>

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги
int X_state, Y_state;
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб
byte defolt = 0;
byte forward = 1;
byte right = 2;
byte left = 3;
byte backward = 4;
byte pegim = 5;
byte transmit_data[4] = {};
bool btn_was_up = false;
bool btn_up = true;
bool mode = 0;
Button btn(8);
Button btn_mode(6);

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
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();        // начать работу
  radio.stopListening();  // не слушаем радиоэфир, мы передатчик
  pinMode(2, INPUT_PULLUP); // Настройка пина 2 как входа с подтягивающим резистором
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(13, OUTPUT); // Настройка пина 13 как выхода (светодиод)
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  //pinMode(8, INPUT_PULLUP);
}
bool send = 1;
void loop() {
  int func_state = 0;
  int buttonState_1 = digitalRead(2); // Чтение состояния кнопки
  int buttonState_2 = digitalRead(3); // Чтение состояния кнопки
  int buttonState_3 = digitalRead(4); // Чтение состояния кнопки
  int buttonState_4 = digitalRead(5); // Чтение состояния кнопки
  bool btn_is_up = digitalRead(6);
  X_state = analogRead(A0);
  Y_state = analogRead(A1);
  //bool BTN_state = digitalRead(8);

  
  btn_mode.tick();
    if (btn_mode.click()) func_state = 5;
  btn.tick();
    if (btn.click()) send = !send;

  /*bool status = 0;
  if (BTN_state != btn_up) {  // состояние изменилось
        status = 0;
        BTN_state = btn_up;     // запомнить новое
        if (btn_up){send = !send; delay(50);}
    btn_up = BTN_state;}*/

  //Serial.println(X_state);
  //Serial.println(Y_state);
  if (buttonState_1 == LOW) { // Если кнопка нажата (LOW, так как используется INPUT_PULLUP)
      func_state = 1;
  }
  if (buttonState_2 == LOW) { // Если кнопка нажата (LOW, так как используется INPUT_PULLUP)

      func_state = 2;
  }
  if (buttonState_3 == LOW) { // Если кнопка нажата (LOW, так как используется INPUT_PULLUP

      func_state = 3;
  }
  if (buttonState_4 == LOW) { // Если кнопка нажата (LOW, так как используется INPUT_PULLUP
      func_state = 4;
  }
  //if (buttonState_5 == LOW) { // Если кнопка нажата (LOW, так как используется INPUT_PULLUP
/*     if (!btn_is_up && btn_was_up){
          delay(10);
          btn_is_up = digitalRead(6);

          if(!btn_is_up){
            func_state = 5;
          }
      }
    btn_was_up = btn_is_up;
  //}*/
    /*switch (func_state){
    case 1:
      radio.write(&forward, sizeof(forward));
      //Serial.println(forward);
      break;
    case 2: 
      radio.write(&right, sizeof(right));
      Serial.println(right);
      break;
    case 3:
      radio.write(&left, sizeof(left));
      Serial.println(left);
      break;
    case 4:
      radio.write(&backward, sizeof(backward));
      Serial.println(backward);
      break;
    case 5:
      radio.write(&pegim, sizeof(pegim));
      Serial.println(pegim);
      break;
    default:
      radio.write(&defolt, sizeof(defolt));
      Serial.println(defolt);
      break;
  }*/
  transmit_data[0] = func_state;
  transmit_data[1] = map(X_state, 0, 1024, 0, 255);
  transmit_data[2] = map(Y_state, 0, 1024, 0, 255);
  transmit_data[3] = send;
  radio.write(&transmit_data, sizeof(transmit_data));
  Serial.println(transmit_data[0]);
  delay(50);
}
