#include "utils/utils.ino"
void setup() {
  pinMode(14,OUTPUT);
  digitalWrite(14,HIGH);
  delay(4000);

}

void loop() {
  setBZ(false);
  while(1);

}
