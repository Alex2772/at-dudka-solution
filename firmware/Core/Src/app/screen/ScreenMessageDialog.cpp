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


#include "ScreenMessageDialog.h"

void ScreenMessageDialog::renderDialog(FramebufferImpl& fb) {
    //fb.pixel({0, 1}, Color::WHITE);
    const auto middle = fb.transformedSize().x / 2;

    if (mIcon) {
        fb.image({middle, 16}, mIcon);
    }

    fb.string({middle, 30}, Color::WHITE, mMessage, FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);
}

void ScreenMessageDialog::onKeyDown(input::Key key) {
    ScreenDialog::onKeyDown(key);

    switch (key) {
        case input::Key::LEFT:
        case input::Key::OK:
            mOnConfirm();
            close();
            break;
    }
}
