
#include <Servo.h>
Servo servogy; //nombre del servo
int xD;
void setup() {
  servogy.attach(6);
  Serial.begin(9600);
  pinMode(10, 1);
  pinMode(12, 1);      //ponemos los pines de los leds y el buzzer en salida
  pinMode(11, 1);
  servogy.write(0);         //seteamos el servo en 0 grados
}
void loop() { 
  digitalWrite(12, 1);     //prendemos el led verde
  pinMode(8, OUTPUT);          //se pone en salida para que emita señal ultrasonica 
  delay(10);
  digitalWrite(8, 1);          // se activa la señal
  delay(10);
  digitalWrite(8, 0);          //se apaga
  pinMode(8, INPUT);           //se pone en emtrada para poder registrar el pulso 
  xD = pulseIn(8, 1);    //se registra el pulso y se coloca en la variable "duracion"
  if ((xD/58.2) < 50) {             //si está a menos de 50cm se activan el led rojo y el buzzer
    digitalWrite(10, 1);    // tambien el servomotor se pone a 90° 
    digitalWrite(11, 1);
    servogy.write(90);
  } else {
    digitalWrite(10, 0);    //si no se setea todo a 0
    digitalWrite(11, 0);
    servogy.write(0);
  }
}