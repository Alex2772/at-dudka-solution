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

class FramebufferImpl {
public:
    void clear();

    /**
     * @brief Draw a pixel.
     * @param position position
     * @param color color
     * @param bits bits used for image drawing.
     */
    void pixel(glm::uvec2 position, Color color, std::uint8_t bits = 1) {
        position = mTransform * glm::ivec3(position, 1);
        if (glm::any(glm::greaterThanEqual(position, glm::uvec2(mSize)))) {
            return;
        }

        const auto index = position.x + (position.y / 8) * mSize.x;
        const auto bit = (bits << (position.y & 7));
        auto& byte = mStorage[index];

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

    bool getPixel(glm::ivec2 position) const noexcept {
        auto transformed = glm::uvec2(position);

        if (glm::any(glm::greaterThanEqual(transformed, glm::uvec2(mSize)))) {
            return false;
        }

        return (mStorage[transformed.y / 8 * mSize.x + transformed.x] >> (transformed.y & 7)) & 1;
    }

    void rect(glm::ivec2 position, glm::ivec2 size, Color color);
    void rectBorder(glm::ivec2 position, glm::ivec2 size, Color color);
    void roundedRect(glm::ivec2 position, glm::ivec2 size, Color color);

    int string(glm::ivec2 position, Color color, std::string_view string, font_face_t font, TextAlign align = TextAlign::LEFT);

    static int stringLength(std::string_view string, font_face_t font);

    void image(glm::ivec2 position, const std::uint8_t* data);

    void setTransform(const glm::imat3x3& transform) {
        mTransform = transform;
    }

    void frameBuffer(glm::ivec2 position, glm::ivec2 size, const FramebufferImpl& fb);

    void shade();


    glm::ivec2 size() const noexcept {
        return mSize;
    }

    glm::ivec2 transformedSize() const noexcept {
        return mTransform * glm::ivec3(mSize, 1);
    }

    const uint8_t* data() const noexcept {
        return mStorage;
    }

    void verticalOrientation() {
        setTransform(glm::imat3x3({
              0, -1,  0,
              1,  0,  0,
              0, 63,  1,
        }));
    }

    const glm::imat3x3& transform() {
        return mTransform;
    }

protected:
    FramebufferImpl(std::uint8_t* storage, glm::ivec2 size): mStorage(storage), mSize(size) {
        clear();
    }

private:
    glm::imat3x3 mTransform = glm::imat3x3(1);
    std::uint8_t* mStorage;
    glm::ivec2 mSize;
};


template<int width = 128, int height = 64>
class Framebuffer: public FramebufferImpl {
public:
    Framebuffer(): FramebufferImpl(mData.data(), { width, height }) {}

private:
    std::array<std::uint8_t, width * height / 8> mData;
};

