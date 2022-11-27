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


#include <string_view>
#include <cstdio>

namespace util {

    /**
     * @brief Calculates coil temperature with TCR, reference temperature, resistance of the coil with the reference
     * temperature, current coil resistance.
     * @param tcr TCR value for the coil's material (i.e. SS316L is 0.000915)
     * @param referenceTemp coil's reference temperature.
     * @param referenceResistanceWithReferenceTemp coil's reference resistance with reference temperature.
     * @param currentResistance coil's current resistance
     * @return coil temperature.
     */
    float tcr(float tcr,
              float referenceTemp,
              float referenceResistanceWithReferenceTemp,
              float currentResistance);



    template<typename...Args>
    std::string_view format(const char* fmt, const Args&... args) {
        static char buf[128];
        return std::string_view(buf, std::sprintf(buf, fmt, args...));
    }

}


