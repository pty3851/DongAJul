#include <SoftwareSerial.h>        // 블루투스 통신을 위한 라이브러리
#include <Adafruit_NeoPixel.h>     // led스트립 사용을 위한 라이브러리
#include "DHT.h"                   // 온습도센서 사용을 위한 라이브러리

#define DHTPIN 5       // 온습도센서 5번핀
#define DHTTYPE DHT11  // 온습도센서 종류
#define flame_1 2      // 1번 불꽃감지 2번핀
#define flame_2 3      // 2번 불꽃감지 3번핀
#define flame_3 4      // 3번 불꽃감지 4번핀
#define vibrator 6     // 진동모터 6번핀
#define BTtx  7        // 블루투스 TX 7번핀
#define BTrx  8        // 블루투스 RX 8번핀

#define PIN_1 13       // 1번 LED 스트립 13번핀
#define PIN_2 12       // 2번 LED 스트립 12번핀
#define PIN_3 11       // 3번 LED 스트립 11번핀
#define PIN_4 10       // 4번 LED 스트립 10번핀
#define PIN_5 9        // 5번 LED 스트립 9번핀

#define NUM_LEDS 4     // led 스트립의 led소자 개수 4개

SoftwareSerial BT(BTtx, BTrx);  //Rx, Tx 
DHT dht(DHTPIN, DHTTYPE);       //온습도센서 핀번호와 종류를 설정

int flame_state_1 = 1; // 불꽃감지 센서 on-off 확인 변수
int flame_state_2 = 1; // 불꽃감지 센서 on-off 확인 변수
int flame_state_3 = 1; // 불꽃감지 센서 on-off 확인 변수
int Gaspin = A0;       // 일산화탄소 감지센서 핀 번호 A0
int situation = 0;     // 현재 상황을 7가지로 나누어 대처
int LED_1 = A1;        // 소화기 위치를 알려줄 led 핀번호 A1
int LED_2 = A2;        // 소화기 위치를 알려줄 led 핀번호 A2
int MOOD_R = A3;       // 무드등 빨간색 핀번호 A3
int MOOD_G = A4;       // 무드등 초록색 핀번호 A4   
int MOOD_Y = A5;       // 무드등 노란색 핀번호 A5

Adafruit_NeoPixel strip_1 = Adafruit_NeoPixel(NUM_LEDS, PIN_1, NEO_GRB + NEO_KHZ800); // LED 스트립 구동 코드(led소자수,핀번호,네오픽셀 종류)
Adafruit_NeoPixel strip_2 = Adafruit_NeoPixel(NUM_LEDS, PIN_2, NEO_GRB + NEO_KHZ800); // LED 스트립 구동 코드(led소자수,핀번호,네오픽셀 종류)
Adafruit_NeoPixel strip_3 = Adafruit_NeoPixel(NUM_LEDS, PIN_3, NEO_GRB + NEO_KHZ800); // LED 스트립 구동 코드(led소자수,핀번호,네오픽셀 종류)
Adafruit_NeoPixel strip_4 = Adafruit_NeoPixel(NUM_LEDS, PIN_4, NEO_GRB + NEO_KHZ800); // LED 스트립 구동 코드(led소자수,핀번호,네오픽셀 종류)
Adafruit_NeoPixel strip_5 = Adafruit_NeoPixel(NUM_LEDS, PIN_5, NEO_GRB + NEO_KHZ800); // LED 스트립 구동 코드(led소자수,핀번호,네오픽셀 종류)

void setup() {
  pinMode(Gaspin, INPUT);     // 가스 감지 센서를 입력으로 설정
  pinMode(LED_1, OUTPUT);     // 1번 소화기 위치 led 출력으로 설정
  pinMode(LED_2, OUTPUT);     // 2번 소화기 위치 led 출력으로 설정
  pinMode(MOOD_R, OUTPUT);    // 무드등 빨간색 출력
  pinMode(MOOD_G, OUTPUT);    // 무드등 초록색 출력
  pinMode(MOOD_Y, OUTPUT);    // 무드등 노란색 출력
  pinMode(flame_1, INPUT);    // 1번 불꽃 감지 센서를 입력으로 설정
  pinMode(flame_2, INPUT);    // 2번 불꽃 감지 센서를 입력으로 설정
  pinMode(flame_3, INPUT);    // 3번 불꽃 감지 센서를 입력으로 설정
  pinMode(vibrator, OUTPUT);  // 진동모터 출력으로 설정
  Serial.begin(9600);         // 시리얼 모니터 통신 시작
  BT.begin(9600);             // 블루투스 통신 시작
  dht.begin();                // 온습도센서
  strip_1.begin();            // 1~5번 led스트립 구동시작 및 초기화
  strip_1.show();             
  strip_2.begin();            
  strip_2.show();             
  strip_3.begin();            
  strip_3.show();             
  strip_4.begin();            
  strip_4.show();            
  strip_5.begin();            
  strip_5.show();             
}
 
