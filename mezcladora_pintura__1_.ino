/*
 * ============================================================
 *   MEZCLADORA DE PINTURA AUTOMÁTICA  v2.0
 * ============================================================
 * Componentes:
 *  - Potenciómetro          → Pin A0
 *  - Motor Paso a Paso      → Pines 8, 9, 10, 11 (ULN2003)
 *  - LCD 16x2 SIN I2C       → Pines 7, 6, 5, 4, 3, 2
 *                              (RS=7, EN=6, D4=5, D5=4, D6=3, D7=2)
 *  - LEDs barra progreso    → Pines A1, A2, A3, A4  (4 LEDs)
 *  - Sensor Ultrasonido     → Trig=12, Echo=13
 *  - Servo (válvula)        → Pin 11  ← NOTA: comparte timer con stepper,
 *                              mover a pin 9 si hay conflicto en tu placa
 * ============================================================
 *
 *  DIAGRAMA PINES LCD 16x2 (modo 4 bits, SIN I2C):
 *
 *   LCD pin 1  (VSS)  → GND
 *   LCD pin 2  (VDD)  → 5V
 *   LCD pin 3  (V0)   → centro del pot de contraste (10kΩ) o GND directo
 *   LCD pin 4  (RS)   → Arduino pin 7
 *   LCD pin 5  (RW)   → GND
 *   LCD pin 6  (EN)   → Arduino pin 6
 *   LCD pin 11 (D4)   → Arduino pin 5
 *   LCD pin 12 (D5)   → Arduino pin 4
 *   LCD pin 13 (D6)   → Arduino pin 3
 *   LCD pin 14 (D7)   → Arduino pin 2
 *   LCD pin 15 (A)    → 5V (backlight +)
 *   LCD pin 16 (K)    → GND (backlight -)
 *
 *  PARA INICIAR: envía 'S' por el Monitor Serial (9600 baudios)
 * ============================================================
 */

#include <LiquidCrystal.h>   // Librería nativa Arduino (sin I2C)
#include <Stepper.h>
#include <Servo.h>

// ─── LCD SIN I2C ─────────────────────────────────────────
//  LiquidCrystal(RS, EN, D4, D5, D6, D7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

// ─── Motor Paso a Paso ───────────────────────────────────
// 2048 pasos = 1 vuelta completa (motor 28BYJ-48)
#define STEPS_PER_REV 2048
Stepper stepper(STEPS_PER_REV, 8, 10, 9, 11);

// ─── Servo (válvula de descarga) ─────────────────────────
Servo valvula;
#define SERVO_PIN      A5
#define VALVULA_CERRADA 0
#define VALVULA_ABIERTA 90

// ─── Ultrasonido HC-SR04 ─────────────────────────────────
#define TRIG_PIN           12
#define ECHO_PIN           13
#define DISTANCIA_VASO_CM  10   // Detecta vaso solo si está < 10 cm

// ─── LEDs barra de progreso (4 LEDs en pines analógicos) ─
const int LED_PINS[] = {A1, A2, A3, A4};
#define NUM_LEDS 4

// ─── Potenciómetro ───────────────────────────────────────
#define POT_PIN A0

// ─── Caracteres personalizados LCD ───────────────────────
// Flecha derecha  →
byte flechaDer[8] = {
  0b00000,
  0b00100,
  0b00010,
  0b11111,
  0b00010,
  0b00100,
  0b00000,
  0b00000
};
// Flecha izquierda  ←
byte flechaIzq[8] = {
  0b00000,
  0b00100,
  0b01000,
  0b11111,
  0b01000,
  0b00100,
  0b00000,
  0b00000
};
// Checkmark  ✓
byte checkMark[8] = {
  0b00000,
  0b00001,
  0b00011,
  0b10110,
  0b11100,
  0b01000,
  0b00000,
  0b00000
};
// Bloque lleno (para barra progreso en LCD)
byte bloqueOn[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};

// ─── Variables globales ──────────────────────────────────
int  tiempoBatido = 0;
bool mezclando   = false;
bool finalizado  = false;

// ══════════════════════════════════════════════════════════
void setup() {
  Serial.begin(9600);

  // LCD 16x2 sin I2C
  lcd.begin(16, 2);
  lcd.createChar(0, flechaDer);
  lcd.createChar(1, flechaIzq);
  lcd.createChar(2, checkMark);
  lcd.createChar(3, bloqueOn);

  // LEDs como salidas digitales
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  // Ultrasonido
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  valvula.attach(SERVO_PIN);
  valvula.write(VALVULA_CERRADA);

  // Velocidad motor (RPM)
  stepper.setSpeed(15);

  // Pantalla de bienvenida
  lcd.setCursor(0, 0);
  lcd.print("  MEZCLADORA DE ");
  lcd.setCursor(0, 1);
  lcd.print("  PINTURA  v2.0 ");
  delay(2000);
  lcd.clear();

  mostrarPantallaInicio();
}

