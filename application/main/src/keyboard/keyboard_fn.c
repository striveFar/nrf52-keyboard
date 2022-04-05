/*
Copyright (C) 2018,2019 Jim Jiang <jim@lotlab.org>

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

#include "keyboard_fn.h"
#include "../main.h"
#include "usb_comm.h"

#include "bootloader.h"
#include "bootmagic.h"
#include "eeconfig.h"
#include "host.h"
#include "keymap.h"
#include "ble_services.h"
#include "ble_bas_service.h"

#ifdef ANIMATION_ENABLE
#include "oled_anim.h"
extern bool anim_play_mode;
#endif

#ifdef WPM_ENABLE
extern bool wpm_monitor;
#endif

#ifdef NKRO_ENABLE

#ifdef BOOTMAGIC_ENABLE
extern keymap_config_t keymap_config;
#endif
/**
 * 切换nkro状态
 */
static void toggle_nkro()
{
    keyboard_nkro = !keyboard_nkro;
#ifdef BOOTMAGIC_ENABLE
    // 写入bootmagic配置
    keymap_config.nkro = keyboard_nkro;
    eeconfig_write_keymap(keymap_config.raw);
#endif
}
#endif

FN_HANDLER_DEF();

__attribute__((weak)) void action_function(keyrecord_t* record, uint8_t id, uint8_t opt)
{
    if (record->event.pressed) {
        switch (id) {
        case KEYBOARD_CONTROL:
            switch (opt) {
            case CONTROL_SLEEP: // 睡眠
#ifdef ANIMATION_ENABLE
	    /* 规定在动画模式下无法关机 */
	    if (!anim_play_mode) {
#endif
                sleep(SLEEP_MANUALLY);
#ifdef ANIMATION_ENABLE
	    }
#endif
                break;
            case CONTROL_NKRO: // 切换NKRO
#ifdef NKRO_ENABLE
                toggle_nkro();
#endif
                break;
            case CONTROL_BOOTLOADER: // 跳转到bootloader
                bootloader_jump();
                break;
            case CONTROL_BATTERY_PERCENTAGE: // 输出电池剩余电量
                print_battery_percentage();
                break;
#ifdef ANIMATION_ENABLE
	    case CONTROL_ANIM_PLAY: // 动画显示开关
		anim_play_mode = !anim_play_mode;
		trig_event_param(USER_EVT_ANIM,
				 anim_play_mode ? ANIM_START : ANIM_STOP);
                break;
#endif
#ifdef WPM_ENABLE
	    case CONTROL_WPM_MONITOR: // 打字速度监测开关
		wpm_monitor = !wpm_monitor;
                break;
#endif
            default:
                break;
            }
            break;

        case SWITCH_DEVICE:
            switch (opt) {
#ifdef HAS_USB
            case SWITCH_DEVICE_USB: // 切换设备
                usb_comm_switch();
                break;
#endif
#ifdef MULTI_DEVICE_SWITCH
            case SWITCH_DEVICE_BLE_0: // 蓝牙设备的切换
                // todo: 切换前清除当前按键
                switch_device_select(0);
                break;
            case SWITCH_DEVICE_BLE_1:
                switch_device_select(1);
                break;
            case SWITCH_DEVICE_BLE_2:
                switch_device_select(2);
                break;
            case SWITCH_DEVICE_BLE_READV:
                advertising_restart(BLE_ADV_MODE_FAST, true);
                break;
            case SWITCH_DEVICE_BLE_REBOND:
                switch_device_rebond();
                break;
#endif
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    // 交给其他Fn处理
    for (uint8_t i = 0; i < FN_HANDLER_COUNT; i++) {
        fn_handler* handler = (fn_handler*)FN_HANDLER_GET(i);
        (*handler)(record, id, opt);
    }
}
