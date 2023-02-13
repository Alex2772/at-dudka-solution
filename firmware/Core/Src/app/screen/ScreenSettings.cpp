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
#include "ScreenHelp.h"
#include <app/sram.h>
#include <app/screen/ScreenAbout.h>

using namespace std::chrono_literals;

extern const std::uint8_t image2cpp_arrow_up_png[];
extern const std::uint8_t image2cpp_arrow_down_png[];

template<typename T, typename Formatter>
std::shared_ptr<ScreenList::IItem> makeSetting(const char* name, Formatter&& formatter, T(sram::Config::* field), T min, T max, T step) {
    static_assert(std::is_invocable_r_v<std::string_view, Formatter, T>, "Formatter expected to be invokable! std::string_view(Formatter)(T)");
    using Field = decltype(field);
    struct ScreenSetting: ScreenLandscape {
        ScreenSetting(const char* name, Formatter formatter, T min, T max, T step, Field field) : name(name), formatter(std::move(formatter)),
                                                                                              min(min), max(max),
                                                                                              step(step), field(field) {}

        T& value() {
            return sram::config().*field;
        }

        void render(FramebufferImpl& fb) override {
            ScreenLandscape::render(fb);
            fb.string({64, 0}, Color::WHITE, name, FONT_FACE_TERMINUS_BOLD_8X14_KOI8_R, TextAlign::MIDDLE);

            constexpr glm::ivec2 CENTER = { 64, 40 };
            constexpr glm::ivec2 SIZE = { 60, 26 };
            constexpr int ARROW_SIZE = 10;

            fb.string(CENTER - glm::ivec2{0, 24 / 2}, Color::WHITE, formatter(value()), FONT_FACE_TERMINUS_BOLD_12X24_ISO8859_1, TextAlign::MIDDLE);

            fb.rect(CENTER - SIZE / 2 + glm::ivec2{1, 0}, { SIZE.x - 2, 1 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 * glm::ivec2{1, -1} + glm::ivec2{1, 0}, { SIZE.x - 2, 1 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 + glm::ivec2{1, -ARROW_SIZE}, { SIZE.x - 2, 1 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 * glm::ivec2{1, -1} + glm::ivec2{1, ARROW_SIZE}, { SIZE.x - 2, 1 }, Color::WHITE);

            fb.rect(CENTER - SIZE / 2 + glm::ivec2{0, 1 - ARROW_SIZE}, { 1, SIZE.y - 1 + ARROW_SIZE * 2 }, Color::WHITE);
            fb.rect(CENTER - SIZE / 2 * glm::ivec2{-1, 1} + glm::ivec2{-1, 1 - ARROW_SIZE}, { 1, SIZE.y - 1 + ARROW_SIZE * 2 }, Color::WHITE);

            if (value() != max) fb.image(CENTER - glm::ivec2{0, SIZE.y - ARROW_SIZE / 2 - 3}, image2cpp_arrow_up_png);
            if (value() != min) fb.image(CENTER + glm::ivec2{0, SIZE.y - ARROW_SIZE + 2}, image2cpp_arrow_down_png);

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
                    if (value() >= max) {
                        value() = max;
                    } else {
                        value() += step;
                        mChangeIndication = 1;
                    }
                    break;

                case input::Key::RIGHT:
                    if (value() <= min) {
                        value() = min;
                    } else {
                        value() -= step;
                        mChangeIndication = -1;
                    }
                    break;
            }
        }

    private:
        int mChangeIndication = 0;
        const char* name;
        Formatter formatter;
        T min, max, step;
        Field field;
    };

    return ScreenList::makeItem(name, [=]() {
        app::showScreen(std::make_unique<ScreenSetting>(name, formatter, min, max, step, field));
    }, [formatter, field]() {
        return formatter(sram::config().*field).data();
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
                    sram::config().*field = v;
                    closeAnimated();
                }, [] { return ""; }));
            }
            return l;
        }()) {
            const auto index = std::find(std::begin(enum_traits<T>::values), std::end(enum_traits<T>::values),
                                         sram::config().*field) - std::begin(enum_traits<T>::values);
            setSelectedItem(index);
        }
    };

    return ScreenList::makeItem(name, [=]() {
        app::showScreen(std::make_unique<ScreenSetting>(name, field));
    }, [field]() {
        return enum_traits<T>::name(sram::config().*field);
    });
}
template<>
std::shared_ptr<ScreenList::IItem> makeSetting(const char* name, bool(sram::Config::* field)) {
    return ScreenList::makeItem(name, [=]() {
        sram::config().*field = !(sram::config().*field);
    }, [field]() {
        return sram::config().*field ? "ON" : "OFF";
    });
}


