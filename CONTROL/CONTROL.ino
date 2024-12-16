// CORRECCIONES EMISOR

 //CODIGO DEL ESP32 EMISOR

// Dependencias
#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pines del OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pines del LoRa
//#define MOSI 23

  //#define MISO 19
  //#define SCK 18
  //#define NSS 5
  //#define D0 4
  //#define RS 14
const int csPin = 5;          // LoRa radio chip select
const int resetPin = 14;      // LoRa radio reset
const int irqPin = 4;         // Cambiar dependiendo de la placa; tiene que ser un pin de interrupcion
  // Pines de entrada
#define PARADA 34
#define SELECTOR 35
#define SUBIR 32
#define BAJAR 33
#define NORTE 25
#define SUR 26
#define ESTE 27
#define OESTE 17
#define HABILITADOR 12


  // Función para mostrar en OLED
void OLED(String mensaje, int x, int y) {
  display.setTextSize(1);       // Tamaño normal 1:1
  display.setTextColor(SSD1306_WHITE); // Color blanco para el texto
  display.setCursor(x, y);      // Comienza en la parte superior de la pantalla
  display.println(mensaje);     // Muestra el mensaje
  display.display();            // Actualiza la pantalla
}
void Input(int pin, String mensaje, int x, int y) {
  if (digitalRead(pin) == HIGH) {  // Verifica si el botón está presionado
    display.fillRect(x, y, 68, 8, SSD1306_BLACK); // Limpia el área donde se mostrará el mensaje
    OLED(mensaje, x, y);  // Muestra el mensaje en las coordenadas especificadas
  }
}


void ENCABEZADO() {
  display.setTextSize(1);       // Tamaño normal 1:1
  display.setTextColor(SSD1306_WHITE); // Color blanco para el texto
  display.setCursor(15, 0);     // Posiciona el cursor en la parte superior
  display.println(F("CONTROL DE GRUA"));
  display.setCursor(0, 10);     // Posiciona el cursor en la parte superior
  display.println(F("SELECTOR:"));
  display.setCursor(0, 20);     // Posiciona el cursor en la parte superior
  display.println(F("DIRECCION:"));
   display.display();            // Muestra el encabezado en la pantalla
}

void SELECCION() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Borra solo la línea donde se muestra el selector
  display.fillRect(60, 10, 68, 8, SSD1306_BLACK); // Borra la parte donde se muestra el selector
  if (digitalRead(SELECTOR) == HIGH) {
    OLED("GRUA 1", 60, 10);  // Muestra "GRUA 1" si el selector está activado
  } else {
    OLED("GRUA 2", 60, 10);  // Muestra "GRUA 2" si no está activado
  }
}

  // Función para mostrar emergencia
void EMERGENCIA(){
  static bool estadoEmergencia = false;  // Variable para controlar el estado de la emergencia

  if (digitalRead(PARADA) == HIGH && !estadoEmergencia) {
    estadoEmergencia = true;  // Establece el estado de emergencia como activado
      display.clearDisplay();   // Borra toda la pantalla

    // Limpiar específicamente el área donde se muestra "GRUA 1" o "GRUA 2"
    display.fillRect(60, 10, 68, 8, SSD1306_BLACK);  // Borra la parte donde se muestra el selector (GRUA 1 o GRUA 2)

    display.setTextSize(2);   // Tamaño grande para el texto de emergencia
    display.setTextColor(SSD1306_WHITE); // Color blanco para el texto
    display.setCursor(0, 10); // Posiciona el cursor en la parte central
    display.println(F("EMERGENCIA"));
    display.display();        // Actualiza la pantalla
  } else if (digitalRead(PARADA) == LOW && estadoEmergencia) {
    estadoEmergencia = false; // Establece el estado de emergencia como desactivado
  }
}

