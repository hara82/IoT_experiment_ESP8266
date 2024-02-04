#include "utils/utils.ino"
void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT);
  delay(4000);
}

void loop() {
    Serial.println("initial call:"+String(detectPushSWON()));
    for(int i=0; i < 1000; ++i){
        Serial.println("detectPushSWON:"+String(detectPushSWON()));
        delay(200);
    }
    while(1);
}
