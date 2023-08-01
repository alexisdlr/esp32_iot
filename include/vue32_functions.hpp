/* -------------------------------------------------------------------
 * AdminESP - ElectronicIOT 2022
 * Sitio WEB: https://electroniciot.com
 * Correo: admin@electroniciot.com
 * Cel_WSP: +591 71243395
 * Plataforma: ESP32
 * Framework:  Arduino
 * Proyecto: Panel Administrativo para el ESP32 con Vue.js
 * Nombre: VUE32 Admin Tool
 * Autor: Ing. Yamir Hidalgo Peña
 * -------------------------------------------------------------------
*/

#include "vue32_LedBlink.hpp"

// -------------------------------------------------------------------
// Genera un log en el puerto Serial
// -------------------------------------------------------------------
void log(String s){
    Serial.println(s);
}
// -------------------------------------------------------------------
// Definir la Plataforma
// -------------------------------------------------------------------
String platform(){
// Optiene la plataforma de hardware
#ifdef ARDUINO_ESP32_DEV
    return "ESP32";
#endif
}
// -------------------------------------------------------------------
// De HEX a String
// -------------------------------------------------------------------
String hexStr(const unsigned long &h, const byte &l = 8){
    String s;
    s = String(h, HEX);
    s.toUpperCase();
    s = ("00000000" + s).substring(s.length() + 8 - l);
    return s;
}
// -------------------------------------------------------------------
// Crear un ID unico desde la direccion MAC
// -------------------------------------------------------------------
String idUnique(){
    // Retorna los ultimos 4 Bytes del MAC rotados
    char idunique[15]; 
    uint64_t chipid = ESP.getEfuseMac();           
    uint16_t chip = (uint16_t)(chipid >> 32);
    snprintf(idunique, 15, "%04X", chip);
    return idunique;
}
// -------------------------------------------------------------------
// Número de serie del Dispositivo único
// -------------------------------------------------------------------
String deviceID(){
    return String(platform()) + hexStr(ESP.getEfuseMac()) + String(idUnique());
}
// -------------------------------------------------------------------
// Configurar los Pines de Salida WIFI - MQTT
// -------------------------------------------------------------------
void settingPines(){
    pinMode(WIFILED, OUTPUT);
    pinMode(DHTPIN, OUTPUT);
    pinMode(LEDCONTROL, OUTPUT);
    setOffSingle(WIFILED);
    setOffSingle(LEDCONTROL);
    setOffSingle(DHTPIN);
}
// -------------------------------------------------------------------
// Convierte un char a IP
// -------------------------------------------------------------------
uint8_t ip[4];    // Variable función convertir string a IP 
IPAddress CharToIP(const char *str){
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
// -------------------------------------------------------------------
// Retorna IPAddress en formato "n.n.n.n" de IP a String
// -------------------------------------------------------------------
String ipStr(const IPAddress &ip){    
    String sFn = "";
    for (byte bFn = 0; bFn < 3; bFn++){
        sFn += String((ip >> (8 * bFn)) & 0xFF) + ".";
    }
    sFn += String(((ip >> 8 * 3)) & 0xFF);
    return sFn;
}
String pathMqtt(){
    return String("v1/devices/"+String(mqtt_cloud_id));
}
void mqttRX(){
    for(int16_t i = 0; i < 1; i++){
        vTaskDelay(10);
    }
}
// -------------------------------------------------------------------
// Parpadeo LED MQTT Transmisión
// -------------------------------------------------------------------
void mqttTX(){
    for(int16_t i = 0; i < 6; i++){
        vTaskDelay(50);
        vTaskDelay(10);
    }
}
String longTimeStr(const time_t &t){        
    String s = String(t / SECS_PER_DAY) + ':';
    if (hour(t) < 10)
    {
        s += '0';
    }
    s += String(hour(t)) + ':';
    if (minute(t) < 10)
    {
        s += '0';
    }
    s += String(minute(t)) + ':';
    if (second(t) < 10)
    {
        s += '0';
    }
    s += String(second(t));
    return s;
}
