#include "utils/utils.ino"
void setup() {
  Serial.begin(9600);
  pinMode(2,INPUT);
}

void loop() {
  if(getPushSWStatus()){
    Serial.println(true);
  }else {
    Serial.println(false);
  }
  delay(300);

}
