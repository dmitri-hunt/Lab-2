#include<Wire.h>

//Pin Declarations
const int joystick_x = 0; // analog pin connected to X output
const int joystick_y = 1; // analog pin connected to Y output
const int active_buzzer = 51;

//Global Variables
int joystick_x_reading;
int joystick_y_reading;
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int incomingByte = 0;
long int shake=0;

void joystick_serial(){
  joystick_x_reading = analogRead(joystick_x);
  joystick_y_reading = analogRead(joystick_y);
  joystick_x_reading = joystick_x_reading - 512;
  joystick_y_reading = joystick_y_reading - 512;
  shake = abs(AcX) + abs(AcY) + abs(AcZ);
  
  if (shake > 30000){
    Serial.print('g');
  }
  else if(abs(joystick_x_reading)>abs(joystick_y_reading)){
    if(joystick_x_reading < -200){
      Serial.print('l');     
    }
    else if (joystick_x_reading > 200){
      Serial.print('r');
    }
  }
  else{
    if(joystick_y_reading < -200){
      Serial.print('d');
    }
    else if(joystick_y_reading > 200){
      Serial.print('u');
    }
  }  
  
}

void read_ag(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void ag_serial(){
  read_ag();
  if(abs(GyX)>abs(GyY)){
    if(GyX < -10000){
      Serial.print('r');     
    }
    else if (GyX > 10000){
      Serial.print('l');
    }
  }
  else{
    if(GyY < -10000){
      Serial.print('u');
    }
    else if(GyY > 10000){
      Serial.print('d');
    }
  }
  
}

void beep(){
  digitalWrite(active_buzzer, HIGH);
  delay(10);
  digitalWrite(active_buzzer, LOW);
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  pinMode(active_buzzer, OUTPUT);
  Serial.begin(9600);
}


void loop() {
  // put your main code here, to run repeatedly:
  joystick_serial();
  ag_serial();
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if(incomingByte == 'E'){
      beep();
    }
  }
  delay(50);
  
}
