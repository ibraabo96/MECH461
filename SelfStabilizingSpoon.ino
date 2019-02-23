//Coded by Ibrahim Abouhassanin -- 2118222
//Syed Mohammad Meesam Raza --  

#include <Wire.h>
#include <Servo.h>   

Servo sgX;
Servo sgY;          

int servo_pin = 2;
int servo_pin3 = 8;

float T, pT, dT;
long acX, acY, acZ;
long gyrX, gyrY, gyrZ;
float gDegX, gDegY, gDegZ;
double aDegX, aDegY, aDegZ;
double aX, aY, aZ;
double X, Y, Z;


//offset values used for calibration
float gOffsetX = 19.0;
float gOffsetY = -66.0;
float gOffsetZ = -47.0;

float aOffsetX = 1355.0;
float aOffsetY = 613.0;
float aOffsetZ = -1013;

float rad_deg= 57.295779513;

void setup ( )

{ 

sgX.attach ( servo_pin );
sgY.attach ( servo_pin3 );


gDegX=0; gDegY=0; gDegZ=0;
aDegX=0; aDegY=0; aDegZ=0;

Wire.begin();

//config
Wire.beginTransmission(0x68);
Wire.write(0x6B);
Wire.write(0x0); 
Wire.endTransmission();  
Wire.beginTransmission(0x68);
Wire.write(0x1B); 
Wire.write(0x0);
Wire.endTransmission(); 
Wire.beginTransmission(0x68);
Wire.write(0x1C);
Wire.write(0x0); 
Wire.endTransmission(); 

T = millis();
}


void loop ( ) 

{ 
pT = T;
T = millis();
dT = (T - pT) / 1000;

//gyro readings
Wire.beginTransmission(0x68);
Wire.write(0x43);
Wire.endTransmission();
Wire.requestFrom(0x68,6);
while(Wire.available() < 6);
{
gyrX = Wire.read()<<8|Wire.read();
gyrY = Wire.read()<<8|Wire.read();
gyrZ = Wire.read()<<8|Wire.read();
}


//converting the raw (LSB) values to real values
gyrX = (gyrX + gOffsetX) / 131.0; 
gyrY = (gyrY + gOffsetY) / 131.0;
gyrZ = (gyrZ + gOffsetZ) / 131.0;


//accelerometer readings
Wire.beginTransmission(0x68);
Wire.write(0x3B); 
Wire.endTransmission();
Wire.requestFrom(0x68,6);
while(Wire.available() < 6);
{
acX = Wire.read()<<8|Wire.read();
acY = Wire.read()<<8|Wire.read();
acZ = Wire.read()<<8|Wire.read();
}

aX = (acX + aOffsetX) / 16384.0;
aY = (acY + aOffsetY) / 16384.0;
aZ = (acZ + aOffsetZ) / 16384.0;

//integrating the deg/s to get degrees
gDegX += gyrX*dT;
gDegY += gyrY*dT;
gDegZ += gyrZ*dT;

//calculating the angles from the distribution of gravity over the accelerometer
aDegY = -rad_deg * atan(aX / sqrt(square(aY) + square(aZ)));
aDegX = rad_deg * atan(aY / sqrt(square(aX) + square(aZ)));

//the complementary filter
X = 0.93*(X + gyrX*dT)+ (0.07*aDegX);
Y = 0.93*(Y + gyrY*dT)+ (0.07*aDegY);

//commanding the servos to move between 90->0 or 90->180
sgX.write((-X)+90);
sgY.write((-Y)+90);
delay (10);

}
