#include <Arduino.h>

#define POT_PIN 9    // Potentiometer pin
#define LED_PIN 39    // LED indicator

TaskHandle_t potTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;

// Potentiometer
void PotTask(void *parameter) {
  (void)parameter;

  for (;;) {
    int potValue = analogRead(POT_PIN);        // Baca nilai ADC (0–4095)
    float voltage = (potValue / 4095.0) * 3.3; // Konversi ke volt (ESP32-S3 ADC 12-bit, 3.3V ref)

    Serial.print("Nilai ADC: ");
    Serial.print(potValue);
    Serial.print(" | Tegangan: ");
    Serial.print(voltage, 2);
    Serial.println(" V");

    vTaskDelay(pdMS_TO_TICKS(500)); // Baca setiap 0.5 detik
  }
}

// LED
void LedTask(void *parameter) {
  (void)parameter;
  pinMode(LED_PIN, OUTPUT);

  for (;;) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(200));
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(800));
  }
}

// SETUP 
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("ESP32-S3 FreeRTOS: Potentiometer on GPIO9");

  pinMode(POT_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Task 1: Potentiometer reader on core 0
  xTaskCreatePinnedToCore(
    PotTask,         // Fungsi task
    "PotTask",       // Nama task
    4096,            // Stack size (bytes)
    NULL,            // Parameter
    1,               // Prioritas
    &potTaskHandle,  // Handle task
    0                // Jalankan di core 0
  );

  // Task 2: LED blinker on core 1
  xTaskCreatePinnedToCore(
    LedTask,
    "LedTask",
    2048,
    NULL,
    1,
    &ledTaskHandle,
    1 // Jalankan di core 1
  );
}

// -------------------- LOOP --------------------
void loop() {
  // Kosong karena semua dikerjakan oleh FreeRTOS
  vTaskDelay(pdMS_TO_TICKS(1000));
}