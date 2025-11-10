#include <Arduino.h>

#define BUZZER_PIN  37   // Buzzer
#define LED_PIN     39   // LED indikator

TaskHandle_t buzzerTaskHandle = NULL;
TaskHandle_t ledTaskHandle = NULL;

// BUZZER
void BuzzerTask(void *parameter) {
  (void)parameter;
  for (;;) {
    digitalWrite(BUZZER_PIN, HIGH);   // bunyi
    vTaskDelay(pdMS_TO_TICKS(300));   // ON 300 ms
    digitalWrite(BUZZER_PIN, LOW);    // diam
    vTaskDelay(pdMS_TO_TICKS(500));   // OFF 500 ms
  }
}

// LED
void LedTask(void *parameter) {
  (void)parameter;
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
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.println("ESP32 FreeRTOS - Buzzer");

  // 2 task untuk RTOS
  xTaskCreatePinnedToCore(
    BuzzerTask,      // fungsi task
    "BuzzerTask",    // nama task
    2048,            // ukuran stack (word)
    NULL,            // parameter
    1,               // prioritas
    &buzzerTaskHandle,
    0                // core 1
  );

  xTaskCreatePinnedToCore(
    LedTask,         // fungsi task
    "LedTask",       // nama task
    2048,            // ukuran stack
    NULL,            // parameter
    1,               // prioritas
    &ledTaskHandle,
    1                // core 0
  );
}

void loop() {
  // Kosong, karena semua kerja dilakukan oleh task
  vTaskDelay(pdMS_TO_TICKS(1000));
}