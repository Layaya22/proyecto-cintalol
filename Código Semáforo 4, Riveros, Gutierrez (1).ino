//Coloca todas las especificaciones para q el programa funcione.
void setup() {
  //Hace que los pines del arduino se seteen como salidas.
  pinMode(LED_BUILTIN, OUTPUT);// put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);  
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}
//Es el código en si, este se pone en bucle indefinidamente
void loop() {
  // Coloca a todos los leds en rojo excepto el número 13 q es un led verde, cada q dice low apaga el led, cada q dice high lo enciende.
  digitalWrite(2, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  //El delay, es un retado para q se ejecute otra acción en un tiempo específico en este caso de 2 seg.
  delay(2000);
  //En esta parte cambia de color los leds, haciendo que un rojo y verde se apaguen y prendan los amarillos.
  digitalWrite(2, LOW);
  digitalWrite(3, HIGH); 
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW); 
  //Este delay es más rápido (1seg) para apagar los leds amarillos y cambiarlos a rojo y verde respectivamente, este parte del código para el cambio de color
  //será ''C2''
  delay(1000);
  digitalWrite(3, LOW);
  digitalWrite(4, HIGH);  
  digitalWrite(5, HIGH);
  digitalWrite(6, LOW);
  //Esta parte es la q mantiene uno de los leds en verde y le pondré ''C1''
  delay(2000);
  //C1
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  delay(1000);
  //C2
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  delay(2000);
  //C1
  digitalWrite(8, LOW);
  digitalWrite(9, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);
  delay(1000);
  //C2
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);
  delay(2000);
  //C1
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  delay(1000);
  //Este delay es para q no se coloquen los parámetros del inicio del bucle instantaneamente despues sin el cambio a amarillo o haga una superposición entre los 2.
}
