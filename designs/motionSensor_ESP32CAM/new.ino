#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include "esp_camera.h"
#include "./config/config.h" // Файл конфигурации с SERVER_URL и BOT_TOKEN

#define PIR_PIN 16 // Подключение OUT датчика SR602 к GPIO16

WiFiClientSecure client;
HTTPClient http;
UniversalTelegramBot bot(BOT_TOKEN, client);

void connectToWiFi() {
  Serial.print("Подключение к Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi подключен");
}

void sendDataToServer(bool detected, String photoBase64) {
  StaticJsonDocument<500> jsonDoc;
  jsonDoc["data"]["message"]["status"] = detected ? 1 : 0;
  jsonDoc["data"]["message"]["code"] = "motion_detected";
  jsonDoc["data"]["message"]["message"] = detected ? "Обнаружено движение" : "Нет движения";
  jsonDoc["data"]["photo"] = photoBase64;

  String jsonData;
  serializeJson(jsonDoc, jsonData);

  http.begin(client, SERVER_URL);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(jsonData);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.print("Ответ сервера: ");
    Serial.println(response);
  } else {
    Serial.print("Ошибка отправки данных. Код ошибки: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

void sendTelegramMessageWithPhoto(String message, String photoBase64) {
  if (bot.sendPhotoByBinary(CHAT_ID, "image/jpeg", (uint8_t*)photoBase64.c_str(), photoBase64.length(), "", message)) {
    Serial.println("Фото отправлено в Telegram");
  } else {
    Serial.println("Ошибка отправки фото в Telegram");
  }
}

String capturePhoto() {
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Ошибка получения кадра");
    return "";
  }

  // Конвертация изображения в base64
  String photoBase64 = "";
  for (size_t i = 0; i < fb->len; i++) {
    photoBase64 += String((char)fb->buf[i]);
  }

  esp_camera_fb_return(fb); // Освобождение буфера
  return photoBase64;
}

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  client.setInsecure();  // Отключаем проверку сертификата
  pinMode(PIR_PIN, INPUT);  // Установка пина PIR как вход
  
  // Настройка камеры
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Ошибка инициализации камеры: 0x%x", err);
    return;
  }
}

void loop() {
  int motionDetected = digitalRead(PIR_PIN);

  if (motionDetected == HIGH) {
    Serial.println("Движение обнаружено! Отправляем уведомления.");
    
    // Делаем фото
    String photoBase64 = capturePhoto();
    if (photoBase64 != "") {
      sendTelegramMessageWithPhoto("Обнаружено движение", photoBase64);
      sendDataToServer(true, photoBase64);
    } else {
      Serial.println("Не удалось сделать фото.");
    }
    
    delay(5000);  // Задержка для предотвращения многократных срабатываний
  } else {
    sendDataToServer(false, "");
  }
  delay(1000);
}
