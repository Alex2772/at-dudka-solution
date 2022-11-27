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


#include "stm32f4xx_hal.h"
#include "Framebuffer.h"

/**
 * @brief SSD1306 display driver.
 * @details
 * Unlike other implementation, this one follows the Single Responsibility Principle (SRP) - this class is responsible
 * only for controlling the display. For drawing function refer to Framebuffer class.
 */
class SSD1306 {
public:
    SSD1306(I2C_HandleTypeDef& i2c, std::uint8_t i2cAddress = 0x3C << 1);

    void push(const Framebuffer& framebuffer);

    void setDisplayEnabled(bool v);

private:
    I2C_HandleTypeDef& mI2C;
    std::uint8_t mI2cAddress;

    void writeCommand(std::uint8_t command);

    void setContrast(std::uint8_t contrast);

    void writeData(const uint8_t* data, size_t size);
};