struct ScreenSleepSettings: ScreenList {
public:
    ScreenSleepSettings(): ScreenList("Сон", {
        makeSetting("Тайм-аут", &sram::Config::sleepTimout),
    }) {}
};

template<typename T, typename... Args>
auto makeShowScreen(Args&&... args) {
    return [t = std::make_tuple(std::forward<Args>(args)...)] {
        app::showScreen(std::apply([](const auto&... args) {
            return std::make_unique<T>(args...);
        }, t));
    };
}

struct ScreenCooldownSettings: ScreenList {
public:
    ScreenCooldownSettings(): ScreenList("КД затяжек", {
        makeSetting("Включено", &sram::Config::cooldownEnabled),
        makeSetting("Длит.блок-вки", &sram::Config::cooldownDuration),
        makeSetting("Кол-во затяжек", [](auto v){ return util::format("%d", v); }, &sram::Config::cooldownThreshold, 1u, 100u, 1u),
        makeItem("Справка", makeShowScreen<ScreenHelp>("КД затяжек",
R"(Кулдаун затяжек
позволяет жеско
ограничивать
длительность
использования
устройства. По
достижении указанного
лимита, вейп
принудительно
отключается, а при
попытке запуска
выводит ошибку с
таймером, т.е. вейп
превращается в
кирпич. С помощью
этого режима можно
избежать состояния
обкуренности
(особенно с
никотинками).

ВНИМАНИЕ! Досрочный
выход из режима
"кирпича" не
предусмотрен!
Извлечение
аккумулятора не
поможет! В это время
вейп можно будет
разве что только в
жопу себе запихать
(вейперам не
привыкать).)")),
        //makeSetting("Тип блок-вки", ),
    }) {}
};

struct ScreenSoftStartSettings: ScreenList {
public:
    ScreenSoftStartSettings(): ScreenList("Плавный пуск", {
        makeSetting("Включено", &sram::Config::softStartEnabled),
        makeSetting("Длительность", [](auto v){ return util::format("%0.1fs", float(v.count()) / 1000.f); }, &sram::Config::softStartDuration, 100ms, 5'000ms, 100ms),
        makeItem("Справка", makeShowScreen<ScreenHelp>("Плавный пуск",
R"(Плавный пуск по
нажатию на кнопку
нагрева постепенно,
в течение указанного
времени поднимает
мощность, от нуля
до максимальной,
указанной в
настройках.

Это позволяет
избежать кашля и
тротхита (удара в
горло).)")),
        //makeSetting("Тип блок-вки", ),
    }) {}
};





ScreenSettings::ScreenSettings(): ScreenList("Настройки", {
    makeSetting("Макс. мощность", [](auto v){ return util::format("%dW", v); }, &sram::Config::maxPower, 4u, 100u, 2u),
    makeSetting("Тип койла", &sram::Config::material),
    makeItem("Плавный пуск", makeShowScreen<ScreenSoftStartSettings>()),
    makeItem("Сон", makeShowScreen<ScreenSleepSettings>()),
    makeItem("Кулдаун затяжек",  makeShowScreen<ScreenCooldownSettings>()),
    makeItem("Об устройстве", makeShowScreen<ScreenAbout>()),
}) {

}
