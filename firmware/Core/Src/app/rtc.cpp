// /*
//  * Copyright (c) Alex2772, https://github.com/alex2772
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, either version 3 of the License, or
//  * (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  *

//
// Created by Alex2772 on 2/11/2023.
//

#include <cstring>
#include "rtc.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"

extern "C" RTC_HandleTypeDef hrtc;


void rtc::resetTime() {
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /** Initialize RTC and set the Time and Date
    */
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 0x0;
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}


std::chrono::seconds rtc::now() {
    RTC_DateTypeDef d;
    RTC_TimeTypeDef t;
    HAL_RTC_GetDate(&hrtc, &d, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &t, RTC_FORMAT_BIN);
    return std::chrono::seconds(int(t.Seconds) + (int(t.Minutes) + (int(t.Hours) + int(d.Date - 1) * 24u) * 60) * 60);
}
