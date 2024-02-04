#include "utils/utils.ino"
void setup() {
  Serial.begin(9600);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  delay(3000);
}

void loop() {
    for(int i=0; i < 100; ++i){
        Serial.print("getDIPSWSatus:");
        Serial.println(getDIPSWSatus());
        delay(1000);
    }
    while(1);
}
