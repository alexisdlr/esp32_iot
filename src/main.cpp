
// -------------------------------------------------------------------
// Librerías
// -------------------------------------------------------------------
#include <Arduino.h>
#include <ArduinoJson.h>
#include "DHTesp.h" // Click here to get the library: http://librarymanager/All#DHTesp
#include <SPIFFS.h>
#include <EEPROM.h>
#include <TimeLib.h>
// -------------------------------------------------------------------
// Archivos *.hpp - Fragmentar el Código
// -------------------------------------------------------------------
#include "vue32_header.hpp"
#include "vue32_functions.hpp"
#include "vue32_settings.hpp"
#include "vue32_wifi.hpp"
#include "vue32_mqtt.hpp"

DHTesp dht;

// -------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  setCpuFrequencyMhz(240);
  // Memoria EEPROM init
  EEPROM.begin(256);
  // Leer el valor de la memoria
  EEPROM.get(Restart_Address, device_restart);
  device_restart++;
  // Guardar el valor a la memoria
  EEPROM.put(Restart_Address, device_restart);
  EEPROM.commit();
  EEPROM.end();
  log("\n[ INFO ] Iniciando Setup");
  log("[ INFO ] Reinicios " + String(device_restart)); 
  log("[ INFO ] Setup corriendo en el Core "+ String(xPortGetCoreID()));
  // Iniciar el SPIFFS
  if(!SPIFFS.begin(true)){
    log("[ ERROR ] Falló la inicialización del SPIFFS");
    while(true);
  }
  // Leer el Archivo settings.json
  if(!settingsRead()){
    settingsSave();
  }
  // Configuración de los LEDs
  settingPines();
  // Setup WIFI
  wifi_setup();
  
  dht.setup(26, DHTesp::DHT11); // Connect DHT sensor to GPIO 17

}
// -------------------------------------------------------------------
// Loop Principal
// -------------------------------------------------------------------
void loop() {
  
// -------------------------------------------------------------------
// WIFI
// -------------------------------------------------------------------
  if(wifi_mode == WIFI_STA){
    wifiLoop();
  }else if (wifi_mode == WIFI_AP){
    wifiAPLoop();
  }
  
 delay(1800);

  humidity = dht.getHumidity();
  temp = dht.getTemperature();
  fah = dht.toFahrenheit(temp);
  heatIndex = dht.computeHeatIndex(temp, humidity, false);
  // Serial.print(dht.getStatusString());
  // Serial.print("\t");
  // Serial.print(humidity, 1);
  // Serial.print("\t\t");
  // Serial.print(temperature, 1);
  // Serial.print("\t\t");
  // Serial.print(, 1);
  // Serial.print("\t\t");
  // Serial.print(dht.computeHeatIndex(temperature, humidity, false), 1);
  // Serial.print("\t\t");
  // Serial.println(dht.computeHeatIndex(dht.toFahrenheit(temperature), humidity, true), 1);

 if((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_STA)){
    if(mqtt_server != 0){
      // Función para el Loop principla de MQTT
       mqttLoop();

        mqtt_publish_temp();
        mqtt_publish_humidity();
        mqtt_publish_fah();
        mqtt_publish_heatIndex();
        mqtt_publish_data();

        
    }
  }
}