// ══════════════════════════════════════════════════════════
void loop() {

  if (!mezclando && !finalizado) {

    // Leer potenciómetro → tiempo de batido (4 a 30 seg)
    int valorPot   = analogRead(POT_PIN);
    tiempoBatido   = map(valorPot, 0, 1023, 4, 30);

    // ── Fila 0: tiempo seleccionado ──────────────────────
    lcd.setCursor(0, 0);
    lcd.print("Tiempo: ");
    if (tiempoBatido < 10) lcd.print(" ");
    lcd.print(tiempoBatido);
    lcd.print(" seg    ");

    // ── Fila 1: instrucción ───────────────────────────────
    lcd.setCursor(0, 1);
    lcd.print("Serial 'S'=Start");

    // Preview LEDs proporcional al tiempo
    int ledsActivos = map(tiempoBatido, 4, 30, 1, NUM_LEDS);
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], i < ledsActivos ? HIGH : LOW);
    }

    // Arrancar cuando se recibe 'S' por Serial Monitor
    if (Serial.available()) {
      char cmd = Serial.read();
      if (cmd == 'S' || cmd == 's') {
        iniciarMezcla();
      }
    }

  } else if (mezclando) {
    ejecutarMezcla();

  } else if (finalizado) {
    verificarVasoYDescargar();
  }
}

// ══════════════════════════════════════════════════════════
//   FUNCIONES PRINCIPALES
// ══════════════════════════════════════════════════════════

void mostrarPantallaInicio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gira POT: tiempo");
  lcd.setCursor(0, 1);
  lcd.print("Serial 'S'=Start");
}

// ─── Iniciar mezcla ──────────────────────────────────────
void iniciarMezcla() {
  mezclando = true;
  apagarLEDs();
  lcd.clear();
  Serial.println(">>> Iniciando mezcla...");
}

// ─── Secuencia completa de mezcla ────────────────────────
void ejecutarMezcla() {
  /*
   *  Secuencia:  2 vueltas DERECHA → 2 vueltas IZQUIERDA
   *  Se repite hasta agotar el tiempo seleccionado.
   *  Los 4 LEDs van encendiéndose conforme avanza el tiempo.
   *  El LCD fila 0 muestra: "Batido  a la  →" o "←"
   *  El LCD fila 1 muestra la barra de bloques de progreso.
   */

  long tiempoInicio = millis();
  long tiempoTotal  = (long)tiempoBatido * 1000;
  int  ledEncendido = 0;

  while ((millis() - tiempoInicio) < tiempoTotal) {

    // ── 2 vueltas DERECHA ─────────────────────────────
    for (int v = 0; v < 2; v++) {
      mostrarDireccion(true);
      int pasosRestantes = STEPS_PER_REV;
      while (pasosRestantes > 0) {
        int bloque = min(pasosRestantes, 64);
        stepper.step(bloque);
        pasosRestantes -= bloque;

        long elapsed = millis() - tiempoInicio;
        int prog = map(elapsed, 0, tiempoTotal, 0, NUM_LEDS);
        if (prog > ledEncendido && prog <= NUM_LEDS) {
          ledEncendido = prog;
          actualizarBarraProgreso(ledEncendido);
        }
        if (elapsed >= tiempoTotal) goto fin_mezcla;
      }
    }

    // ── 2 vueltas IZQUIERDA ───────────────────────────
    for (int v = 0; v < 2; v++) {
      mostrarDireccion(false);
      int pasosRestantes = STEPS_PER_REV;
      while (pasosRestantes > 0) {
        int bloque = min(pasosRestantes, 64);
        stepper.step(-bloque);
        pasosRestantes -= bloque;

        long elapsed = millis() - tiempoInicio;
        int prog = map(elapsed, 0, tiempoTotal, 0, NUM_LEDS);
        if (prog > ledEncendido && prog <= NUM_LEDS) {
          ledEncendido = prog;
          actualizarBarraProgreso(ledEncendido);
        }
        if (elapsed >= tiempoTotal) goto fin_mezcla;
      }
    }
  }

  fin_mezcla:

  // Liberar bobinas del motor (evita sobrecalentamiento)
  digitalWrite(8, LOW); digitalWrite(9, LOW);
  digitalWrite(10, LOW); digitalWrite(11, LOW);

  // Barra completamente llena
  actualizarBarraProgreso(NUM_LEDS);

  // Mensaje de fin
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Mezcla lista!  ");
  lcd.setCursor(7, 1);
  lcd.write(byte(2));   // checkmark
  Serial.println(">>> Mezcla completada.");

  delay(1500);
  mezclando  = false;
  finalizado = true;
}

