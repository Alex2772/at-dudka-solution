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

#pragma once

#include "ScreenLandscape.h"
#include <vector>
#include <memory>

class ScreenList: public ScreenLandscape {
public:
    struct IItem {
        virtual ~IItem() = default;
        virtual const char* getName() = 0;
        virtual const char* getValue() = 0;
        virtual void onClick() = 0;
    };

    template<typename OnClick>
    static std::shared_ptr<IItem> makeItem(const char* name, OnClick&& onClick) {
        struct Item: IItem {
            const char* name;
            OnClick click;

            Item(const char* name, OnClick click) : name(name), click(std::move(click)) {}

            ~Item() override = default;

            const char* getName() override {
                return name;
            }

            const char* getValue() override {
                return nullptr;
            }
            void onClick() override {
                click();
            }
        };
        return std::make_shared<Item>(name, std::forward<OnClick>(onClick));
    }

    template<typename OnClick, typename ValueCallback>
    static std::shared_ptr<IItem> makeItem(const char* name, OnClick&& onClick, ValueCallback&& valueCallback) {
        struct Item: IItem {
            const char* name;
            OnClick click;
            ValueCallback valueCallback;

            Item(const char* name, OnClick click, ValueCallback valueCallback) : name(name), click(std::move(click)), valueCallback(std::move(valueCallback)) {}

            ~Item() override = default;

            const char* getName() override {
                return name;
            }

            const char* getValue() override {
                return valueCallback();
            }
            void onClick() override {
                click();
            }
        };
        return std::make_shared<Item>(name, std::forward<OnClick>(onClick), std::forward<ValueCallback>(valueCallback));
    }

    using List = std::vector<std::shared_ptr<IItem>>;


    ScreenList(const char* title, List&& list) noexcept: mTitle(title), mList(std::move(list)) {}

    void render(FramebufferImpl& fb) override;

    void onKeyLongPressFrame(input::Key key) override;

    void onKeyDown(input::Key key) override;

    void setSelectedItem(int selectedItem) {
        mSelectedItem = selectedItem;
    }

private:
    List mList;
    const char* mTitle;
    int mSelectedItem = 0;
    int mScroll = 0;
    int mSmoothScroll = 0;
    int mSkipScrollAnimationFrames = 10;

    int itemPosYByIndex(int index);
};
