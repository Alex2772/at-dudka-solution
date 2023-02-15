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


#include <cstdint>
#include <array>
#include <cassert>
#include <cstring>
#include "sram.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_flash.h"




namespace {
    struct ConfigWithCrc {
        unsigned crc;
        sram::Config config;
    };

    __attribute__((__section__(".user_data"))) std::aligned_storage<sizeof(ConfigWithCrc), alignof(ConfigWithCrc)> gFlashConfig;

    const ConfigWithCrc& flashConfig() {
        return reinterpret_cast<ConfigWithCrc&>(gFlashConfig);
    }
}


bool operator==(const sram::Config& lhs, const sram::Config& rhs) {
    return std::memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}


extern "C" CRC_HandleTypeDef hcrc;

static unsigned computeCrc(const sram::Config& cfg) {
    unsigned crc = 0;
    for (const auto b : reinterpret_cast<const std::array<std::uint8_t, sizeof(cfg)>&>(cfg)) {
        crc ^= b;
        crc <<= 1;
    }
    return crc;
}

ConfigWithCrc& ramConfigWithCrc() {
    static ConfigWithCrc configWithCrc = [] {
        if (computeCrc(flashConfig().config) == flashConfig().crc &&  // hashsum check
            flashConfig().config.sizeOfConfig == sizeof(sram::Config) // version check
            ) {
            return flashConfig();
        }
        return ConfigWithCrc{};
    }();

    return configWithCrc;
}

sram::Config& sram::config() {
    return ramConfigWithCrc().config;
}

void sram::save() {
    if (config() == flashConfig().config) {
        return;
    }

    // unlock memory
    if (HAL_FLASH_Unlock() != HAL_OK) {
        assert(0);
    }

    FLASH_Erase_Sector(1, FLASH_VOLTAGE_RANGE_3);

    config().writeCounter += 1;

    ramConfigWithCrc().crc = computeCrc(config());

    auto sourceBegin = reinterpret_cast<std::uint32_t*>(&ramConfigWithCrc());
    auto sourceEnd = sourceBegin + (sizeof(ramConfigWithCrc())) / sizeof(std::uint32_t);

    for (std::uint32_t dst = reinterpret_cast<std::uint32_t>(&flashConfig()), *src = sourceBegin; src != sourceEnd; ++src, dst += sizeof(std::uint32_t)) {
        HAL_FLASH_Program(TYPEPROGRAM_WORD, dst, *src);
    }

    HAL_FLASH_Lock();

    auto flashCrc = computeCrc(flashConfig().config);
    assert(flashCrc == flashConfig().crc);
}

