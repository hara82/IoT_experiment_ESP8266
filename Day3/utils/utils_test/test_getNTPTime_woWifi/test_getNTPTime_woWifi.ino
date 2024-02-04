#include "utils/utils.ino"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
char ssid[] = "...";
char pass[] = "..."; // Wrong Password
const char* ntp_servers[] = {"ntp.nict.jp", "133.243.238.243", "www.gutp.jp", "abc.gutp.jp"};

int id = 0;

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
  const char* ntp_server = ntp_servers[id];
  Serial.println(ntp_server);

  unsigned long t = getNTPTime(ntp_server);
  Serial.print("unix time: ");
  Serial.println(t);
  if (t != 0) {
    char str_time[30];
    sprintf(str_time, "%04d-%02d-%02dT%02d:%02d:%02d",
            year(t), month(t), day(t),
            hour(t), minute(t), second(t));

    Serial.println(str_time);
  }
  delay(4000);
  id = (id + 1) % 4;
}
