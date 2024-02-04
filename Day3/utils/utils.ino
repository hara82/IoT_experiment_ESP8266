#include <WiFiUdp.h> 
#include <TimeLib.h> 
/**
 * @brief 
 * ブザーをon/offする
 * 事前にpinMode(14,OUTPUT)の処理が必要
 * @param on 
 * when on = true, turn on the buzzer
 * when on = false, turn off the buzzer
 */
void setBZ(boolean on) {
    if (on) {
        digitalWrite(14, HIGH);
    } else {
        digitalWrite(14, LOW);
    }
}

/**
 * @brief push SWが押されているかどうかを返す
 * 事前にpinMode(2,INPUT)の処理が必要
 * 
 * @return boolean 
 * true: 押されている
 * false: 押されていない
 */
boolean getPushSWStatus() {
    if (digitalRead(2) == LOW) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief push SWが押されたことを判定する
 * 事前にpinMode(2,INPUT)の処理が必要
 * 
 * @return boolean 
 * 前回の呼び出し時に押されていない、かつ、今回の呼び出し時に押されている場合にtrueを返す
 * ほかはfalseを返す。
 * 初回呼び出しもfalseを返す。
 */
boolean detectPushSWON() {
    // 最初にtrueで初期化することで、初回呼び出しは必ずfalseを返すようにする。
    static bool prev_on = true;
    bool cur_on = getPushSWStatus();

    if(cur_on && !prev_on) {
        prev_on = cur_on;
        return true;
    }
    prev_on = cur_on;
    return false;
}

/**
 * @brief DIP SWの状態を取得する
 * 事前にpinMode(12,INPUT)とpinMode(13,INPUT)の処理が必要
 * 
 * @return int 
 * 両方offの場合0, 1つ目のみがonの場合1, 2つ目のみがonの場合2, 両方onの場合3を返す
 * というように、２進数の値を返す。
 */
int getDIPSWSatus() {
    int sw_stat = 0;
    int bit_0 = digitalRead(13);
    int bit_1 = digitalRead(12);

    if (bit_0 == LOW) {
        sw_stat += 1 << 0;
    }
    if (bit_1 == LOW) {
        sw_stat += 1 << 1;
    }
    return sw_stat;
}

/**
 * @brief MD: Motion Detectorの状態を取得する
 * 事前にpinMode(16,INPUT)の処理が必要
 * 
 * @return boolean 
 * Detected: true
 * Not detected: false
 */
boolean getMDStatus() {
    if(digitalRead(16) == HIGH) {
        return true;
    } else{
        return false;
    }
}

/**
 * @brief 現在の照度の値を取得する(lux単位)
 * 事前にpinMode(A0,INPUT)の処理が必要
 * 
 * @return int 
 * 現在の照度(lux)を返す
 */
int getIlluminance() {
    int digitalVoltage = analogRead(A0);
    double actualVoltage = digitalVoltage / 1024.0;
    double illuminance = pow(actualVoltage * 3.210 / 2.8715 * 1000, 1 / 0.9816);
    return illuminance;
}

/**
 * @brief NTPサーバから現在時刻を取得する
 * 事前にWiFiに関する接続処理が行われている必要がある。
 * 10回まで500ms秒間隔でリトライする。
 * 
 * @param ntp_server 
 * ntp_serverのホスト名またはipアドレス。デフォルトはntp.nict.jp
 * @return unsigned long 
 * JST(+9時間)のunix timeを返す。
 * 時刻の取得に失敗した場合は0を返す。
 */
unsigned long getNTPTime(const char* ntp_server="ntp.nict.jp") {
    WiFiUDP udp; 
    udp.begin(8888); 
    unsigned long unix_time=0UL; 
    byte packet[48]; 
    memset(packet, 0, 48); 
    packet[0] = 0b11100011; 
    packet[1] = 0; 
    packet[2] = 6; 
    packet[3] = 0xEC; 
    packet[12] = 49; 
    packet[13] = 0x4E; 
    packet[14] = 49; 
    packet[15] = 52; 
    udp.beginPacket(ntp_server, 123); 
    udp.write(packet, 48); 
    udp.endPacket(); 
 
    for(int i=0;i<10;i++){ 
        delay(500); 
        if(udp.parsePacket()){ 
            udp.read(packet, 48); 
            unsigned long highWord = word(packet[40], packet[41]); 
            unsigned long lowWord = word(packet[42], packet[43]); 
            unsigned long secsSince1900 = highWord << 16 | lowWord; 
            const unsigned long seventyYears = 2208988800UL; 
            unix_time = secsSince1900 - seventyYears + 32400UL; // 32400 = 9 hours (JST) 
            udp.stop();
            return unix_time;
        } 
    } 
    udp.stop(); 
    // return 0 when failed to get NTP time.
    return 0;
}


/**
 * @brief 指定したNTPサーバと時刻同期を行う。
 * 
 * @param ntp_server 
 * ntp_serverのホスト名またはipアドレス。デフォルトはntp.nict.jp
 * 
 * @return boolean 
 * true: 同期に成功した
 * false: 同期に失敗した
 */
boolean syncNTPTime(const char* ntp_server="ntp.nict.jp") {
    unsigned long unix_time=getNTPTime(ntp_server); 
    if(unix_time>0){ 
        setTime(unix_time); 
        return true; 
    } 
    return false; 
}