boolean motionDetected = false;
unsigned long prevTime = 0;
void setup() {
  // 人感センサーは16番ピンに接続
  pinMode(16,INPUT);
  Serial.begin(9600);
}

void loop() {
  // 1 when detected in the past 1 min.
  unsigned long currentTime = millis();
  boolean currentState = digitalRead(16);
  motionDetected = motionDetected || currentState ;

  if (currentTime - prevTime >= 60000) {
    prevTime = currentTime;
    if(motionDetected){
      Serial.println("human detected");
    }    else {
      Serial.println("human not detected");
    }
    motionDetected = false;
  }
}
