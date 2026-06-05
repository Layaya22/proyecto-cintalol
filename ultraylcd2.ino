#include <LiquidCrystal.h>

// --- LCD: RS, E, D4, D5, D6, D7 ---
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// --- Pines Ultrasonido 1 ---
const int TRIG1 = 6;
const int ECHO1 = 7;

// --- Pines Ultrasonido 2 ---
const int TRIG2 = 8;
const int ECHO2 = 9;

// --- Pines Ultrasonido 3 ---
const int TRIG3 = 10;
const int ECHO3 = 13;

// Umbral de detección en cm
const int UMBRAL = 10;

// --- Función para medir distancia ---
long medirDistancia(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH, 30000);
  long distancia = duracion * 0.034 / 2;
  return distancia;
}

void setup() {
  // Configurar pines ultrasonidos
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);

  // Iniciar LCD 16 columnas, 2 filas
  lcd.begin(16, 2);

  // --- Fila 0: "TACHO LLENO" ---
  lcd.setCursor(0, 0);
  lcd.print("TACHO LLENO");

  // --- Fila 1: comas permanentes en columna 1 y 3 (índice 0) ---
  // Columna 2 del enunciado = índice 1
  // Columna 4 del enunciado = índice 3
  lcd.setCursor(1, 1);
  lcd.print(",");
  lcd.setCursor(3, 1);
  lcd.print(",");
}

void loop() {
  long d1 = medirDistancia(TRIG1, ECHO1);
  long d2 = medirDistancia(TRIG2, ECHO2);
  long d3 = medirDistancia(TRIG3, ECHO3);

  // --- Ultrasonido 1: columna 1 del enunciado = índice 0 ---
  lcd.setCursor(0, 1);
  if (d1 > 0 && d1 < UMBRAL) {
    lcd.print("1");
  } else {
    lcd.print(" ");
  }

  // Redibujar "," fija en índice 1 (columna 2)
  lcd.setCursor(1, 1);
  lcd.print(",");

  // --- Ultrasonido 2: columna 3 del enunciado = índice 2 ---
  lcd.setCursor(2, 1);
  if (d2 > 0 && d2 < UMBRAL) {
    lcd.print("2");
  } else {
    lcd.print(" ");
  }

  // Redibujar "," fija en índice 3 (columna 4)
  lcd.setCursor(3, 1);
  lcd.print(",");

  // --- Ultrasonido 3: columna 5 del enunciado = índice 4 ---
  lcd.setCursor(4, 1);
  if (d3 > 0 && d3 < UMBRAL) {
    lcd.print("3");
  } else {
    lcd.print(" ");
  }

  delay(200);
}