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

#include "ScreenLandscape.h"
#include "app/app.h"

ScreenLandscape::ScreenLandscape() {
    mAppearanceAnimation.play();
}

void ScreenLandscape::render(FramebufferImpl& fb) {
    bool paused = mAppearanceAnimation.isPaused();
    auto f = mAppearanceAnimation.nextFrame();
    fb.setTransform(glm::imat3x3({
         -1, 0, 0,
         0, -1, 0,
         (128 - 1) - f, (64 - 1), 1,
    }));
    if (!paused) {
        if (!mCloseAfterAnimation) {
            fb.shade();
        }
        fb.rect({-2, 0,}, {130, 64}, Color::BLACK);
        fb.rect({-1, 0,}, {1, 64}, Color::WHITE);
    }
    if (mCloseAfterAnimation && mAppearanceAnimation.currentFrame() == 0) {
        app::runOnUiThread([this] {
            close();
        });
    }
    fb.string({0, 1}, Color::WHITE, "<", FONT_FACE_TERMINUS_6X12_KOI8_R, TextAlign::LEFT);
}

bool ScreenLandscape::hasTransparency() {
    return !mAppearanceAnimation.isPaused();
}

void ScreenLandscape::onKeyDown(input::Key key) {
    IScreen::onKeyDown(key);
    if (key == input::Key::DOWN) {
        closeAnimated();
    }
}

void ScreenLandscape::closeAnimated() {
    mCloseAfterAnimation = true;
    mAppearanceAnimation.playReverse();
}
