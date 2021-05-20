
#include <MPU6050_tockn.h>
#include <Wire.h>

// name the motor control pins
#define PWMa 7
#define PWMb 5
#define PWMc 8
#define PWMd 6

MPU6050 mpu6050(Wire);

int command_input = 0;
char char_progress[16] = "incomplete"; //Create String;

void setup() {
  Serial.begin(9600);
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  pinMode(PWMa, OUTPUT); // configure the motor control pins as outputs
  pinMode(PWMb, OUTPUT);
  pinMode(PWMc, OUTPUT);
  pinMode(PWMd, OUTPUT);
  Wire.begin(0x07); //Set Arduino up as an I2C slave at address 0x07
  Wire.onRequest(requestEvent); //Prepare to send data
  Wire.onReceive(receiveEvent); //Prepare to recieve data
}

void loop() {
  mpu6050.update();
  mpu6050.getAngleZ() == 0;
  //i2c get data from raspberry pi
  //Serial.println(command_input);

  if (command_input >= 100) //reverse for n seconds
  {
    int n_seconds_reverse;
    n_seconds_reverse = command_input - 100;
    Serial.println("Reverse");
    Serial.println(n_seconds_reverse);
    digitalWrite(PWMa, LOW);
    analogWrite(PWMb, 130);
    digitalWrite(PWMc, LOW);
    analogWrite(PWMd, 130);
    delay(n_seconds_reverse * 1000);
    Serial.println("Completed reverse");
    digitalWrite(PWMa, LOW);
    digitalWrite(PWMb, LOW);
    digitalWrite(PWMc, LOW);
    digitalWrite(PWMd, LOW);
    command_input = 0;
  }
  else if (command_input >= 50)//forward for n
  {
    int n_seconds_forward;
    n_seconds_forward = command_input - 50;
    Serial.println("Forward");
    Serial.println(n_seconds_forward);
    digitalWrite(PWMa, HIGH);
    analogWrite(PWMb, 130);
    digitalWrite(PWMc, HIGH);//left motor
    analogWrite(PWMd, 130);
    delay(n_seconds_forward * 1000);
    Serial.println("Completed forward");
    digitalWrite(PWMa, LOW);
    digitalWrite(PWMb, LOW);
    digitalWrite(PWMc, LOW);
    digitalWrite(PWMd, LOW);
    command_input = 0;
  }
  else if (command_input == 2)//left for 90
  {
    float angle_pre_left;
    float angle_current_left;
    mpu6050.update();
    angle_pre_left = mpu6050.getAngleZ();
    angle_current_left = mpu6050.getAngleZ();
    Serial.println("start 90 left");
    while ((angle_current_left - angle_pre_left) <= 90)
    {
      mpu6050.update();
      angle_current_left = mpu6050.getAngleZ();
      //turn left
      digitalWrite(PWMa, HIGH);
      analogWrite(PWMb, 130);
      digitalWrite(PWMc, LOW);
      analogWrite(PWMd, 130);
      Serial.println((angle_current_left - angle_pre_left));
    }
    digitalWrite(PWMa, LOW);
    digitalWrite(PWMb, LOW);
    digitalWrite(PWMc, LOW);
    digitalWrite(PWMd, LOW);
    command_input = 0;
    Serial.println("end Left 90");
  }
  else if (command_input == 3)//right for 90
  {
    float angle_pre_right;
    float angle_current_right;
    mpu6050.update();
    angle_pre_right = mpu6050.getAngleZ();
    angle_current_right = mpu6050.getAngleZ();
    Serial.println("start 90 right");
    while ((angle_current_right - angle_pre_right) >= -90)
    {
      mpu6050.update();
      angle_current_right = mpu6050.getAngleZ();
      digitalWrite(PWMa, LOW);
      analogWrite(PWMb, 130);
      digitalWrite(PWMc, HIGH);
      analogWrite(PWMd, 130);
      Serial.println((angle_current_right - angle_pre_right));
    }
    digitalWrite(PWMa, LOW);
    digitalWrite(PWMb, LOW);
    digitalWrite(PWMc, LOW);
    digitalWrite(PWMd, LOW);
    command_input = 0;
    Serial.println("end Right 90");
  }
  else if (command_input == 4)//180
  {
    float angle_pre_180;
    float angle_current_180;
    mpu6050.update();
    angle_pre_180 = mpu6050.getAngleZ();
    angle_current_180 = mpu6050.getAngleZ();
    Serial.println("start 180");
    while ((angle_current_180 - angle_pre_180) >= -180)
    {
      mpu6050.update();
      angle_current_180 = mpu6050.getAngleZ();
      digitalWrite(PWMa, LOW);
      analogWrite(PWMb, 130);
      digitalWrite(PWMc, HIGH);
      analogWrite(PWMd, 130);
      Serial.println((angle_current_180 - angle_pre_180));
    }
    digitalWrite(PWMa, LOW);
    digitalWrite(PWMb, LOW);
    digitalWrite(PWMc, LOW);
    digitalWrite(PWMd, LOW);
    command_input = 0;
    Serial.println("end 180");
  }
  else if (command_input == 5)//360
  {
    float angle_pre_360;
    float angle_current_360;
    mpu6050.update();
    angle_pre_360 = mpu6050.getAngleZ();
    angle_current_360 = mpu6050.getAngleZ();
    Serial.println("start 360");
    while ((angle_current_360 - angle_pre_360) >= -360)
    {
      mpu6050.update();
      angle_current_360 = mpu6050.getAngleZ();
      digitalWrite(PWMa, LOW);
      analogWrite(PWMb, 130);
      digitalWrite(PWMc, HIGH);
      analogWrite(PWMd, 130);
      Serial.println((angle_current_360 - angle_pre_360));
    }
    digitalWrite(PWMa, LOW);
    digitalWrite(PWMb, LOW);
    digitalWrite(PWMc, LOW);
    digitalWrite(PWMd, LOW);
    command_input = 0;
    Serial.println(" end 360");
  }
  mpu6050.update();
  //Serial.print("angleZ : ");
  //Serial.println(mpu6050.getAngleZ());
  delay(100);
}


void requestEvent()
{
  Wire.write(char_progress, 16); //Write String to Pi.
}

void receiveEvent(int numBytes) {
  //Set Up Vars
  int count = 0;
  //We'll recieve one byte at a time. Stop when none left
  while (Wire.available())
  {
    char c = Wire.read();    // receive a byte as character
    //Create Int from the Byte Array
    command_input = c << (8 * count) | command_input;
    count++;
  }
}
