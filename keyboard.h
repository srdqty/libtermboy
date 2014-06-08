#pragma once

#define K_ESC 0x01
#define K_A   0x1E
#define K_B   0x30
#define K_C   0x2E
#define K_D   0x20
#define K_E   0x12
#define K_F   0x21
#define K_G   0x22


void keyboard_listen(void);
void keyboard_register_press(int key, void *(*handler)(void *), void *args);
void keyboard_register_release(int key, void *(*handler)(void *), void *args);
int keyboard_pressed(int key);
int keyboard_rawmode(void);
int keyboard_restore(void);
