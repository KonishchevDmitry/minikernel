#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <types.h>

void keyboard_interrupt_handler(u8 data);
void keyboard_health_check();
bool keyboard_read(char* key);

#endif