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

#include <stm32f4xx.h>

#include <cstdint>
#include <cstring>
#include "input.h"
#include "app.h"
#include "config.h"


std::array<bool, 8> gKeys;

std::uint32_t gLastSignalTime = 0;
std::uint32_t gLastKeyRepeatTime = 0;
input::Key gLastKey = input::Key::NONE;

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    gLastKeyRepeatTime = gLastSignalTime = HAL_GetTick();

    input::Key k;
    bool state;
    switch (GPIO_Pin) {
        case config::pin::JOY_UP.number():
            k = input::Key::UP;
            state = config::pin::JOY_UP.digitalRead();
            break;
        case config::pin::JOY_LEFT.number():
            k = input::Key::LEFT;
            state = config::pin::JOY_LEFT.digitalRead();
            break;
        case config::pin::JOY_RIGHT.number():
            k = input::Key::RIGHT;
            state = config::pin::JOY_RIGHT.digitalRead();
            break;
        case config::pin::JOY_DOWN.number():
            k = input::Key::DOWN;
            state = config::pin::JOY_DOWN.digitalRead();
            break;
        case config::pin::JOY_OK.number():
            k = input::Key::OK;
            state = config::pin::JOY_OK.digitalRead();
            break;

        default:
            return;
    }

    state = !state;

    app::runOnUiThread([=] {
        if (gKeys[int(k)] != state) {
            gKeys[int(k)] = state;
            gLastKey = k;
            if (state) {
                app::onKeyDown(k);
            } else {
                app::onKeyUp(k);
            }
        }
    });

}

void input::init() {
    std::memset(&gKeys, 0, sizeof(gKeys));
}

bool input::isKeyDown(input::Key key) {
    return gKeys[int(key)];
}

void input::frame() {
    if (HAL_GetTick() - gLastSignalTime > 1000) {
        if (isKeyDown(gLastKey)) {

            if (HAL_GetTick() - gLastKeyRepeatTime > config::INPUT_KEY_REPEAT_DELAY) {
                app::onKeyLongPressFrame(gLastKey);
                gLastKeyRepeatTime = HAL_GetTick() / config::INPUT_KEY_REPEAT_DELAY * config::INPUT_KEY_REPEAT_DELAY;
            }
        }
    }
}
