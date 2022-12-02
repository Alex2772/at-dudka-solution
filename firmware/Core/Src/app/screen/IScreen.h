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

#pragma once

#include <ssd1306/Framebuffer.h>
#include "app/input.h"

class IScreen {
public:
    virtual ~IScreen() = default;

    virtual void render(FramebufferImpl& fb) = 0;
    virtual void onKeyDown(input::Key key);
    virtual void onKeyUp(input::Key key);
    virtual void onKeyLongPressFrame(input::Key key);

    virtual bool hasTransparency();

    void close();
};