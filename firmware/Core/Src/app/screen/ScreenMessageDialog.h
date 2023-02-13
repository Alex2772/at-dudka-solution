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
#include <memory>
#include "ScreenDialog.h"

class ScreenMessageDialog: public ScreenDialog {
public:

    struct Action {
        input::Key key;
        const char* name;
        std::function<void()> onSelected;
    };

    using Actions = std::vector<Action>;

    void renderDialog(FramebufferImpl& fb) override;

    void onKeyDown(input::Key key) override;

    void setIcon(const uint8_t* icon) {
        mIcon = icon;
    }


    static std::unique_ptr<ScreenMessageDialog> make(std::string message, Actions actions = {{ input::Key::OK, "Закрыть", []{} }}) {
        return std::unique_ptr<ScreenMessageDialog>(new ScreenMessageDialog(std::move(message), std::move(actions)));
    }

protected:
    std::string mMessage;
    const std::uint8_t* mIcon;
    Actions mActions;

    ScreenMessageDialog(std::string message, Actions actions): mMessage(std::move(message)), mActions(std::move(actions)) {}

    const uint8_t* getKeyImage(input::Key key);
};


