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
#include <app/SettingsUtil.h>

using namespace std::chrono_literals;


struct ScreenSleepSettings: ScreenList {
public:
    ScreenSleepSettings(): ScreenList("Сон", {
        makeSetting("Тайм-аут", &sram::Config::sleepTimout),
    }) {}
};

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
    makeSetting("Макс. мощность", [](auto v){ return util::format("%dW", v); }, &sram::Config::maxPower, 4u, 200u, 2u),
    makeSetting("М-риал койла", &sram::Config::material),
    makeItem("Плавный пуск", makeShowScreen<ScreenSoftStartSettings>()),
    makeItem("Сон", makeShowScreen<ScreenSleepSettings>()),
    makeItem("Кулдаун затяжек", makeShowScreen<ScreenCooldownSettings>()),
    makeItem("Об устройстве", makeShowScreen<ScreenAbout>()),
}) {

}
