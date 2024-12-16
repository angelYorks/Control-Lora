
// CORRECCIONES RECEPTOR
#include <SPI.h>
#include <LoRa.h>

// Pines LoRa
const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;      // LoRa radio reset
const int irqPin = 4;         // Cambiar dependiendo de la placa; tiene que ser un pin de interrupcion

// Pines de LEDs
#define LED_PARADA 22
#define LED_SELECTOR 21
#define LED_SUBIR 32
#define LED_BAJAR 33
#define LED_NORTE 25
#define LED_SUR 26
#define LED_ESTE 27
#define LED_OESTE 17

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Configuración de pines de LEDs
  pinMode(LED_PARADA, OUTPUT);
  pinMode(LED_SELECTOR, OUTPUT);
  pinMode(LED_SUBIR, OUTPUT);
  pinMode(LED_BAJAR, OUTPUT);
  pinMode(LED_NORTE, OUTPUT);
  pinMode(LED_SUR, OUTPUT);
  pinMode(LED_ESTE, OUTPUT);
  pinMode(LED_OESTE, OUTPUT);

  // Inicializar LEDs apagados
  digitalWrite(LED_PARADA, LOW);
  digitalWrite(LED_SELECTOR, LOW);
  digitalWrite(LED_SUBIR, LOW);
  digitalWrite(LED_BAJAR, LOW);
  digitalWrite(LED_NORTE, LOW);
  digitalWrite(LED_SUR, LOW);
  digitalWrite(LED_ESTE, LOW);
  digitalWrite(LED_OESTE, LOW);

  Serial.println("LoRa Receptor");
  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(915E6)) {
    Serial.println("Error al iniciar LoRa");
    while (1);
  }
}
void encenderLED(String mensaje) {
  int separador = mensaje.indexOf(':');
  if (separador == -1) return; // Si no encuentra ":", descartar mensaje

  String comando = mensaje.substring(0, separador); // Comando (PARADA, SUBIR, etc.)
  int estado = mensaje.substring(separador + 1).toInt(); // Estado (0 o 1)

  // Actualizar el LED correspondiente
  if (comando == "PARADA") digitalWrite(LED_PARADA, estado);
  else if (comando == "SELECTOR") digitalWrite(LED_SELECTOR, estado);
  else if (comando == "SUBIR") digitalWrite(LED_SUBIR, estado);
  else if (comando == "BAJAR") digitalWrite(LED_BAJAR, estado);
  else if (comando == "NORTE") digitalWrite(LED_NORTE, estado);
  else if (comando == "SUR") digitalWrite(LED_SUR, estado);
  else if (comando == "ESTE") digitalWrite(LED_ESTE, estado);
  else if (comando == "OESTE") digitalWrite(LED_OESTE, estado);
}

void procesarMensaje(String mensaje) {
  String comando = mensaje.substring(0, mensaje.indexOf(':'));
  String estado = mensaje.substring(mensaje.indexOf(':') + 1);

  // Determinar el LED a controlar
  int pinLED = -1;
  if (comando == "PARADA") pinLED = LED_PARADA;
  else if (comando == "SELECTOR") pinLED = LED_SELECTOR;
  else if (comando == "SUBIR") pinLED = LED_SUBIR;
  else if (comando == "BAJAR") pinLED = LED_BAJAR;
  else if (comando == "NORTE") pinLED = LED_NORTE;
  else if (comando == "SUR") pinLED = LED_SUR;
  else if (comando == "ESTE") pinLED = LED_ESTE;
  else if (comando == "OESTE") pinLED = LED_OESTE;

  // Cambiar el estado del LED
  if (pinLED != -1) {
    digitalWrite(pinLED, estado == "ON" ? HIGH : LOW);
    Serial.println("LED " + comando + " -> " + estado);
  }
}

void loop() {
  if (LoRa.parsePacket()) {
    String mensaje = "";
    while (LoRa.available()) {
      char c = (char)LoRa.read(); // Leer un carácter
      Serial.print(c);            // Mostrar en monitor serie
      mensaje += c;               // Añadir al mensaje
    }
    Serial.println(); // Salto de línea después del mensaje
    encenderLED(mensaje);
  }
}
