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
// Created by Alex2772 on 2/9/2023.
//

#include "ScreenList.h"
#include <glm/gtc/matrix_transform.hpp>

int ScreenList::itemPosYByIndex(int index) {
    return index * 17 + 14;
}

void ScreenList::render(FramebufferImpl& fb) {
    ScreenLandscape::render(fb);

    mScroll = itemPosYByIndex(mSelectedItem) - 14;

    if (mAppearanceAnimation.isPaused()) {
        if (mSkipScrollAnimationFrames > 0) {
            --mSkipScrollAnimationFrames;
        } else {
            mSmoothScroll += (mScroll - mSmoothScroll) * 0.5f;
        }
    }
    const int currentScroll = mSmoothScroll;

    if (currentScroll > 14) {
        fb.rect({0, 0,}, {128, 1}, Color::WHITE);
    }

    if (currentScroll + 64 < itemPosYByIndex(mList.size())) {
        fb.rect({0, 63,}, {128, 1}, Color::WHITE);
    }

    fb.string({64, -currentScroll}, Color::WHITE, mTitle, FONT_FACE_TERMINUS_BOLD_8X14_KOI8_R, TextAlign::MIDDLE);

    for (int i = 0; i < mList.size(); ++i) {
        const auto pos = glm::ivec2{0, itemPosYByIndex(i) - currentScroll};
        if (pos.y < -15) {
            continue;
        }
        if (pos.y >= 64) {
            break;
        }
        if (i != 0) fb.rect(pos + glm::ivec2{1, 0}, {126, 1}, Color::WHITE);
        fb.string(pos + glm::ivec2{4, 3}, Color::WHITE, mList[i]->getName(), FONT_FACE_TERMINUS_6X12_KOI8_R);
        if (auto val = mList[i]->getValue()) {
            fb.string(pos + glm::ivec2{128 - 3, 4}, Color::WHITE, val, FONT_FACE_BITOCRA_6X11, TextAlign::RIGHT);
        } else {
            fb.string(pos + glm::ivec2{128 - 3, 6}, Color::WHITE, ">", FONT_FACE_BITOCRA_4X7, TextAlign::RIGHT);
        }

        if (i == mSelectedItem) {
            fb.rect(pos + glm::ivec2{0, 1}, {128, 16}, Color::INVERT);
        }
    }
}

void ScreenList::onKeyDown(input::Key key) {
    ScreenLandscape::onKeyDown(key);

    if (key == input::Key::OK) {
        if (!mAppearanceAnimation.isPaused()) {
            return;
        }
        mList[mSelectedItem]->onClick();
        return;
    }

    onKeyLongPressFrame(key);
}

void ScreenList::onKeyLongPressFrame(input::Key key) {
    switch (key) {
        case input::Key::LEFT:
            mSelectedItem -= 1;
            if (mSelectedItem < 0){
                mSelectedItem = mList.size() - 1;
            }
            mSkipScrollAnimationFrames = 0;
            break;

        case input::Key::RIGHT:
            mSelectedItem += 1;
            if (mSelectedItem >= mList.size()){
                mSelectedItem = 0;
            }
            mSkipScrollAnimationFrames = 0;
            break;
    }
}
