

// -------------------------------------------------------------------
// Definiciones
// -------------------------------------------------------------------
#define WIFILED 14  
#define DHTTYPE 11      
#define DHTPIN 33  
#define LEDCONTROL 13                    
// -------------------------------------------------------------------
// CALCULAR LA CAPACIDAD DEL JSON
// Asistente ArduinoJson: https://arduinojson.org/v6/assistant/
// Documentación: https://arduinojson.org/v6/api/json/deserializejson/
// -------------------------------------------------------------------
const size_t capacitySettings = 1536;
// -------------------------------------------------------------------
// Versión de Firmware desde las variables de entorno platformio.ini
// -------------------------------------------------------------------
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String device_fw_version = ESCAPEQUOTE(BUILD_TAG);
// -------------------------------------------------------------------
// Version de Hardware y Fabricante
// -------------------------------------------------------------------
#define device_hw_version   "ADMINVUE32 v1 00000000"    // Versión del hardware
#define device_manufacturer "IOTHOST"                   // Fabricante


float temp;
float humidity;
float fah;
float heatIndex;

// -------------------------------------------------------------------
// Zona configuración Dispositivo
// ------------------------------------------------------------------- 
boolean device_config_file;         // Identificador para archivo de configuración
char    device_config_serial[30];   // Numero de serie de cada Archivo configuración unico
char    device_id[30];              // ID del dispositivo  
int     device_restart;             // Número de reinicios 
char    device_old_user[15];        // Usuario para acceso al servidor Web
char    device_old_password[15];    // Contraseña del usuario servidor Web
// -------------------------------------------------------------------
// Zona configuración WIFI modo Cliente
// -------------------------------------------------------------------
boolean wifi_ip_static;             // Uso de IP Estática DHCP
char    wifi_ssid[30];              // Nombre de la red WiFi                 
char    wifi_password[30];          // Contraseña de la Red WiFi 
char    wifi_ipv4[15];              // Dir IPv4 Estático   
char    wifi_gateway[15];           // Dir IPv4 Gateway     
char    wifi_subnet[15];            // Dir IPv4 Subred    
char    wifi_dns_primary[15];       // Dir IPv4 DNS primario  
char    wifi_dns_secondary[15];     // Dir IPv4 DNS secundario 
// -------------------------------------------------------------------
// Zona configuración WIFI modo AP
// -------------------------------------------------------------------
boolean ap_mode;                    // Uso de Modo AP 
char    ap_ssid[31];                // Nombre del SSID AP      
char    ap_password[63];            // Contraseña del AP
int     ap_chanel;                  // Canal AP           
int     ap_visibility;              // Es visible o no el AP  (0 - Visible  1 - Oculto)  
int     ap_connect;                 // Número de conexiones en el AP máx 8 conexiones ESP32
// -------------------------------------------------------------------
// Zona configuración MQTT 
// ------------------------------------------------------------------- 
boolean mqtt_cloud_enable;          // Habilitar MQTT Broker
char    mqtt_cloud_id[30];          // Cliente ID MQTT Broker       
char    mqtt_user[30];              // Usuario MQTT Broker 
char    mqtt_password[39];          // Contraseña del MQTT Broker
char    mqtt_server[39];            // Servidor del MQTT Broker
int     mqtt_port;                  // Puerto servidor MQTT Broker
boolean mqtt_retain;                // Habilitar mensajes retenidos
int     mqtt_qos;                   // Calidad del servicio
boolean mqtt_time_send;             // Habilitar en envio de datos
int     mqtt_time_interval;         // Tiempo de envio por MQTT en segundos
boolean mqtt_status_send;           // Habilitar en envio de estados
// -------------------------------------------------------------------
// Zona Firmware Update TODO: Para La API REST
// -------------------------------------------------------------------
size_t  content_len;
#define U_PART U_SPIFFS
// -------------------------------------------------------------------
// Zona EEPROM para contador de reinicios
// -------------------------------------------------------------------
#define Start_Address 0
#define Restart_Address Start_Address + sizeof(int)