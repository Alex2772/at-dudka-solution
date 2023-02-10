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

#include "ScreenSettings.h"
#include "app/app.h"
#include "app/util.h"
#include <app/sram.h>
#include <app/screen/ScreenAbout.h>

extern const std::uint8_t image2cpp_arrow_up_png[];
extern const std::uint8_t image2cpp_arrow_down_png[];

template<typename T>
std::shared_ptr<ScreenList::IItem> makeSetting(const char* name, const char* fmt, T(sram::Config::* field), T min, T max, T step) {
    using Field = decltype(field);
    struct ScreenSetting: ScreenLandscape {
        ScreenSetting(const char* name, const char* fmt, T min, T max, T step, Field field) : name(name), fmt(fmt),
                                                                                              min(min), max(max),
                                                                                              step(step), field(field) {}

        T& value() {
            return sram::ram().*field;
        }

        void render(FramebufferImpl& fb) override {
            ScreenLandscape::render(fb);
            fb.string({64, 0}, Color::WHITE, name, FONT_FACE_TERMINUS_BOLD_8X14_KOI8_R, TextAlign::MIDDLE);

            constexpr glm::ivec2 CENTER = { 64, 40 };
            constexpr glm::ivec2 SIZE = { 60, 26 };
            constexpr int ARROW_SIZE = 10;

            fb.string(CENTER - glm::ivec2{0, 24 / 2}, Color::WHITE, util::format(fmt, value()), FONT_FACE_TERMINUS_BOLD_12X24_ISO8859_1, TextAlign::MIDDLE);

            fb.rect(CENTER - SIZE / 2 + glm::ivec2{1, 0}, { SIZE.x - 2, 1 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 * glm::ivec2{1, -1} + glm::ivec2{1, 0}, { SIZE.x - 2, 1 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 + glm::ivec2{1, -ARROW_SIZE}, { SIZE.x - 2, 1 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 * glm::ivec2{1, -1} + glm::ivec2{1, ARROW_SIZE}, { SIZE.x - 2, 1 }, Color::WHITE);

            fb.rect(CENTER - SIZE / 2 + glm::ivec2{0, 1 - ARROW_SIZE}, { 1, SIZE.y - 1 + ARROW_SIZE * 2 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 * glm::ivec2{-1, 1} + glm::ivec2{-1, 1 - ARROW_SIZE}, { 1, SIZE.y - 1 + ARROW_SIZE * 2 }, Color::WHITE);

            if (value() != max) fb.image(CENTER - glm::ivec2{0, SIZE.y - ARROW_SIZE / 2}, image2cpp_arrow_up_png);
            if (value() != min) fb.image(CENTER + glm::ivec2{0, SIZE.y - ARROW_SIZE }, image2cpp_arrow_down_png);

            if (mChangeIndication == 1) {
                fb.rect(CENTER - SIZE / 2 + glm::ivec2{1, 1 - ARROW_SIZE}, glm::ivec2{SIZE.x - 2, ARROW_SIZE - 1}, Color::INVERT);
            } else if (mChangeIndication == -1) {
                fb.rect(CENTER + glm::ivec2{-SIZE.x, SIZE.y} / 2 + glm::ivec2{1, 1}, glm::ivec2{SIZE.x - 2, ARROW_SIZE - 1}, Color::INVERT);
            }
            mChangeIndication = 0;
        }

        void onKeyDown(input::Key key) override {
            ScreenLandscape::onKeyDown(key);
            handleKey(key);
        }

        void onKeyLongPressFrame(input::Key key) override {
            IScreen::onKeyLongPressFrame(key);
            handleKey(key);
        }

        void handleKey(input::Key key) {
            switch (key) {
                case input::Key::LEFT:
                    value() += step;
                    if (value() > max) {
                        value() = max;
                    } else {
                        mChangeIndication = 1;
                    }
                    break;

                case input::Key::RIGHT:
                    value() -= step;
                    if (value() < min) {
                        value() = min;
                    } else {
                        mChangeIndication = -1;
                    }
                    break;
            }
        }

    private:
        int mChangeIndication = 0;
        const char* name, *fmt;
        T min, max, step;
        Field field;
    };

    return ScreenList::makeItem(name, [=]() {
        app::showScreen(std::make_unique<ScreenSetting>(name, fmt, min, max, step, field));
    }, [fmt, field]() {
        return util::format(fmt, sram::ram().*field).data();
    });
}


template<typename T>
std::shared_ptr<ScreenList::IItem> makeSetting(const char* name, T(sram::Config::* field)) {
    using Field = decltype(field);
    struct ScreenSetting: ScreenList {
        ScreenSetting(const char* name, Field field): ScreenList(name, [&] {
            ScreenList::List l;
            l.reserve(std::size(enum_traits<T>::values));
            for (auto v : enum_traits<T>::values) {
                l.push_back(makeItem(enum_traits<T>::name(v), [this, field, v]() {
                    sram::ram().*field = v;
                    closeAnimated();
                }, [] { return ""; }));
            }
            return l;
        }()) {
            const auto index = std::find(std::begin(enum_traits<T>::values), std::end(enum_traits<T>::values), sram::ram().*field) - std::begin(enum_traits<T>::values);
            setSelectedItem(index);
        }
    };

    return ScreenList::makeItem(name, [=]() {
        app::showScreen(std::make_unique<ScreenSetting>(name, field));
    }, [field]() {
        return enum_traits<T>::name(sram::ram().*field);
    });
}



ScreenSettings::ScreenSettings(): ScreenList("Настройки", {
    makeSetting("Макс. мощность", "%dW", &sram::Config::maxPower, 4u, 100u, 2u),
    makeSetting("Тип койла", &sram::Config::material),
    makeItem("Спящий режим", [] {} ),                                           
    makeItem("Кулдаун затяжек", [] {} ),
    makeItem("Об устройстве", [] { app::showScreen(std::make_unique<ScreenAbout>()); } ),
}) {

}
