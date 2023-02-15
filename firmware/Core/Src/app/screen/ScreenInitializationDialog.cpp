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
// Created by Alex2772 on 2/15/2023.
//

#include "ScreenInitializationDialog.h"
#include "app/app.h"
#include "app/adc.h"

extern const std::uint8_t image2cpp_coil_png[];

ScreenInitializationDialog::ScreenInitializationDialog(): ScreenConfirmDialog("Инициализировать?", []{}) {
    setIcon(image2cpp_coil_png);
}

void ScreenInitializationDialog::renderDialog(FramebufferImpl& fb) {
    if (mInitializationStep) {
        fb.string({WIDTH / 2, 20}, Color::WHITE, "Инициализация...", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);

        /*
         * app::initCoil is a blocking function; should defer the initialization to next frame in order to initialization
         * message to be drawn
         */
        app::runOnUiThread([this] {
            app::initCoil();
            close();
        });

    } else {
        ScreenMessageDialog::renderDialog(fb);
    }
}

void ScreenInitializationDialog::beginInitializationStep() {
    mInitializationStep = true;
}

void ScreenInitializationDialog::onKeyDown(input::Key key) {
    if (key == input::Key::OK) {
        beginInitializationStep();
        return;
    }
    ScreenMessageDialog::onKeyDown(key);
}
