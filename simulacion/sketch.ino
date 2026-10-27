// =====================================================
// YoLo v1 — Simulación en Wokwi (Fase 2b)
// Capa 1: sensor ultrasónico
// Mide la distancia cada 60 ms, filtra con mediana de 3
// y la muestra por el monitor serie.
// =====================================================

// ---------- Pines (simulación con ESP32 DevKit) ----------
// En la ESP32-CAM real: TRIG = 1 (U0T), ECHO = 3 (U0R)
const int PIN_TRIG = 5;
const int PIN_ECHO = 18;

// Motores representados con LEDs (se usan en la capa 4)
// En la ESP32-CAM real: IN1 = 4, IN2 = 13, IN3 = 14, IN4 = 15
const int PIN_IN1 = 25;
const int PIN_IN2 = 26;
const int PIN_IN3 = 27;
const int PIN_IN4 = 33;

// ---------- Parámetros ----------
const unsigned long INTERVALO_MS = 60;      // cada cuánto se mide
const unsigned long TIMEOUT_ECO_US = 30000; // ~5 m ida y vuelta
const float DISTANCIA_MAX_CM = 400.0;       // si no hay eco, se toma como "lejos"

// ---------- Estado ----------
float lecturas[3] = {DISTANCIA_MAX_CM, DISTANCIA_MAX_CM, DISTANCIA_MAX_CM};
int indice = 0;
unsigned long ultimaMedicion = 0;

// Dispara el sensor y devuelve la distancia en cm
float medirCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long duracion = pulseIn(PIN_ECHO, HIGH, TIMEOUT_ECO_US);
  if (duracion == 0) return DISTANCIA_MAX_CM;  // sin eco

  return duracion * 0.0343 / 2.0;  // velocidad del sonido: 0,0343 cm/µs
}

// Devuelve el valor del medio de tres lecturas (descarta picos)
float mediana3(float a, float b, float c) {
  if (a > b) { float t = a; a = b; b = t; }
  if (b > c) { float t = b; b = c; c = t; }
  if (a > b) { float t = a; a = b; b = t; }
  return b;
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  // Motores apagados desde el arranque
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);

  Serial.println("YoLo v1 - Capa 1: sensor listo");
}

void loop() {
  if (millis() - ultimaMedicion >= INTERVALO_MS) {
    ultimaMedicion = millis();

    lecturas[indice] = medirCm();
    indice = (indice + 1) % 3;

    float distancia = mediana3(lecturas[0], lecturas[1], lecturas[2]);

    Serial.print("Distancia: ");
    Serial.print(distancia, 1);
    Serial.println(" cm");
  }
}
