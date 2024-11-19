#define LED 13

// Variables para controlar el tiempo de lectura de sensores
unsigned long previousSensorMillis = 0;
const long sensorInterval = 4000; // Intervalo de 4 segundos para lecturas de sensores

int cont = 0;

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {

    // Leer comandos seriales
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('$');
    command.trim(); // Eliminar espacios en blanco y caracteres de nueva línea
    Serial.println(command);

    if (command.equalsIgnoreCase("Abrir tubo")) {
      digitalWrite(LED, HIGH);
    } else if (command.equalsIgnoreCase("Cerrar tubo")) {
      digitalWrite(LED, LOW);
    }
  }

  // Enviar lecturas de sensores cada 4 segundos
  sendSensorReadings();

}


// Función para enviar lecturas de sensores
void sendSensorReadings() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousSensorMillis >= sensorInterval) {
    previousSensorMillis = currentMillis;

    // Lectura de humedad de suelo
    int soilMoistureValue = 20;

    // Lectura del DHT22
    float temperature = 25;
    float humidity = 60;

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer el sensor DHT22");
    } else {
        if(cont < 5){
          // Enviar datos en formato CSV
          Serial.print(soilMoistureValue + cont*10);
          Serial.print(",");
          Serial.print(temperature);
          Serial.print(",");
          Serial.println(humidity);
          cont++;
        }
        else{
          cont = 0;
        }
      }
    }
}
