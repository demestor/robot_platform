#include <Wire.h>
#include <SPI.h>
#include "RF24.h"
#include "nRF24L01.h"
#include <NewPing.h>

RF24 radio(9, 10);
byte address[][6] = {"1Node"};

NewPing sonar(8, A0, 100);

int mode = 0;
int x, y;
bool hand_state = 1;

// ====== ДВИЖЕНИЕ ======
void forward(int speed_R = 255, int speed_L=255){
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  analogWrite(5, speed_R);
  analogWrite(6, speed_L);
}
void stop(){
  analogWrite(5, 0);
  analogWrite(6, 0);
}
void right1(int speed_R = 10, int speed_L=255){
  digitalWrite(4, LOW);
  digitalWrite(7, LOW);
  analogWrite(5, speed_R);
  analogWrite(6, speed_L);
}
void left1(int speed_R = 255, int speed_L=10){
  digitalWrite(4, LOW);
  digitalWrite(7, HIGH);
  analogWrite(5, speed_R);
  analogWrite(6, speed_L);
}
void back(int speed_R = 200, int speed_L=200){
  digitalWrite(4, HIGH);
  digitalWrite(7, HIGH);
  analogWrite(5, speed_R);
  analogWrite(6, speed_L);
}

// ====== УЛЬТРАЗВУК ======
void ultraSonic_move(){
  int flag = sonar.ping_cm();

  if (flag >= 9 || flag == 0){
    forward(175,175);
  } else {
    int statik = random(1,3);
    back();
    delay(700);
    statik == 1 ? right1() : left1();
    delay(750);
  }
}

void setup() {
  Wire.begin();              // мастер
  Wire.setClock(50000);
  Serial.begin(9600);

  randomSeed(analogRead(A1));

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0,15);
  radio.enableAckPayload();
  radio.setPayloadSize(32);
  radio.openReadingPipe(1, address[0]);
  radio.setChannel(0x60);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  pinMode(4, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop() {
  byte pipeNo;
  uint8_t gotByte[4];

  while (radio.available(&pipeNo)) {
    radio.read(&gotByte, sizeof(gotByte));
    Serial.println(gotByte[0]);
    x = gotByte[1];
    y = gotByte[2];
    hand_state = gotByte[3];

    switch (gotByte[0]){
      case 11: forward(); break;
      case 12: right1(); break;
      case 13: left1(); break;
      case 14: back(); break;
      case 15: ultraSonic_move(); break;
      case 10: stop(); break;
    }
  }

  // ====== ОТПРАВКА НА СЛЕЙВ ======
  Wire.beginTransmission(1);
  Wire.write(gotByte[0]);
  Wire.write(x);
  Wire.write(y);
  Wire.write(hand_state);
  Wire.endTransmission();

}