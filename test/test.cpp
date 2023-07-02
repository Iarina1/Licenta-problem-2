#include <bitset>
#include <iostream>
#include <vector>

#include "ArduinoUnitTests.h"
#include "ci/ObservableDataStream.h"

#include "LiquidCrystal_CI.h"

const byte rs = 1;
const byte rw = 2;
const byte enable = 3;
const byte d0 = 10;
const byte d1 = 11;
const byte d2 = 12;
const byte d3 = 13;
const byte d4 = 14;
const byte d5 = 15;
const byte d6 = 16;
const byte d7 = 17;

class BitCollector : public DataStreamObserver {
private:
  bool fourBitMode;
  bool showData;
  vector<int> pinLog;
  GodmodeState *state;

public:
  BitCollector(bool showData = false, bool fourBitMode = true)
      : DataStreamObserver(false, false) {
    this->fourBitMode = fourBitMode;
    this->showData = showData;
    state = GODMODE();
    state->reset();
    state->digitalPin[enable].addObserver("lcd", this);
  }

  ~BitCollector() { state->digitalPin[enable].removeObserver("lcd"); }

  virtual void onBit(bool aBit) {
    if (aBit) {
      int value = 0;
      value = (value << 1) + state->digitalPin[rs];
      value = (value << 1) + state->digitalPin[rw];
      value = (value << 1) + state->digitalPin[d7];
      value = (value << 1) + state->digitalPin[d6];
      value = (value << 1) + state->digitalPin[d5];
      value = (value << 1) + state->digitalPin[d4];
      value = (value << 1) + state->digitalPin[d3];
      value = (value << 1) + state->digitalPin[d2];
      value = (value << 1) + state->digitalPin[d1];
      value = (value << 1) + state->digitalPin[d0];
      pinLog.push_back(value);
      if (showData) {
        std::cout.width(5);
        std::cout << std::right << value << " : " << ((value >> 9) & 1) << "  "
                  << ((value >> 8) & 1) << "  ";
        if (fourBitMode) {
          std::bitset<4> bits((value >> 4) & 0x0F);
          if ((pinLog.size() - 1) % 2) {
            std::cout << "    ";
          }
          std::cout << bits;
        } else {
          std::bitset<8> bits(value & 0xFF);
          std::cout << bits;
        }
        std::cout << std::endl;
      }
    }
  }

  bool isEqualTo(const vector<int> &expected) {
    if (pinLog.size() != expected.size()) {
      return false;
    }
    for (int i = 0; i < pinLog.size(); ++i) {
      if (pinLog.at(i) != expected.at(i)) {
        return false;
      }
    }
    return true;
  }

  virtual String observerName() const { return "BitCollector"; }
};

