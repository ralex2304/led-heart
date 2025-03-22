#include <Arduino.h>

#include "DisplayMax7219.h"

#include "bitmaps.h"

const byte CS_PIN   = 3;
const byte MOSI_PIN = 0;
const byte SCK_PIN  = 2;
const byte BTN_PIN  = 4;

const byte DEFAULT_BRIGHTNESS = 1;

const int FPS = 60;

DisplayMax7219<CS_PIN, MOSI_PIN, SCK_PIN> disp;

void cycle_bitmap(const byte frames_per_cycle, const byte* bmaps, const byte bmaps_num);
void scroll_bitmap(const byte frames_per_cycle, const byte* bmap, const byte bmap_width);
void draw_bitmap(const byte* bmap);

byte counter = 0;
byte frame_counter = 0;

#define BITMAPS_AND_SIZE(bmaps_) bmaps_, (sizeof(bmaps_) / disp.NUM_DIGITS)
#define BITMAP_AND_WIDTH(bmap_)  bmap_, sizeof(bmap_)

int main() {
    init();

    pinMode(BTN_PIN, INPUT_PULLUP);

    disp.begin(DEFAULT_BRIGHTNESS);

    while (true) {
        static byte mode = 0;

        switch (mode) {
            case 0:     cycle_bitmap( 60, BITMAPS_AND_SIZE(HEART_BLINK));           break;
            case 1:     scroll_bitmap( 7, BITMAP_AND_WIDTH(FOR_SMBDY_WITH_LOVE));   break;
            case 2:     cycle_bitmap( 50, BITMAPS_AND_SIZE(HEART_MINI));            break;
            case 3:     cycle_bitmap( 30, BITMAPS_AND_SIZE(HEART_HALVES));          break;
            case 4:     cycle_bitmap( 30, BITMAPS_AND_SIZE(HEART_MESSAGES));        break;
            default:    mode = 0;                                                   break;
        }

        static bool btn_pressed = false;

        if (!btn_pressed && !digitalRead(BTN_PIN)) {
            delay(10);
            if (!digitalRead(BTN_PIN)) {
                mode++;
                btn_pressed = true;

                disp.clear();
                counter = 0;
                frame_counter = 0;
                continue;
            }
        } else if (btn_pressed && digitalRead(BTN_PIN)) {
            delay(10);
            if (digitalRead(BTN_PIN))
                btn_pressed = false;
        }

        delay(1000 / FPS);
    }
}

void draw_bitmap(const byte* bmap) {
    for (byte i = 0; i < disp.NUM_DIGITS; i++) {
        disp.buf[i] = pgm_read_byte(&bmap[i]);
    }

    disp.update_all();
}

void cycle_bitmap(const byte frames_per_cycle, const byte* bmaps, const byte bmaps_num) {
    if (counter >= bmaps_num)
        counter = 0;

    if (frame_counter++ >= frames_per_cycle) {
        frame_counter = 0;
        draw_bitmap(bmaps + disp.NUM_DIGITS * counter++);
    }
}

void scroll_bitmap(const byte frames_per_cycle, const byte* bmap, const byte bmap_width) {
    if (counter > bmap_width + disp.NUM_DIGITS)
        counter = 0;

    if (frame_counter++ >= frames_per_cycle) {
        frame_counter = 0;

        if (counter == 0) {
            counter++;
            return;
        }

        for (byte i = 0; i < disp.NUM_DIGITS - 1; i++) {
            disp.buf[i] = disp.buf[i + 1];
        }
        if (counter - 1 < bmap_width)
            disp.buf[disp.NUM_DIGITS - 1] = pgm_read_byte(&bmap[counter - 1]);
        else
            disp.buf[disp.NUM_DIGITS - 1] = 0x00;

        disp.update_all();
        counter++;
    }
}

