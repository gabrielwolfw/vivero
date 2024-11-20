#include <DHT.h>
#include <AccelStepper.h>

// Definiciones de pines y tipos de sensor
#define DHTPIN 2       // Pin digital conectado al DHT22
#define DHTTYPE DHT22  // Definir el tipo de sensor DHT

DHT dht(DHTPIN, DHTTYPE);

int soilMoisturePin = A0; // Pin analógico conectado al sensor de humedad de suelo

// Pines para el motor paso a paso y solenoide
#define MotorInterfaceType 4

#define STEPPER_IN1 10
#define STEPPER_IN2 11
#define STEPPER_IN3 12
#define STEPPER_IN4 13

AccelStepper stepper(MotorInterfaceType, STEPPER_IN1, STEPPER_IN3, STEPPER_IN2, STEPPER_IN4);

const int STEPS_PER_REV = 2048;
const int stepsFor120Degrees = (int)((120.0 / 360.0) * STEPS_PER_REV + 0.5); // 682 pasos

#define RELAY_PIN 8  // Pin módulo de relé para el solenoide

// Variables para los LEDs
int ledPins[] = {3, 4, 5, 6, 7}; // Pines de los LEDs
const int numLeds = 5; // Número de LEDs
unsigned long previousLedMillis = 0; // Tiempo anterior para el control de LEDs
const long ledInterval = 200; // Intervalo para la secuencia lenta en milisegundos
int currentLed = 0; // LED actual en la secuencia
bool ledsEncendidos = false; // Indicador de si los LEDs están encendidos

// Variables para controlar el tiempo de lectura de sensores
unsigned long previousSensorMillis = 0;
const long sensorInterval = 4000; // Intervalo de 4 segundos para lecturas de sensores

// Variable para la humedad de suelo
float soilMoisturePercent = 0.0;

// Definir estados del tubo
enum TubeState {
  TUBE_CLOSED,
  TUBE_OPENING,
  TUBE_OPEN,
  TUBE_CLOSING
};

TubeState tubeState = TUBE_CLOSED;

// Definir estados de los LEDs
enum LedState {
  LED_OFF,
  LED_SLOW_SEQUENCE
};

LedState ledState = LED_OFF;

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL2V56); // Usar referencia analógica interna de 2.56V
  dht.begin();

  // Configurar pines
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relé (solenoide) apagado inicialmente

  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // LEDs apagados inicialmente
  }

  // Configurar el motor paso a paso
  stepper.setMaxSpeed(300.0);      // Reducir velocidad máxima para movimientos más suaves
  stepper.setAcceleration(50.0);   // Reducir aceleración para evitar movimientos bruscos
  stepper.setCurrentPosition(0);    // Inicializar posición actual

  Serial.println("Arduino listo y esperando comandos.");
}

void loop() {
  // Leer comandos seriales
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Eliminar espacios en blanco y caracteres de nueva línea

    if (command.equalsIgnoreCase("Abrir tubo")) {
      abrirTubo();
    } else if (command.equalsIgnoreCase("Cerrar tubo")) {
      cerrarTubo();
    }
  }

  // Máquina de estados para el control del tubo
  switch (tubeState) {
    case TUBE_OPENING:
      // Mover el motor hacia la posición objetivo
      if (stepper.distanceToGo() != 0) {
        stepper.run();
      } else {
        // El motor ha llegado a la posición, activar el solenoide
        digitalWrite(RELAY_PIN, HIGH); // Activar el solenoide
        tubeState = TUBE_OPEN;
        Serial.println("Tubo abierto.");

        // Iniciar la secuencia lenta de LEDs
        ledsEncendidos = true;
        ledState = LED_SLOW_SEQUENCE;
        currentLed = 0;
        previousLedMillis = millis();
      }
      break;

    case TUBE_OPEN:
      // El tubo está abierto, mantener los LEDs encendidos en secuencia lenta
      manageLEDs();
      break;

    case TUBE_CLOSING:
      // Mover el motor hacia la posición objetivo
      if (stepper.distanceToGo() != 0) {
        stepper.run();
      } else {
        // El motor ha llegado a la posición, desactivar el solenoide
        digitalWrite(RELAY_PIN, LOW); // Desactivar el solenoide
        tubeState = TUBE_CLOSED;
        Serial.println("Tubo cerrado.");

        // Apagar los LEDs y asegurar que todos estén apagados
        turnOffLEDs();
        ledsEncendidos = false;
        currentLed = 0;
        ledState = LED_OFF;
      }
      break;

    case TUBE_CLOSED:
      // El tubo está cerrado, asegurar que los LEDs están apagados
      if (ledsEncendidos) {
        turnOffLEDs();
        ledsEncendidos = false;
        currentLed = 0;
        ledState = LED_OFF;
      }
      break;
  }

  // Siempre llamar a stepper.run() para mover el motor
  if (tubeState == TUBE_OPENING || tubeState == TUBE_CLOSING) {
    stepper.run();
  }

  // Controlar los LEDs si están encendidos y el tubo está abierto o cerrando
  if (ledsEncendidos && (tubeState == TUBE_OPEN || tubeState == TUBE_CLOSING)) {
    manageLEDs();
  }

  // Enviar lecturas de sensores cada 4 segundos
  sendSensorReadings();
}

// Función para abrir el tubo con movimiento suave
void abrirTubo() {
  if (tubeState == TUBE_CLOSED) {
    tubeState = TUBE_OPENING;

    // Actualizar la posición objetivo del motor
    stepper.moveTo(stepper.currentPosition() + stepsFor120Degrees);

    Serial.println("Iniciando apertura del tubo...");
  }
}

// Función para cerrar el tubo con movimiento suave
void cerrarTubo() {
  if (tubeState == TUBE_OPEN) {
    tubeState = TUBE_CLOSING;

    // Actualizar la posición objetivo del motor
    stepper.moveTo(stepper.currentPosition() - stepsFor120Degrees);

    Serial.println("Iniciando cierre del tubo...");
  }
}

// Función para gestionar los LEDs en secuencia lenta
void manageLEDs() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousLedMillis >= ledInterval) {
    previousLedMillis = currentMillis;

    // Apagar todos los LEDs
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }

    // Encender el LED actual
    digitalWrite(ledPins[currentLed], HIGH);

    // Avanzar al siguiente LED
    currentLed++;

    // Reiniciar si se llega al final
    if (currentLed >= numLeds) {
      currentLed = 0;
    }
  }
}

// Función para apagar todos los LEDs
void turnOffLEDs() {
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], LOW);
  }
}

// Función para enviar lecturas de sensores
void sendSensorReadings() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;

    // Lectura de humedad de suelo
    int soilMoistureValue = analogRead(soilMoisturePin);
    soilMoisturePercent = map(soilMoistureValue, 820, 470, 0, 100);
    soilMoisturePercent = constrain(soilMoisturePercent, 0, 100);

    // Lectura del DHT22
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer el sensor DHT22");
    } else {
      // Enviar datos en formato CSV
      Serial.print(soilMoisturePercent);
      Serial.print(",");
      Serial.print(temperature);
      Serial.print(",");
      Serial.println(humidity);
    }
  }
}
