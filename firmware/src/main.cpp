#include <Arduino.h>

#include "DisplayMax7219.h"

#define CS_PIN      3
#define MOSI_PIN    0
#define SCK_PIN     2
#define BTN_PIN     4

#define DEFAULT_BRIGHTNESS 6
#define NUM_MODES          2

DisplayMax7219<CS_PIN, MOSI_PIN, SCK_PIN> disp;

const byte HEART_CYCLE[] PROGMEM = {
    0x00, 0x18, 0x24, 0x44, 0x88, 0x44, 0x24, 0x18,
    0x00, 0x0c, 0x12, 0x22, 0x44, 0x22, 0x12, 0x0c,
    0x06, 0x09, 0x11, 0x22, 0x11, 0x09, 0x06, 0x00,
    0x0c, 0x12, 0x22, 0x44, 0x22, 0x12, 0x0c, 0x00,
};

void cycle_bitmap(const byte* bmaps, const byte bmaps_num);
void draw_bitmap(const byte* bmap);

byte mode = 0;
byte counter = 0;

void setup() {
    pinMode(BTN_PIN, INPUT_PULLUP);

    disp.begin(DEFAULT_BRIGHTNESS);
}

void loop() {
    cycle_bitmap(HEART_CYCLE, sizeof(HEART_CYCLE) / disp.NUM_DIGITS);
}

void draw_bitmap(const byte* bmap) {
    for (byte i = 0; i < disp.NUM_DIGITS; i++) {
        disp.buf[i] = pgm_read_byte(&bmap[i]);
    }

    disp.update_all();
}

void cycle_bitmap(const byte* bmaps, const byte bmaps_num) {
    if (counter >= bmaps_num)
        counter = 0;

    draw_bitmap(bmaps + disp.NUM_DIGITS * counter++);

    delay(750);
}

