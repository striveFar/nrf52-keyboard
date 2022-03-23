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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "keycode.h"
#include "keyboard.h"

#ifndef WPM_ESTIMATED_WORD_SIZE
#    define WPM_ESTIMATED_WORD_SIZE 1
#endif
#ifndef WPM_SMOOTHING
//#    define WPM_SMOOTHING 0.0487
//#    define WPM_SMOOTHING 0.9
#    define WPM_SMOOTHING 0.75
#endif

bool wpm_keycode(uint16_t keycode);
bool wpm_keycode_kb(uint16_t keycode);
bool wpm_keycode_user(uint16_t keycode);

#ifdef WPM_ALLOW_COUNT_REGRESSION
uint8_t wpm_regress_count(uint16_t keycode);
#endif

void    set_current_wpm(uint8_t);
uint8_t get_current_wpm(void);

void	decay_wpm(void);
void	update_wpm(uint16_t);
void	trig_wpm_handle(keyevent_t event);
void	print_wpm_via_tap(uint8_t val);