// ─── Detectar vaso y abrir válvula ───────────────────────
void verificarVasoYDescargar() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Acerque el vaso ");
  Serial.println(">>> Esperando vaso bajo sensor (< 10 cm)...");

  bool vasoDetectado = false;

  while (!vasoDetectado) {

    float distancia = medirDistancia();

    Serial.print("Distancia: ");
    Serial.print(distancia, 1);
    Serial.println(" cm");

    lcd.setCursor(0, 1);
    lcd.print("Dist:");
    if (distancia < 100) lcd.print(" ");
    lcd.print((int)distancia);
    lcd.print("cm ");
    // Indicador gráfico en LCD: lleno si cerca
    if (distancia < DISTANCIA_VASO_CM) {
      lcd.print("[OK]    ");
    } else {
      lcd.print("[---]   ");
    }

    if (distancia > 0 && distancia < DISTANCIA_VASO_CM) {
      vasoDetectado = true;
    } else {
      // Parpadeo alterno LEDs mientras espera
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], (i % 2 == 0) ? HIGH : LOW);
      }
      delay(250);
      for (int i = 0; i < NUM_LEDS; i++) {
        digitalWrite(LED_PINS[i], (i % 2 == 1) ? HIGH : LOW);
      }
      delay(250);
    }
  }

  // ── Vaso detectado: abrir válvula ────────────────────
  apagarLEDs();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Vaso detectado! ");
  lcd.setCursor(0, 1);
  lcd.print("Abriendo valvula");
  Serial.println(">>> Vaso OK. Abriendo valvula...");

  for (int ang = VALVULA_CERRADA; ang <= VALVULA_ABIERTA; ang += 5) {
    valvula.write(ang);
    delay(50);
  }

  // LEDs encendiéndose uno a uno como descarga
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], HIGH);
    delay(500);
  }

  delay(3000);  // Tiempo de descarga

  // ── Cerrar válvula ────────────────────────────────────
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cerrando valvula");
  Serial.println(">>> Cerrando valvula.");

  for (int ang = VALVULA_ABIERTA; ang >= VALVULA_CERRADA; ang -= 5) {
    valvula.write(ang);
    delay(50);
  }

  // ── Proceso completo ──────────────────────────────────
  apagarLEDs();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" PROCESO LISTO! ");
  lcd.setCursor(0, 1);
  lcd.print("Reinicia Arduino");
  Serial.println(">>> Proceso completo. Reiniciar para nueva mezcla.");

  // Parpadeo LED0 indicando fin
  while (true) {
    digitalWrite(LED_PINS[0], HIGH); delay(800);
    digitalWrite(LED_PINS[0], LOW);  delay(800);
  }
}

// ══════════════════════════════════════════════════════════
//   FUNCIONES AUXILIARES
// ══════════════════════════════════════════════════════════

/*
 *  Fila 0 → "Batido  a la  ->"  o  "<- Batido  a la  "
 *  Fila 1 → barra de bloques (no se toca aquí, la maneja
 *            actualizarBarraProgreso)
 *
 *  Formato fila 0 (16 chars):
 *    derecha:   "Batido  a la  ->"   (flecha custom al final)
 *    izquierda: "<-Batido  a la  "   (flecha custom al inicio)
 */
void mostrarDireccion(bool derecha) {
  lcd.setCursor(0, 0);
  if (derecha) {
    // "Batido  a la  →"  (carácter 0 = flecha derecha)
    lcd.print("Batido  a la  ");
    lcd.write(byte(0));
    lcd.write(byte(0));
  } else {
    // "←← Batido a la  "
    lcd.write(byte(1));
    lcd.write(byte(1));
    lcd.print(" Batido a la    ");
  }
}

/*
 *  Actualiza los 4 LEDs físicos Y la fila 1 del LCD.
 *  cantidad: 0 = todos apagados, 4 = todos encendidos.
 *
 *  Fila 1 del LCD usa el carácter bloque (char 3) para
 *  representar cada LED encendido y espacio para apagado.
 *  Ejemplo con 2 de 4: "Prog:[## ] "
 */
void actualizarBarraProgreso(int cantidad) {
  // LEDs físicos
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], i < cantidad ? HIGH : LOW);
  }
  // LCD fila 1
  lcd.setCursor(0, 1);
  lcd.print("Prog:[");
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < cantidad) {
      lcd.write(byte(3));  // bloque lleno
    } else {
      lcd.print(" ");
    }
  }
  lcd.print("]       ");
}

void apagarLEDs() {
  for (int i = 0; i < NUM_LEDS; i++) {
    digitalWrite(LED_PINS[i], LOW);
  }
}

/*
 *  Mide distancia con HC-SR04.
 *  Timeout de 600 µs → cubre exactamente 10 cm (ida+vuelta ≈ 583 µs).
 *  Si no hay eco en ese tiempo retorna 999 (= sin objeto cerca).
 */
float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Timeout ajustado a 10 cm: (10*2/0.0343)*1000 ≈ 583000 µs → usamos 600000
  long duracion = pulseIn(ECHO_PIN, HIGH, 600000UL);
  if (duracion == 0) return 999.0;

  return (duracion * 0.0343) / 2.0;
}
