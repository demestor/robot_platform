#include <Wire.h>

int x, y;
float real_x = 1500;
float real_y = 1500;
float add_x, add_y;
bool btn = 1;
bool state = 1;
//Servo servo;

void setup() {
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  //servo_x.attach(5);
  //servo_y.asyncMode();
  //servo_x.asyncMode();
  //servo_x.delayMode();
  Serial.begin(9600);
  //Wire.onReceive(receiveEvent);
}

void receiveEvent(int bytes) {
  while (Wire.available()) {
    x = Wire.read();
    y = Wire.read();
    btn = Wire.read();
    //servo_y.tick(); //ошибка пропущена точка заптая??? настроить диапазон вращения заного, сбился диапазон.
    add_x = map(x, 0, 254, 10, -10);
    add_y = map(y, 0, 254, 10, -10);
    real_x = constrain(real_x+add_x, 1500, 2500);
    real_y = constrain(real_y+add_y/2, 1000, 2500);

    //servo_x.tick();
    //servo_y.tick();
  //delay(5);
    Serial.print("x: ");
    Serial.println(add_x);
    //Serial.println(real_x);
    //Serial.println(add_y);
    //Serial.print("y: ");
    //Serial.println(real_y);
    //servo.write(180);
    //servo_x.write(real_x);
    //servo_x.write(real_x);
    digitalWrite(5, HIGH);
    delayMicroseconds(real_x);
    digitalWrite(5, LOW);

    digitalWrite(6, HIGH);
    delayMicroseconds(real_y);
    digitalWrite(6, LOW);
    delay(20);
    
    /*if (btn == 0){
      state = !state;
      //Serial.println("ZOV");
      delay(100);
    }*/
    if (btn == 0){
      digitalWrite(9,HIGH);
      delayMicroseconds(1200);
      digitalWrite(9, LOW);
      //Serial.println("ZOVVVVVVVVVVVVVVVVVVVV");

    }
    else{
      digitalWrite(9,HIGH);
      delayMicroseconds(500);
      digitalWrite(9, LOW);
    }
  }
}

void loop() {
  //servo.write(180);
  
  Wire.begin(1); // адрес Slave
  Wire.onReceive(receiveEvent);
  //analogWrite(6, y);
  
}