void LECTURA(int PIN, String mensaje) {
  if (digitalRead(PIN) == HIGH) {
    LoRa.beginPacket();
    // Enviamos el mensaje
    LoRa.print(mensaje);
    LoRa.endPacket();
    delay(100);
    Serial.println("Paquete enviado: " + mensaje);
  }
}


void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Transmisor");

  LoRa.setPins(csPin, resetPin, irqPin); // Configura los pines de LoRa

  if (!LoRa.begin(915E6)) {
    Serial.println("Error al Iniciar LoRa!");
    while (1);
  }

  // Inicialización OLED

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Error al inicializar OLED"));
    for (;;);
  }

  // Limpiar la pantalla e inicializar encabezado
  display.clearDisplay();
  ENCABEZADO();


    // Configuración de los pines de entrada
  pinMode(PARADA, INPUT);
  pinMode(SELECTOR, INPUT);
  pinMode(SUBIR, INPUT);
  pinMode(BAJAR, INPUT);
  pinMode(NORTE, INPUT);
  pinMode(SUR, INPUT);
  pinMode(ESTE, INPUT);
  pinMode(OESTE, INPUT);
  pinMode(HABILITADOR, INPUT);

  }
// Función para enviar estado
void enviarEstado(String pinName, bool estado) {
  String mensajeEstado = pinName + ":" + String(estado);

  LoRa.beginPacket();
  LoRa.print(mensajeEstado);
  LoRa.endPacket();

  // Mostrar el mensaje en el monitor serial para depuración
  Serial.println("Paquete enviado: " + mensajeEstado);
  delay(1);
}

// Función para leer los pines y enviar constantemente el estado
void LECTURA(int pin, String pinName, bool &estadoAnterior) {
  bool estadoActual = digitalRead(pin); // Leer el estado actual del pin

  if (estadoActual != estadoAnterior) { // Detecta cambio en el estado
    estadoAnterior = estadoActual;      // Actualiza el estado anterior
    enviarEstado(pinName, estadoActual); // Envia el cambio de estado
  }

  // Siempre enviamos el estado actual del pin, independientemente de si hubo cambio
  enviarEstado(pinName, estadoActual);
}

// Variables para guardar el estado anterior de los pines
bool estadoAnteriorParada = LOW;
bool estadoAnteriorSelector = LOW;
bool estadoAnteriorSubir = LOW;
bool estadoAnteriorBajar = LOW;
bool estadoAnteriorNorte = LOW;
bool estadoAnteriorSur = LOW;
bool estadoAnteriorEste = LOW;
bool estadoAnteriorOeste = LOW;

void loop() {
  
  LECTURA(PARADA, "PARADA", estadoAnteriorParada);
  LECTURA(SELECTOR, "SELECTOR", estadoAnteriorSelector);
  LECTURA(SUBIR, "SUBIR", estadoAnteriorSubir);
  LECTURA(BAJAR, "BAJAR", estadoAnteriorBajar);
  LECTURA(NORTE, "NORTE", estadoAnteriorNorte);
  LECTURA(SUR, "SUR", estadoAnteriorSur);
  LECTURA(ESTE, "ESTE", estadoAnteriorEste);
  LECTURA(OESTE, "OESTE", estadoAnteriorOeste);
    
  // Llama a la función Input para cada botón
  if (digitalRead(PARADA) == HIGH) {
    EMERGENCIA();
  } else {
    display.clearDisplay();   // Borra toda la pantalla
    ENCABEZADO();             // Redibuja el encabezado cuando la emergencia se desactiva
    SELECCION();              // Redibuja el selector de grúa (GRUA 1 o GRUA 2)
    display.display(); 
    
    // Muestra el estado de los botones
    Input(SUBIR, "SUBIR", 60, 20);
    Input(BAJAR, "BAJAR", 60, 20);
    Input(NORTE, "NORTE", 60, 20);
    Input(SUR, "SUR", 60, 20);
    Input(ESTE, "ESTE", 60, 20);
    Input(OESTE, "OESTE", 60, 20);
  }
}
