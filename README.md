# Control-Lora
Proyecto de mando inalambrico para el control de maquinas de potencia

- Para este proyecto en el diseño se utilizo un modulo de comunicacion LORA RYLR998 que utiliza el protocolo serial(TX - RX).
- Como en la region donde me encuentro no esta disponible el modulo se opto por el RF LORA SX1278 de 433MHz pero usa el protocolo SPI
- Los cambios de pines se puede revisar en la programacion de cada esp32.
- La pantalla es un OLED 128x64 monocromatico de 2 capas de colores, amarillo para la parte superior y azul para la inferior.
- La frecuencia utilizada es de 915 MHz y el board es de 115200 para la lectura de datos.