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


#include <numeric>
#include "ScreenMessageDialog.h"

extern const std::uint8_t image2cpp_left_png[];
extern const std::uint8_t image2cpp_right_png[];
extern const std::uint8_t image2cpp_up_png[];
extern const std::uint8_t image2cpp_down_png[];
extern const std::uint8_t image2cpp_ok_png[];

void ScreenMessageDialog::renderDialog(FramebufferImpl& fb) {
    //fb.pixel({0, 1}, Color::WHITE);
    const auto middle = fb.transformedSize().x / 2;

    if (mIcon) {
        fb.image({middle, 16}, mIcon);
    }

    fb.string({middle, 28}, Color::WHITE, mMessage, FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::MIDDLE);

    const int buttonPanelWidth = std::accumulate(mActions.begin(), mActions.end(), 0, [](int v, const Action& c) {
        return v + FramebufferImpl::stringLength(c.name, FONT_FACE_TERMINUS_6X12_KOI8_R) + 11 + 4;
    });
    int pos = (WIDTH - buttonPanelWidth) / 2;
    for (const auto& a : mActions) {
        fb.image({pos + 5, 42 + 6},  getKeyImage(a.key));
        pos += 11;
        pos += fb.string({ pos, 42}, Color::WHITE, a.name, FONT_FACE_TERMINUS_6X12_KOI8_R) + 4;
    }
}



void ScreenMessageDialog::onKeyDown(input::Key key) {
    ScreenDialog::onKeyDown(key);

    if (auto it = std::find_if(mActions.begin(), mActions.end(), [&](const auto& v) { return v.key == key; }); it != mActions.end()) {
        it->onSelected();
        close();
    }
}

const uint8_t* ScreenMessageDialog::getKeyImage(input::Key key) {
    // rotated key images
    switch (key) {
        case input::Key::DOWN: return image2cpp_left_png;
        case input::Key::UP: return image2cpp_right_png;
        case input::Key::LEFT: return image2cpp_up_png;
        case input::Key::RIGHT: return image2cpp_down_png;
        case input::Key::OK: return image2cpp_ok_png;
    }
    return nullptr;
}
