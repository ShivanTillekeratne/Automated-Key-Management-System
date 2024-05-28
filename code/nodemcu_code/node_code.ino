// for com between node and ard 
#include <SoftwareSerial.h>  
SoftwareSerial ardesp(14, 12); 
String serial_data; 
byte x; 
 
//for wifi  
#include <ESP8266WiFi.h>  
#include <ESP8266HTTPClient.h>  
#include <WiFiClient.h>  
WiFiClient wifiClient; 
String wifidata[3] = {};   
 
// for led 
int states[12] = {}; 
int data = 13; //D7 
int clk = 4; //D1 
int latch = 5; //D2 
int pulse = 10; //clk pulse width for sr 
 
// for servo 
#include <Servo.h> 
Servo servo; 
int servo_signal = 0; //D3 
 
void setup() { 
  // for com between node and ard 
  ardesp.begin(9600); 
  Serial.begin(115200); 
 
  // for led 
  pinMode(data,OUTPUT); 
  pinMode(clk,OUTPUT); 
  pinMode(latch,OUTPUT); 
 
  // for servo 
  servo.attach(servo_signal); 
 
  // for wifi 
  WiFi.mode(WIFI_AP);  
  WiFi.softAP("NodeMCU", "");  
  delay(200);  
  while (WiFi.softAPgetStationNum() != 1) {  //loop here while no AP is connected to this station  
    Serial.print(".");  
    delay(100);  
  } 
} 
 
void loop() { 
  GetSerialData(); 
 
  // for led 
  if(serial_data == "led"){ 
    ardesp.println("OK"); 
    GetSerialData(); 
    StrToInt();  
    //add code to push states to shift registers  
    for (int i = 0; i < 12; i++) {  
      Serial.print(states[i]);  
    }  
    Serial.print("\n"); 
    push(); 
  } 
 
  // for wifi 
  if (serial_data == "wifi") { 
    ardesp.println("OK"); 
    GetSerialData(); 
    StrToStr(); 
    // add code to send data to pc 
    SendDataToPc(); 
  } 
 
  // for servo 
  if (serial_data == "servo") { 
    ardesp.println("OK"); 
    GetSerialData(); 
    // add code to rotate servo 
    if (serial_data == "up") { 
      Serial.println("servo up"); 
      servo.writeMicroseconds(2400); 
    } 
    if (serial_data == "down") { 
      Serial.println("servo down"); 
      servo.writeMicroseconds(500); 
    } 
  } 
} 
 
// for com between node and ard 
void GetSerialData() {  
  serial_data = "";  
  while (serial_data == "") {  
    while (ardesp.available() > 0) {  
      serial_data = ardesp.readStringUntil('\n');  
    }  
  } 
  serial_data.trim();  
} 
 
// for led 
void StrToInt() {  
  char* str = (char*)serial_data.c_str();  
  
  char* p = strtok(str, ",");  
  size_t index = 0;  
  
  while (p != nullptr && index < 12) {  
    states[index++] = atoi(p);  
    p = strtok(NULL, ",");  
  }  
  str = "";  
} 
void feed(int x) {  
  digitalWrite(clk, LOW);  
  digitalWrite(data, x);  
  delay(pulse);  
  digitalWrite(clk,HIGH); 
} 
void push() {  
  digitalWrite(latch, LOW);  
  int count = 0;  
  for (int i = 0; i < 16; i++) {  
    if (i < 2  i == 7  i == 15) {  
      feed(0);  
      continue;  
    }  
    feed(states[11 - count]);  
    count++;  
  }  
  digitalWrite(latch, HIGH);  
} 
 
// for wifi 
void StrToStr() {  
  char* str = (char*)serial_data.c_str();  
  
  char* p = strtok(str, ",");  
  size_t index = 0;  
  
  while (p != nullptr && index < 3) {  
    wifidata[index++] = p;  
    p = strtok(NULL, ",");  
  }  
  str = "";  
} 
void SendDataToPc() {  
  HTTPClient http;  
  String postData = "finger_id=" + wifidata[0] + "&key_num=" + wifidata[1] + "&keytaken=" + wifidata[2];  
  
  Serial.println(postData);  
  http.begin(wifiClient, "http://192.168.4.2/post-esp-data.php");       // Connect to host where MySQL database is hosted  
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //content-type header  
  int httpCode = http.POST(postData);  
  
  if (httpCode == 200) {  
    Serial.println("Values uploaded successfully.");  
    Serial.println(httpCode);  
    String webpage = http.getString();  // Get html webpage output and store it in a string  
    Serial.println(webpage + "\n");  
  } else {  
  
    Serial.println(httpCode);  
    Serial.println("Failed to upload values. \n");  
    http.end();  
    return;  
  }  
}