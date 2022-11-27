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


#include "app.h"
#include "config.h"
#include "adc.h"
#include "util.h"
#include "input.h"
#include "FunctionQueue.h"
#include "app/screen/ScreenMain.h"
#include "app/screen/ScreenConfirmDialog.h"
#include "app/screen/ScreenCalibration.h"
#include "sram.h"
#include <ssd1306/SSD1306.h>
#include <glm/gtc/constants.hpp>
#include <vector>



app::Globals app::globals;

FunctionQueue gUiThreadQueue;
std::vector<std::unique_ptr<IScreen>> gScreens;

extern "C" void app_run() {
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 10000);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);

    adc::init();
    input::init();

    SSD1306 display(hi2c1);


    app::resetAutoShutdownTimer();

    Framebuffer fb;
    fb.setTransform(glm::imat3x3({
         0, -1,  0,
         1,  0,  0,
         0, 63,  1,
    }));

    if constexpr(config::CALIBRATION) {
        app::showScreen(std::make_unique<ScreenCalibration>());
    } else {
        if (sram::ram().lock) {
            if (!input::isKeyDown(input::Key::OK)) {
                fb.string({32, 32}, Color::WHITE, "Блокировка", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                fb.string({32, 45}, Color::WHITE, "Нажмите", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                fb.string({32, 45 + 12}, Color::WHITE, "клавишу ОК", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                display.push(fb);

                auto timeout = HAL_GetTick() + 3000;
                while (!input::isKeyDown(input::Key::OK) && timeout > HAL_GetTick()) {
                    HAL_Delay(1);
                }
                if (!input::isKeyDown(input::Key::OK)) {
                    app::shutdown();
                }
            }

            sram::ram().lock = false;
        }

        fb.string({32, 32}, Color::WHITE, "Запуск...", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
        display.push(fb);

        app::fireMosfet() = 10000;

        auto coilCheckTick = HAL_GetTick();

        while (!(app::globals.initialResistance = adc::coilResistance())) {
            if (HAL_GetTick() - coilCheckTick > 1000) {
                break;
            }
        }

        app::fireMosfet() = 0;

        HAL_TIM_Base_Start_IT(&htim4);
        if (!app::globals.initialResistance) {
            fb.clear();
            fb.string({32, 32}, Color::WHITE, "Нет койла", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
            display.push(fb);
            HAL_Delay(1000);
            app::shutdown();
        }

        app::globals.maxTemperature = sram::ram().maxTemperature;

        app::showScreen(std::make_unique<ScreenMain>());
    }

    app::globals.smoothBatteryVoltage = adc::batteryVoltage();

    for (;;) {
        gUiThreadQueue.process();

        input::frame();

        fb.clear();
        if (!gScreens.empty()) {
            gScreens.back()->render(fb);
        }

        display.push(fb);

        if (HAL_GetTick() - app::globals.lastActionTick > std::chrono::duration_cast<std::chrono::milliseconds>(config::AUTOSHUTDOWN_TIMEOUT).count()) {
            app::shutdown();
        }
    }

}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4) {
        const bool isFiring = app::globals.fireAllowed && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
        auto instantCurrent = adc::current();

        app::globals.currentResistance = adc::coilResistance();

        auto instantTemperature = app::globals.currentResistance ? glm::clamp(int(util::tcr(0.000915, config::DEFAULT_TEMPERATURE, app::globals.initialResistance.value_or(0), *app::globals.currentResistance)), config::DEFAULT_TEMPERATURE, app::globals.maxTemperature + 400) : config::DEFAULT_TEMPERATURE;
        app::globals.currentTemperature = glm::mix(app::globals.currentTemperature, instantTemperature, app::fireMosfet() > 0 ? 1.1f : 0.0001);
        app::globals.smoothCurrent = glm::mix(app::globals.smoothCurrent, instantCurrent, 0.02f);
        app::globals.smoothBatteryVoltage = glm::mix(app::globals.smoothBatteryVoltage, adc::batteryVoltage(), 0.01f);


        if (app::globals.fireAllowed) {
            if (isFiring) {
                app::resetAutoShutdownTimer();
                app::fireMosfet() = app::globals.currentTemperature + 20 < app::globals.maxTemperature ? 10000 : 0;
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 10000);
            } else {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
                static unsigned frameIndex = 0;
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 10000 * glm::abs(glm::sin(frameIndex / 500.f * glm::pi<float>() * 2.f)));
                frameIndex += 1;
                frameIndex %= 500;
            }
        }

    }
}

unsigned& app::fireMosfet() {
    return (unsigned int&) htim3.Instance->CCR2;
}


void app::runOnUiThread(std::function<void()> callback) {
    gUiThreadQueue.enqueue(std::move(callback));
}

void app::onKeyDown(input::Key key) {
    app::resetAutoShutdownTimer();

    if (gScreens.size() <= 1 && key == input::Key::LEFT) {
        app::showScreen(std::make_unique<ScreenConfirmDialog>("Выключить?", [] {
            sram::ram().lock = true;
            app::shutdown();
        }));
    } else {
        if (!gScreens.empty()) {
            gScreens.back()->onKeyDown(key);
        }
    }
}


void app::onKeyUp(input::Key key) {
    if (!gScreens.empty()) {
        gScreens.back()->onKeyUp(key);
    }
}

void app::showScreen(std::unique_ptr<IScreen> screen) {
    gScreens.push_back(std::move(screen));
}


void app::closeScreen(IScreen* screen) {
    gScreens.erase(std::find_if(gScreens.begin(), gScreens.end(), [&](const auto& lhs) { return lhs.get() == screen; }), gScreens.end());
}


void app::onKeyLongPressFrame(input::Key key) {
    app::resetAutoShutdownTimer();
    if (!gScreens.empty()) {
        gScreens.back()->onKeyLongPressFrame(key);
    }
}

void app::shutdown() {
    sram::save();
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
    HAL_PWR_EnterSTANDBYMode();
    for(;;);
}

void app::resetAutoShutdownTimer() {
    globals.lastActionTick = HAL_GetTick();
}
