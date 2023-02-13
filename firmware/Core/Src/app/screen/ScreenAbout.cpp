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
// Created by Alex2772 on 2/10/2023.
//

#include "ScreenAbout.h"
#include "app/app.h"
#include "ScreenSecretSettings.h"

extern const std::uint8_t image2cpp_logo_med_png[];
extern const std::uint8_t image2cpp_qr_png[];

ScreenAbout::ScreenAbout() {

}

void ScreenAbout::render(FramebufferImpl& fb) {
    ScreenLandscape::render(fb);

    fb.image({110, 64 - 16}, image2cpp_qr_png);

    fb.image({22, 8}, image2cpp_logo_med_png);
    fb.string({40, -2}, Color::WHITE, "Dudka", FONT_FACE_TERMINUS_BOLD_8X14_ISO8859_1);
    fb.string({40, 11}, Color::WHITE, "Solution", FONT_FACE_BITOCRA_4X7);

    fb.string({0, 18}, Color::WHITE, "Дата сборки прошивки:", FONT_FACE_TERMINUS_6X12_KOI8_R);
    fb.string({0, 18 + 12}, Color::WHITE, __DATE__ " " __TIME__, FONT_FACE_BITOCRA_4X7);

    fb.string({0, 40}, Color::WHITE, "Версия:", FONT_FACE_TERMINUS_6X12_KOI8_R);
    fb.string({0, 40 + 12}, Color::WHITE, GIT_SHA1, FONT_FACE_BITOCRA_4X7);
}

void ScreenAbout::onKeyDown(input::Key key) {
    ScreenLandscape::onKeyDown(key);
    if (mSecretSettingsCounter++ == 7) {
        app::showScreen(std::make_unique<ScreenSecretSettings>());
    }
}
