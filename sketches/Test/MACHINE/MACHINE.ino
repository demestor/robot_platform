/*   Данный скетч делает следующее: передатчик (TX) отправляет массив
     данных, который генерируется согласно показаниям с кнопки и с
     двух потенциомтеров. Приёмник (RX) получает массив, и записывает
     данные на реле, сервомашинку и генерирует ШИМ сигнал на транзистор.
    by AlexGyver 2016
*/
#include<Wire.h>
#include <SPI.h>
#include "RF24.h"
#include "nRF24L01.h"
#include <NewPing.h>
RF24 radio(9, 10);  // "создать" модуль на пинах 9 и 10 Для Уно
//RF24 radio(9,53); // для Меги

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

NewPing sonar(8, A0, 100);

int status = 0;

int x, y;
bool btn = 1;
bool res = 0;

void forward(int speed_R = 255, int speed_L= 255){
    digitalWrite(4, LOW);
    digitalWrite(7, LOW);
    analogWrite(5, speed_R);
    analogWrite(6, speed_L);
}
void stop(){
    analogWrite(5, 0);
    analogWrite(6, 0);
}
void right1(int speed_R = 10, int speed_L= 255){
    digitalWrite(4, LOW);
    digitalWrite(7, LOW);
    analogWrite(5, speed_R);
    analogWrite(6, speed_L);
}
void left1(int speed_R = 255, int speed_L= 10){
    digitalWrite(4, LOW);
    digitalWrite(7, HIGH);
    analogWrite(5, speed_R);
    analogWrite(6, speed_L);
}
void back(int speed_R = 200, int speed_L= 200){
    digitalWrite(4, HIGH);
    digitalWrite(7, HIGH);
    analogWrite(5, speed_R);
    analogWrite(6, speed_L);
}

void line_forward(bool left, bool right){


  if (left    == right) {
        // едем вперёд
          Serial.println("llj");
      forward(200, 200);
  } else if (left) {
        // левый сенсор упёрся в трек
        // поворачиваем налево

      right1(10, 200);
  } else {
      left1(200, 10);
    }}

void ultraSonic_move(){

    int flag = sonar.ping_cm();
    Serial.println(flag);
    if (flag >= 9 || flag == 0){
      forward(255, 255);
    }else{
      int statik = rand() % (2 + 1 - 1) + 1;
      back();
      delay(700);
      statik == 1 ? right1() : left1();
      delay(750);
    }
}

void switch_status(){
  status = status + 1;
  if (status == 2){
    status = 0;
  }
  delay(1000);
}
void setup() {
  pinMode(3, INPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(4, OUTPUT);
  pinMode(2, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
 
  /*digitalWrite(4, HIGH);
  digitalWrite(7, HIGH);*/
  Wire.begin();
  Serial.begin(9600);         // открываем порт для связи с ПК
  radio.begin();              // активировать модуль
  radio.setAutoAck(1);        // режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    // (время между попыткой достучаться, число попыток)
  radio.enableAckPayload();   // разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(32);   // размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);   // хотим слушать трубу 0
  radio.setChannel(0x60);     // выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX);   // уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_250KBPS); // скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp();        // начать работу
  radio.startListening(); // начинаем слушать эфир, мы приёмный модуль

}

void loop() {


  int targetState;
  byte pipeNo;
  uint8_t gotByte[4];
  while (radio.available(&pipeNo)) { 
    radio.read(&gotByte, sizeof(gotByte));  // чиатем входящий сигнал
    Serial.print("Recieved: ");
    Serial.println(gotByte[1]);
    x = gotByte[1];
    y = gotByte[2];
    btn = gotByte[3];
    /*unsigned int distance = sonar.ping_cm();
    Serial.print(distance);
    Serial.println("см");*/

    bool left = !digitalRead(2);
    bool right = !digitalRead(3);
    if (status == 1){
     ultraSonic_move();
     delay(100);
    }
    switch (gotByte[0]){
    case 1:
      forward();
      break;
    case 2: 
      right1();
      break;
    case 3:
      left1();
      break;
    case 4:
      back();
      break;
    case 5:
      switch_status();
      break;
    default:
      stop();
      break;
      }
  }
  /*if (status == 2){
    ultraSonic_move();
  }*/
  Wire.beginTransmission(1);
  Wire.write(x);
  Wire.write(y);
  Wire.write(btn);
  res = Wire.endTransmission();
  if (res){
    Serial.println("error");
  }

  }

  /*
  if (status == 1){
  if (left == right) {
        // едем вперёд
      targetState = 1;
      forward();
  } else if (left) {
        // левый сенсор упёрся в трек
        // поворачиваем налево
      targetState = 2;
      left1();
  } else {
      targetState = 3;
      right1();
    }
    Serial.println(targetState);
  }}}*\
  /*Serial.println(left);
  Serial.println(right);*/


