#pragma once

#include <stdbool.h>

// Alphabet keys
#define SM_KEY_A GLFW_KEY_A
#define SM_KEY_B GLFW_KEY_B
#define SM_KEY_C GLFW_KEY_C
#define SM_KEY_D GLFW_KEY_D
#define SM_KEY_E GLFW_KEY_E
#define SM_KEY_F GLFW_KEY_F
#define SM_KEY_G GLFW_KEY_G
#define SM_KEY_H GLFW_KEY_H
#define SM_KEY_I GLFW_KEY_I
#define SM_KEY_J GLFW_KEY_J
#define SM_KEY_K GLFW_KEY_K
#define SM_KEY_L GLFW_KEY_L
#define SM_KEY_M GLFW_KEY_M
#define SM_KEY_N GLFW_KEY_N
#define SM_KEY_O GLFW_KEY_O
#define SM_KEY_P GLFW_KEY_P
#define SM_KEY_Q GLFW_KEY_Q
#define SM_KEY_R GLFW_KEY_R
#define SM_KEY_S GLFW_KEY_S
#define SM_KEY_T GLFW_KEY_T
#define SM_KEY_U GLFW_KEY_U
#define SM_KEY_V GLFW_KEY_V
#define SM_KEY_W GLFW_KEY_W
#define SM_KEY_X GLFW_KEY_X
#define SM_KEY_Y GLFW_KEY_Y
#define SM_KEY_Z GLFW_KEY_Z

// Number keys
#define SM_KEY_0 GLFW_KEY_0
#define SM_KEY_1 GLFW_KEY_1
#define SM_KEY_2 GLFW_KEY_2
#define SM_KEY_3 GLFW_KEY_3
#define SM_KEY_4 GLFW_KEY_4
#define SM_KEY_5 GLFW_KEY_5
#define SM_KEY_6 GLFW_KEY_6
#define SM_KEY_7 GLFW_KEY_7
#define SM_KEY_8 GLFW_KEY_8
#define SM_KEY_9 GLFW_KEY_9

// Function keys
#define SM_KEY_F1  GLFW_KEY_F1
#define SM_KEY_F2  GLFW_KEY_F2
#define SM_KEY_F3  GLFW_KEY_F3
#define SM_KEY_F4  GLFW_KEY_F4
#define SM_KEY_F5  GLFW_KEY_F5
#define SM_KEY_F6  GLFW_KEY_F6
#define SM_KEY_F7  GLFW_KEY_F7
#define SM_KEY_F8  GLFW_KEY_F8
#define SM_KEY_F9  GLFW_KEY_F9
#define SM_KEY_F10 GLFW_KEY_F10
#define SM_KEY_F11 GLFW_KEY_F11
#define SM_KEY_F12 GLFW_KEY_F12

// Arrow keys
#define SM_KEY_UP    GLFW_KEY_UP
#define SM_KEY_DOWN  GLFW_KEY_DOWN
#define SM_KEY_LEFT  GLFW_KEY_LEFT
#define SM_KEY_RIGHT GLFW_KEY_RIGHT

// Modifier keys
#define SM_KEY_LEFT_SHIFT    GLFW_KEY_LEFT_SHIFT
#define SM_KEY_RIGHT_SHIFT   GLFW_KEY_RIGHT_SHIFT
#define SM_KEY_LEFT_CONTROL  GLFW_KEY_LEFT_CONTROL
#define SM_KEY_RIGHT_CONTROL GLFW_KEY_RIGHT_CONTROL
#define SM_KEY_LEFT_ALT      GLFW_KEY_LEFT_ALT
#define SM_KEY_RIGHT_ALT     GLFW_KEY_RIGHT_ALT
#define SM_KEY_LEFT_SUPER    GLFW_KEY_LEFT_SUPER
#define SM_KEY_RIGHT_SUPER   GLFW_KEY_RIGHT_SUPER

// Navigation keys
#define SM_KEY_HOME      GLFW_KEY_HOME
#define SM_KEY_END       GLFW_KEY_END
#define SM_KEY_PAGE_UP   GLFW_KEY_PAGE_UP
#define SM_KEY_PAGE_DOWN GLFW_KEY_PAGE_DOWN
#define SM_KEY_INSERT    GLFW_KEY_INSERT
#define SM_KEY_DELETE    GLFW_KEY_DELETE
#define SM_KEY_BACKSPACE GLFW_KEY_BACKSPACE
#define SM_KEY_TAB       GLFW_KEY_TAB
#define SM_KEY_ENTER     GLFW_KEY_ENTER
#define SM_KEY_ESCAPE    GLFW_KEY_ESCAPE

