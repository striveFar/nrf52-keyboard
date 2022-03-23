#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "wpm.h"
#include "oled_graph.h"
#include "ssd1306_oled.h"
#include "events.h"
#include "keyboard_evt.h"


// WPM-responsive animation stuff here
#define IDLE_FRAMES 5
#define IDLE_SPEED 40 // below this wpm value your animation will idle

// #define PREP_FRAMES 1 // uncomment if >1

#define TAP_FRAMES 2
#define TAP_SPEED 60 // above this wpm value typing animation to triggere

#define ANIM_SIZE 636 // number of bytes in array, minimize for adequate firmware size, max is 1024

extern bool anim_paly_mode;
void anim_timer_init(void);
