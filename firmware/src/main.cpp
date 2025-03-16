#include <Arduino.h>

#include "DisplayMax7219.h"

#include "bitmaps.h"

#define CS_PIN      3
#define MOSI_PIN    0
#define SCK_PIN     2
#define BTN_PIN     4

#define DEFAULT_BRIGHTNESS 6
#define NUM_MODES          2

DisplayMax7219<CS_PIN, MOSI_PIN, SCK_PIN> disp;

void cycle_bitmap(const int period, const byte* bmaps, const byte bmaps_num);
void draw_bitmap(const byte* bmap);

byte mode = 0;
byte counter = 0;

void setup() {
    pinMode(BTN_PIN, INPUT_PULLUP);

    disp.begin(DEFAULT_BRIGHTNESS);
}

#define CYCLE_BITMAP(period_, bmaps_) cycle_bitmap(period_, bmaps_, sizeof(bmaps_) / disp.NUM_DIGITS)

void loop() {
    CYCLE_BITMAP(1000, HEART_BLINK);
    // CYCLE_BITMAP( 750, HEART_MINI);
    // CYCLE_BITMAP( 500, HEART_HALVES);
    // CYCLE_BITMAP( 500, HEART_MESSAGES);
}

void draw_bitmap(const byte* bmap) {
    for (byte i = 0; i < disp.NUM_DIGITS; i++) {
        disp.buf[i] = pgm_read_byte(&bmap[i]);
    }

    disp.update_all();
}

void cycle_bitmap(const int period, const byte* bmaps, const byte bmaps_num) {
    if (counter >= bmaps_num)
        counter = 0;

    draw_bitmap(bmaps + disp.NUM_DIGITS * counter++);

    delay(period);
}

