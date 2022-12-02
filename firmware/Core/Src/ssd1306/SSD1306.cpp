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



#include <cassert>
#include <cstdio>
#include "SSD1306.h"

SSD1306::SSD1306(I2C_HandleTypeDef& i2c, std::uint8_t i2cAddress) :
        mI2C(i2c), mI2cAddress(i2cAddress) {

    HAL_Delay(100);
    setDisplayEnabled(false);

    writeCommand(0x20); //Set Memory Addressing Mode
    writeCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
    // 10b,Page Addressing Mode (RESET); 11b,Invalid

    writeCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

    writeCommand(0xC8); //Set COM Output Scan Direction

    writeCommand(0x00); //---set low column address
    writeCommand(0x10); //---set high column address

    writeCommand(0x40); //--set start line address - CHECK

    setContrast(0xFF);

    writeCommand(0xA1); //--set segment re-map 0 to 127 - CHECK

    writeCommand(0xA6); //--set normal color


    writeCommand(0xFF); // width = 128

    writeCommand(0x3F); // height = 64

    writeCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    writeCommand(0xD3); //-set display offset - CHECK
    writeCommand(0x00); //-not offset

    writeCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    writeCommand(0xF0); //--set divide ratio

    writeCommand(0xD9); //--set pre-charge period
    writeCommand(0x22); //

    writeCommand(0xDA); //--set com pins hardware configuration - CHECK

    writeCommand(0x12); // height = 64

    writeCommand(0xDB); //--set vcomh
    writeCommand(0x20); //0x20,0.77xVcc

    writeCommand(0x8D); //--set DC-DC enable
    writeCommand(0x14); //
    setDisplayEnabled(true); //--turn on SSD1306 panel

}

void SSD1306::push(const Framebuffer<>& framebuffer) {

    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < 64/8; i++) {
        writeCommand(0xB0 + i); // Set the current RAM page address.
        writeCommand(0x00 + 0);
        writeCommand(0x10 + 0);
        writeData(&framebuffer.data()[128*i], 128);
    }
}

void SSD1306::setDisplayEnabled(bool v) {
    writeCommand(v ? 0xAF : 0xAE);
}

void SSD1306::writeCommand(std::uint8_t command) {
    for (int i = 0; i < 10; ++i) {
        auto s = HAL_I2C_Mem_Write(&mI2C, mI2cAddress, 0x00, 1, &command, 1, HAL_MAX_DELAY);
        if (s == HAL_OK) {
            return;
        }
        HAL_Delay(100);
    }

    assert(("i2c failed", false));
}

void SSD1306::writeData(const std::uint8_t* data, std::size_t size) {
    auto s = HAL_I2C_Mem_Write(&mI2C, mI2cAddress, 0x40, 1, (uint8_t*) data, size, HAL_MAX_DELAY);
    assert(s == HAL_OK);
}

void SSD1306::setContrast(std::uint8_t contrast) {
    writeCommand(0x81);
    writeCommand(contrast);
}
