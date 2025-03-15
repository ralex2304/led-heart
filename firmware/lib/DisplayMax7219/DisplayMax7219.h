#include <Arduino.h>
#include <avr/io.h>

template<byte CS, byte DATA, byte CLK>
class DisplayMax7219 {
    public:
        static constexpr byte NUM_DIGITS = 8;
        byte buf[NUM_DIGITS] = {};

        void begin(byte brightness) {
            pinMode(CS,   OUTPUT);
            pinMode(DATA, OUTPUT);
            pinMode(CLK,  OUTPUT);

            send_cmd(DISP_TEST,   0x00);
            send_cmd(DECODE_MODE, 0x00);
            send_cmd(SCAN_LIMIT,  0x0f);
            send_cmd(SHUTDOWN,    0x01);

            clear();
            set_brightness(brightness);
        }

        void set_brightness(byte brightness) const {
            send_cmd(INTENSITY, brightness);
        }

        void clear() {
            for (byte i = 0; i < NUM_DIGITS; i++) {
                buf[i] = 0;
                update_digit(i);
            }
        }

        void update_digit(byte num) const {
            send_cmd(DIGIT_0 + num, buf[num]);
        }

        void update_all() const {
            for (byte i = 0; i < NUM_DIGITS; i++) {
                update_digit(i);
            }
        }

    private:

        void send_cmd(byte addr, byte val) const {
            digitalWrite(CS, LOW);

            send_byte(addr);
            send_byte(val);

            digitalWrite(CS, HIGH);
        }

        void send_byte(byte val) const {
            for (byte i = 0; i < 8; i++) {
                digitalWrite(DATA, val & (1 << 7));
                val <<= 1;
                digitalWrite(CLK, HIGH);
                digitalWrite(CLK, LOW);
            }
        }

        enum ADDR: byte {
            NOP         = 0x00,
            DIGIT_0     = 0x01,
            DIGIT_1     = 0x02,
            DIGIT_2     = 0x03,
            DIGIT_3     = 0x04,
            DIGIT_4     = 0x05,
            DIGIT_5     = 0x06,
            DIGIT_6     = 0x07,
            DIGIT_7     = 0x08,
            DECODE_MODE = 0x09,
            INTENSITY   = 0x0A,
            SCAN_LIMIT  = 0x0B,
            SHUTDOWN    = 0x0C,
            DISP_TEST   = 0x0F,
        };
};

