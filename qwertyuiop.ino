#include <Servo.h> // esto es una libreria de servos
Servo servo_rem;

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT); //aqui el arduino usara el pin A01 y resibira informacion
  servo_rem.attach(5, 900, 2100);//se va a usar el pin numero 5 y los otros son valores limites de pulso en microsegundos. y el attach sirve para conectar el servo al pin digital.

}

void loop() {
  // put your main code here, to run repeatedly:
  servo_rem.write(analogRead(0) / (1023/180)); //aqui se divide el numero de movimientos del potenciometro por el numero de grados que maneja el microservo
  delay(10); // este delay para no sobre calentar el micro servo
}
