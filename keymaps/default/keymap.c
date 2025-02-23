#include QMK_KEYBOARD_H

// Layer definitions
enum layers {
    _BASE,
    _LAYER1,
    _LAYER2,
    _LAYER3
};

uint8_t display_layer = _BASE;  // Tracks the currently displayed layer
uint8_t active_layer = _BASE;   // Tracks the actually active layer
enum custom_keycodes {
    GIT_ADD = SAFE_RANGE,
    GIT_COMMIT,
    OPEN_CHATGPT,
    GIT_PUSH,
    MUTE
};

static uint32_t mute_timer = 0;
static bool mute_key_pressed = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case GIT_ADD:
                if (active_layer == _BASE) {
                    SEND_STRING("git add .\n");
                    return false; // Prevent further processing
                }
                return true; // allow other layers to be processed

            case GIT_COMMIT:
                if (active_layer == _BASE) {
                    SEND_STRING("git commit -m \".\"\n");
                    return false; // Prevent further processing
                }
                return true; // allow other layers to be processed

            case GIT_PUSH:
                if (active_layer == _BASE) {
                    SEND_STRING("git push\n");
                    return false; // Prevent further processing
                }
                return true; // allow other layers to be processed

            case OPEN_CHATGPT: {
                if (active_layer == _BASE) {
                    SEND_STRING("git status\n");
                    return false; // allow other layers to be processed
                }
                return true;  // Prevent further processing of OPEN_CHATGPT
            }

            case MUTE:
                mute_key_pressed = true;
                mute_timer = timer_read32();
                return false; 

            default:
                return true;
        }
    } else {
        if (keycode == MUTE) {
            if (mute_key_pressed) {
                mute_key_pressed = false;
                if (timer_elapsed32(mute_timer) > 1000) {
                    SEND_STRING(SS_TAP(X_MUTE));
                } else {
                    display_layer = (display_layer + 1) % 4;

                    layer_off(active_layer);
                    active_layer = display_layer;
                    layer_on(active_layer);
                }
            }
            return false;
        }
    }
    return true;  // Always return true unless you're explicitly preventing the keycode
}

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        GIT_ADD, GIT_COMMIT, OPEN_CHATGPT,
        GIT_PUSH, MUTE
    ),
    [_LAYER1] = LAYOUT(
        KC_A, KC_B, KC_C,
        KC_D, MUTE
    ),
    [_LAYER2] = LAYOUT(
        KC_E, KC_F, KC_G,
        KC_H, MUTE
    ),
    [_LAYER3] = LAYOUT(
        KC_I, KC_J, KC_K,
        KC_L, MUTE
    )
};

// Encoder remains for volume control
const uint16_t PROGMEM encoder_map[][1][2] = {
    [_BASE]   = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [_LAYER1] = { ENCODER_CCW_CW(KC_LEFT, KC_RIGHT) },
    [_LAYER2] = { ENCODER_CCW_CW(KC_PGDN, KC_PGUP) },
    [_LAYER3] = { ENCODER_CCW_CW(KC_HOME, KC_END) }
};

#ifdef OLED_ENABLE
#include "timer.h"

static bool show_logo = true;       // Flag for showing the logo
static uint16_t logo_start_time = 0;  // Stores the start time

bool oled_task_user(void) {
    if (show_logo) {
        if (logo_start_time == 0) {
            logo_start_time = timer_read();  // Initialize timer on first call
        }

        if (timer_elapsed(logo_start_time) < 3000) {
            uint8_t idkpad_width = 6 * 6;
            uint8_t center_x = (128 - idkpad_width) / 2 / 6;

            oled_set_cursor(center_x, 2);
            oled_write_P(PSTR("IDKPAD"), false);
            return false;

        } else {
            show_logo = false;  
            oled_clear();
        }
    }

    oled_set_cursor(0, 0);
    oled_write_P(PSTR("LAYER:"), false);

    oled_set_cursor(7, 0);
    switch (active_layer) {  // use active_layer to properly display current selected layer.
        case _BASE:
            oled_write_P(PSTR("BASE"), false);
            break;
        case _LAYER1:
            oled_write_P(PSTR("L1  "), false);
            break;
        case _LAYER2:
            oled_write_P(PSTR("L2  "), false);
            break;
        case _LAYER3:
            oled_write_P(PSTR("L3  "), false);
            break;
    }

    return false;
}

#endif

void keyboard_post_init_user(void) {
    gpio_set_pin_output(GP11);
    gpio_write_pin_high(GP11);
    rgblight_enable();
}