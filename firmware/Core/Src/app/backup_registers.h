#pragma once


#include <cstdint>
#include <array>

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_pwr.h>

namespace backup_registers {
    using Raw = std::array<std::uint32_t, 20>;

    static constexpr std::uint32_t MAGIC_NUMBER = 0xf0cb00ba;

    struct Config {
        /**
         * @brief Used to check for valid data
         */
        std::uint32_t magic = MAGIC_NUMBER;

        std::uint32_t puffCount = 0;
    };

    static_assert(sizeof(Config) <= sizeof(Raw));

    const Raw& raw();
    const Config& config();


    template<typename Visitor>
    void visit(Visitor&& visit) {
        HAL_PWR_EnableBkUpAccess();
        visit(const_cast<Config&>(config()));
        HAL_PWR_DisableBkUpAccess();
    }
}
