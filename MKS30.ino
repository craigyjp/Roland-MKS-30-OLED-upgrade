// EXAMPLE 7 SEGMENT DISPLAY RENDERING FOR ADAFRUIT_GFX & SSD1306
// Copyright 2018 T&A Laboratories LLC
// writen by: Aaron Williams on 9/7/2018
//
// Do not distribute without permission of the author
// !!!!!!!

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RoxMux.h>

#define OLED_RESET 11
Adafruit_SSD1306 display(OLED_RESET);


#define SELECT1 5
#define SELECT2 6

uint8_t pinStates[1];
int digit;
int digit1;
int digit2;
int prevdigit1;
int prevdigit2;

// define segment truth table for each digit
static const int digit_array[41][8] = {
  {1, 1, 1, 1, 1, 1, 0, 0},  // 0
  {0, 1, 1, 0, 0, 0, 0, 0},  // 1
  {1, 1, 0, 1, 1, 0, 1, 0},  // 2
  {1, 1, 1, 1, 0, 0, 1, 0},  // 3
  {0, 1, 1, 0, 0, 1, 1, 0},  // 4
  {1, 0, 1, 1, 0, 1, 1, 0},  // 5
  {1, 0, 1, 1, 1, 1, 1, 0},  // 6
  {1, 1, 1, 0, 0, 0, 0, 0},  // 7
  {1, 1, 1, 1, 1, 1, 1, 0},  // 8
  {1, 1, 1, 0, 0, 1, 1, 0},  // 9 - 10
  {0, 0, 0, 0, 0, 0, 1, 0},   // -
  {0, 0, 0, 0, 0, 0, 0, 0},   // blank
  {1, 0, 0, 1, 1, 1, 0, 0},   // C
  {0, 1, 1, 0, 1, 1, 1, 0},   // H
  {0, 1, 1, 0, 0, 0, 0, 1},   // 1.
  {1, 1, 0, 1, 1, 0, 1, 1},   // 2.
  {1, 1, 1, 1, 0, 0, 1, 1},   // 3.
  {0, 1, 1, 0, 0, 1, 1, 1},   // 4.
  {1, 0, 1, 1, 0, 1, 1, 1},   // 5.
  {1, 0, 1, 1, 1, 1, 1, 1},   // 6. - 20
  {1, 1, 1, 0, 0, 0, 0, 1},  // 7.
  {1, 1, 1, 1, 1, 1, 1, 1},  // 8.
  {1, 1, 1, 0, 0, 1, 1, 1},  // 9.
  {1, 1, 0, 0, 1, 1, 1, 0},  // P
  {0, 0, 1, 0, 1, 0, 1, 0},  // n
  {0, 0, 1, 0, 0, 0, 0, 0},  // |
  {0, 0, 1, 1, 1, 0, 1, 0},  // o
  {1, 1, 1, 0, 1, 1, 0, 0},  // M
  {0, 0, 1, 1, 1, 1, 1, 0},  // b
  {0, 0, 1, 0, 1, 1, 0, 0},  // value 52
  {0, 0, 0, 0, 1, 0, 0, 0},  // value 16
  {0, 1, 1, 0, 1, 0, 0, 0},  // value 22
  {0, 0, 1, 0, 1, 0, 0, 0},  // value 20
  {0, 0, 0, 0, 1, 1, 0, 0},  // value 48
  {1, 1, 0, 0, 0, 1, 1, 0},  // value 99
  {0, 0, 0, 1, 1, 1, 1, 0},   // t
  {1, 0, 0, 1, 0, 0, 0, 0},   // =
  {0, 0, 0, 0, 1, 0, 1, 0},   // r
  {1, 0, 0, 1, 1, 1, 1, 0},   // E
  {1, 0, 1, 1, 1, 1, 0, 0},   // G
  {0, 1, 1, 1, 1, 0, 1, 0},   // d

};

Rox74HC165 <1> mux;
#define PIN_DATA  2 // pin 9 on 74HC165 (DATA)
#define PIN_LOAD  3 // pin 1 on 74HC165 (LOAD)
#define PIN_CLK   4 // pin 2 on 74HC165 (CLK))

void setup()
{

  pinMode(SELECT1, INPUT);
  pinMode(SELECT2, INPUT);
  Wire.setClock(100000);
  Serial.begin(115200);
  mux.begin(PIN_DATA, PIN_LOAD, PIN_CLK);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();   // Clear the buffer.
  // func params: render_digits(pox_x, pos_y, spacing, WHITE)
  render_digits(8, 4, 24, 88, WHITE);
  display.display();
  delay(50);
  display.clearDisplay();   // Clear the buffer.
  display.display();
}

