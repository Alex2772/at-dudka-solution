/*
 * Copyright (c) Alex2772, https://github.com/alex2772
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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


#include <functional>
#include <memory>
#include "input.h"
#include "config.h"
#include "app/screen/IScreen.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"

extern "C" I2C_HandleTypeDef hi2c1;
extern "C" TIM_HandleTypeDef htim3;
extern "C" TIM_HandleTypeDef htim4;

namespace app {
    struct Globals {
        bool fireAllowed = false;
        int maxTemperature = 210;
        std::optional<float> currentResistance;
        std::optional<float> initialResistance;

        float smoothCurrent = 0.f;
        float smoothBatteryVoltage = 0.f;
        int currentTemperature = config::DEFAULT_TEMPERATURE;
        uint32_t lastActionTick = 0;
        float cooldownStreak = 0.f;
        uint32_t fireBeginTime = 0;
    };
    extern Globals globals;


    unsigned& fireMosfet();

    void runOnUiThread(std::function<void()> callback);

    void showScreen(std::unique_ptr<IScreen> screen);
    void closeScreen(IScreen* screen);

    void shutdown();

    void resetAutoShutdownTimer();

    void onKeyDown(input::Key key);
    void onKeyUp(input::Key key);
    void onKeyLongPressFrame(input::Key key);

    int batteryLevel();

    bool fireButtonPressed();
    bool isCharging();

    float maxPowerIncludingSoftStart();
}


