#include "utils/utils.ino"

void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);
  delay(3000);
}

void loop() {
    for(int i=0; i < 100; ++i){
        Serial.print("getIlluminance:");
        Serial.println(getIlluminance());
        delay(1000);
    }
    while(1);
}
