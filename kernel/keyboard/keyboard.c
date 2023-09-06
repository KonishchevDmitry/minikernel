// See https://wiki.osdev.org/Keyboard for details

#include "../textio.h"

#include "keyboard.h"
#include "keys.h"

enum Response {
    RESPONSE_ERROR_1 = 0x00,
    RESPONSE_ERROR_2 = 0xFF,

    RESPONSE_COMMAND_ACK = 0xFA,
    RESPONSE_COMMAND_RESEND = 0xFE,

    RESPONSE_ECHO_RESPONSE = 0xEE,

    RESPONSE_SELF_TEST_OK = 0xAA,
    RESPONSE_SELF_TEST_FAIL_1 = 0xFC,
    RESPONSE_SELF_TEST_FAIL_2 = 0xFD,
};

enum ScanCode {
    CODE_EXTENDED_KEY = 0xE0,
    CODE_KEY_RELEASED = 0xF0,
};

typedef enum {
    STATE_NONE,
    STATE_KEY_RELEASED,

    STATE_EXTENDED_KEY,
    STATE_EXTENDED_KEY_RELEASED,
} State;

static State STATE = STATE_NONE;
static bool SHIFT_PRESSED = false;

static volatile register_t INPUT = 0;
static volatile register_t TOTAL_ERRORS = 0;
static volatile register_t TOTAL_UNEXPECTED_RESPONSES = 0;

#include "../isr_start.h"
    void keyboard_interrupt_handler(u8 data) {
        switch(data) {
            case RESPONSE_ERROR_1:
            case RESPONSE_ERROR_2:
                TOTAL_ERRORS++;
                return;

            case RESPONSE_COMMAND_ACK:
            case RESPONSE_COMMAND_RESEND:
            case RESPONSE_ECHO_RESPONSE:
            case RESPONSE_SELF_TEST_OK:
            case RESPONSE_SELF_TEST_FAIL_1:
            case RESPONSE_SELF_TEST_FAIL_2:
                TOTAL_UNEXPECTED_RESPONSES++;
                return;
        }

        switch(data) {
            case CODE_EXTENDED_KEY:
                if(STATE == STATE_NONE) {
                    STATE = STATE_EXTENDED_KEY;
                } else {
                    TOTAL_UNEXPECTED_RESPONSES++;
                    STATE = STATE_NONE;
                }
                return;

            case CODE_KEY_RELEASED:
                switch(STATE) {
                    case STATE_NONE:
                        STATE = STATE_KEY_RELEASED;
                        break;

                    case STATE_EXTENDED_KEY:
                        STATE = STATE_EXTENDED_KEY_RELEASED;
                        break;

                    default:
                        TOTAL_UNEXPECTED_RESPONSES++;
                        STATE = STATE_NONE;
                        break;
                }
                return;
        }

        bool extended = false;
        bool released = false;

        switch(STATE) {
            case STATE_NONE:
                break;

            case STATE_KEY_RELEASED:
                released = true;
                break;

            case STATE_EXTENDED_KEY:
                extended = true;
                break;

            case STATE_EXTENDED_KEY_RELEASED:
                extended = true;
                released = true;
                break;

            default:
                TOTAL_ERRORS++;
                STATE = STATE_NONE;
                return;
        }

        STATE = STATE_NONE;

        // Unsupported key
        if(extended || data >= sizeof KEY_TABLE / sizeof KEY_TABLE[0]) {
            return;
        }

        Key key = KEY_TABLE[data];

        if(key == KEY_LEFT_SHIFT || key == KEY_RIGHT_SHIFT) {
            SHIFT_PRESSED = !released;
            return;
        } if(released || key >= SPECIAL_KEY_START) {
            return;
        }

        if(SHIFT_PRESSED && key >= 'a' && key <= 'z') {
            key -= 'a' - 'A';
        }

        INPUT = key;
    }
#include "../isr_end.h"

static size_t REPORTED_ERRORS = 0;
static size_t REPORTED_UNEXPECTED_RESPONSES = 0;

void keyboard_health_check() {
    size_t errors = TOTAL_ERRORS;
    if(REPORTED_ERRORS != errors) {
        printlnf("Got %d errors from keyboard controller.", errors - REPORTED_ERRORS);
        REPORTED_ERRORS = errors;
    }

    size_t unexpected_responses = TOTAL_UNEXPECTED_RESPONSES;
    if(REPORTED_UNEXPECTED_RESPONSES != unexpected_responses) {
        printlnf("Got %d unexpected responses from keyboard controller.", unexpected_responses - REPORTED_ERRORS);
        REPORTED_UNEXPECTED_RESPONSES = unexpected_responses;
    }
}

bool keyboard_read(char* key) {
    char input = INPUT;
    if(!input) {
        return false;
    }

    INPUT = 0;
    *key = input;
    return true;
}