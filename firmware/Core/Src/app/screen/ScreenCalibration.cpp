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


#include "ScreenCalibration.h"
#include "app/util.h"
#include "app/adc.h"
#include "app/app.h"

void ScreenCalibration::render(FramebufferImpl& fb) {
    auto row = [&](unsigned y, std::string_view title, std::string_view value) {
        fb.string({0, y}, Color::WHITE, title, FONT_FACE_BITOCRA_4X7);
        fb.string({60, y}, Color::WHITE, value, FONT_FACE_BITOCRA_4X7, TextAlign::RIGHT);
    };

    row(0, std::string(util::format("%d", adc::calibrationData().batteryVoltage)), std::string(util::format("%0.2fV", adc::batteryVoltage())));
    row(8, std::string(util::format("%d", adc::calibrationData().shuntVoltage)), std::string(util::format("%0.2fV", adc::shuntVoltage())));
    row(16, std::string(util::format("%d", adc::calibrationData().coilVoltage)), std::string(util::format("%0.2fV", adc::coilVoltage())));
    row(24, std::string(util::format("%.3f", adc::batteryVoltage() - adc::shuntVoltage())), std::string(util::format("%.2fA", app::globals.smoothCurrent)));
    row(32, std::string(util::format("%.3f", adc::shuntVoltage() - adc::coilVoltage())), std::string(util::format("%.2fOhm", adc::coilResistance().value_or(0.f))));
}