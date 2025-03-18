#include <Arduino.h>

#include "DisplayMax7219.h"

#include "bitmaps.h"

#define CS_PIN      3
#define MOSI_PIN    0
#define SCK_PIN     2
#define BTN_PIN     4

#define DEFAULT_BRIGHTNESS 6
#define NUM_MODES          2

#define FPS 30

DisplayMax7219<CS_PIN, MOSI_PIN, SCK_PIN> disp;

void cycle_bitmap(const int frames_per_cycle, const byte* bmaps, const byte bmaps_num);
void draw_bitmap(const byte* bmap);

byte mode = 0;
byte counter = 0;
byte frame_counter = 0;

void setup() {
    pinMode(BTN_PIN, INPUT_PULLUP);

    disp.begin(DEFAULT_BRIGHTNESS);
}

#define BITMAPS_AND_SIZE(bmaps_) bmaps_, (sizeof(bmaps_) / disp.NUM_DIGITS)

void loop() {
    cycle_bitmap(30, BITMAPS_AND_SIZE(HEART_BLINK));
    // cycle_bitmap(25, BITMAPS_AND_SIZE(HEART_MINI));
    // cycle_bitmap(15, BITMAPS_AND_SIZE(HEART_HALVES));
    // cycle_bitmap(15, BITMAPS_AND_SIZE(HEART_MESSAGES));

    delay(1000 / FPS);
}

void draw_bitmap(const byte* bmap) {
    for (byte i = 0; i < disp.NUM_DIGITS; i++) {
        disp.buf[i] = pgm_read_byte(&bmap[i]);
    }

    disp.update_all();
}

void cycle_bitmap(const int frames_per_cycle, const byte* bmaps, const byte bmaps_num) {
    if (counter >= bmaps_num)
        counter = 0;

    if (frame_counter++ >= frames_per_cycle) {
        frame_counter = 0;
        draw_bitmap(bmaps + disp.NUM_DIGITS * counter++);
    }
}

