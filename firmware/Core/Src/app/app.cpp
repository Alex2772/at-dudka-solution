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
#include "app/screen/ScreenDebug.h"
#include "sram.h"
#include "rtc.h"
#include "app/screen/ScreenInitializationDialog.h"
#include "backup_registers.h"
#include <ssd1306/SSD1306.h>
#include <glm/gtc/constants.hpp>
#include <vector>

extern const std::uint8_t image2cpp_logo_png[];
extern const std::uint8_t image2cpp_no_coil_png[];
extern const std::uint8_t image2cpp_lock_png[];
extern const std::uint8_t image2cpp_warning_png[];
extern const std::uint8_t image2cpp_cooldown_png[];


using namespace std::chrono;
using namespace std::chrono_literals;

app::Globals app::globals;

FunctionQueue gUiThreadQueue;
std::vector<std::unique_ptr<IScreen>> gScreens;
unsigned gLastBatteryUpdate = 0;
unsigned gBatteryLevel = 0;
bool gCoilDisconnectedFlag = false;


static ScreenMessageDialog::Action makeShutdownAction() {
    return { input::Key::OK, "Выключить", [] { app::shutdown(); } };
}

extern "C" void app_run() {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);


    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);

    adc::init();
    input::init();

    SSD1306 display(hi2c1);


    app::resetAutoShutdownTimer();

    Framebuffer fb;
    fb.verticalOrientation();

    if constexpr(config::CALIBRATION) {

        app::globals.initialResistance = 10.f;
        app::globals.fireAllowed = true;
        app::fireMosfet() = 0;

        HAL_TIM_Base_Start_IT(&htim4);

        app::showScreen(std::make_unique<ScreenDebug>());
    } else {
        if (sram::config().cooldownNextUnlock) {
            if (rtc::now() < *sram::config().cooldownNextUnlock) {
                fb.image({32,  32}, image2cpp_cooldown_png);
                fb.string({32, 52}, Color::WHITE, "Кулдаун", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                fb.string({32, 52 + 14}, Color::WHITE, "осталось", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);

                auto delta = *sram::config().cooldownNextUnlock - rtc::now();

                fb.string({32, 52 + 14 + 12},
                          Color::WHITE,
                          delta < 1min ? "<1м" : util::format("%dч %dм", int(floor<hours>(delta).count()), int(floor<minutes>(delta).count()) % 60),
                          FONT_FACE_TERMINUS_6X12_KOI8_R,
                          TextAlign::MIDDLE);
                display.push(fb);
                HAL_Delay(3000);
                app::shutdown();
            } else {
                sram::config().cooldownNextUnlock.reset();
            }
        }

        if (sram::config().lock) {
            if (!input::isKeyDown(input::Key::OK)) {
                auto drawLockScreen = [&] {
                    fb.image({32,  32}, image2cpp_lock_png);
                    fb.string({32, 52}, Color::WHITE, "Блокировка", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                    fb.string({32, 52 + 14}, Color::WHITE, "Нажмите", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                    fb.string({32, 52 + 14 + 12}, Color::WHITE, "клавишу ОК", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
                    display.push(fb);
                };

                drawLockScreen();
                bool isLockScreenVisible = true;

                auto waitFor = [&](std::chrono::milliseconds time) {
                    auto waitStartTime = HAL_GetTick();
                    auto lockScreenAppearedTime = HAL_GetTick();
                    while (!input::isKeyDown(input::Key::OK) && waitStartTime + time.count() > HAL_GetTick()) {
                        HAL_Delay(1);
                        gUiThreadQueue.process(); // process input antidithering frames

                        if (isLockScreenVisible) {
                            if (HAL_GetTick() - lockScreenAppearedTime > 3'000) {
                                fb.clear();
                                display.push(fb);
                                isLockScreenVisible = false;
                                app::powerLed() = 0;
                            }
                        }

                        if (app::fireButtonPressed()) {
                            waitStartTime = HAL_GetTick();

                            if (!isLockScreenVisible) {
                                drawLockScreen();
                                isLockScreenVisible = true;
                                lockScreenAppearedTime = HAL_GetTick();
                                app::powerLed() = 10000;
                            }
                        }
                    }

                    return !input::isKeyDown(input::Key::OK);
                };

                if (waitFor(5s)) {
                    app::shutdown();
                }

            }

            sram::config().lock = false;
            fb.clear();
        }

        const bool isCoilAttached = adc::coilVoltage() > 0.2f;

        if (!isCoilAttached) {
            fb.clear();
            fb.string({32, 0}, Color::WHITE, "Где койл?", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
            fb.image({32, 64}, image2cpp_no_coil_png);
            display.push(fb);
            HAL_Delay(1000);
            HAL_TIM_Base_Start_IT(&htim4);
        } else {
            fb.image({32, 64}, image2cpp_logo_png);
            display.push(fb);

            app::initCoil();
        }

        app::globals.maxTemperature = sram::config().maxTemperature;

        app::showScreen(std::make_unique<ScreenMain>());
    }

    app::globals.smoothBatteryVoltage = adc::batteryVoltage();

    for (;;) {
        gUiThreadQueue.process();
        input::frame();

        if (!app::globals.initialResistance && app::fireButtonPressed() && !app::isDialogShown()) {
            auto dialog = ScreenMessageDialog::make("Нет койла");
            dialog->setIcon(image2cpp_warning_png);
            app::showScreen(std::move(dialog));
        }

        fb.clear();
        if (!gScreens.empty()) {
            auto firstNonTransparent = (std::find_if(gScreens.rbegin(), gScreens.rend(), [](const auto& screen) {
                return !screen->hasTransparency();
            }) + 1).base();

            for (auto it = firstNonTransparent; it != gScreens.end(); ++it) {
                (*it)->render(fb);
            }
        }

        display.push(fb);

        if (auto autoShutdown = enum_traits<SleepDuration>::duration(sram::config().sleepTimout); autoShutdown.count() != 0) {
            if (HAL_GetTick() - app::globals.lastActionTick > autoShutdown.count()) {
                app::shutdown();
            }
        }
    }

}

bool app::fireButtonPressed() {
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
}


extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4) { // called 100 times per second
        const bool isFiring = app::globals.fireAllowed && app::fireButtonPressed();
        if (static bool wasFiring = false; isFiring != wasFiring) {
            wasFiring = isFiring;
            if (isFiring) {
                app::globals.fireBeginTime = HAL_GetTick();
                backup_registers::visit([](backup_registers::Config& c) {
                    c.puffCount += 1;
                });
            }
        }
        auto instantCurrent = adc::current();

        app::globals.currentResistance = adc::coilResistance();

        const float TCR = enum_traits<Material>::tcr(sram::config().material);
        auto instantTemperature = app::globals.currentResistance ? glm::clamp(int(util::tcr(TCR, config::DEFAULT_TEMPERATURE, app::globals.initialResistance.value_or(0), *app::globals.currentResistance)), config::DEFAULT_TEMPERATURE, app::globals.maxTemperature + 400) : config::DEFAULT_TEMPERATURE;
        app::globals.currentTemperature = glm::mix(app::globals.currentTemperature, instantTemperature, app::fireMosfet() > 0 ? 0.05f : 0.1f);
        app::globals.smoothCurrent = glm::mix(app::globals.smoothCurrent, instantCurrent, 0.2f);
        app::globals.smoothBatteryVoltage = glm::mix(app::globals.smoothBatteryVoltage, adc::batteryVoltage(), 0.1f);

        if (app::globals.fireAllowed) {
            if (app::globals.smoothCurrent >= config::MAX_CURRENT && !sram::config().mechModMode) {
                if constexpr (!config::CALIBRATION) {
                    app::runOnUiThread([] {
                        auto dialog = ScreenMessageDialog::make("КЗ койла", { { input::Key::OK, "Закрыть", [] {
                            app::globals.fireAllowed = true;
                        } } });
                        dialog->setIcon(image2cpp_warning_png);
                        app::showScreen(std::move(dialog));
                    });

                    app::fireMosfet() = 0;
                    app::globals.fireAllowed = false;

                    return;
                }
            }

            if (isFiring) {
                app::resetAutoShutdownTimer();

                if (config::CALIBRATION || sram::config().mechModMode || app::globals.burnoutMode) {

                    app::fireMosfet() = 10000;
                    app::powerLed() = 10000;
                } else {
                    const auto power = app::globals.smoothCurrent * app::globals.smoothBatteryVoltage;

                    bool doTheFiringOnThisFrame = app::globals.currentTemperature + 20 < app::globals.maxTemperature
                                               && power < app::maxPowerIncludingSoftStart();

                    if (sram::config().cooldownEnabled && doTheFiringOnThisFrame) {
                        app::globals.cooldownStreak += 0.01f;
                    }

                    app::fireMosfet() = doTheFiringOnThisFrame ? 10000 : 0;
                    app::powerLed() = 10000;
                }

            } else {
                app::fireMosfet() = 0;
                static unsigned frameIndex = 0;
                app::powerLed() = 10000 * glm::abs(glm::sin(frameIndex / 500.f * glm::pi<float>() * 2.f));
                frameIndex += 1;
                frameIndex %= 500;

                if (static bool triggered = false; !triggered && !sram::config().mechModMode) {
                    if (app::globals.smoothBatteryVoltage < config::BATTERY_LEVEL_WARNING && !app::isCharging()) {
                        triggered = true;
                        app::runOnUiThread([] {
                            auto dialog = ScreenMessageDialog::make("Аккум разряжен");
                            dialog->setIcon(image2cpp_warning_png);
                            app::showScreen(std::move(dialog));
                        });
                    }
                }

                if (static bool triggered = false; !triggered && !sram::config().mechModMode && !config::CALIBRATION) {
                    if (app::globals.smoothBatteryVoltage < config::BATTERY_LEVEL_CRITICAL && !app::isCharging()) {
                        triggered = true;
                        app::runOnUiThread([] {
                            auto dialog = ScreenMessageDialog::make("Аккум разряжен в хлам", { makeShutdownAction() });
                            dialog->setIcon(image2cpp_warning_png);
                            app::showScreen(std::move(dialog));
                        });
                    }
                }

                if (adc::coilVoltage() < 0.2f && app::globals.smoothCurrent < 0.01f) {

                    if constexpr (!config::CALIBRATION) {
                        if (!gCoilDisconnectedFlag && !sram::config().mechModMode) {
                            gCoilDisconnectedFlag = true;
                            app::runOnUiThread([] {
                                app::globals.fireAllowed = false;
                                app::globals.initialResistance.reset();
                                app::powerLed() = 10000;
                                auto dialog = ScreenMessageDialog::make("Отвал койла");
                                dialog->setIcon(image2cpp_warning_png);
                                app::showScreen(std::move(dialog));
                            });
                        }
                    }
                }
            }
            if (sram::config().cooldownEnabled && !isFiring) {
                if (static bool once = true; once) {
                    if (app::globals.cooldownStreak >= sram::config().cooldownThreshold) {
                        once = false;
                        app::globals.fireAllowed = false;
                        app::runOnUiThread([] {
                            rtc::resetTime();
                            sram::config().cooldownNextUnlock = std::chrono::duration_cast<std::chrono::seconds>(enum_traits<CooldownDuration>::duration(
                                    sram::config().cooldownDuration));
                            sram::save();

                            auto dialog = ScreenMessageDialog::make("Лимит исчерпан", { makeShutdownAction() });
                            dialog->setIcon(image2cpp_cooldown_png);
                            app::showScreen(std::move(dialog));
                        });
                    }
                }
                app::globals.cooldownStreak = glm::ceil(app::globals.cooldownStreak);
            }
        } else if (!app::globals.initialResistance) {
            const bool coilAttached = adc::coilVoltage() > 3.f;
            static bool askedForReinitialization = false;
            if (coilAttached) {
                if (!askedForReinitialization && !app::isDialogShown()) {
                    askedForReinitialization = true;
                    app::runOnUiThread([] {
                        app::showScreen(std::make_unique<ScreenInitializationDialog>());
                    });
                }
            } else {
                askedForReinitialization = false;
            }
        }

        if (int(HAL_GetTick()) - gLastBatteryUpdate > 60'000 || gLastBatteryUpdate == 0) {
            if (app::fireMosfet() == 0) {
                if (glm::abs(adc::batteryVoltage() - app::globals.smoothBatteryVoltage) < 0.0001f) { // filter
                    gLastBatteryUpdate = HAL_GetTick();
                    constexpr auto MIN = 3.3f;
                    constexpr auto MAX = 4.15f;

                    gBatteryLevel = glm::clamp(int((app::globals.smoothBatteryVoltage - MIN) / (MAX - MIN) * 100.f), 1,
                                               100);
                }
            }
        }
    }
}

unsigned& app::fireMosfet() {
#if AT_DUDKA_REV >= 2
    return (unsigned int&) htim2.Instance->CCR2;
#else
    return (unsigned int&) htim3.Instance->CCR2;
#endif
}

unsigned& app::powerLed() {
    return (unsigned int&) htim3.Instance->CCR1;
}


void app::runOnUiThread(std::function<void()> callback) {
    gUiThreadQueue.enqueue(std::move(callback));
}

void app::onKeyDown(input::Key key) {
    app::resetAutoShutdownTimer();

    if (!gScreens.empty()) {
        gScreens.back()->onKeyDown(key);
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

int app::batteryLevel() {
    return gBatteryLevel;
}

bool app::isCharging() {
    return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
}

float app::maxPowerIncludingSoftStart() {
    if (!sram::config().softStartEnabled) {
        return float(sram::config().maxPower);
    }

    auto firingTime = milliseconds(HAL_GetTick() - app::globals.fireBeginTime);
    return float(sram::config().maxPower) * glm::clamp(float(firingTime.count()) / float(sram::config().softStartDuration.count()), 0.f, 1.f);
}

void app::initCoil() {
    adc::setShutter(config::SHUTTER_BOOT);
    HAL_TIM_Base_Stop_IT(&htim4);
    // several attempts  to measure the initial coil resistance; we have not found another solution to stabilize the measures between reboots
    app::globals.initialResistance.reset();
    for (int i = 0; i < 3; ++i) {
        app::fireMosfet() = 10000;

        auto coilCheckTick = HAL_GetTick();

        decltype(adc::coilResistance()) attempt;
        while (!(attempt = adc::coilResistance())) {
            if (HAL_GetTick() - coilCheckTick > 1000) {
                break;
            }
        }

        app::fireMosfet() = 0;
        if (attempt) {
            app::globals.initialResistance = std::min(app::globals.initialResistance.value_or(2.f), *attempt);
        }
        HAL_Delay(300);
    }
    adc::setShutter(config::SHUTTER_DEFAULT);
    HAL_TIM_Base_Start_IT(&htim4);

    gCoilDisconnectedFlag = false;
    // reduce the initial resistance to decrease minimal temperature control power
    if (app::globals.initialResistance) {
        *app::globals.initialResistance *= 0.8f;

        if (*app::globals.initialResistance < 0.01f && !sram::config().mechModMode) {
            app::globals.fireAllowed = false;
            app::globals.initialResistance.reset();
            app::runOnUiThread([] {
                auto dialog = ScreenMessageDialog::make("КЗ койла");
                dialog->setIcon(image2cpp_warning_png);
                app::showScreen(std::move(dialog));
            });
        } else {
            app::globals.fireAllowed = true;
        }
    } else {
        app::runOnUiThread([] {
            auto dialog = ScreenMessageDialog::make("Отвал койла");
            dialog->setIcon(image2cpp_warning_png);
            app::showScreen(std::move(dialog));
        });
    }

}

bool app::isDialogShown() {
    return !gScreens.empty() && gScreens.back()->hasTransparency();
}

