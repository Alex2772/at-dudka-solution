//
// Created by alex2772 on 9/2/23.
//

#include "backup_registers.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"

extern "C"
RTC_HandleTypeDef hrtc;

const backup_registers::Raw& backup_registers::raw() {
    return (const Raw&) *reinterpret_cast<volatile Raw*>(&hrtc.Instance->BKP0R);
}

const backup_registers::Config& backup_registers::config() {
    auto& value = const_cast<Config&>(reinterpret_cast<const Config&>(raw()));
    if (value.magic != MAGIC_NUMBER) {
        HAL_PWR_EnableBkUpAccess();
        value = {};
        HAL_PWR_DisableBkUpAccess();
    }
    return value;
}
