#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "DHT.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= PIN DEFINITIONS =================
#define DHTPIN 4
#define DHTTYPE DHT22

#define BUTTON_PIN 15
#define RELAY_PIN 23
#define BUZZER_PIN 19
#define LED_PIN 16
#define POT_PIN 34

// ================= GLOBAL VARIABLES =================
DHT dht(DHTPIN, DHTTYPE);

volatile bool emergencyStop = false;
float temperature = 0;
float filteredTemp = 0;
int potValue = 0;
float hvacSetpoint = 30.0;

SemaphoreHandle_t mutex;

// ================= DEBOUNCE =================
volatile unsigned long lastInterruptTime = 0;

// ================= BUTTON ISR =================
void IRAM_ATTR buttonISR() {
    unsigned long now = esp_timer_get_time() / 1000;

    if (now - lastInterruptTime > 200) {
        emergencyStop = true;
        lastInterruptTime = now;
    }
}

// ================= TASK HANDLES =================
TaskHandle_t sensorHandle = NULL;
TaskHandle_t controlHandle = NULL;
TaskHandle_t logHandle = NULL;

// ================= SENSOR TASK =================
float lastTemp = NAN;

void sensorTask(void *pvParameters) {
    while (1) {

        esp_task_wdt_reset();

        float temp = dht.readTemperature();
        int adc = analogRead(POT_PIN);

        bool anomaly = false;

        if (isnan(temp) || temp < -40 || temp > 100) anomaly = true;
        else if (!isnan(lastTemp) && abs(temp - lastTemp) > 15.0) anomaly = true;

        if (anomaly) {
            emergencyStop = true;
            Serial.println("Sensor anomaly detected!");
        } else {
            lastTemp = temp;

            filteredTemp = 0.8 * filteredTemp + 0.2 * temp;
            float newSetpoint = (adc / 4095.0) * 50.0;

            xSemaphoreTake(mutex, portMAX_DELAY);
            temperature = filteredTemp;
            hvacSetpoint = newSetpoint;
            potValue = adc;
            xSemaphoreGive(mutex);
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// ================= CONTROL TASK =================
void controlTask(void *pvParameters) {
    while (1) {

        esp_task_wdt_reset();

        xSemaphoreTake(mutex, portMAX_DELAY);
        float temp = temperature;
        float setpoint = hvacSetpoint;
        bool estop = emergencyStop;
        xSemaphoreGive(mutex);

        //  EMERGENCY STOP 
        if (estop) {
            digitalWrite(RELAY_PIN, LOW);
            digitalWrite(LED_PIN, HIGH);
            tone(BUZZER_PIN, 2500);   // alarm tone
        }

        // NORMAL OPERATION 
        else {

            // TEMPERATURE ALARM
            if (temp >= 35.0) {
                digitalWrite(LED_PIN, HIGH);
                tone(BUZZER_PIN, 2000);
            } else {
                digitalWrite(LED_PIN, LOW);
                noTone(BUZZER_PIN);
            }

            // RELAY CONTROL
            digitalWrite(RELAY_PIN, temp < setpoint ? HIGH : LOW);
        }

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}

//  LOG + LCD TASK 
void logTask(void *pvParameters) {
    while (1) {

        esp_task_wdt_reset();

        xSemaphoreTake(mutex, portMAX_DELAY);
        float temp = temperature;
        float setpoint = hvacSetpoint;
        int pot = potValue;
        bool estop = emergencyStop;
        xSemaphoreGive(mutex);

        // SERIAL OUTPUT
        Serial.print("Temp: ");
        Serial.print(temp);
        Serial.print(" | Setpoint: ");
        Serial.print(setpoint);
        Serial.print(" | ADC: ");
        Serial.print(pot);
        Serial.print(" | E-Stop: ");
        Serial.println(estop ? "ON" : "OFF");

        // LCD OUTPUT
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("T:");
        lcd.print(temp, 1);
        lcd.print(" S:");
        lcd.print(setpoint, 0);

        lcd.setCursor(0, 1);

        if (estop) {
            lcd.print("!!! EMERGENCY !!!");
        } else if (temp >= 35.0) {
            lcd.print("!!! HOT !!!");
        } else {
            lcd.print("ADC:");
            lcd.print(pot);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

//  SETUP 
void setup() {
    Serial.begin(115200);

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_PIN, OUTPUT);

    digitalWrite(RELAY_PIN, LOW);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

    dht.begin();

    // LCD INIT
    Wire.begin(21, 22);
    lcd.init();
    lcd.backlight();

    lcd.setCursor(0, 0);
    lcd.print("System Booting");

    mutex = xSemaphoreCreateMutex();
    filteredTemp = dht.readTemperature();

    // WATCHDOG
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 5000,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true
    };

    esp_task_wdt_init(&wdt_config);

    // TASKS
    xTaskCreate(sensorTask, "Sensor Task", 2048, NULL, 2, &sensorHandle);
    xTaskCreate(controlTask, "Control Task", 2048, NULL, 3, &controlHandle);
    xTaskCreate(logTask, "Log Task", 4096, NULL, 1, &logHandle);

    esp_task_wdt_add(sensorHandle);
    esp_task_wdt_add(controlHandle);
    esp_task_wdt_add(logHandle);
}

// ================= LOOP =================
void loop() {
    // FreeRTOS handles everything
}