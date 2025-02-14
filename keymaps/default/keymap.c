#include QMK_KEYBOARD_H

    /*
     * ┌───┬───────┬───┐
     * │key│  key  │key│
     * ├───┼───────┼───┤
     * │key│display│ * │
     * └───┴───────┴───┘
     */

enum custom_keycodes {
    GIT_ADD = SAFE_RANGE,
    GIT_COMMIT,
    OPEN_CHATGPT,
    GIT_PUSH,
    MUTE
};

static char last_key_pressed[20] = "None";  // Store last key pressed

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case GIT_ADD:
                strcpy(last_key_pressed, "git add");
                SEND_STRING("git add .\n");
                return false;
            case GIT_COMMIT:
                strcpy(last_key_pressed, "git commit");
                SEND_STRING("git commit -m \".\"\n");
                return false;
            case GIT_PUSH:
                strcpy(last_key_pressed, "git push");
                SEND_STRING("git push\n");
                return false;
            case OPEN_CHATGPT:
                strcpy(last_key_pressed, "ChatGPT");
                // Open a new tab (Ctrl + N)
                tap_code16(C(KC_N));
                wait_ms(100);  // Small delay to ensure the new tab opens before typing
                SEND_STRING("https://chat.openai.com\n");
                return false;
            case MUTE:
                tap_code(KC_MUTE);
                return false;
        }
    }
    return true;
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        GIT_ADD, GIT_COMMIT, OPEN_CHATGPT,
        GIT_PUSH, MUTE
    )
};

const uint16_t PROGMEM encoder_map[][1][2] = {
    [0] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD) }
};

// Encoder rotation handling
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (clockwise) {
        tap_code(KC_VOLU);
    } else {
        tap_code(KC_VOLD);
    }
    return false;
}

#ifdef OLED_ENABLE
void oled_clear(void);
bool oled_task_user(void) {
    oled_set_cursor(0, 5);
    oled_write("Idkpad", false);
    oled_set_cursor(0, 15);
    oled_write_ln_P(PSTR("Key: "), false);
    oled_write_ln(last_key_pressed, false);
    return false;  
}
#endif
