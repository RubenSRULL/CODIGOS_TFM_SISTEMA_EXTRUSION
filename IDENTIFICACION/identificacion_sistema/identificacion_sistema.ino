// Autor: Ruben Sahuquillo Redondo
// Descripción: Código para adquisición de temperatura con termopar MAX31856 y control de relé mediante PWM

// Funcionnamiento
// 1. El sistema espera a recibir datos por Serial en el formato: "INICIO,<pct_pwm>,<temp_maxima>".
// 2. Una vez recibidos los datos, el sistema entra en estado de adquisición, donde lee la temperatura del termopar cada TIEMPO_MUESTREO ms y controla el relé
//    según el porcentaje de encendido (pct_pwm) en un ciclo de TIEMPO_CICLO ms.
// 3. Si la temperatura supera el límite especificado (temp_maxima), el sistema entra en estado LIMITE_SUPERADO, apaga el relé y el LED, y envía un mensaje por Serial
//    indicando que se ha superado el límite.
// 4. El sistema puede volver al estado ESPERA si recibe el mensaje "PARAR" por Serial durante la adquisición.


// ---------------------
// ----- LIBRERIAS -----
// ---------------------
#include <Adafruit_MAX31856.h>      // Librería para manejo del termopar MAX31856

// -----------------
// ----- PINES -----
// -----------------
#define DRDY_PIN 25                  // Pin "data ready" de MAX31856
#define CS_PIN 27                    // Pin "chip select" para comunicación SPI con MAX31856
#define RELE 22                     // Pin de salida conectado a relé
#define LED 2                       // Pin de salida LED integrado

// ---------------------
// ----- CONSTANTES ----
// ---------------------
#define TIEMPO_MUESTREO 1000        // Intervalo de lectura de temperatura (ms)
#define TIEMPO_CICLO 1000           // Duración de un ciclo PWM (ms)

// ---------------------
// ----- VARIABLES -----
// ---------------------
float temperatura = 0.0;            // Valor de la temperatura leída del termopar
float intervalo = 0.0;              // Tiempo transcurrido desde inicio de adquisición (s)
char msg[50];                       // Buffer para enviar datos por Serial
unsigned long tiempo_actual = 0;    // Tiempo actual (ms)
unsigned long tiempo_anterior = 0;  // Última lectura de temperatura (ms)
unsigned long tiempo_inicial = 0;   // Tiempo de inicio del experimento (ms)
unsigned long tiempo_ciclo = 0;     // Tiempo de inicio del ciclo PWM (ms)
int pct_pwm = 0;                    // Porcentaje de encendido del relé en un ciclo
float temperatura_maxima = 0;       // Temperatura máxima permitida

// ---------------------
// ----- ESTADOS -------
// ---------------------
enum Estado {ESPERA, ADQUISICION, LIMITE_SUPERADO};       // Estados de la máquina
Estado estado_actual = ESPERA;                            // Estado inicial: ESPERA

// -------------------
// ----- OBJETOS -----
// -------------------
Adafruit_MAX31856 maxthermo = Adafruit_MAX31856(CS_PIN);  // Objeto para manejar termopar

// ---------------------
// ----- FUNCIONES -----
// ---------------------

// Función que lee la temperatura del termopar
float leer_temperatura() {
  int count = 0;
  // Espera hasta que el pin DRDY_PIN indique que hay dato disponible
  while (digitalRead(DRDY_PIN)) {
    if (count++ > 200) {
      count = 0;
    }
  }
  return maxthermo.readThermocoupleTemperature();
}

// Función para inicializar y configurar el termopar
void config_termopar() {
  // Inicialización de comunicación SPI con MAX31856
  if (!maxthermo.begin()) {                         
    while (true) {
      delay(10);
    }
  }
  // Configura termopar tipo K (modo continuo)
  maxthermo.setThermocoupleType(MAX31856_TCTYPE_K); 
  maxthermo.setConversionMode(MAX31856_CONTINUOUS);
}

// ---------------------
// ----- SETUP ---------
// ---------------------
void setup() {
  // Inicializa comunicación Serial a 115200 baudios
  Serial.begin(115200);           
  while (!Serial) {
    delay(10);
  }
  // Configuración de pines
  pinMode(DRDY_PIN, INPUT);
  pinMode(RELE, OUTPUT);
  pinMode(LED, OUTPUT);
  // Inicialización de salidas
  digitalWrite(RELE, LOW);
  digitalWrite(LED, LOW);
  // Inicialización de termopar
  config_termopar();           
  // Mensaje de inicio
  delay(3000);
  Serial.println("OK");
  // Inicializa variables de tiempo
  tiempo_anterior = millis();
  tiempo_inicial = tiempo_anterior;
  tiempo_ciclo = tiempo_anterior;
}

// ---------------------
// ----- LOOP ----------
// ---------------------
void loop() {
  tiempo_actual = millis();
  switch (estado_actual) {
    // Estado ESPERA
    case ESPERA:
      digitalWrite(RELE, LOW);
      // Si llegan datos por Serial, leer hasta salto de línea y eliminar espacios al inicio y fin
      if (Serial.available() > 0) {
        String mensaje = Serial.readStringUntil('\n');        
        mensaje.trim();
        // Busca las comas que separan los datos
        int coma1 = mensaje.indexOf(',');
        int coma2 = mensaje.indexOf(',', coma1 + 1);
        if (coma1 != -1 && coma2 != -1) {
          // Extrae porcentaje PWM y temperatura máxima
          String str1 = mensaje.substring(coma1 + 1, coma2);
          String str2 = mensaje.substring(coma2 + 1);
          // Convertir a valores numericos
          pct_pwm = str1.toInt();
          temperatura_maxima = str2.toFloat();
          // Inicializa tiempos
          tiempo_inicial = tiempo_actual;
          tiempo_anterior = tiempo_actual;
          tiempo_ciclo = tiempo_actual;
          estado_actual = ADQUISICION;
        }
      }
      break;
    // Estado ADQUISICION
    case ADQUISICION:
    {
      // Calcula el tiempo transcurrido dentro del ciclo PWM
      unsigned long tiempo_dentro_ciclo = (tiempo_actual - tiempo_ciclo) % TIEMPO_CICLO;
      unsigned long tiempo_encendido = (pct_pwm * TIEMPO_CICLO) / 100;
      // Control del relé y LED según porcentaje PWM
      if (tiempo_dentro_ciclo < tiempo_encendido) {
        digitalWrite(RELE, HIGH);
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(RELE, LOW);
        digitalWrite(LED, LOW);
      }
      // Cada TIEMPO_MUESTREO ms, lee la temperatura
      if ((tiempo_actual - tiempo_anterior) >= TIEMPO_MUESTREO) {
        temperatura = leer_temperatura();
        intervalo = (tiempo_actual - tiempo_inicial) / 1000.0;
        sprintf(msg, "%.2f,%.2f", temperatura, intervalo);
        Serial.println(msg);
        tiempo_anterior = tiempo_actual;
        // Si la temperatura supera el límite, cambia de estado
        if (temperatura >= temperatura_maxima) {
          estado_actual = LIMITE_SUPERADO;
        }
      }
      if (Serial.available() > 0) {
        String mensaje = Serial.readStringUntil('\n');        
        mensaje.trim();
        if (mensaje == "PARAR") {
          estado_actual = ESPERA;
        }
      }
      break;
    }
    // Estado LIMITE_SUPERADO
    case LIMITE_SUPERADO:
      digitalWrite(RELE, LOW);
      digitalWrite(LED, LOW);
      Serial.println("Limite superado");
      estado_actual = ESPERA;
      break;
  }
}