void loop() {
  flame_state_1 = digitalRead(flame_1);    // 1번 불꽃 감지 센서값 저장
  flame_state_2 = digitalRead(flame_2);    // 2번 불꽃 감지 센서값 저장
  flame_state_3 = digitalRead(flame_3);    // 3번 불꽃 감지 센서값 저장
  float temp = dht.readTemperature();  // temp 변수에 온도값 소수점 단위 저장
  float humi = dht.readHumidity();     // humi 변수에 습도값 소수점 단위 저장
  int conc = analogRead(Gaspin);       // 일산화탄소 센서값 정수형 변수 conc에 저장

  if (conc > 200){                     // 일산화탄소 농도 200ppm 이상일 때
    if (flame_state_1 == 0 && flame_state_2 == 1 && flame_state_3 == 1){           // 1번 위치에 불이 났을 경우 & 2,3번 화재 감지 X
      situation = 1;                   // 현재 상황 : 1번 위치 화재 발생
      Serial.println("Fire_1");        // 화재 발생
      analogWrite(vibrator,200);       // 진동모터 on
      digitalWrite(MOOD_R, HIGH);      // 무드등 빨간색 on
      digitalWrite(MOOD_G, LOW);       // 무드등 초록색 off
      digitalWrite(MOOD_Y, LOW);       // 무드등 노란색 off
      digitalWrite(LED_1, HIGH);       // 소화기 위치 알림등 on
      digitalWrite(LED_2, HIGH);       // 소화기 위치 알림등 on

      for(int i=0;i<4;i++){            // 탈출경로를 알리기 위해 led 스트립 소자 순서대로 켜지도록 함
        strip_1.setPixelColor(i, 0, 0, 0);   // 1번 스트립(led소자 순서,red,green,blue)
        strip_2.setPixelColor(i, 0, 0, 0);   // 2번 스트립(led소자 순서,red,green,blue)
        strip_3.setPixelColor(i, 0, 255, 0); // 3번 스트립(led소자 순서,red,green,blue)  
        strip_4.setPixelColor(i, 0, 255, 0); // 4번 스트립(led소자 순서,red,green,blue)
        strip_5.setPixelColor(i, 0, 255, 0); // 5번 스트릡(led소자 순서,red,green,blue)
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
      delay(1000);

      for(int i=0;i<4;i++){
        strip_3.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 0, 0);
        strip_5.setPixelColor(i, 0, 0, 0);
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
      delay(10);
    }
    else if (flame_state_1 == 1 && flame_state_2 == 0 && flame_state_3 == 1){      // 2번 위치에 불이 났을 경우 & 1,3번 화재 감지 X
      situation = 2;                   // 현재 상황 : 2번 위치 화재 발생
      Serial.println("Fire_2");        // 화재 발생
      analogWrite(6,200);              // 진동모터 on 
      digitalWrite(MOOD_R, HIGH);      // 무드등 빨간색 on
      digitalWrite(MOOD_G, LOW);       // 무드등 초록색 off
      digitalWrite(MOOD_Y, LOW);       // 무드등 노란색 off
      digitalWrite(LED_1, HIGH);       // 소화기 위치 알림등 on
      digitalWrite(LED_2, HIGH);       // 소화기 위치 알림등 on
      
      for(int i=0;i<4;i++){          // 탈출경로를 알리기 위해 led 스트립 소자 순서대로 켜지도록 함
        strip_1.setPixelColor(4-i, 0, 255, 0);
        strip_2.setPixelColor(4-i, 0, 255, 0);
        strip_3.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 255, 0);
        strip_5.setPixelColor(i, 0, 255, 0);
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
      delay(1000);

      for(int i=0;i<4;i++){
        strip_1.setPixelColor(i, 0, 0, 0);
        strip_2.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 0, 0);
        strip_5.setPixelColor(i, 0, 0, 0);
        strip_1.show();
        strip_2.show();
        strip_4.show();
        strip_5.show();
      }
      delay(10);
    }
    else if (flame_state_1 == 1 && flame_state_2 == 1 && flame_state_3 == 0){      // 3번 위치에 불이 났을 경우 & 1,2번 위치 화재 감지 X
      situation = 3;                   // 현재 상황 : 3번 위치 화재 발생
      Serial.println("Fire_3");        // 화재 발생
      analogWrite(6,200);              // 진동모터 on 
      digitalWrite(MOOD_R, HIGH);      // 무드등 빨간색 on
      digitalWrite(MOOD_G, LOW);       // 무드등 초록색 off
      digitalWrite(MOOD_Y, LOW);       // 무드등 노란색 off
      digitalWrite(LED_1, HIGH);       // 소화기 위치 알림등 on
      digitalWrite(LED_2, HIGH);       // 소화기 위치 알림등 on

      for(int i=0;i<4;i++){            // 탈출경로를 알리기 위해 led 스트립 소자 순서대로 켜지도록 함
        strip_1.setPixelColor(4-i, 0, 255, 0);
        strip_2.setPixelColor(4-i, 0, 255, 0);
        strip_3.setPixelColor(4-i, 0, 255, 0);
        strip_4.setPixelColor(4-i, 0, 255, 0);
        strip_5.setPixelColor(i, 0, 0, 0);
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
      delay(1000);

      for(int i=0;i<4;i++){
        strip_1.setPixelColor(i, 0, 0, 0);
        strip_2.setPixelColor(i, 0, 0, 0);
        strip_3.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 0, 0);
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
      }
      delay(10);
    }
    else if(flame_state_1 == 1 && flame_state_2 == 1 && flame_state_3 == 1){       // 1,2,3 위치 모두 불이 안났을 경우(가스만 감지되었을 경우)
      situation = 4;                   // 현재 상황 : 가스 감지(경고)
      Serial.println("Gas Detection"); // 가스 감지
      analogWrite(6,0);                // 진동모터 off
      digitalWrite(MOOD_R, LOW);       // 무드등 빨간색 off
      digitalWrite(MOOD_G, LOW);       // 무드등 초록색 off
      digitalWrite(MOOD_Y, HIGH);      // 무드등 노란색 on
      digitalWrite(LED_1, LOW);        // 소화기 위치 알림등 off
      digitalWrite(LED_2, LOW);        // 소화기 위치 알림등 off
      delay(10);

      for(int i=0;i<4;i++){
        strip_1.setPixelColor(i, 0, 0, 0);
        strip_2.setPixelColor(i, 0, 0, 0);
        strip_3.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 0, 0);
        strip_5.setPixelColor(i, 0, 0, 0);
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
    }

  }
  else {                               // 일산화탄소 농도 200ppm 이하일 때
    if (flame_state_1 == 0 || flame_state_2 == 0 || flame_state_3 == 0){  // 셋 중 하나라도 불꽃이 감지되면
      situation = 5;                   // 현재 상황 : 불꽃 감지(경고)
      Serial.println("Warning");       // 불꽃 발생 위험
      analogWrite(6,0);                // 진동모터 off
      digitalWrite(MOOD_R, LOW);       // 무드등 빨간색 off
      digitalWrite(MOOD_G, LOW);       // 무드등 초록색 off
      digitalWrite(MOOD_Y, HIGH);      // 무드등 노란색 on
      digitalWrite(LED_1, LOW);       // 소화기 위치 알림등 off
      digitalWrite(LED_2, LOW);       // 소화기 위치 알림등 off

      for(int i=0;i<4;i++){
        strip_1.setPixelColor(i, 0, 0, 0);
        strip_2.setPixelColor(i, 0, 0, 0);
        strip_3.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 0, 0);
        strip_5.setPixelColor(i, 0, 0, 0);
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
      delay(10);
    }
    else{
      situation = 6;                   // 현재 상황 : 안전
      Serial.println("SAFE");          // 불꽃 감지 x
      analogWrite(6,0);                // 진동모터 off
      digitalWrite(MOOD_R, LOW);       // 무드등 빨간색 off
      digitalWrite(MOOD_G, HIGH);      // 무드등 초록색 on
      digitalWrite(MOOD_Y, LOW);       // 무드등 노란색 off
      digitalWrite(LED_1, LOW);        // 소화기 위치 알림등 off
      digitalWrite(LED_2, LOW);        // 소화기 위치 알림등 off
      for(int i=0;i<4;i++){
        strip_1.setPixelColor(i, 0, 0, 0);
        strip_2.setPixelColor(i, 0, 0, 0);
        strip_3.setPixelColor(i, 0, 0, 0);
        strip_4.setPixelColor(i, 0, 0, 0);
        strip_5.setPixelColor(i, 0, 0, 0);
        strip_1.show();
        strip_2.show();
        strip_3.show();
        strip_4.show();
        strip_5.show();
      }
    
    delay(10);
    }
  }
  
  float data[3]={temp,hum,situation};  // 온도, 습도, 일산화탄소 농도, 현재상황을 데이터 배열로 저장

  for(int i=0;i<3;i++)
    {
      BT.println(data[i]);           // 블루투스를 통해 앱인벤터로 데이터 전달
      BT.print(",");                 // 각 센서값 ","로 구분
    }

  delay(1000);                       // 1초 주기로 반복
}