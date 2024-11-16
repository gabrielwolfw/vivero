#define LED 13

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()){
    char command = Serial.read();
    if(command == '1'){
      digitalWrite(LED, HIGH);
    }
    else if(command == '0'){
      digitalWrite(LED, LOW);
    }
    else if(command == '2'){
      Serial.write("Hello");
    }
  }

}
