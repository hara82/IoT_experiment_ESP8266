/**
 * This is a client program to run on ESP8266.
 * It sends a message to the server every 10 seconds.
 * If the server sends a message "buzzar:on", the ESP8266 will turn on the buzzar.
 * The server can turn off the buzzar by sending a message "buzzar:off".
 * 
 */
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "utils/utils.ino"

extern "C" {
  #include "user_interface.h"
  #include "wpa2_enterprise.h"
}

// WiFi setups.
char* ssid = "....";
char* username = "...";
char* password = "....";
const char* ntp_server = "ntp.nict.jp";

const char* host = "iot.hongo.wide.ad.jp";
const int port = 10211;

unsigned long last_sync_time = 0;

#define OLED_RESET 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define MAX_CONNECTION_ATTEMPT 20
#define MESSAGE_INTERVAL 10 // in seconds
#define WPA2Personal 0
#define WPA2Enterprise 1

// initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // 14番ピンのsetup
  pinMode(14, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.print("Connecting to ");
  display.print(ssid);
  display.print(":");
  display.display();

#if WPA2Personal
  // Connects to WiFi.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
#endif


#if WPA2Enterprise
 // WPA2 Enterpriseの設定
 // UTokyo WiFiにつなぎたいときに必要
  WiFi.mode(WIFI_STA);
  wifi_station_disconnect();
  struct station_config sta_config;
  memset(&sta_config, 0, sizeof(sta_config));
  strcpy((char*)sta_config.ssid, ssid);

  wifi_station_set_config(&sta_config);
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen(password));
  wifi_station_connect();

#endif

  for (int i = 0; i < MAX_CONNECTION_ATTEMPT; i++) {
    delay(500);
    if (WiFi.status() == WL_CONNECTED) {
      display.println("SUCCESS");
      display.println("IP address: ");
      display.println(WiFi.localIP());
      display.display();
      break;
    }

    if (i == MAX_CONNECTION_ATTEMPT-1) {
      display.println("FAILURE");
      display.print("status:");
      display.println(WiFi.status());
      display.display();
      exit(1);
    }
  }

  // Sync to the current time.
  display.print("Sync to ");
  display.print(ntp_server);
  display.print(":");
  display.display();
  for (int i = 0; i < MAX_CONNECTION_ATTEMPT; i++) {
    
    if (syncNTPTime(ntp_server)) {
      display.println("SUCCESS");
      display.display();
      break;
    }

    if (i == MAX_CONNECTION_ATTEMPT-1) {
      display.println("FAILURE");
      display.display();
      exit(1);
    }
  }

  // Displays the current time.
  display.println(formatDateTime(now()));
  display.display();

  delay(5000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.display();
}

void loop(){
  unsigned long t = now();
  if(t/MESSAGE_INTERVAL != last_sync_time/MESSAGE_INTERVAL){
    char* newMessage = createMessage();
    display.println(newMessage);
    display.display();

    sendToServer(newMessage);

    last_sync_time = t;

    // Cleans up the display.
    delay((MESSAGE_INTERVAL-1)*1000);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.display();
  }
  
}



char* formatDateTime(unsigned long timeStamp) {
  static char strTime[30];  // 静的な文字列バッファを用意
  sprintf(strTime, "%04d-%02d-%02dT%02d:%02d:%02d",
          year(timeStamp), month(timeStamp), day(timeStamp),
          hour(timeStamp), minute(timeStamp), second(timeStamp));
  return strTime;
}



char* createMessage(){
  static char message[100];
  int id = getDIPSWSatus();
  char* curTime = formatDateTime(now());
  int curIlluminance = getIlluminance();
  boolean mdStatus = getMDStatus();

  sprintf(message, "%d,%s,%d,%d",
          id, curTime, curIlluminance, mdStatus);
  return message;
}

void sendToServer(char* message){
  WiFiClient client;
  if (!client.connect(host, port)) {
    display.print("...ERR\r\n");
    display.display();
    return;
  }

  client.print(message);
  delay(1000);

  // サーバーからの応答を受信
  String response = "";
  while (client.available()) {
    char c = client.read();
    response += c;
  }

  // Splits response by first newline.
  String data_format, buzzar_status;
  int split_index = response.indexOf("$");
  if(split_index != -1) {
    data_format = response.substring(0, split_index);
    buzzar_status = response.substring(split_index+1);
  }else{
    data_format = response;
  }


  // 応答に基づいて表示を更新
  if (data_format == "OK\r\n") {
    display.print("...OK\r\n");
  } else if (data_format == "ERROR\r\n") {
    display.print("...NG\r\n");
  } else if (!client.connected()) {
    display.print("...ERR\r\n");
  }
  display.display();

  // Buzzarの状態を更新
  if (buzzar_status == "buzzar:on") {
    display.display();
    setBZ(true);
  } else if (buzzar_status == "buzzar:off") {
    setBZ(false);
  } else {
    // Buzzarの状態を更新しない
  }

  // TCP接続を切断
  client.stop();
}
