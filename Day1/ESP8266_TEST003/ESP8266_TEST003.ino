// プッシュ検出プログラム 
int prev_stat=LOW; 
void setup(){ 
 pinMode(2,INPUT); 
 Serial.begin(9600); 
} 
void loop(){ 
 int stat=digitalRead(2); 
 if(stat==LOW && prev_stat==HIGH){ 
 Serial.println("Pushed!!"); 
 } 
 // チャタリングを防ぐために追加
 delay(1);
 prev_stat=stat; 
} 
