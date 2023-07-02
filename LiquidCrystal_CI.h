#pragma once
#include "Arduino.h"
#include <LiquidCrystal.h>
#ifndef ARDUINO_CI_COMPILATION_MOCKS
#define LiquidCrystal_CI LiquidCrystal
#else
#include <string>
#include <vector>

class LiquidCrystal_CI : public LiquidCrystal {
public:
  LiquidCrystal_CI(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1,
                   uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6,
                   uint8_t d7);
  LiquidCrystal_CI(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0,
                   uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5,
                   uint8_t d6, uint8_t d7);
  LiquidCrystal_CI(uint8_t rs, uint8_t rw, uint8_t enable, uint8_t d0,
                   uint8_t d1, uint8_t d2, uint8_t d3);
  LiquidCrystal_CI(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1,
                   uint8_t d2, uint8_t d3);
  ~LiquidCrystal_CI() { LiquidCrystal_CI::_instances[_rs_pin] = nullptr; }
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS);
  void clear();
  void home();
  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void leftToRight();
  void rightToLeft();
  void autoscroll();
  void noAutoscroll();
  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t);
  size_t write(uint8_t);
  size_t write(const char *buffer, size_t size);
  virtual String className() const { return "LiquidCrystal_CI"; }

  // testing methods
  static LiquidCrystal_CI *forRsPin(uint8_t rs) {
    return (LiquidCrystal_CI *)LiquidCrystal_CI::_instances[rs];
  }
  std::vector<String> getLines() { return _lines; }
  int getRows() { return _rows; }
  bool isAutoscroll() { return _autoscroll; }
  bool isBlink() { return _blink; }
  bool isCursor() { return _cursor; }
  bool isDisplay() { return _display; }
  byte *getCustomCharacter(uint8_t customChar) {
    return _customChars[customChar];
  }
  int getCursorCol() { return _col; }
  int getCursorRow() { return _row; }

private:
  static LiquidCrystal_CI *_instances[MOCK_PINS_COUNT];
  int _col, _cols, _row, _rows, _rs_pin;
  bool _display, _cursor, _blink, _autoscroll, _isInCreateChar;
  std::vector<String> _lines;
  byte _customChars[8][8];
  void init(uint8_t rs);
};

#endif
