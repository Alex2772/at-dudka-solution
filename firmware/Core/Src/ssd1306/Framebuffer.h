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


#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include "fonts.h"


enum class Color {
    BLACK,
    WHITE,
    INVERT,
};

enum class TextAlign {
    LEFT,
    MIDDLE,
    RIGHT
};

class Framebuffer {
public:
    static constexpr auto WIDTH = 128;
    static constexpr auto HEIGHT = 64;
    static constexpr auto SIZE = glm::uvec2{WIDTH};


    Framebuffer();

    const auto& data() const noexcept {
        return mData;
    }

    auto& data() noexcept {
        return mData;
    }

    void clear();

    /**
     * @brief Draw a pixel.
     * @param position position
     * @param color color
     * @param bits bits used for image drawing.
     */
    void pixel(glm::uvec2 position, Color color, std::uint8_t bits = 1) {
        position = mTransform * glm::ivec3(position, 1);
        if (glm::any(glm::greaterThanEqual(position, SIZE))) {
            return;
        }

        const auto index = position.x + (position.y / 8) * WIDTH;
        const auto bit = (bits << (position.y & 7));
        auto& byte = mData[index];

        switch (color) {
            case Color::BLACK:
                byte &= ~bit;
                break;
            case Color::WHITE:
                byte |= bit;
                break;
            case Color::INVERT:
                byte ^= bit;
                break;
        }

    }

    void string(glm::ivec2 position, Color color, std::string_view string, font_face_t font, TextAlign align = TextAlign::LEFT);

    void setTransform(const glm::imat3x3& transform) {
        mTransform = transform;
    }

private:
    std::array<std::uint8_t, WIDTH * HEIGHT> mData;

    glm::imat3x3 mTransform = glm::imat3x3(1);
};


