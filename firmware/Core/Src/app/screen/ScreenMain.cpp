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

#include <stm32f4xx.h>

void ScreenMain::render(Framebuffer& fb) {
    auto row = [&](unsigned y, std::string_view title, std::string_view value) {
        fb.string({0, y + 1}, Color::WHITE, title, FONT_FACE_TERMINUS_6X12_KOI8_R);
        fb.string({60, y}, Color::WHITE, value, FONT_FACE_TERMINUS_BOLD_8X14_ISO8859_1, TextAlign::RIGHT);
    };


    fb.string({32, 0}, Color::WHITE, util::format("%d\xb0""C", app::globals.maxTemperature), FONT_FACE_TERMINUS_BOLD_12X24_ISO8859_1, TextAlign::MIDDLE);



    row(28, "Бат", util::format("%0.2fV", app::globals.smoothBatteryVoltage));
    row(42, "Ток", util::format("%0.1fA", app::globals.smoothCurrent));
    {
        row(56, "T", util::format("%d\xb0""C", app::globals.currentTemperature));

        row(70, "R", util::format("%0.2f\x80", app::globals.currentResistance.value_or(*app::globals.initialResistance)));
    }


    std::string gavno;

    if (input::isKeyDown(input::Key::UP)) {
        gavno += "^";
    }
    if (input::isKeyDown(input::Key::DOWN)) {
        gavno += "v";
    }
    if (input::isKeyDown(input::Key::LEFT)) {
        gavno += "<";
    }
    if (input::isKeyDown(input::Key::RIGHT)) {
        gavno += ">";
    }
    if (input::isKeyDown(input::Key::OK)) {
        gavno += "o";
    }


    fb.string({0, 100}, Color::WHITE, gavno, FONT_FACE_TERMINUS_6X12_KOI8_R);

}

ScreenMain::ScreenMain() {

    adc::setShutter(config::SHUTTER_DEFAULT);
    app::globals.fireAllowed = true;
}

void ScreenMain::onKeyDown(input::Key key) {
    IScreen::onKeyDown(key);
    handleKeyTemperature(key);
}

void ScreenMain::onKeyLongPressFrame(input::Key key) {
    IScreen::onKeyLongPressFrame(key);
    handleKeyTemperature(key);
}

void ScreenMain::onKeyUp(input::Key key) {
    IScreen::onKeyUp(key);

    sram::ram().maxTemperature = app::globals.maxTemperature;
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
    app::globals.maxTemperature = glm::clamp(app::globals.maxTemperature, 50, 400);
}
