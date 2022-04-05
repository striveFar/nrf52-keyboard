/*
 * Copyright 2020 Richard Sutherland (rich@brickbots.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "action.h"
#include "hook.h"
#include "wpm.h"
#include "events.h"
#include "keyboard_evt.h"
#include "action_layer.h"
#include "action_util.h"
#include "../../driver/ssd1306/oled_graph.h"
// keymap
#include "keymap.h"
#include "keymap_common.h"

//打字速度监测，由用户选择决定亮屏规则
bool wpm_monitor = false;
// WPM Stuff
static int16_t  current_wpm = 0;
//static uint8_t last_wpm = 0;
static uint16_t wpm_timer   = 0;
static uint16_t cur_keycode = 0;
static uint8_t cur_layer    = 0;

// This smoothing is 40 keystrokes
static const float wpm_smoothing = WPM_SMOOTHING;

void set_current_wpm(uint8_t new_wpm) { current_wpm = new_wpm; }

uint8_t get_current_wpm(void) { return current_wpm; }

bool wpm_keycode(uint16_t keycode) { return wpm_keycode_kb(keycode); }

__attribute__((weak)) bool wpm_keycode_kb(uint16_t keycode) { return wpm_keycode_user(keycode); }

__attribute__((weak)) bool wpm_keycode_user(uint16_t keycode) {
    if (keycode > 0xFF) {
        keycode = 0;
    }
    if ((keycode >= KC_A && keycode <= KC_0) || (keycode >= KC_TAB && keycode <= KC_SLASH)) {
        return true;
    }

    return false;
}

#ifdef WPM_ALLOW_COUNT_REGRESSION
__attribute__((weak)) uint8_t wpm_regress_count(uint16_t keycode) {
    bool weak_modded = (keycode >= KC_LCTRL && keycode < KC_LSHIFT) || (keycode >= KC_RCTRL && keycode < KC_RSHIFT);

    if (keycode > 0xFF) {
        keycode = 0;
    }
    if (keycode == KC_DEL || keycode == KC_BSPC) {
        if (((get_mods() | get_oneshot_mods()) & MOD_MASK_CTRL) || weak_modded) {
            return WPM_ESTIMATED_WORD_SIZE;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}
#endif

void update_wpm(uint16_t keycode) {
    if (wpm_keycode(keycode)) {
        if (wpm_timer > 0) {
            current_wpm += ((1000 / timer_elapsed(wpm_timer) / WPM_ESTIMATED_WORD_SIZE) - current_wpm) * wpm_smoothing;
	    if(current_wpm > 255)
		    current_wpm = 255;
        }
        wpm_timer = timer_read();
    }
#ifdef WPM_ALLOW_COUNT_REGRESSION
    uint8_t regress = wpm_regress_count(keycode);
    if (regress) {
        if ((current_wpm -= regress) < 0)
		current_wpm = 0;
        wpm_timer = timer_read();
    }
#endif
}

void decay_wpm(void) {
    if (timer_elapsed(wpm_timer) > 1000) {
        current_wpm += (-current_wpm) * wpm_smoothing;
        wpm_timer = timer_read();
    }
}

void trig_wpm_handle(keyevent_t event) {
        cur_layer = current_layer_for_key(event.key);
        cur_keycode = keymap_key_to_keycode(cur_layer, event.key);
        update_wpm(cur_keycode);
}

bool hook_process_action(keyrecord_t *record) {
    if (record->event.pressed) {
        trig_wpm_handle(record->event);
    }
    return false;
}
void print_wpm_via_tap(uint8_t val)
{
    int digits[10] = { KC_0, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9 };

        type_code(KC_SPACE);
        if (val >= 100) {
            int index100 = (val / 100) % 10;
            int digit100 = digits[index100];
            type_code(digit100);
	}
        if (val >= 10) {
            int index10 = (val / 10) % 10;
            int digit10 = digits[index10];
            type_code(digit10);
        }
        int index1 = val % 10;
        int digit1 = digits[index1];
        type_code(digit1);
        type_code(KC_SPACE);
}