// we don't look at the pins here, just verify that we can call the constructors
unittest(constructors) {
  LiquidCrystal_CI lcd1(rs, enable, d4, d5, d6, d7);
  LiquidCrystal_CI lcd2(rs, rw, enable, d4, d5, d6, d7);
  LiquidCrystal_CI lcd3(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  LiquidCrystal_CI lcd4(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  LiquidCrystal_CI *lcd5 = new LiquidCrystal_CI(rs, enable, d4, d5, d6, d7);
  LiquidCrystal_CI *lcd6 =
      new LiquidCrystal_CI(rs, rw, enable, d4, d5, d6, d7);
  LiquidCrystal_CI *lcd7 =
      new LiquidCrystal_CI(rs, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  LiquidCrystal_CI *lcd8 =
      new LiquidCrystal_CI(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
  assertNotNull(lcd5);
  assertNotNull(lcd6);
  assertNotNull(lcd7);
  assertNotNull(lcd8);
  delete lcd8;
  delete lcd7;
  delete lcd6;
  delete lcd5;
}

/*     rs rw  d7 to d0
   48 : 0  0  00110000      set to 8-bit mode (takes three tries)
   48 : 0  0  00110000      set to 8-bit mode
   48 : 0  0  00110000      set to 8-bit mode
   32 : 0  0  00100000      set to 4-bit mode, 1 line, 8-bit font
   32 : 0  0  0010          \
    0 : 0  0      0000       set to 4-bit mode, 1 line, 8-bit font
    0 : 0  0  0000          \
  192 : 0  0      1100       display on, cursor off, blink off
    0 : 0  0  0000          \
  016 : 0  0      0001       clear display
    0 : 0  0  0000          \
   96 : 0  0      0110       increment cursor position, no display shift
*/
unittest(init) {
  vector<int> expected{48, 48, 48, 32, 32, 0, 0, 192, 0, 16, 0, 96};
  BitCollector pinValues(false); // test the next line (a constructor)
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   48 : 0  0  00110000      set to 8-bit mode (takes three tries)
   48 : 0  0  00110000      set to 8-bit mode
   48 : 0  0  00110000      set to 8-bit mode
   32 : 0  0  00100000      set to 4-bit mode, 1 line, 8-bit font
   32 : 0  0  0010          \
  128 : 0  0      1000       set to 4-bit mode, 2 lines, 8-bit font
    0 : 0  0  0000          \
  192 : 0  0      1100       display on, cursor off, blink off
    0 : 0  0  0000          \
  016 : 0  0      0001       clear display
    0 : 0  0  0000          \
   96 : 0  0      0110       increment cursor position, no display shift
*/
unittest(begin) {
  vector<int> expected{48, 48, 48, 32, 32, 128, 0, 192, 0, 16, 0, 96};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  BitCollector pinValues(false); // test the next line
  lcd.begin(16, 2);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    208 : 0  0      1101  00001101 = display on, cursor blink on
*/
unittest(blink) {
  vector<int> expected{0, 208};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.blink();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    192 : 0  0      1100  00001100 = display on, cursor blink off
*/
unittest(noBlink) {
  vector<int> expected{0, 192};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noBlink();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    224 : 0  0      1110  00001110 = display on, cursor on
*/
unittest(cursor) {
  vector<int> expected{0, 224};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.cursor();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
      0 : 0  0  0000
    192 : 0  0      1100  00001100 = display on, cursor off
*/
unittest(noCursor) {
  vector<int> expected{0, 192};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noCursor();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   64 : 0  0  0100
    0 : 0  0      0000
  512 : 1  0  0000
  512 : 1  0      0000
  528 : 1  0  0001
  528 : 1  0      0001
  512 : 1  0  0000
  512 : 1  0      0000
  512 : 1  0  0000
  512 : 1  0      0000
  528 : 1  0  0001
  528 : 1  0      0001
  512 : 1  0  0000
  736 : 1  0      1110
  512 : 1  0  0000
  512 : 1  0      0000
  512 : 1  0  0000
  512 : 1  0      0000
*/
unittest(createChar) {
  vector<int> expected{64,  0,   512, 512, 528, 528, 512, 512, 512,
                       512, 528, 528, 512, 736, 512, 512, 512, 512};
  byte smiley[8] = {B00000, B10001, B00000, B00000,
                    B10001, B01110, B00000, B00000};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  BitCollector pinValues(false); // test the next line
  lcd.createChar(0, smiley);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000          \
   16 : 0  0      0001       clear
*/
unittest(clear) {
  vector<int> expected{0, 16};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.clear();
  assertTrue(pinValues.isEqualTo(expected));
}

/*      rs rw  d7 to d0
    576 : 1  0  0100      \
    640 : 1  0      1000  0x48 H
    608 : 1  0  0110      \
    592 : 1  0      0101  0x65 e
    608 : 1  0  0110      \
    704 : 1  0      1100  0x6C l
    608 : 1  0  0110      \
    704 : 1  0      1100  0x6C l
    608 : 1  0  0110      \
    752 : 1  0      1111  0x6F o
*/
unittest(print_hello) {
  vector<int> expected{576, 640, 608, 592, 608, 704, 608, 704, 608, 752};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.print("Hello");
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   16 : 0  0  0001      \
  128 : 0  0      1000   00011000 = shift display left
*/
unittest(scrollDisplayLeft) {
  vector<int> expected{16, 128};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.scrollDisplayLeft();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
   16 : 0  0  0001      first half of command
  192 : 0  0      1100  full command: 00011100 = shift display right
*/
unittest(scrollDisplayRight) {
  vector<int> expected{16, 192};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.scrollDisplayRight();
  assertTrue(pinValues.isEqualTo(expected));
}

unittest(failIfWrongSize) {
  vector<int> expected{0};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.clear();
  assertFalse(pinValues.isEqualTo(expected));
}

unittest(failIfWrongValues) {
  vector<int> expected{0, 255};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.clear();
  assertFalse(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   32 : 0  0      0010
*/
unittest(home) {
  vector<int> expected{0, 32};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.home();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   96 : 0  0      0110  => left to right
*/
unittest(leftToRight) {
  vector<int> expected{0, 96};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.leftToRight();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   64 : 0  0      0100  => right to left
*/
unittest(rightToLeft) {
  vector<int> expected{0, 64};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.rightToLeft();
  assertTrue(pinValues.isEqualTo(expected));
}

/*       rs rw  d7 to d0
      0 : 0  0  0000      \
    192 : 0  0      1100   00001100 = turns on LCD display
*/
unittest(display) {
  vector<int> expected{0, 192};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.display();
  assertTrue(pinValues.isEqualTo(expected));
}

/*      rs rw  d7 to d0
    0 :  0  0  0000      \
  128 :  0  0      1000    00001000 = turns off LCD display
*/
unittest(noDisplay) {
  vector<int> expected{0, 128};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noDisplay();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
  112 : 0  0      0111
*/
unittest(autoscroll) {
  vector<int> expected{0, 112};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.autoscroll();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
    0 : 0  0  0000
   96 : 0  0      0110
*/
unittest(noAutoscroll) {
  vector<int> expected{0, 96};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.noAutoscroll();
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
  128 : 0  0  1000      \
    0 : 0  0      0000  full command: 10000000 = set cursor (0,0)
  128 : 0  0  1000      \
   16 : 0  0      0001  full command: 10000001 = set cursor (1,0)
  128 : 0  0  1000      \
   32 : 0  0      0010  full command: 10000010 = set cursor (2,0)
  128 : 0  0  1000      \
   48 : 0  0      0011  full command: 10000011 = set cursor (3,0)
  128 : 0  0  1000      \
   64 : 0  0      0100  full command: 10000100 = set cursor (4,0)
  128 : 0  0  1000      \
   80 : 0  0      0101  full command: 10000101 = set cursor (5,0)
  128 : 0  0  1000      \
   96 : 0  0      0110  full command: 10000110 = set cursor (6,0)
  128 : 0  0  1000      \
  112 : 0  0      0111  full command: 10000111 = set cursor (7,0)
  128 : 0  0  1000      \
  128 : 0  0      1000  full command: 10001000 = set cursor (8,0)
  128 : 0  0  1000      \
  144 : 0  0      1001  full command: 10001001 = set cursor (9,0)
  128 : 0  0  1000      \
  160 : 0  0      1010  full command: 10001010 = set cursor (10,0)
  128 : 0  0  1000      \
  176 : 0  0      1011  full command: 10001011 = set cursor (11,0)
  128 : 0  0  1000      \
  192 : 0  0      1100  full command: 10001100 = set cursor (12,0)
  128 : 0  0  1000      \
  208 : 0  0      1101  full command: 10001101 = set cursor (13,0)
  128 : 0  0  1000      \
  224 : 0  0      1110  full command: 10001110 = set cursor (14,0)
  128 : 0  0  1000      \
  240 : 0  0      1111  full command: 10001111 = set cursor (15,0)

  192 : 0  0  1100      \
    0 : 0  0      0000  full command: 11000000 = set cursor (0,1)
  192 : 0  0  1100      \
   16 : 0  0      0001  full command: 11000001 = set cursor (1,1)
  192 : 0  0  1100      \
   32 : 0  0      0010  full command: 11000010 = set cursor (2,1)
  192 : 0  0  1100      \
   48 : 0  0      0011  full command: 11000011 = set cursor (3,1)
  192 : 0  0  1100      \
   64 : 0  0      0100  full command: 11000100 = set cursor (4,1)
  192 : 0  0  1100      \
   80 : 0  0      0101  full command: 11000101 = set cursor (5,1)
  192 : 0  0  1100      \
   96 : 0  0      0110  full command: 11000110 = set cursor (6,1)
  192 : 0  0  1100      \
  112 : 0  0      0111  full command: 11000111 = set cursor (7,1)
  192 : 0  0  1100      \
  128 : 0  0      1000  full command: 11001000 = set cursor (8,1)
  192 : 0  0  1100      \
  144 : 0  0      1001  full command: 11001001 = set cursor (9,1)
  192 : 0  0  1100      \
  160 : 0  0      1010  full command: 11001010 = set cursor (10,1)
  192 : 0  0  1100      \
  176 : 0  0      1011  full command: 11001011 = set cursor (11,1)
  192 : 0  0  1100      \
  192 : 0  0      1100  full command: 11001100 = set cursor (12,1)
  192 : 0  0  1100      \
  208 : 0  0      1101  full command: 11001101 = set cursor (13,1)
  192 : 0  0  1100      \
  224 : 0  0      1110  full command: 11001110 = set cursor (14,1)
  192 : 0  0  1100      \
  240 : 0  0      1111  full command: 11001111 = set cursor (15,1)
 */
unittest(setCursor) {
  vector<int> expected{
      128, 0,   128, 16,  128, 32,  128, 48,  128, 64,  128, 80,  128,
      96,  128, 112, 128, 128, 128, 144, 128, 160, 128, 176, 128, 192,
      128, 208, 128, 224, 128, 240, 192, 0,   192, 16,  192, 32,  192,
      48,  192, 64,  192, 80,  192, 96,  192, 112, 192, 128, 192, 144,
      192, 160, 192, 176, 192, 192, 192, 208, 192, 224, 192, 240,
  };
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  // top row
  lcd.setCursor(0, 0);
  lcd.setCursor(1, 0);
  lcd.setCursor(2, 0);
  lcd.setCursor(3, 0);
  lcd.setCursor(4, 0);
  lcd.setCursor(5, 0);
  lcd.setCursor(6, 0);
  lcd.setCursor(7, 0);
  lcd.setCursor(8, 0);
  lcd.setCursor(9, 0);
  lcd.setCursor(10, 0);
  lcd.setCursor(11, 0);
  lcd.setCursor(12, 0);
  lcd.setCursor(13, 0);
  lcd.setCursor(14, 0);
  lcd.setCursor(15, 0);
  // bottom row
  lcd.setCursor(0, 1);
  lcd.setCursor(1, 1);
  lcd.setCursor(2, 1);
  lcd.setCursor(3, 1);
  lcd.setCursor(4, 1);
  lcd.setCursor(5, 1);
  lcd.setCursor(6, 1);
  lcd.setCursor(7, 1);
  lcd.setCursor(8, 1);
  lcd.setCursor(9, 1);
  lcd.setCursor(10, 1);
  lcd.setCursor(11, 1);
  lcd.setCursor(12, 1);
  lcd.setCursor(13, 1);
  lcd.setCursor(14, 1);
  lcd.setCursor(15, 1);
  assertTrue(pinValues.isEqualTo(expected));
}

/*     rs rw  d7 to d0
  576 : 1  0  0100
  528 : 1  0      0001  0x41
  624 : 1  0  0111
  592 : 1  0      0101  0x75
  624 : 1  0  0111
  560 : 1  0      0011  0x73
  624 : 1  0  0111
  576 : 1  0      0100  0x74
  608 : 1  0  0110
  656 : 1  0      1001  0x69
  608 : 1  0  0110
  736 : 1  0      1110  0x6E
*/
unittest(write) {
  vector<int> expected{576, 528, 624, 592, 624, 560,
                       624, 576, 608, 656, 608, 736};
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  BitCollector pinValues(false); // test the next line
  lcd.write('A');
  lcd.write('u');
  lcd.write('s');
  lcd.write('t');
  lcd.write('i');
  lcd.write('n');
  assertTrue(pinValues.isEqualTo(expected));
}

unittest(getRows) {
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  assertEqual(1, lcd.getRows());
  lcd.begin(16, 2);
  assertEqual(2, lcd.getRows());
}

// based on Autoscroll.ino example
unittest(autoscroll_high) {
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  assertFalse(lcd.isAutoscroll());
  lcd.begin(16, 2);
  // get currently displayed lines
  std::vector<String> lines;
  // set the cursor to (0,0):
  lcd.setCursor(0, 0);
  // print from 0 to 9:
  for (int thisChar = 0; thisChar < 10; thisChar++) {
    lcd.print(thisChar);
    delay(500);
  }
  // verify display not empty
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  // uncomment when print works
  assertEqual(10, lines.at(0).length());
  assertEqual("0123456789", lines.at(0));
  assertEqual(0, lines.at(1).length());

  // set the cursor to (16,1):
  lcd.setCursor(16, 1);
  // set the display to automatically scroll:
  lcd.autoscroll();
  assertTrue(lcd.isAutoscroll());
  // print from 0 to 9:
  String zeroTo9 = "0123456789";
  String text = "                ";

  for (int i = 0; i < 10; i++) {
    // send character to screen
    lcd.print(i);

    // calculate expected value for second line
    String expected = text.substr(0, 15 - i);
    expected += zeroTo9;
    expected = expected.substr(0, 16);

    // compare expected to actual
    lines = lcd.getLines();
    assertEqual(16, lines.at(1).length());
    assertEqual(expected, lines.at(1));

    delay(500);
  }
  // turn off automatic scrolling
  lcd.noAutoscroll();
  assertFalse(lcd.isAutoscroll());

  // clear screen for the next loop:
  lcd.clear();
}

unittest(clear_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  // get currently displayed lines
  std::vector<String> lines = lcd.getLines();
  // verify that display contains 1 empty line
  assertEqual(1, lines.size());
  assertEqual(0, lines.at(0).length());

  // reset lcd to have two lines
  lcd.begin(16, 2);
  // verify that begin clears the display
  lines = lcd.getLines();
  // verify that display contains 2 empty lines
  assertEqual(2, lines.size());
  assertEqual(0, lines.at(0).length());
  assertEqual(0, lines.at(1).length());

  // write something to display
  lcd.print("hello world");

  // verify display not empty
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(11, lines.at(0).length());
  assertEqual("hello world", lines.at(0));
  assertEqual(0, lines.at(1).length());

  // clear display
  lcd.clear();

  // verify display is empty
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(0, lines.at(0).length());
  assertEqual(0, lines.at(1).length());
}

unittest(createChar_high) {
  // Setup display
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);

  // create some chars
  byte smiley[8] = {B00000, B10001, B00000, B00000,
                    B10001, B01110, B00000, B00000};
  byte spaceship[8] = {B00000, B00100, B01110, B01110,
                       B01110, B01010, B00000, B00000};
  lcd.createChar(0, smiley);
  lcd.createChar(4, spaceship);

  // check they were created where we expected them
  byte *character0 = lcd.getCustomCharacter(0);
  byte *character4 = lcd.getCustomCharacter(4);
  for (int bite = 0; bite < 8; bite++) {
    assertEqual(smiley[bite], *(character0 + bite));
    assertEqual(spaceship[bite], *(character4 + bite));
  }
}

unittest(write_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  // get currently displayed lines
  std::vector<String> lines = lcd.getLines();
  // verify that display contains 2 empty lines
  assertEqual(2, lines.size());
  assertEqual(0, lines.at(0).length());
  assertEqual(0, lines.at(1).length());

  // Write stuff
  lcd.write('A');

  // Testing one character
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(1, lines.at(0).length());
  assertEqual("A", lines.at(0));
  assertEqual('A', lines.at(0).at(0));
  assertEqual(0, lines.at(1).length());

  // testing multiple character inputs
  lcd.write('u');
  lcd.write('s');
  lcd.write('t');
  lcd.write('i');
  lcd.write('n');
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(6, lines.at(0).length());
  assertEqual("Austin", lines.at(0));
}

unittest(print_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  lcd.print(F("ABCD"));
  lcd.setCursor(13, 1);
  lcd.print(F("XYZ"));
  std::vector<String> lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(4, lines.at(0).length());
  assertEqual("ABCD", lines.at(0));
  assertEqual(16, lines.at(1).length());
  assertEqual("             XYZ", lines.at(1));
}

unittest(setCursor_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);

  // reset lcd to have two lines
  lcd.begin(16, 2);

  // verify cursor is at beginning
  assertEqual(0, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());

  lcd.print("Line0");
  assertEqual(5, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());

  // set cursor to second line
  lcd.setCursor(0, 1);
  // verify cursor position
  assertEqual(0, lcd.getCursorCol());
  assertEqual(1, lcd.getCursorRow());

  lcd.print("Line1");
  assertEqual(5, lcd.getCursorCol());
  assertEqual(1, lcd.getCursorRow());

  // set cursor to middle of first line
  lcd.setCursor(4, 0);
  // verify cursor position
  assertEqual(4, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());
  lcd.write('X');
  assertEqual(5, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());

  lcd.setCursor(8, 0);
  lcd.write('Y');

  std::vector<String> lines = lcd.getLines();
  assertEqual("LineX   Y", lines.at(0));
  assertEqual("Line1", lines.at(1));
}

unittest(home_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);

  // Go home
  lcd.home();
  // Check Cursor is at upper-left
  assertEqual(0, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());

  // Set Cursor to new location
  lcd.setCursor(3, 1);
  // Check Cursor Location
  assertEqual(3, lcd.getCursorCol());
  assertEqual(1, lcd.getCursorRow());

  // Return Home
  lcd.home();
  // Check Cursor is at upper-left
  assertEqual(0, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());

  // Print message
  lcd.print("Test line");
  // Check Cursor Location
  assertNotEqual(0, lcd.getCursorCol());
  ;

  // Return Home
  lcd.home();
  // Check Cursor is at upper-left
  assertEqual(0, lcd.getCursorCol());
  assertEqual(0, lcd.getCursorRow());
}

unittest(display_high) {
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);

  bool isDisplay = lcd.isDisplay();

  // default
  assertEqual(0, isDisplay);

  // test is display on
  lcd.display();
  isDisplay = lcd.isDisplay();
  assertEqual(1, isDisplay);

  // test is display off
  lcd.noDisplay();
  isDisplay = lcd.isDisplay();
  assertEqual(0, isDisplay);
}

unittest(blink_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);

  bool blinking = lcd.isBlink();

  // check default - noBlink
  assertFalse(blinking);

  // check blink function
  lcd.blink();
  blinking = lcd.isBlink();
  assertTrue(blinking);

  // check noBlink function
  lcd.noBlink();
  blinking = lcd.isBlink();
  assertFalse(blinking);
}

unittest(cursor_high) {
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);

  // assert startup no cursor is set
  assertEqual(false, lcd.isCursor());

  // check cursor function
  lcd.cursor();
  assertEqual(true, lcd.isCursor());

  // check noCursor function
  lcd.noCursor();
  assertEqual(false, lcd.isCursor());
}

unittest(printLines_high) {

  std::vector<String> lines;
  // create lcd object
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  // reset lcd to have two lines
  lcd.begin(16, 2);

  // Test C String
  lcd.print("C String");
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(8, lines.at(0).length());
  assertEqual("C String", lines.at(0));
  assertEqual(0, lines.at(1).length());
  lcd.clear();

  // Test String
  lcd.setCursor(0, 0);
  lcd.print(String("Test String"));
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(11, lines.at(0).length());
  assertEqual("Test String", lines.at(0));
  assertEqual(0, lines.at(1).length());
  lcd.clear();

  // Test INT
  lcd.setCursor(0, 0);
  lcd.print(String(100, DEC));
  lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(3, lines.at(0).length());
  assertEqual("100", lines.at(0));
  assertEqual(0, lines.at(1).length());
  lcd.clear();
}

unittest(createChar_and_print) {
  LiquidCrystal_CI lcd(rs, enable, d4, d5, d6, d7);
  lcd.begin(16, 2);
  uint8_t OL1[8] = {0b00000, 0b00000, 0b00001, 0b00011,
                    0b00011, 0b00111, 0b00111, 0b00110};
  lcd.createChar(0, OL1);
  lcd.clear();
  lcd.print(F("ABC"));
  std::vector<String> lines = lcd.getLines();
  assertEqual(2, lines.size());
  assertEqual(3, lines.at(0).length());
  assertEqual(0, lines.at(1).length());
}

unittest_main()