// Keypad keys
#define SM_KEY_KP_0        GLFW_KEY_KP_0
#define SM_KEY_KP_1        GLFW_KEY_KP_1
#define SM_KEY_KP_2        GLFW_KEY_KP_2
#define SM_KEY_KP_3        GLFW_KEY_KP_3
#define SM_KEY_KP_4        GLFW_KEY_KP_4
#define SM_KEY_KP_5        GLFW_KEY_KP_5
#define SM_KEY_KP_6        GLFW_KEY_KP_6
#define SM_KEY_KP_7        GLFW_KEY_KP_7
#define SM_KEY_KP_8        GLFW_KEY_KP_8
#define SM_KEY_KP_9        GLFW_KEY_KP_9
#define SM_KEY_KP_DECIMAL  GLFW_KEY_KP_DECIMAL
#define SM_KEY_KP_DIVIDE   GLFW_KEY_KP_DIVIDE
#define SM_KEY_KP_MULTIPLY GLFW_KEY_KP_MULTIPLY
#define SM_KEY_KP_SUBTRACT GLFW_KEY_KP_SUBTRACT
#define SM_KEY_KP_ADD      GLFW_KEY_KP_ADD
#define SM_KEY_KP_ENTER    GLFW_KEY_KP_ENTER
#define SM_KEY_KP_EQUAL    GLFW_KEY_KP_EQUAL

// Special character keys
#define SM_KEY_SPACE         GLFW_KEY_SPACE
#define SM_KEY_APOSTROPHE    GLFW_KEY_APOSTROPHE    // '
#define SM_KEY_COMMA         GLFW_KEY_COMMA         // ,
#define SM_KEY_MINUS         GLFW_KEY_MINUS         // -
#define SM_KEY_PERIOD        GLFW_KEY_PERIOD        // .
#define SM_KEY_SLASH         GLFW_KEY_SLASH         // /
#define SM_KEY_SEMICOLON     GLFW_KEY_SEMICOLON     // ;
#define SM_KEY_EQUAL         GLFW_KEY_EQUAL         // =
#define SM_KEY_LEFT_BRACKET  GLFW_KEY_LEFT_BRACKET  // [
#define SM_KEY_BACKSLASH     GLFW_KEY_BACKSLASH     // forward slash
#define SM_KEY_RIGHT_BRACKET GLFW_KEY_RIGHT_BRACKET // ]
#define SM_KEY_GRAVE_ACCENT  GLFW_KEY_GRAVE_ACCENT  // `

// Lock keys
#define SM_KEY_CAPS_LOCK    GLFW_KEY_CAPS_LOCK
#define SM_KEY_SCROLL_LOCK  GLFW_KEY_SCROLL_LOCK
#define SM_KEY_NUM_LOCK     GLFW_KEY_NUM_LOCK
#define SM_KEY_PRINT_SCREEN GLFW_KEY_PRINT_SCREEN
#define SM_KEY_PAUSE        GLFW_KEY_PAUSE

#define SM_MOUSE_BUTTON_LEFT   GLFW_MOUSE_BUTTON_LEFT
#define SM_MOUSE_BUTTON_RIGHT  GLFW_MOUSE_BUTTON_RIGHT
#define SM_MOUSE_BUTTON_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE

// Additional mouse buttons (many mice have these extra buttons)
#define SM_MOUSE_BUTTON_4 GLFW_MOUSE_BUTTON_4
#define SM_MOUSE_BUTTON_5 GLFW_MOUSE_BUTTON_5
#define SM_MOUSE_BUTTON_6 GLFW_MOUSE_BUTTON_6
#define SM_MOUSE_BUTTON_7 GLFW_MOUSE_BUTTON_7
#define SM_MOUSE_BUTTON_8 GLFW_MOUSE_BUTTON_8

// Mouse button count definition
#define SM_MOUSE_BUTTON_COUNT (GLFW_MOUSE_BUTTON_LAST + 1)

typedef int smKey;

// Only returns true on the first frame that a key is pressed
bool smInput_GetKeyDown(smKey key);
// Returns true if the key is pressed
bool smInput_GetKey(smKey key);

// Much like the key functions but with mouse input

bool smInput_GetMouseButtonDown(smKey mouseKey);
bool smInput_GetMouseButton(smKey mouseKey);

// These functions get the screen-space position of the mouse

int smInput_GetMouseInputHorizontal();
int smInput_GetMouseInputVertical();
