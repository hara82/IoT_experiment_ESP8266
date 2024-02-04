#include "utils/utils.ino"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
char ssid[] = "...";
char pass[] = "...";
//const char* ntp_server = "ntp.nict.jp";
const char* ntp_servers[] = {"ntp.nict.jp", "www.gutp.jp"};
int idx = 0;

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {

  delay(1000);
  int sync_successful = syncNTPTime(ntp_servers[idx]);
  Serial.println(ntp_servers[idx]);
  
  Serial.print("syncNTPTime:");
  Serial.println(sync_successful);
  delay(15000);
  time_t t = now();
  char str_time[30];
  sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d",
          year(t), month(t), day(t),
          hour(t), minute(t), second(t));

  Serial.println(str_time);

  idx = (idx+1)%2;
  delay(3000);

}
