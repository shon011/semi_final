#include "esp_camera.h"
#include <WiFi.h>
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "FS.h"
#include "SD_MMC.h"
#include <HardwareSerial.h>
#include <HTTPClient.h>


const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASS";
String server = "http://ip_addr/semi_final/insert.php";
HardwareSerial gsm(1);

#define trigpin 3
#define echopin 4
#define ledpin 5
#define buzzer 6

log duration;
int distance;
String status;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  gsm.begin(9600,2,3);
  digitalWrite(trigpin, LOW);
  delay(2);
  digitalWrite(trigpin, HIGH);
  delay(10);
  digitalWrite(trigpin, LOW);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }

  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Frame capture failed");
    return;
  }
  Serial.printf("Captured %u bytes\n", fb->len);
  esp_camera_fb_return(fb);
}

void loop() {
  duration = pulseIn(echopin, HIGH);
  distance = duration * 0.034/2;
  Serial.print("distance = ");
  Serial.print(distance);
  Serial.print("status");
  Serial.println(status);
  if (distance <= 10){
    beep();
    sendSMS();
    makeCall();
    digitalWrite(ledpin, HIGH);
    status = "warning";
  }else{
    digitalWrite(ledpin, LOW);
    status = "safe";
  }
}
  void sendSMS(){
    gsm.println("AT+CMGF=1");
    delay(500);
    gsm.println("CMGS=\"+25076545\"");
    delay(500);
    gsm.println("motion detected");
    Serial.println("message sent");
    delay(500);
    gsm.write(26);
  }
  void makeCall(){
    Serial.println("calling...");
    gsm.println("ATD+250787984");
    delay(15000);
    gsm.println("ATH");
    Serial.println("call ended")
  }
  void beep(){
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
  }
  void captureAndSend(){
    camera_fb_t*fb=esp_camera_fb_get();
    if(!fb){
      Serial.println("camera failed");
      return;
    }
    HTTPClient http;
    String url = "http://semi_final/upload.php";
    http.begin(url);
    http.addHeader("Comtent-Type","image/jpeg");
    int responseCode = http.POST(fb->buf,fb->len);
    if(responseCode > 0){
      Serial.print("response code = ");
      Serial.println(responseCode);
    }
    else{
      Serial.print("response code = ");
      Serial.println(responseCode);
    }
    esp_camera_fb_return(fb);
  }
  {
    /* code */


  delay(10000);
}

#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void handleCapture() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  server.sendHeader("Content-Type", "image/jpeg");
  server.sendHeader("Content-Length", String(fb->len));
  server.send(200, "image/jpeg", "");
  WiFiClient client = server.client();
  client.write(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // camera init same as example 1
  // ...
  server.on("/capture", HTTP_GET, handleCapture);
  server.begin();
}

void loop() {
  server.handleClient();
}
#include "esp_camera.h"


void setup() {
  Serial.begin(115200);
  SD_MMC.begin();
  // camera init same as example 1
  // ...
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    File file = SD_MMC.open("/photo.jpg", FILE_WRITE);
    if (file) {
      file.write(fb->buf, fb->len);
      file.close();
      Serial.println("Saved photo.jpg");
    }
    esp_camera_fb_return(fb);
  }
}

void loop() {
  delay(10000);
}


WebServer server(80);

String streamResponse() {
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
  return s;
}

void handleStream() {
  WiFiClient client = server.client();
  client.write(streamResponse().c_str());
  while (true) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) break;
    client.printf("--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.write("\r\n");
    esp_camera_fb_return(fb);
    delay(100);
  }
}

void setup() {
  // init WiFi + camera...
  server.on("/stream", HTTP_GET, handleStream);
  server.begin();
}