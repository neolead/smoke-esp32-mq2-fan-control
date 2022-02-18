// Pinout - relay pin25 , pwm - pin26, sensor pin34
// WebSerial is accessible at "<IP Address>/webserial" in browser
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WebSerial.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#include <TroykaMQ.h>
AsyncWebServer server(80);

const char* ssid = "wifi";
const char* password = "passw";
//
#define GPIO_FAN 26 // Fan wire connect to 26 pin esp32
#define PWM_FREQUENCY 25000
#define PWM_RESOUTION 8 // bits
#define INITIAL_FAN_PERCENTAGE 100
#define MIN_PERCENTAGE 0
#define MAX_PERCENTAGE 100
#define PERCENTAGE_STEP 10
int percentage = INITIAL_FAN_PERCENTAGE;
// библиотека для работы с датчиками MQ 
#define INTERVAL_GET_DATA 2000  // интервала измерений, мс 
#define INTERVAL1_GET_DATA 600000  // длительность включения вентилятора 
// пин, к которому подключен датчик
#define MQ2PIN          34 //pin d34
// создаём объект для работы с датчиком
MQ2 mq2(MQ2PIN);
//const int ledPin = 10; // LED connected to digital pin 13
int rel=25;//relay pin d25
int relay_on=0;
// переменная для интервала измерений
unsigned long millis_int1=0;
unsigned long millis_int2=0;
// fan

//
void recvMsg(uint8_t *data, size_t len){
  WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  WebSerial.println(d);
  if (d == "ON"){
    WebSerial.println("Received Data...");
  }
  if (d=="OFF"){
    WebSerial.println("Received Data...");
  }
}

void setup() {

    pinMode(rel,OUTPUT);
  // открываем последовательный порт
  Serial.begin(115200);
  // калибровка
  mq2.calibrate();
  mq2.getRo();
  pinMode(GPIO_FAN, OUTPUT);
  ledcSetup(0, PWM_FREQUENCY, PWM_RESOUTION);
  ledcAttachPin(GPIO_FAN, 0);
Serial.println("Setupeed");
Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
   ArduinoOTA.setHostname("smoke_esp");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
}

void loop() {
  ArduinoOTA.handle();  
//    percentage += PERCENTAGE_STEP;
//    if (percentage > MAX_PERCENTAGE) percentage = MIN_PERCENTAGE;
    
//    int dutyCycle = 255 * percentage / 100;
//    if (dutyCycle > 255) dutyCycle = 255;
//    if (dutyCycle < 0) dutyCycle = 0;

//    int dutyCycle = 255;
//    ledcWrite(0, dutyCycle);
  

    ledcWrite(0, 255);
       if(millis()-millis_int1 >= INTERVAL_GET_DATA) {
      // получение данных c датчика mq2
      float smoke=get_data_ppmsmoke();
      // выводим значения газа в ppm
      Serial.print("smoke=");
      WebSerial.print("smoke=");
      Serial.print(smoke);
      Serial.println(" ppm ");
      WebSerial.print(smoke);
      WebSerial.println(" ppm ");
      // старт интервала отсчета
      millis_int1=millis();
      
    if (smoke >= 10) {
      if (relay_on == 0){
        digitalWrite(rel, HIGH);
        //digitalWrite(LED_BUILTIN, HIGH);
        relay_on=1;
        millis_int2=millis();
        Serial.println("I turn on fan");
        WebSerial.println("I turn on fan");
      }
      Serial.println("I see dead people");
      WebSerial.println("I see dead people");
    }
    
    if (smoke <= 9.99) {
      if (relay_on == 1){
        
//        if(millis()-millis_int2 >= (INTERVAL1_GET_DATA / 2)) {
 //       ledcWrite(0, 100);
   //     Serial.println("switch to 50%");
     //   WebSerial.println("switch to 50%");
       // }

        if(millis()-millis_int2 >= INTERVAL1_GET_DATA) {
        digitalWrite(rel, LOW);
        relay_on=0;
        Serial.println("I turn off fan");
        WebSerial.println("I turn off fan");
        }

        
        Serial.println("I know, that fan is fanning. not timeouted.");
        WebSerial.println("I know, that fan is fanning. not timeouted.");
      }
        Serial.println("I'm ok");
        WebSerial.println("I'm ok");
       
      }
   }} 
  
float get_data_ppmsmoke()  {

  Serial.println(mq2.readRatio());
  WebSerial.println(mq2.readRatio());
  
  // получение значения 
 ///// float value=mq2.readSmoke();
  float value=mq2.readRatio();
  return value;
}
  
