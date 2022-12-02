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

#include "ScreenDialog.h"

void ScreenDialog::onKeyLongPressFrame(input::Key key) {
    IScreen::onKeyLongPressFrame(key);
    if (key == input::Key::LEFT) {
        close();
    }
}

void ScreenDialog::render(FramebufferImpl& fb) {
    fb.shade();

    constexpr glm::ivec2 SIZE = { 64 - 8, 128 - 24 };
    const glm::ivec2 POSITION = (glm::ivec2{fb.size().y, fb.size().x} - SIZE) / 2;

    fb.rect(POSITION - glm::ivec2(3), SIZE + glm::ivec2(3 * 2), Color::BLACK);
    fb.rectBorder(POSITION - glm::ivec2(2), SIZE + glm::ivec2(2 * 2), Color::WHITE);

    Framebuffer<SIZE.x, SIZE.y> dialogFb;
    dialogFb.clear();


    dialogFb.setTransform(glm::imat3x3({
       0, -1,  0,
       1, 0,  0,
       0, SIZE.y - 1,  0,
    }));

    renderDialog(dialogFb);

    fb.frameBuffer(POSITION, SIZE, dialogFb);
}


bool ScreenDialog::hasTransparency() {
    return true;
}
