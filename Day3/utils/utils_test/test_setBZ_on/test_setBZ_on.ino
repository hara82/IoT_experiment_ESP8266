#include "utils/utils.ino"
void setup() {
  pinMode(14,OUTPUT);
  digitalWrite(14,LOW);
  delay(1000);

}

void loop() {
  setBZ(true);
  while(1);

}
