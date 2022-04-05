/*
Copyright (C) 2019 Jim Jiang <jim@lotlab.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "wpm.h"
#include "power_save.h"
#include "keyboard_evt.h"
#include "store_config.h"
#include <stdint.h>

#if LED_AUTOOFF_TIME > 0

static bool power_save_mode = true;
static uint32_t tick_counter = 0;

static void notify_mode(enum power_save_mode mode)
{
    trig_event_param(USER_EVT_POWERSAVE, mode);
}

/**
 * @brief 设置省电模式状态
 *
 * @param on
 * on = 0; 有线连接，不是省电模式
 * on = 1; 无线，开启省点模式
 */
void power_save_set_mode(bool on)
{
    if (tick_counter)
        tick_counter = 0;

    power_save_mode = on;
    if (on)
        power_save_reset();
    else
        notify_mode(PWR_SAVE_EXIT);

    notify_mode(on ? PWR_SAVE_ON : PWR_SAVE_OFF);
}

/**
 * @brief 启动自动关闭计时器
 * @brief 重新启动自动关闭计时器,且仅通知屏幕退出省电模式,
 * 防止有连续敲击事件时指示灯常亮,指示灯状态直接通过屏幕进行持续反馈，
 * 防止增加无意义的耗电项目
 * @作用: 1. 处于睡眠模式则通知各个事件处理处理器退出睡眠模式
 *	  2. 重新定时
 */
void power_save_reset(void)
{
    if (power_save_mode) {
        // 若当前已经处于睡眠模式,则触发退出事件
        if (!tick_counter)
	     notify_mode(PWR_SAVE_EXIT
#ifdef WPM_ENABLE
			| PWR_SAVE_WPM_AUTO
#endif
			 );

         tick_counter = get_led_powersave_timeout();
    }
}

#ifdef WPM_ENABLE
#ifdef ANIMATION_ENABLE
    extern bool anim_play_mode;
#endif
    extern bool wpm_monitor;
#endif

/**
 * @brief 秒定时处理,到达省电时间后触发省电模式
 * 若开启WPM_ENABLE,则到达省电时间后且WPM为零时触发省电模式
 */
static void ps_event_handler(enum user_event event, void* arg)
{
    if (event == USER_EVT_TICK) {
        if (power_save_mode && tick_counter) {
#ifdef WPM_ENABLE
	     if (
#ifdef ANIMATION_ENABLE
		 !anim_play_mode &&
#endif
		 !wpm_monitor) {
		 tick_counter--;
	     } else if (!get_current_wpm())
#endif
	     {
		 tick_counter--;
	     }

            // 时间到了,触发省电模式
            if (tick_counter == 0)
                notify_mode(PWR_SAVE_ENTER);
	    }
    }
}

EVENT_HANDLER(ps_event_handler);
#endif
