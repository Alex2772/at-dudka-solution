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


#include <functional>
#include "ScreenDialog.h"

class ScreenMessageDialog: public ScreenDialog {
public:
    ScreenMessageDialog(std::string message, std::function<void()> onConfirm): mMessage(std::move(message)), mOnConfirm(std::move(onConfirm)) {}

    void renderDialog(FramebufferImpl& fb) override;

    void onKeyDown(input::Key key) override;

    void setIcon(const uint8_t* icon) {
        mIcon = icon;
    }

protected:
    std::string mMessage;
    const std::uint8_t* mIcon;
    std::function<void()> mOnConfirm;
};


