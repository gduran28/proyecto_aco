#include <LiquidCrystal_I2C.h>  // Librería para manejar pantallas LCD con interfaz I2C

LiquidCrystal_I2C lcd(0x20, 16, 2);  // Inicializa el LCD con dirección I2C 0x20 y tamaño 16x2

// Declaración de pines conectados a sensores y actuadores
const int ldrPin = A1;        // Sensor de luz (LDR)
const int relayPin = 3;       // Relé para encender/apagar luces
const int tempPin = A0;       // Sensor de temperatura analógico
const int outTempPin = 4;     // Salida para ventilador
const int pirPin = 7;         // Sensor PIR (movimiento)
const int alarmPin = 11;      // Pin para alarma (buzzer)
const int botonPin = 8;       // Botón para desactivar alarma
const int gasPin = A3;        // Sensor de gas

// Variables para guardar valores de sensores
int ldrValue = 0;
float tempValue = 0;
int threshold = 500;          // Umbral de luz para encender luces
float maxTemp = 32.0;         // Temperatura máxima antes de activar ventilador
int gasValue = 0;
int gasThreshold = 600;       // Umbral para alerta de gas

// Estado actual de cada sensor (para evitar mostrar mensajes repetidos)
bool sensorState[4] = {false, false, false, false};
bool currentState[4] = {false, false, false, false};

void setup() {
  // Configura pines de salida y entrada
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);         // Relé apagado por defecto (inverso)
  
  pinMode(outTempPin, OUTPUT);
  
  pinMode(pirPin, INPUT);
  pinMode(alarmPin, OUTPUT);
  pinMode(botonPin, INPUT_PULLUP);      // Botón con resistencia pull-up interna
  digitalWrite(alarmPin, LOW);          // Apaga la alarma al iniciar
  
  Serial.begin(9600);                   // Inicia comunicación serial

  // Inicializa la pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Iniciando...");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Lectura de sensores
  ldrValue = analogRead(ldrPin);
  tempValue = (analogRead(tempPin) * (5.0 / 1023.0) - 0.5) * 100.0; // Conversión de voltaje a ºC
  int botonEstado = digitalRead(botonPin);
  int pirValue = digitalRead(pirPin);
  gasValue = analogRead(gasPin);

  // Control de luces según luz ambiental
  if (ldrValue > threshold) {
    digitalWrite(relayPin, HIGH);  // Luz encendida
    sensorState[0] = true;
    if(sensorState[0] != currentState[0]) {
    	printMessage("Luces", "encendidas!");
      	currentState[0] = sensorState[0];
    }
  } else {
    digitalWrite(relayPin, LOW);   // Luz apagada
    sensorState[0] = false;
    if(sensorState[0] != currentState[0]) {
	    printMessage("Luces", "apagadas!");
        currentState[0] = sensorState[0];
    }
  }

  delay(1000);  // Espera entre lecturas
  lcd.clear();
  
  // Control del ventilador por temperatura
  if (tempValue > maxTemp) {
    digitalWrite(outTempPin, HIGH);  // Ventilador encendido
    sensorState[1] = true;
    if(sensorState[1] != currentState[1]) {
    	printMessage("Ventilador encendido!", String(tempValue, 1) + " °C");
      	currentState[1] = sensorState[1];
    }
  } else {
    digitalWrite(outTempPin, LOW);   // Ventilador apagado
    sensorState[1] = false;
    if(sensorState[1] != currentState[1]) {
	    printMessage("Ventilador apagado!", String(tempValue, 1) + " °C");
        currentState[1] = sensorState[1];
    }
  }

  // Alarma por detección de movimiento
  if (pirValue == HIGH) {
    if (!sensorState[2]) {
      printMessage("Alarma", "encendida!");
      tone(alarmPin, 1000);      // Activa buzzer
      sensorState[2] = true;
    }
  } else {
    if (botonEstado == LOW && sensorState[2]) {
      printMessage("Alarma", "apagada!");
	  noTone(alarmPin);          // Apaga buzzer si se presiona el botón
      sensorState[2] = false;
    }
  }
  
  delay(1000);
  lcd.clear();

  // Alerta por gas
  if (gasValue > gasThreshold) {
    sensorState[3] = true;
    if(sensorState[3] != currentState[3]) {
    	printMessage("Alerta", "gas detectado.");
      	tone(alarmPin, 1000);     // Alarma de gas
	    currentState[3] = sensorState[3];
    }
  } else {
    sensorState[3] = false;
    if(sensorState[3] != currentState[3]) {
	    printMessage("Alerta de gas", "apagada.");
      	noTone(alarmPin);         // Apaga alarma de gas
        currentState[3] = sensorState[3];
    }
  }
  delay(1000);
  lcd.clear();
}

// Función auxiliar para imprimir dos líneas de texto en el LCD
void printMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  delay(2000);  // Espera para que el mensaje sea legible
}
