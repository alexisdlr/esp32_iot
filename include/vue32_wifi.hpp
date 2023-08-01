
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPmDNS.h> 

const byte DNSSERVER_PORT = 53;
DNSServer dnsServer;

IPAddress ap_IPv4(192,168,1,15);
IPAddress ap_subnet(255,255,255,0);

// WiFi.mode(WIFI_STA)      - station mode: the ESP32 connects to an access point
// WiFi.mode(WIFI_AP)       - access point mode: stations can connect to the ESP32
// WiFi.mode(WIFI_AP_STA)   - access point and a station connected to another access point

int wifi_mode = WIFI_STA;
bool wifi_change = false;

unsigned long previousMillisWIFI = 0;
unsigned long previousMillisAP = 0;
unsigned long intervalWIFI = 30000; // 30 Segundos

// http://adminesp32.local
const char *esp_hostname = device_id;

// -------------------------------------------------------------------
// Iniciar WIFI Modo AP
// -------------------------------------------------------------------
void startAP(){
    log("[ INFO ] Iniciando Modo AP");
    WiFi.disconnect(true);
    WiFi.softAPConfig(ap_IPv4, ap_IPv4, ap_subnet);
    WiFi.hostname(esp_hostname);
    WiFi.softAP(ap_ssid, ap_password, ap_chanel, ap_visibility, ap_connect);
    log("[ INFO ] WiFi AP " + String(ap_ssid) + " - IP " + ipStr(WiFi.softAPIP()));
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNSSERVER_PORT, "*", ap_IPv4);
    wifi_mode = WIFI_AP;
}
// -------------------------------------------------------------------
// Iniciar WIFI Modo Estación
// -------------------------------------------------------------------
void startClient(){
    log("[ INFO ] Iniciando Modo Estación");
    WiFi.mode(WIFI_STA);
   
    WiFi.hostname(esp_hostname);
    WiFi.begin(wifi_ssid, wifi_password);
    log("[ INFO ] Conectando al SSID " + String(wifi_ssid));
    byte b = 0;
    while( WiFi.status() != WL_CONNECTED && b < 60){
        b++;
        log("[ WARNING ] Intentando conexión WiFi ...");
        vTaskDelay(500);
        blinkSingle(100, WIFILED);        
    }
    if(WiFi.status() == WL_CONNECTED){
        log("[ INFO ] WiFi conectado (" + String(WiFi.RSSI()) + ") dBm IPv4 " + ipStr(WiFi.localIP()));
        blinkRandomSingle(10, 100, WIFILED);
        wifi_mode = WIFI_STA;
        wifi_change = true;
    }else{
        log("[ ERROR ] WiFi no conectado");        
        blinkRandomSingle(10, 100, WIFILED);
        wifi_change = true;
        startAP();
    }
}
// -------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------
void wifi_setup(){
    WiFi.disconnect(true);

    // 2) Caso contrario en Modo Estación
        startClient();
        if(WiFi.status() == WL_CONNECTED){
            log("[ INFO ] WiFI Modo Estación");
        }
   
}
// -------------------------------------------------------------------
// Loop Modo Estación
// -------------------------------------------------------------------
byte w = 0;
void wifiLoop(){
    unsigned long currentMillis = millis();
    if(WiFi.status() != WL_CONNECTED && (currentMillis - previousMillisWIFI >= intervalWIFI)){
        w++;
        blinkSingle(100, WIFILED);
        WiFi.disconnect(true);
        WiFi.reconnect();
        previousMillisWIFI = currentMillis;
        // 2 = 1 minuto
        if(w == 2){
            log("[ INFO ] Cambiando a Modo AP");
            wifi_change = true;
            w = 0;
            startAP(); 
        }else{
            log("[ WARNING ] SSID " + String(wifi_ssid) + " desconectado ");
        }
    }else{
       blinkSingleAsy(10, 500, WIFILED); 
    }
}
// -------------------------------------------------------------------
// Loop Modo AP
// -------------------------------------------------------------------
byte a = 0;
void wifiAPLoop(){
    blinkSingleAsy(5, 100, WIFILED);
    dnsServer.processNextRequest(); // Portal captivo DNS
    unsigned long currentMillis = millis();
    if((currentMillis - previousMillisAP >= intervalWIFI) && wifi_change){
        a++;
        previousMillisAP = currentMillis;
        // 20 es igual a 10 minuto
        if(a == 20){
            log("[ INFO ] Cambiando a Modo Estación");
            wifi_change = false;
            a = 0;
            startClient();
        }
    }
}

