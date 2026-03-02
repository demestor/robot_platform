#include <Wire.h>
#include "ServoSmooth.h"

int x, y, state;
bool btn = 1;

int real_x = 1900;
int real_y = 1000;

int add_x, add_y;

uint32_t myTimer;

ServoSmooth servoX;
ServoSmooth servoY;
ServoSmooth grip;

void receiveEvent(int bytes){
  if (Wire.available() >= 4){
    state = (int)Wire.read();
    y = (int)Wire.read();
    x = (int)Wire.read();
    btn = Wire.read();
  }
  //Serial.println(x);
}

void setup() {
  servoX.attach(5, 120);
  servoX.smoothStart();

  servoY.attach(6, 120);
  servoY.smoothStart();

  grip.attach(9);

  Serial.begin(115200);

  Wire.begin(1);                // адрес слейва
  Wire.setClock(50000);
  Wire.onReceive(receiveEvent); // обработчик
}

void loop() {
  // ====== ВЫЧИСЛЕНИЕ ======
  if (millis() - myTimer >= 10 && state == 10) {
    myTimer = millis();

    if(x >= 129 || x <= 120){
      add_x = map(x, 0, 254, -10, 10) - 1;
      real_x = constrain(real_x+add_x, 1700, 2600);
      servoX.setTarget(real_x);
    }

    if(y >= 129 || y <= 120){
      add_y = map(y, 0, 254, 10, -10);
      real_y = constrain(real_y+add_y/2, 800, 2000);
      servoY.setTarget(real_y);
    }
  }
  servoX.tick();
  servoY.tick();
  //Serial.print("X");
  //Serial.println(real_x);
  //Serial.print("Y");
  //Serial.println(real_y);
  //Serial.println();

  // ====== ХВАТ ======
  grip.setTarget(btn == 0 ? 1200 : 500);
  grip.tick();
}