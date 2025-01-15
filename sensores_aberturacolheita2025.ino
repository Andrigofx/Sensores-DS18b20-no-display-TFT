//Sketch para monitoramento de sensores de temperatura DS18B20 com display TFT
//Desenvolvido por Andrigo Farias Xavier
//Laboratório de Agrotecnologia - UFPEL
//Modificado em 09/01/2025

#include <Adafruit_GFX.h>    // Biblioteca Core graphics 
#include <MCUFRIEND_kbv.h>      
#include <OneWire.h>
#include <DallasTemperature.h>   // Biblioteca sensores

#define ONE_WIRE_BUS 2  // Pino onde os sensores serao conectados

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1, sensor2, sensor3 ;

MCUFRIEND_kbv tft;

// Atribua nomes legíveis a alguns valores de cores comuns de 16 bits:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

uint16_t g_identifier;

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  
  tft.reset();

#ifdef USE_OPENSMART_SHIELD_PINOUT
  Serial.println(F("Using OPENSMART 3.2\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using OPENSMART 3.2\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());

  uint32_t when = millis();
  if (!Serial) delay(5000);           
  Serial.println("Serial took " + String((millis() - when)) + "ms to start");
  
  g_identifier = tft.readID();
  Serial.print("ID = 0x");
  Serial.println(g_identifier, HEX);
  
  tft.begin(0x9327); // Para ativar o código do driver ILI9327
  tft.setRotation(0);

  // Colocando o plano de fundo na cor preto
  tft.fillScreen(BLACK);

  // Texto estatico da parte de cima
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(3, 0);
  tft.println("LAB. AGROTEC.");

  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.println("   MONITORAMENTO");
  tft.println("        DE");
  tft.println("    TEMPERATURA");
  sensors.begin();

  if (!sensors.getAddress(sensor1, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(sensor2, 1)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(sensor3, 2)) Serial.println("Unable to find address for Device 2");
 
  sensors.setResolution(sensor1, 10);
  sensors.setResolution(sensor2, 10);
  sensors.setResolution(sensor3, 10);
}

void loop(void) {
  sensors.requestTemperatures();

  float temp1 = sensors.getTempC(sensor1);
  float temp2 = sensors.getTempC(sensor2);
  float temp3 = sensors.getTempC(sensor3);

  // Atualiza os valores lidos pelos sensores
  tft.fillRect(0, 80, 240, 20, BLACK);
  tft.setCursor(0, 80);
  tft.setTextColor(MAGENTA);
  tft.setTextSize(2);
  if (temp1 == DEVICE_DISCONNECTED_C) {
    tft.print("Sensor 1: ERRO");
  } else {
    tft.print("Sensor 1: ");
    tft.print(temp1);
    tft.println(" C");
  }

  tft.fillRect(0, 120, 240, 20, BLACK);
  tft.setCursor(0, 120);
  if (temp2 == DEVICE_DISCONNECTED_C) {
    tft.print("Sensor 2: ERRO");
  } else {
    tft.print("Sensor 2: ");
    tft.print(temp2);
    tft.println(" C");
  }

  tft.fillRect(0, 160, 240, 20, BLACK);
  tft.setCursor(0, 160);
  if (temp3 == DEVICE_DISCONNECTED_C) {
    tft.print("Sensor 3: ERRO");
  } else {
    tft.print("Sensor 3: ");
    tft.print(temp3);
    tft.println(" C");
  }

  // Checar a temperatura e se for maior que a pré estipulada, imprimir o ALERTA
  tft.fillRect(0, 200, 240, 40, BLACK); // Limpa mensagem anterior
  tft.setCursor(1, 200);
  if (temp1 > 30.0 || temp2 > 30.0 || temp3 > 30.0) {
    tft.setTextColor(RED);
    tft.setTextSize(2);
    tft.println("ALERTA TEMPERATURA ");
  }

  // Checar se algum sensor está desconectado
  tft.fillRect(0, 250, 240, 40, BLACK); // Limpa mensagem anterior
  tft.setCursor(1, 250);
  if (temp1 == DEVICE_DISCONNECTED_C || temp2 == DEVICE_DISCONNECTED_C || temp3 == DEVICE_DISCONNECTED_C) {
    tft.setTextColor(RED);
    tft.setTextSize(2);
    tft.println("ALERTA SENSOR DESC.");
  }

  delay(1000); // Espera 1 segundo antes de atualizar
}
