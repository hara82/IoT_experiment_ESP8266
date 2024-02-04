#include "utils/utils.ino"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
char ssid[] = "...";
char pass[] = "..."; // Wrong Password
const char* ntp_server = "ntp.nict.jp";

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  delay(1000);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wrong Password");
    delay(1000);
  }
  Serial.print("WiFi status:");
  Serial.println(WiFi.status());
  delay(1000);

}

void loop() {
  delay(1000);
  int sync_successful = syncNTPTime(ntp_server);
  
  Serial.print("syncNTPTime:");
  Serial.println(sync_successful);
  delay(15000);
  time_t t = now();
  char str_time[30];
  sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d",
          year(t), month(t), day(t),
          hour(t), minute(t), second(t));

  Serial.println(str_time);
}