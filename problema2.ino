#include <Arduino.h>

//TODO: put your pin number here;
uint8_t led_pin = 5;

void turn_led_on(uint8_t led_pin) {
    //TODO: turn on the led
    digitalWrite(led_pin, HIGH);
}

void setup() {
    turn_led_on(led_pin);
}

void loop() {
}
