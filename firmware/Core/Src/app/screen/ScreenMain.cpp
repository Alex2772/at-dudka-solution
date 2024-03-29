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


#include "ScreenMain.h"
#include "app/app.h"
#include "app/adc.h"
#include "app/util.h"
#include "glm/ext/scalar_constants.hpp"
#include "app/sram.h"
#include "ScreenSettings.h"
#include "ScreenMessageDialog.h"
#include "ScreenConfirmDialog.h"
#include "app/backup_registers.h"

#include <stm32f4xx.h>

extern const std::uint8_t image2cpp_logo_small_png[];
extern const std::uint8_t image2cpp_charging_png[];
extern const std::uint8_t image2cpp_lock_png[];
extern const std::uint8_t image2cpp_coil_png[];
extern const std::uint8_t image2cpp_poweroff_png[];

void ScreenMain::render(FramebufferImpl& fb) {
    fb.verticalOrientation();
    auto row = [&](unsigned y, std::string_view title, std::string_view value) {
        fb.string({0, y + 1}, Color::WHITE, title, FONT_FACE_TERMINUS_6X12_KOI8_R);
        fb.string({60, y}, Color::WHITE, value, FONT_FACE_TERMINUS_BOLD_8X14_ISO8859_1, TextAlign::RIGHT);
    };


    {
        auto topbarString = util::format("%d\xb0""C", app::globals.maxTemperature);
        if (app::globals.burnoutMode) {
            topbarString = "MAX";
        }
        fb.string({32, 0},
                  Color::WHITE,
                  topbarString,
                  FONT_FACE_TERMINUS_BOLD_12X24_ISO8859_1,
                  TextAlign::MIDDLE);
    }
    fb.rect({0, 0}, {64, 24}, Color::INVERT);

    if (sram::config().cooldownEnabled) {
        fb.rect({0, 21}, {64, 1}, Color::BLACK);

        int w = app::globals.cooldownStreak / sram::config().cooldownThreshold * 64.f;
        fb.rect({64 - w, 22}, {w, 2}, Color::BLACK);
    }

    {
        auto w = fb.string({32, 30}, Color::WHITE, util::format("%lu", backup_registers::config().puffCount), FONT_FACE_BITOCRA_7X13, TextAlign::MIDDLE);
        fb.roundedRect({32 - w / 2 - 3, 29}, { w + 5, 15 }, Color::INVERT);
    }

    {
        auto str = "<Выкл|Меню>";
        if (app::globals.burnoutMode) {
            str = "ПРОЖИГ";
        }
        fb.string({32, 44}, Color::WHITE, str, FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
    }

    row(116 - 12 * 5, "Бат", util::format("%0.2fV", app::globals.smoothBatteryVoltage));
    auto power = app::globals.smoothCurrent * app::globals.smoothBatteryVoltage;
    if (!sram::config().mechModMode) power = glm::min(power, app::maxPowerIncludingSoftStart());
    static constexpr auto MAX_SHOW_PERIOD_MS = 1000;
    const bool showMaxPower = (HAL_GetTick() % MAX_SHOW_PERIOD_MS < MAX_SHOW_PERIOD_MS / 2) && !app::fireButtonPressed();
    if (showMaxPower) {
        power = sram::config().maxPower;
    }
    row(116 - 12 * 4,
        showMaxPower ? "Макс" : "Мощ",
        showMaxPower ? util::format("%0.0fW", power) : util::format(power > 99.f ? "%0.0fW" : "%0.1fW", power));
    row(116 - 12 * 3, "Ток", util::format("%0.1fA", app::globals.smoothCurrent));

    if (app::globals.initialResistance) {
        row(116 - 12 * 2, "T", sram::config().material == Material::KANTHAL ? "KAN" : util::format("%d\xb0""C", app::globals.currentTemperature));
        row(116 - 12, "R", util::format("%0.2f\x80", app::globals.currentResistance.value_or(*app::globals.initialResistance)));
    } else {
        row(116 - 12 * 2, "T", "?\xb0""C");
        row(116 - 12, "R", "?\x80");
    }

    fb.image({7, 124}, image2cpp_logo_small_png);

    // battery
    if (app::isCharging()) fb.image({18, 124}, image2cpp_charging_png);
    if (app::batteryLevel() != 0) {
        fb.rectBorder({24, 120}, { 20, 8 }, Color::WHITE);
        fb.rect({24 + 20, 120 + 2}, { 2, 4 }, Color::WHITE);
        fb.rect({24 + 2, 120 + 2}, {app::batteryLevel() * 16 / 100, 4}, Color::INVERT);
        fb.string({63, 121}, Color::WHITE, util::format("%d%%", app::batteryLevel()), FONT_FACE_BITOCRA_4X7, TextAlign::RIGHT);
    } else {
        
    }
}

ScreenMain::ScreenMain() {
    adc::setShutter(config::SHUTTER_DEFAULT);
    app::globals.fireAllowed = true;
}

void ScreenMain::onKeyDown(input::Key key) {
    IScreen::onKeyDown(key);

    if (app::globals.burnoutMode) {
        app::globals.burnoutMode = false;
        return;
    }

    handleKeyTemperature(key);

    switch (key) {
        case input::Key::RIGHT:
            app::showScreen(std::make_unique<ScreenSettings>());
            break;
        case input::Key::LEFT: {
            auto dialog = std::make_unique<ScreenConfirmDialog>("Заблокировать?", [] {
                sram::config().lock = true;
                app::shutdown();
            });
            dialog->setIcon(image2cpp_poweroff_png);
            app::showScreen(std::move(dialog));
            break;
        }
    }
}

void ScreenMain::onKeyLongPressFrame(input::Key key) {
    IScreen::onKeyLongPressFrame(key);
    handleKeyTemperature(key);
}

void ScreenMain::onKeyUp(input::Key key) {
    IScreen::onKeyUp(key);

    sram::config().maxTemperature = app::globals.maxTemperature;
}

void ScreenMain::handleKeyTemperature(input::Key key) const {
    switch (key) {
        case input::Key::UP:
            app::globals.maxTemperature += 5;
            break;
        case input::Key::DOWN:
            app::globals.maxTemperature -= 5;
            break;
    }
    app::globals.maxTemperature = glm::clamp(app::globals.maxTemperature, 50, 900);
}
