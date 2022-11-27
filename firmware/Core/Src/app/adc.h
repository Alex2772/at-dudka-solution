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

namespace adc {
    void init();

    float batteryVoltage();
    float shuntVoltage();
    float coilVoltage();
    float current();

    /**
     * @return Coil resistance.
     * @details
     * Due to hardware limitations, we're unable to determine coil resistance while MOSFET is closed.
     */
    std::optional<float> coilResistance();


    struct CalibrationData {
        int batteryVoltage;
        int shuntVoltage;
        int coilVoltage;
    };

    CalibrationData calibrationData();

    void setShutter(unsigned s);
}


