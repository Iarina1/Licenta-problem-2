#include <Arduino.h>
#include "Led.h"

void Led::setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN1, OUTPUT);
  pinMode(LED_BUILTIN2, OUTPUT);
  pinMode(LED_BUILTIN3, OUTPUT);
}

void Led::loop() {
  digitalWrite(LED_BUILTIN1, HIGH);
  digitalWrite(LED_BUILTIN2, HIGH);
  digitalWrite(LED_BUILTIN3, HIGH);
}
