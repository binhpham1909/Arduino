//#include <wirish/wirish.h>
//#include "libraries/FreeRTOS/MapleFreeRTOS.h"
#include <MapleFreeRTOS821.h>
#define BOARD_LED_PIN PC13
boolean ledstt;
static void vLEDFlashTask(void *pvParameters) {
    pinMode(BOARD_LED_PIN, OUTPUT);
    for (;;) {
        digitalWrite(BOARD_LED_PIN, ledstt);
    }
}
static void vLEDFlashTaskTime(void *pvParameters) {
    for (;;) {
        vTaskDelay(1000);
        ledstt = LOW;
        vTaskDelay(50);
        ledstt = HIGH;
    }
}
void setup() {
    // initialize the digital pin as an output:
    xTaskCreate(vLEDFlashTask,
                "Task1",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    xTaskCreate(vLEDFlashTaskTime,
                "Task2",
                configMINIMAL_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY + 2,
                NULL);
    vTaskStartScheduler();
}

void loop() {
    // Insert background code here
}


