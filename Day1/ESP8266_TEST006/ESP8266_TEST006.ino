void setup() {
  // put your setup code here, to run once:
  pinMode(A0,INPUT);
  Serial.begin(9600);

}

void loop() {
  int digitalVoltage = analogRead(A0);
  double actualVoltage = digitalVoltage/1024.0;
  double illuminance = pow(actualVoltage*3.210/2.8715 * 1000, 1/0.9816);
  Serial.println(illuminance);
  delay(1000);

}
