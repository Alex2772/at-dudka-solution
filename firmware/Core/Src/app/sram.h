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
        /*
         * Common settings
         */
        unsigned maxTemperature = 100;        // 100 is neutral temperature; big enough to provide taste and small
                                              // enough to avoid overheat

        unsigned maxPower = 10;               // 10 is the most comfortable power; no throat hit
        bool lock = false;
        Material material = Material::SS316L; // popular material for TC
        SleepDuration sleepTimout = SleepDuration::k2MIN;

        /*
         * Cooldown settings
         */
        bool cooldownEnabled = false;
        CooldownDuration cooldownDuration = CooldownDuration::k1MIN;
        unsigned cooldownThreshold = 10;

        /**
         * If set, the device is being blocked by cooldown feature. The value stores timepoint which real time clock
         * (RTC) should reach to unblock the device.
         */
        std::optional<std::chrono::seconds> cooldownNextUnlock = std::nullopt;
    };

    /**
     * @return Config structure stored in RAM.
     * @details
     * If modified, the config from RAM is dumped to FLASH memory on shutdown.
     */
    Config& config();

    /**
     * @brief Stores config from RAM to FLASH memory.
     */
    void save();
}


