#include <WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <HC_SR04.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define MAX_DISTANCE 200 // Distancia máxima en cm que puede medir el sensor

const char* ssid     = "#####"; // Cambia esto a tu SSID
const char* password = "#####"; // Cambia esto a tu contraseña

WiFiClient client;

unsigned long myChannelNumber = 1;
const char * myWriteAPIKey = "##########"; //Ingresa tu api Key proporcuonada al crear el canal en thingSpeak

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
// inicializa el sensor en el pin 12-tigger, 13-echo. 
HC_SR04<13> sensor(12);   // sensor with echo and trigger pin

//variables temporales
unsigned long lastTime = 0;
unsigned long timerDelay = 15000; //envio de datos cada 15 segundos minimo para la version libre de Thingspeak
float distance;

void setup() {
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  sensor.begin();
  
  // Start I2C Communication SDA = 5 and SCL = 4 on Wemos Lolin32 ESP32 with built-in SSD1306 OLED
  Wire.begin(5, 4);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(200); // Pause for 0.2 seconds
  // Clear the buffer.
  display.clearDisplay();
  display.display();

  // Conectar a WiFi
  connectToWiFi();
}

void loop() {

  if((millis() - lastTime) > timerDelay) {

    if(WiFi.status() != WL_CONNECTED) {
      connectToWiFi();
    }

    // Lectura de datos
    sensor.startMeasure();
    distance = sensor.getDist_cm();
    Serial.print("Distancia: ");
    Serial.println(distance);
    mostrardatos(distance); //muestra datos por pantalla oled
    
    // Write to ThingSpeak
    int x = ThingSpeak.writeField(myChannelNumber, 1, distance, myWriteAPIKey);
    if(x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
    
    lastTime = millis();
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    delay(5000);
    Serial.print(".");
  }
  Serial.println("\nConnected.");
}

void mostrardatos(double distance) {
  // Borrar la pantalla
  display.clearDisplay();

  // Configurar el tamaño del texto
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Mostrar la distancia
  display.println(" ThingSpeak ");
  display.println("IOT/EnfasisIV ");
  display.println("Sensor:HC_SR04 ");
  display.println("");
  display.setTextSize(2);
  display.print("Distancia: ");
  display.print(distance);
  display.println(" cm");

  // Mostrar en pantalla
  display.display();
}

