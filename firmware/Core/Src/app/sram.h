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


#include <optional>
#include "material.h"
#include "duration_enum.h"

namespace sram {

    struct Config {
        unsigned maxTemperature = 210;
        bool lock = false;
        unsigned maxPower = 100;
        Material material = Material::SS316L;
        SleepDuration sleepTimout = SleepDuration::k3MIN;
        bool cooldownEnabled = false;
        CooldownDuration cooldownDuration = CooldownDuration::k1MIN;
        unsigned cooldownThreshold = 10;
    };

    Config& ram();

    void save();
}


