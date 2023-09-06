// See for details:
// * https://wiki.osdev.org/%228042%22_PS/2_Controller
// * http://www.brokenthorn.com/Resources/OSDev19.html

#include "interrupts.h"
#include "keyboard/keyboard.h"
#include "misc.h"
#include "pic.h"
#include "pit.h"
#include "ps2.h"
#include "textio.h"

enum Port {
    STATUS_PORT  = 0x64,
    COMMAND_PORT = 0x64,
    DATA_PORT    = 0x60,
};

enum Status {
    STATUS_OUT_FULL = 1 << 0,
    STATUS_IN_FULL  = 1 << 1,
};

enum Command {
    CMD_READ_CONFIG  = 0x20,
    CMD_WRITE_CONFIG = 0x60,

    CMD_ENABLE_FIRST_PORT   = 0xAE,
    CMD_DISABLE_FIRST_PORT  = 0xAD,
    CMD_DISABLE_SECOND_PORT = 0xA7,

    CMD_TEST_CONTROLLER = 0xAA,
    CMD_TEST_FIRST_PORT = 0xAB,

    CMD_SYSTEM_RESET = 0xFE,
};

enum Configuration {
    CFG_FIRST_PORT_INTERRUPT   = 1 << 0,
    CFG_SECOND_PORT_INTERRUPT  = 1 << 1,
    CFG_FIRST_PORT_TRANSLATION = 1 << 6,
};

#include "isr_start.h"
    static void interrupt_handler(irq_t) {
        keyboard_interrupt_handler(inb(DATA_PORT));
    }
#include "isr_end.h"

static error __must_check wait(bool in) {
    time_ms_t deadline = uptime() + 100;

    do {
        u8 status = inb(STATUS_PORT);

        if(in) {
            if(!(status & STATUS_IN_FULL)) {
                return NULL;
            }
        } else {
            if(status & STATUS_OUT_FULL) {
                return NULL;
            }
        }
    } while(uptime() < deadline);

    return "PS/2 communication timeout";
}

static error __must_check send_command(u8 command) {
    error err;

    if((err = wait(true))) {
        return err;
    }
    outb(COMMAND_PORT, command);

    return NULL;
}

static error __must_check read_data(u8* data) {
    error err;

    if((err = wait(false))) {
        return err;
    }
    *data = inb(DATA_PORT);

    return NULL;
}

static error __must_check write_data(u8 data) {
    error err;

    if((err = wait(true))) {
        return err;
    }
    outb(DATA_PORT, data);

    return NULL;
}

void system_reset() {
    printf("The system will reset in:");
    for(int i = 5; i > 0; i--) {
        printf(" %d..", i);
        sleep(1000);
    }

    error err = send_command(CMD_SYSTEM_RESET);
    if(!err) {
        err = "PS/2 controller has ignored the request";
    }

    panic("\nFailed to reset the system: %s.", err);
}

error __must_check configure_keyboard() {
    u8 result;
    error err;

    // Disable both ports and flush output buffer

    if((err = send_command(CMD_DISABLE_FIRST_PORT))) {
        return err;
    }
    if((err = send_command(CMD_DISABLE_SECOND_PORT))) {
        return err;
    }
    inb(DATA_PORT);

    // Configure the controller

    u8 config;

    if((err = send_command(CMD_READ_CONFIG))) {
        return err;
    }
    if((err = read_data(&config))) {
        return err;
    }

    config &= ~(CFG_FIRST_PORT_INTERRUPT | CFG_SECOND_PORT_INTERRUPT | CFG_FIRST_PORT_TRANSLATION);

    if((err = send_command(CMD_WRITE_CONFIG))) {
        return err;
    }
    if((err = write_data(config))) {
        return err;
    }

    // Perform controller self-test

    if((err = send_command(CMD_TEST_CONTROLLER))) {
        return err;
    }
    if((err = read_data(&result))) {
        return err;
    }
    if(result != 0x55) {
        return "PS/2 controller has failed self-test";
    }

    // Test first PS/2 port

    if((err = send_command(CMD_TEST_FIRST_PORT))) {
        return err;
    }
    if((err = read_data(&result))) {
        return err;
    }
    if(result != 0) {
        printlnf("Attention: Keyboard is not connected.");
        return NULL;
    }

    // Enable the keyboard

    if((err = set_interrupt_handler(PIC_KEYBOARD_IRQ, interrupt_handler))) {
        return err;
    }

    config |= CFG_FIRST_PORT_INTERRUPT;

    if((err = send_command(CMD_WRITE_CONFIG))) {
        return err;
    }
    if((err = write_data(config))) {
        return err;
    }

    if((err = send_command(CMD_ENABLE_FIRST_PORT))) {
        return err;
    }

    return NULL;
}