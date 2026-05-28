#include <LiquidCrystal.h>
  #include <Stepper.h>
int stepsPerRevolution = 2048;
int motSpeed = 10;
Stepper myStepper(stepsPerRevolution, 8,10,9,11);
int dt = 1500;
const int rs = 6, en = 5, d4 = 4, d5 = 3, d6 = 2, d7 = 1;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

void setup() {
  // put your setup code here, to run once:
 myStepper.setSpeed(motSpeed);
 lcd.begin(16,2)
}

void loop() {
  // put your main code here, to run repeatedly:
 SetCursor(0, 0);
 ldc.print("5")
 myStepper.step(stepsPerRevolution);
 delay(dt);
 ldc.print("5")
 myStepper.step(stepsPerRevolution);
 delay(dt);
 myStepper.step(stepsPerRevolution);
 delay(dt);
 myStepper.step(stepsPerRevolution);
 delay(dt);
 myStepper.step(-stepsPerRevolution);
 delay(dt);
 myStepper.step(-stepsPerRevolution);
 delay(dt);
 myStepper.step(-stepsPerRevolution);
 delay(dt);
  myStepper.step(-stepsPerRevolution);
 delay(dt);
  myStepper.step(-stepsPerRevolution);
 delay(dt);
}
