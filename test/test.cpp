#include <Arduino.h>
#include <ArduinoUnitTests.h>
#include "Led.h"

#define LED_BUILTIN1 13
#define LED_BULTIN2 12
#define LED_BUILTIN3 11

unittest(blink)
{
    Led led;
    GodmodeState* state = GODMODE();
    bool values1[2];
    bool values2[2];
    bool values3[2];
    unsigned long start;
    start = micros();
    assertEqual(0, start);
    state->reset(); // clock and pins
    assertEqual(1, state->digitalPin[LED_BUILTIN1].historySize());
    assertEqual(1, state->digitalPin[LED_BUILTIN2].historySize());
    assertEqual(1, state->digitalPin[LED_BUILTIN3].historySize());
    led.setup();
    assertEqual(1, state->digitalPin[LED_BUILTIN1].historySize());
    assertEqual(1, state->digitalPin[LED_BUILTIN2].historySize());
    assertEqual(1, state->digitalPin[LED_BUILTIN3].historySize());
    led.loop();
    assertEqual(2, state->digitalPin[LED_BUILTIN1].historySize());
    assertEqual(2, state->digitalPin[LED_BUILTIN2].historySize());
    assertEqual(2, state->digitalPin[LED_BUILTIN3].historySize());
    assertEqual(1, values1[1]);
    assertEqual(1, values2[1]);
    assertEqual(1, values3[1]);
    
}

unittest_main()