void loop()
{

  if (digitalRead(SELECT1)  == 1)
  {
    mux.update();
    for (uint8_t i = 0; i < 1; i++) {
      // read all pins from the mux specified by "i"
      uint8_t data = mux.read(i);
      // compare the mux states to the last saved states
      //      if (data != pinStates[i]) {
      // if they change update the last states and print their values
      pinStates[i] = data;
//      Serial.print("Data 1 ");
//      Serial.println(data);
      convertData(data);
      // printStates(i);
      //display.clearDisplay();   // Clear the buffer.
      render_digit(30, 4, prevdigit1, BLACK);
      render_digit(30, 4, digit, WHITE);
      prevdigit1 = digit;
      display.display();
      //     }
    }
  }

  if (digitalRead(SELECT2) == 1)
  {
    mux.update();
    for (uint8_t i = 0; i < 1; i++) {
      // read all pins from the mux specified by "i"
      uint8_t data = mux.read(i);
      // compare the mux states to the last saved states
      //     if (data != pinStates[i]) {
      // if they change update the last states and print their values
      pinStates[i] = data;
//      Serial.print("Data 2 ");
//      Serial.println(data);
      convertData(data);
      // printStates(i);
      //        display.clearDisplay();   // Clear the buffer.
      render_digit(72, 4, prevdigit2, BLACK);
      render_digit(72, 4, digit, WHITE);
      prevdigit2 = digit;
      display.display();
      //     }
    }
  }
}

void convertData(uint8_t data)
{

  switch (data)
  {
    case 6:
      digit = 1;
      return;
    case 91:
      digit = 2;
      return;
    case 79:
      digit = 3;
      return;
    case 102:
      digit = 4;
      return;
    case 109:
      digit = 5;
      return;
    case 125:
      digit = 6;
      return;
    case 7:
      digit = 7;
      return;
    case 127:
      digit = 8;
      return;
    case 111:
      digit = 9;
      return;
    case 63:
      digit = 0;
      return;
    case 64:
      digit = 10;
      return;
    case 0:
      digit = 11;
      return;
    case 57:
      digit = 12;
      return;
    case 118:
      digit = 13;
      return;
    case 134:  // 1.
      digit = 14;
      return;
    case 219:  // 2.
      digit = 15;
      return;
    case 207: // 3.
      digit = 16;
      return;
    case 230: // 4.
      digit = 17;
      return;
    case 237: // 5.
      digit = 18;
      return;
    case 253: // 6.
      digit = 19;
      return;
    case 135: // 7.
      digit = 20;
      return;
    case 255: // 8.
      digit = 21;
      return;
    case 254: // 9.
      digit = 22;
      return;
    case 115: // P
      digit = 23;
      return;
    case 84: // n
      digit = 24;
      return;
    case 4: // |
      digit = 25;
      return;
    case 92: // o
      digit = 26;
      return;
    case 55: // M
      digit = 27;
      return;
    case 124: // b
      digit = 28;
      return;
    case 52:
      digit = 29;
      return;
    case 16:
      digit = 30;
      return;
    case 22:
      digit = 31;
      return;
    case 20:
      digit = 32;
      return;
    case 48: //
      digit = 33;
      return;
    case 99: //
      digit = 34;
      return;
    case 120: // t
      digit = 35;
      return;
    case 9: // =
      digit = 36;
      return;
    case 80: // =
      digit = 37; // r
      return;
    case 121: // E
      digit = 38;
      return;
    case 61: // G
      digit = 39;
      return;
    case 94: // d
      digit = 40; 
      return;
  }
}

void printStates(uint8_t n) {
  Serial.print("Mux: ");
  Serial.println(n);

  for (uint8_t i = 0; i < 8; i++) {
    Serial.print(bitRead(pinStates[n], i));
  }
  Serial.println("");
  Serial.println("------------");
}

void render_digits(uint8_t pos_x, uint8_t pos_y,
                   uint8_t spacing_x, uint16_t digit, uint8_t color) {
  uint8_t digit_array[] = {digit / 100 % 10, digit / 10 % 10, digit % 10};
  if (digit > 99) {
    render_digit(pos_x, pos_y, digit_array[0], color);
  }
  if (digit > 9) {
    render_digit(pos_x + spacing_x, pos_y, digit_array[1], color);
  }
  render_digit(pos_x + (spacing_x * 2.8), pos_y, digit_array[2], color);
}

// RENDER DIGIT
// don't use this unless you only need a single digit
// use render_digits() func above instead
void render_digit(uint8_t pos_x, uint8_t pos_y,
                  uint8_t digit, uint8_t color) {
  // loop through 7 segments
  for (uint8_t i = 0; i < 8; i++) {
    bool seg_on = digit_array[digit][i];
    // if seg_on is true draw segment
    if (seg_on) {
      switch (i) {
        case 0:
          display.fillRoundRect(2 + pos_x, 0 + pos_y, 22, 3, 2, color); // SEG a
          break;
        case 1:
          display.fillRoundRect(22 + pos_x, 2 + pos_y, 5, 9, 2, color); // SEG b
          break;
        case 2:
          display.fillRoundRect(22 + pos_x, 12 + pos_y, 5, 9, 2, color); // SEG c
          break;
        case 3:
          display.fillRoundRect(2 + pos_x, 20 + pos_y, 22, 3, 2, color); // SEG d
          break;
        case 4:
          display.fillRoundRect(0 + pos_x, 12 + pos_y, 5, 9, 2, color); // SEG e
          break;
        case 5:
          display.fillRoundRect(0 + pos_x, 2 + pos_y, 5, 9, 2, color); // SEG f
          break;
        case 6:
          display.fillRoundRect(2 + pos_x, 10 + pos_y, 22, 3, 2, color); // SEG g
          break;
        case 7:
          display.fillRoundRect(30 + pos_x, 18 + pos_y, 5, 3, 4, color); // DP
          break;
      }
      seg_on = false;
    }
  }
}
