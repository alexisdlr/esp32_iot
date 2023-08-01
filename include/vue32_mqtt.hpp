
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttClient(espClient);

char topic[150];

String mqtt_data = "";

long lastMqttReconnectAttempt = 0;
long lastMsg = 0;

void callback(char *topic, byte *payload, unsigned int length);
String Json();

char    willTopic[150];
bool    willQoS      = 0;
bool    willRetain   = false;
String  willMessage  = "{\"connected\": false}";
bool    cleanSession = true;

// -------------------------------------------------------------------
// MQTT Connect
// -------------------------------------------------------------------
boolean mqtt_connect(){
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(callback);
    log("[ INFO ] Intentando conexión al Broker MQTT...");
    // https://pubsubclient.knolleary.net/api.html  
    // https://www.amebaiot.com/zh/rtl8722dm-arduino-api-pubsubclient/

    // Topico para enviar los estados.
    String topic_publish = pathMqtt()+"/status";
    topic_publish.toCharArray(willTopic, 150);

    // Función para generar la conexión al Broker
    // Intento de reconectar
    if(mqttClient.connect(mqtt_cloud_id, mqtt_user, mqtt_password, willTopic, willQoS, willRetain, willMessage.c_str(), cleanSession)){
        log("[ INFO ] Conectado al Broker MQTT");
        String topic_subscribe = pathMqtt()+"/command";
        topic_subscribe.toCharArray(topic, 150);

        // Nos suscribimos a comandos Topico: v1/device/usuario/dispositivo/comando
    
        // * boolean subscribe (topic, [qos])
        // qos - optional the qos to subscribe at (int: 0 or 1 only)
        // Función para suscribirnos al Topico
        if(mqttClient.subscribe(topic, mqtt_qos)){
            log("[ INFO ] Suscrito: " + String(topic));
        }else{
            log("[ ERROR ] Failed, to suscribe!"); 
        }
        // Si tenemos habilitado en envío de estados enviamos en mensaje de conectado
        if(mqtt_status_send){
            // int publish (topic, payload)
            //  * int publish (topic, payload, retained)
            // int publish (topic, payload, length, retained)
            // topic - the topic to publish to (const char[])
            // payload - the message to publish (const char[])
            // retained - informacion retenida (boolean)
            // length - the length of the message (byte)
            mqttClient.publish(willTopic, "{\"connected\": true}", mqtt_retain);
        }
    }else{
        log("[ ERROR ] failed, rc= " + mqttClient.state());
        return (0);
    }
    return (1);
}
// -------------------------------------------------------------------
// Manejo de los Mensajes Entrantes
// -------------------------------------------------------------------
void callback(char *topic, byte *payload, unsigned int length){
    String mensaje = "";
    String str_topic(topic);
    for(int16_t i = 0; i < length; i++){
        mensaje += (char)payload[i];
        // Parpadeo del Led en cada recepción de caracteres
    }
    mensaje.trim();
    log("[ INFO ] Topico -->" + str_topic);
    log("[ INFO ] Mensaje -->" + mensaje);

    Serial.println(mensaje=="on");
    if(mensaje == "on") {
        
        setOnSingle(LEDCONTROL);
        log("led encendido");
    } 
    if(mensaje == "off") {
        setOffSingle(LEDCONTROL);
    }
}
// -------------------------------------------------------------------
// Manejo de los Mensajes Salientes
// ------------------------------------------------------------------- 
void mqtt_publish_data(){
    String topic = pathMqtt()+"/data";
    mqtt_data = Json();
    mqttClient.publish(topic.c_str(), mqtt_data.c_str(), mqtt_retain);
    mqtt_data = "";
    mqttTX();
}
void mqtt_publish_temp(){
    String topic = pathMqtt()+"/temp";
    char mensaje[10];
    dtostrf(temp, 4, 2, mensaje);
    mqttClient.publish(topic.c_str(), mensaje, mqtt_retain);
    mqtt_data = "";
}
void mqtt_publish_humidity() {
    String topic = pathMqtt()+"/hum";
    char mensaje[10];
    dtostrf(humidity, 4, 2, mensaje);
    mqttClient.publish(topic.c_str(), mensaje, mqtt_retain);
    mqtt_data = "";
}
void mqtt_publish_fah() {
    String topic = pathMqtt()+"/fah";
    char mensaje[10];
    dtostrf(fah, 4, 2, mensaje);
    mqttClient.publish(topic.c_str(), mensaje, mqtt_retain);
    mqtt_data = "";
}
void mqtt_publish_heatIndex() {
    String topic = pathMqtt()+"/heat";
    char mensaje[10];
    dtostrf(heatIndex, 4, 2, mensaje);
    mqttClient.publish(topic.c_str(), mensaje, mqtt_retain);
    mqtt_data = "";
}

// -------------------------------------------------------------------
// JSON con información del Dispositivo para envio por MQTT
// ------------------------------------------------------------------- 
String Json(){
    String response;
    DynamicJsonDocument jsonDoc(3000);
    jsonDoc["temp"] = temp;
    jsonDoc["fah"] = fah;
    jsonDoc["hum"] = humidity;
    jsonDoc["heatIndex"] = heatIndex;
   
    jsonDoc["device_restart"] = String(device_restart);
    serializeJson(jsonDoc, response);
    return response;
}
// -------------------------------------------------------------------
// MQTT Loop Principal
// -------------------------------------------------------------------
void mqttLoop(){
        if(!mqttClient.connected()){
            long now = millis();
            // Intente conectarse continuamente durante los primeros 60 segundos
            // y luego vuelva a intentarlo una vez cada 120 segundos
            if((now < 60000) || ((now - lastMqttReconnectAttempt) > 120000)){
                lastMqttReconnectAttempt = now;
                // Intento de reconectar
                if(mqtt_connect()){
                    lastMqttReconnectAttempt = 0;
                }
                // Poner en ON el Led del MQTT
            }
        }else{
            mqttClient.loop();
            // Poner en OFF el Led del MQTT
        }
    }