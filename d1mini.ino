#define BLYNK_TEMPLATE_ID   "TMPLS3pHPisU"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN    "JLQYYyL1SL6LFQ3SnOlihw77pfwaBPPR"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

// Set WiFi credentials
//#define WIFI_SSID "ZTE_2.4G_6w4NGA"
//#define WIFI_PASS "samarinda2020"

#define WIFI_SSID "Realme C12"
#define WIFI_PASS "mamank1234"

//mendefinisikan pin virtual blynk untuk LED
WidgetLED ledWaspada(V6);
WidgetLED ledAwas(V7);
WidgetLED ledSiaga(V8);
WidgetLED ledAman(V9);

//mendefinisikan variabel notifSensor untuk nonaktif ultrasonik
int notifSensor = 0;
int notifOn = 0;
BLYNK_WRITE(V15){
  notifOn = param.asInt();
}

//Mendefinisikan variabel pin ultrasonik
int trigPin = D5;
int echoPin = D6;

//Mendefinisikan variabel relay
int relay = D7;

// interval notif
const long intervalSiaga = 15000;
const long intervalAwas = 10000;
const long intervalWaspada = 5000;

// deklarasi millis
unsigned long previousMillis = 0;

//Tombol relay ultrasonik
int tblUltrasonik;
BLYNK_WRITE(V4){
  tblUltrasonik = param.asInt();

  Blynk.virtualWrite(V14, tblUltrasonik);
}

void setup() {
  // Setup serial port
  Serial.begin(9600);
//  Serial.println();

  //mendefinisikan pin ultrasonik
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //mendefinisikan pin relay
  pinMode(relay, OUTPUT);

  // Begin WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  // Loop continuously while WiFi is not connected
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  Blynk.begin(BLYNK_AUTH_TOKEN,WIFI_SSID, WIFI_PASS);
  // You can also specify server;
  // Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  // Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();

  unsigned long currentMillis = millis();
  
    //Switch ultrasonik relay
  if (tblUltrasonik == 0){
    digitalWrite(relay, 1);
  } else {
    digitalWrite(relay, 0);
  }

  //mendefinisikan variabel ultrasonik
  long duration, distance, level;

  //proses untuk mendapatkan input ultrasonik
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //formula untuk mendapatkan jarak
  distance = (duration/2)/29.1;
  Blynk.virtualWrite(V11, distance);
  
  //formula untuk mendapatkan level ketinggian air
  if (distance == 0 || distance > 20){
    level = 0;
  } else {
    level = 20 - distance;
  }
  

  //menampilkan jarak ultrasonik ke permukaan air pada serial monitor
  Serial.print(distance);
  Serial.println("CM");

  //Decision nonaktif ultrasonik
  if((distance == 0) && (tblUltrasonik == 0)){
    //menampilkan ketinggian air pada widget level = 0
    Blynk.virtualWrite(V5, 0); 
    
    ledAman.off();
    ledSiaga.off();
    ledAwas.off();
    ledWaspada.off();

  }
  else
  {
    //menampilkan ketinggian air pada widget level
    Blynk.virtualWrite(V5, level); 

   //LED ketinggian Air & Notifikasi
    if ((level >= 0)&&(level <= 5))
    {
      ledAman.on();
      ledSiaga.off();
      ledAwas.off();
      ledWaspada.off();
    }
    else if ((level >= 6)&&(level <= 10))
    {
      ledAman.off();
      ledSiaga.on();
      ledAwas.off();
      ledWaspada.off();

      if (currentMillis - previousMillis >= intervalSiaga){
        previousMillis = currentMillis;

        if (notifSensor != 1 && notifOn == 1){
          notifSensor = 1;
          Blynk.logEvent("peringatan_siaga", String("Level Air SIAGA!! ") + level +(" CM"));
        } else {
          notifSensor = 0;
        }
      }
    }
    else if ((level >= 11)&&(level <= 15))
    {
      ledAman.off();
      ledSiaga.off();
      ledAwas.on();
      ledWaspada.off();


      if (currentMillis - previousMillis >= intervalAwas){
        previousMillis = currentMillis;

        if (notifSensor != 2 && notifOn == 1){
          notifSensor = 2;
          Blynk.logEvent("peringatan_awas", String("Level Air AWAS!! ") + level + (" CM"));
        } else {
          notifSensor = 0;
        }
      }
    }
    else
    {
      ledAman.off();
      ledSiaga.off();
      ledAwas.off();
      ledWaspada.on();

      if (currentMillis - previousMillis >= intervalWaspada){
        previousMillis = currentMillis;

        if (notifSensor != 3 && notifOn == 1){
          notifSensor = 3;
          Blynk.logEvent("peringatan_awas", String("Level Air WASPADA!!! ") + level + (" CM"));
        } else {
          notifSensor = 0;
        }
      }
    }
  }
  
  delay(100);
}
