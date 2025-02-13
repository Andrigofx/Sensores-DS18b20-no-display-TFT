// Sketch para monitoramento de sensores de temperatura DS18B20 e SHT20 com display TFT
// Desenvolvido por Andrigo Farias Xavier e Rita Monteiro
// Laboratório de Agrotecnologia - UFPEL
// Modificado em 07/02/2025

            
#include "DFRobot_SHT20.h"    // Biblioteca para sensor SHT20
#include <Adafruit_GFX.h>     // Biblioteca Core graphics
#include <MCUFRIEND_kbv.h>
#include <OneWire.h>           // Biblioteca para comunicação I2C
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2  // Pino onde os sensores DS18B20 serão conectados
#define LED_VERDE 8     // Pino do LED verde
#define LED_AMARELO 9   // Pino do LED amarelo
#define LED_VERMELHO 10 // Pino do LED vermelho
#define BUZZER 11       // Pino do buzzer

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1, sensor2, sensor3, sensor4;
DFRobot_SHT20 sht20;

MCUFRIEND_kbv tft;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  tft.reset();

  tft.begin(0x9327);
  tft.setRotation(0);
  tft.fillScreen(BLACK);

  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(3, 0);
  tft.println("LAB. AGROTEC.");
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.println("   MONITORAMENTO");
  tft.println("     DE SECAGEM ");
  tft.println("      DE GRAOS");

  sensors.begin();
  if (!sensors.getAddress(sensor1, 0)) Serial.println("Sensor DS18B20 0 nao encontrado");
  if (!sensors.getAddress(sensor2, 1)) Serial.println("Sensor DS18B20 1 nao encontrado");
  if (!sensors.getAddress(sensor3, 2)) Serial.println("Sensor DS18B20 2 nao encontrado");
  if (!sensors.getAddress(sensor4, 3)) Serial.println("Sensor DS18B20 3 nao encontrado");

  sht20.initSHT20();
  
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop(void) {
  // Leitura dos sensores
  sensors.requestTemperatures();
  float temp1 = sensors.getTempC(sensor1);
  float temp2 = sensors.getTempC(sensor2);
  float temp3 = sensors.getTempC(sensor3);
  float temp4 = sensors.getTempC(sensor4);
  float tempSHT20 = sht20.readTemperature();
  float umidadeSHT20 = sht20.readHumidity();

  // Depuração: exibe as temperaturas no Serial Monitor
  Serial.print("Temp1: "); Serial.println(temp1);
  Serial.print("Temp2: "); Serial.println(temp2);
  Serial.print("Temp3: "); Serial.println(temp3);
  Serial.print("Temp4: "); Serial.println(temp4);
  Serial.print("Temp SHT20: "); Serial.println(tempSHT20);
  Serial.print("Umidade SHT20: "); Serial.println(umidadeSHT20);

// Verifica as temperaturas
bool alerta = false;
bool atencao = false;

float temperaturas[] = {temp1, temp2, temp3, temp4};
for (int i = 0; i < 4; i++) {
    if (temperaturas[i] > 30.0) {
        alerta = true; // Se qualquer temperatura for maior que 30°C, ativa o alerta
        atencao = false; // Desativa a atenção se o alerta estiver ativo
    } else if (abs(temperaturas[i] - 30.0) < 0.1) { // Verifica se a temperatura está próxima de 30°C
        atencao = true; // Se qualquer temperatura for próxima de 30°C, ativa a atenção
    }
}

// Controle dos LEDs e buzzer
if (alerta) {
    // Alerta: temperatura acima de 30°C
    digitalWrite(LED_VERDE, LOW);       // LED verde desligado
    digitalWrite(LED_AMARELO, LOW);    // LED amarelo desligado
    digitalWrite(LED_VERMELHO, HIGH);  // LED vermelho ligado
    tone(BUZZER, 1000);                // Buzzer ligado
    tft.fillRect(0, 280, 240, 40, BLACK);
    tft.setCursor(0, 280);
    tft.setTextColor(RED);
    tft.setTextSize(2);
    tft.println("ALERTA TEMPERATURA!");
} else if (atencao) {
    // Atenção: temperatura igual a 30°C
    digitalWrite(LED_VERDE, LOW);      // LED verde desligado
    digitalWrite(LED_AMARELO, HIGH);   // LED amarelo ligado
    digitalWrite(LED_VERMELHO, LOW);   // LED vermelho desligado
    noTone(BUZZER);                    // Buzzer desligado
    digitalWrite(BUZZER, LOW);        // Garante que o pino do buzzer está em LOW
} else {
    // Normal: todas as temperaturas abaixo de 30°C
    digitalWrite(LED_VERDE, HIGH);     // LED verde ligado
    digitalWrite(LED_AMARELO, LOW);    // LED amarelo desligado
    digitalWrite(LED_VERMELHO, LOW);   // LED vermelho desligado
    noTone(BUZZER);                    // Buzzer desligado
    digitalWrite(BUZZER, LOW);        // Garante que o pino do buzzer está em LOW
}
  // Atualização do display (apenas se necessário)
  static unsigned long ultimaAtualizacaoDisplay = 0;
  if (millis() - ultimaAtualizacaoDisplay >= 1000) { // Atualiza o display a cada 1 segundo
    ultimaAtualizacaoDisplay = millis();

    // Exibe as temperaturas no display TFT
    tft.fillRect(0, 80, 240, 20, BLACK);
    tft.setCursor(0, 80);
    tft.setTextColor(MAGENTA);
    tft.setTextSize(2);
    tft.print("Sensor 1: "); tft.print(temp1); tft.println(" C");

    tft.fillRect(0, 120, 240, 20, BLACK);
    tft.setCursor(0, 120);
    tft.print("Sensor 2: "); tft.print(temp2); tft.println(" C");

    tft.fillRect(0, 160, 240, 20, BLACK);
    tft.setCursor(0, 160);
    tft.print("Sensor 3: "); tft.print(temp3); tft.println(" C");

    tft.fillRect(0, 200, 240, 20, BLACK);
    tft.setCursor(0, 200);
    tft.print("Sensor 4: "); tft.print(temp4); tft.println(" C");

    // Exibe temperatura e umidade do SHT20
    tft.fillRect(0, 240, 240, 40, BLACK);
    tft.setCursor(0, 240);
    tft.setTextColor(CYAN);
    tft.print("Temp.do ar: "); tft.print(tempSHT20); tft.println(" C");
    tft.print("Umid.do ar: "); tft.print(umidadeSHT20); tft.println(" %");
  }

  // Pequeno delay para evitar sobrecarga
  delay(10);
}
