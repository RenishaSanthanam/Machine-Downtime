#define BLYNK_TEMPLATE_ID "TMPLVq4_Qpgg"
#define BLYNK_TEMPLATE_NAME "MACHINE DOWNTIME"
#define BLYNK_AUTH_TOKEN "C5UnuPk0rkUzpniK7tUyvcDFKThdZSNk"
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#define SENSOR_PIN A0
#define vibr_Pin D5
#define DHTTYPE DHT11   // DHT 11
#define dht_dpin D1
#define BLYNK_PRINT Serial
#define BLYNK_TIMEOUT_MS 5000
DHT dht(dht_dpin, DHTTYPE);
char auth[] = "C5UnuPk0rkUzpniK7tUyvcDFKThdZSNk";
const char* ssid = "TECNO POVA 5G";
const char* pass = "1234567890";

const int sampleWindow = 50;
unsigned int sample;
float t;
long measurement;
int db;

String phoneNumber = "+919629769419";
String apiKey = "9776165";

void sendMessage(String message){
  Serial.print("message : ");
  Serial.println(message);
  // Data to send with HTTP POST
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
//  http://api.callmebot.com/whatsapp.php?phone=+919629769419&apikey=9776165&text=HI
  WiFiClient client;    
  HTTPClient http;
  http.begin(client, url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.println("Message sent successfully");
  }
  else{
//    Serial.println("Error sending the message");
//    Serial.print("HTTP response code: ");
//    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
  delay(2000);
}

BlynkTimer timer;

void setup() {

  Serial.begin(115200);


  Blynk.begin(auth, ssid, pass);
  
  WiFi.begin(ssid, pass);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.begin(9600);
  dht.begin();
  pinMode (SENSOR_PIN, INPUT);
  pinMode(vibr_Pin, INPUT); //set vibr_Pin input for measurment
  Serial.println("Humidity and temperature \n\n");
  Serial.println("==========================================");
  delay(700);
}
void temp_data()
{
  float h = dht.readHumidity();
        t = dht.readTemperature();
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t);
  Serial.println("C  ");
  delay(800);
}
void loop() {
  unsigned long startMillis = millis();  // Start of sample window
  float peakToPeak = 0;  // peak-to-peak level
  unsigned int signalMax = 0;  //minimum value
  unsigned int signalMin = 1024;  //maximum value
  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(SENSOR_PIN);  //get reading from microphone
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  Serial.print("peakToPeak : ");
  Serial.println(peakToPeak);
  db = map(peakToPeak, 20, 900, 49.5, 90);  //calibrate for deciBels;
  Serial.print("DBLevel:");
  Serial.println(db);
  measurement = TP_init();
  delay(50);
  Serial.print("Vibration = ");
  Serial.println(measurement);
  if (measurement > 1000) {
  }
  else {
  }
  temp_data();
  Machine_downtime();
  
    notify();
  Blynk.run();
  timer.run();
}
long TP_init() {
  delay(10);
  measurement = pulseIn (vibr_Pin, HIGH); //wait for the pin to get HIGH and returns measurement
  return measurement;
}

void Machine_downtime()
{
 
  if((t>20.00) && (measurement>50.00) && (db>40))
  {
    Serial.println("Machine running properly");
  }
  else
  {
    Serial.println("Machine stopped");
    sendMessage("Machine Stopped");
  }
}
void notify()
{
 
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V2, measurement);
    Blynk.virtualWrite(V3, db);


}
