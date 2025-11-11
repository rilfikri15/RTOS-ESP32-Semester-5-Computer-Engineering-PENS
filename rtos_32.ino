#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

// ================= OLED =================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define SDA_PIN 13
#define SCL_PIN 14
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ================= SERVO =================
#define SERVO_PIN 20
Servo myServo;
int servoPos = 0;
bool servoDir = true;

// ================= STEPPER =================
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4
AccelStepper stepper(AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4);
bool stepDir = true;

// ================= POTENSIOMETER =================
#define POT_PIN 9

// ================= ROTARY ENCODER =================
#define CLK_PIN 16
#define DT_PIN 17
#define SW_PIN 18
int encoderPos = 0;
int lastCLK = HIGH;

// ================= BUTTON, LED, BUZZER =================
#define BUTTON_PIN 36
#define LED_PIN 39
#define BUZZER_PIN 37

// ================= TASK HANDLE =================
TaskHandle_t oledTaskHandle;
TaskHandle_t servoTaskHandle;
TaskHandle_t potTaskHandle;
TaskHandle_t encoderTaskHandle;
TaskHandle_t buttonTaskHandle;
TaskHandle_t ledTaskHandle;
TaskHandle_t buzzerTaskHandle;
TaskHandle_t stepperTaskHandle;

// ================= TASK 1: OLED =================
void OledTask(void *pvParameters) {
  for (;;) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 20);
    display.println("FreeRTOS Nyoba Multitasking :)");
    display.display();
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// ================= TASK 2: SERVO =================
void ServoTask(void *pvParameters) {
  for (;;) {
    myServo.write(servoPos);
    if (servoDir) servoPos += 5;
    else servoPos -= 5;

    if (servoPos >= 180) servoDir = false;
    if (servoPos <= 0) servoDir = true;

    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

// ================= TASK 3: POTENTIOMETER =================
void PotTask(void *pvParameters) {
  for (;;) {
    int potValue = analogRead(POT_PIN);
    Serial.print("[POT] Nilai ADC: ");
    Serial.println(potValue);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// ================= TASK 4: ROTARY ENCODER =================
void EncoderTask(void *pvParameters) {
  for (;;) {
    int clkState = digitalRead(CLK_PIN);
    if (clkState != lastCLK && clkState == LOW) {
      if (digitalRead(DT_PIN) != clkState)
        encoderPos++;
      else
        encoderPos--;
      Serial.print("[ENCODER] Posisi: ");
      Serial.println(encoderPos);
    }
    lastCLK = clkState;
    if (digitalRead(SW_PIN) == LOW) {
      encoderPos = 0;
      Serial.println("[ENCODER] Direset ke 0");
      vTaskDelay(pdMS_TO_TICKS(300));
    }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// ================= TASK 5: BUTTON =================
void ButtonTask(void *pvParameters) {
  for (;;) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      Serial.println("[BUTTON] Ditekan!");
      vTaskDelay(pdMS_TO_TICKS(300)); // debounce
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ================= TASK 6: LED =================
void LedTask(void *pvParameters) {
  for (;;) {
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// ================= TASK 7: BUZZER =================
void BuzzerTask(void *pvParameters) {
  for (;;) {
    digitalWrite(BUZZER_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(1000)); // buzzer terus menyala
  }
}

// ================= TASK 8: STEPPER =================
void StepperTask(void *pvParameters) {
  stepper.setMaxSpeed(500);
  stepper.setAcceleration(200);
  for (;;) {
    if (stepDir)
      stepper.moveTo(200);
    else
      stepper.moveTo(0);

    stepper.run();
    if (stepper.distanceToGo() == 0) stepDir = !stepDir;

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Gagal inisialisasi OLED!");
    for (;;);
  }
  display.clearDisplay();
  display.display();

  // I/O
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP);

  // Servo
  myServo.attach(SERVO_PIN);
  // Stepper
  stepper.setSpeed(200);

  // ================= BUAT SEMUA TASK =================
  xTaskCreatePinnedToCore(OledTask, "OLED", 4096, NULL, 1, &oledTaskHandle, 0);
  xTaskCreatePinnedToCore(ServoTask, "Servo", 2048, NULL, 1, &servoTaskHandle, 1);
  xTaskCreatePinnedToCore(PotTask, "Pot", 2048, NULL, 1, &potTaskHandle, 0);
  xTaskCreatePinnedToCore(EncoderTask, "Encoder", 2048, NULL, 1, &encoderTaskHandle, 1);
  xTaskCreatePinnedToCore(ButtonTask, "Button", 2048, NULL, 1, &buttonTaskHandle, 0);
  xTaskCreatePinnedToCore(LedTask, "LED", 2048, NULL, 1, &ledTaskHandle, 1);
  xTaskCreatePinnedToCore(BuzzerTask, "Buzzer", 2048, NULL, 1, &buzzerTaskHandle, 0);
  xTaskCreatePinnedToCore(StepperTask, "Stepper", 4096, NULL, 1, &stepperTaskHandle, 1);

  Serial.println("=== FreeRTOS 8 Task Started ===");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}