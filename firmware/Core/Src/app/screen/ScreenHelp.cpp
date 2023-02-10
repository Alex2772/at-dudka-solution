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

#include <algorithm>
#include <cstring>
#include "ScreenHelp.h"

ScreenHelp::ScreenHelp(const char* title, const char* helpStr) : mTitle(title), mHelpStr(helpStr) {
    mMaxScroll = 18 + (std::count(helpStr, helpStr + std::strlen(helpStr), '\n') + 1) * 12 - 64;
    if (mMaxScroll < 0) { mMaxScroll = 0; }
}

void ScreenHelp::render(FramebufferImpl& fb) {
    ScreenLandscape::render(fb);

    if (input::isKeyDown(input::Key::LEFT) || input::isKeyDown(input::Key::RIGHT)) { // overscroll effect
        mScroll = glm::clamp(mScroll, 0 - 8, mMaxScroll + 8);
    } else {
        mScroll = glm::clamp(mScroll, 0, mMaxScroll);
    }

    if (input::isKeyDown(input::Key::LEFT)) {
        mScroll -= 4;
    }

    if (input::isKeyDown(input::Key::RIGHT)) {
        mScroll += 4;
    }

    mSmoothScroll += (mScroll - mSmoothScroll) * 0.3;

    const int currentScroll = mSmoothScroll;

    if (currentScroll > 2) {
        fb.rect({0, 0}, {128, 1}, Color::WHITE);
    }
    if (currentScroll < mMaxScroll - 2) {
        fb.rect({0, 63}, {128, 1}, Color::WHITE);
    }

    fb.string({64, -currentScroll}, Color::WHITE, mTitle, FONT_FACE_TERMINUS_BOLD_8X14_KOI8_R, TextAlign::MIDDLE);
    fb.string({0, 18 - currentScroll}, Color::WHITE, mHelpStr, FONT_FACE_TERMINUS_6X12_KOI8_R);

    fb.rect({126, mSmoothScroll * (64 - 20) / mMaxScroll}, {2, 20}, Color::WHITE);
}

