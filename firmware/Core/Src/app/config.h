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

#include <glm/glm.hpp>
#include <stm32f4xx.h>
#include <chrono>

namespace config {
    namespace calibrated {
        /**
         * @brief Calibration based on 2 reference measures.
         */
        struct CalibrationCurve  {
        public:
            constexpr CalibrationCurve(glm::vec2 adc, glm::vec2 voltmeter): adc(adc), voltmeter(voltmeter) {

            }

            float operator()(float in) const {
                return glm::mix(voltmeter[0], voltmeter[1], (in - adc[0]) / (adc[1] - adc[0]));
            }

        private:
            glm::vec2 adc;
            glm::vec2 voltmeter;
        };

        static constexpr CalibrationCurve BATTERY_VOLTAGE({2190.f, 2390.f},
                                                          {3.55f, 3.8f});

        static constexpr CalibrationCurve SHUNT_VOLTAGE({2190.f, 2395.f},
                                                  {3.549f, 3.818f});


        static constexpr CalibrationCurve COIL_VOLTAGE({64.f, 2396.f},
                                                       {0.21f, 3.76f});

        static constexpr CalibrationCurve SHUNT_CURRENT({0.056f, 0.207f},
                                                        {1.14f, 12.5f});

    }


    /**
     * @brief Time of adc measuring. The higher SHUTTER the higher accuracy.
     */
    static constexpr auto SHUTTER_BOOT = 100;
    static constexpr auto SHUTTER_DEFAULT = 4;

    static constexpr auto DEFAULT_TEMPERATURE = 30;

    using namespace std::chrono_literals;

    static constexpr auto AUTOSHUTDOWN_TIMEOUT = 120s;

    static constexpr auto INPUT_KEY_REPEAT_DELAY = 200;

    static constexpr auto CALIBRATION = false;

    namespace pin {
        struct Pin {
            enum GPIOx {
                A, B, C, D
            } mGpiox;
            int mNumber;


            constexpr Pin(GPIOx gpiox, int number) : mGpiox(gpiox), mNumber(number) {}


            constexpr GPIO_TypeDef* gpiox() const noexcept {
                switch (mGpiox) {
                    case A:
                        return GPIOA;
                    case B:
                        return GPIOB;
                    case C:
                        return GPIOC;
                    case D:
                        return GPIOD;
                }
            }

            constexpr int number() const noexcept {
                return mNumber;
            }


            bool digitalRead() const noexcept {
                return HAL_GPIO_ReadPin(gpiox(), number());
            }
        };

        static constexpr Pin JOY_DOWN  = { Pin::B, GPIO_PIN_15 };
        static constexpr Pin JOY_OK    = { Pin::B, GPIO_PIN_14 };
        static constexpr Pin JOY_RIGHT = { Pin::B, GPIO_PIN_13 };
        static constexpr Pin JOY_UP    = { Pin::B, GPIO_PIN_12 };
        static constexpr Pin JOY_LEFT  = { Pin::A, GPIO_PIN_8 };

    }
}