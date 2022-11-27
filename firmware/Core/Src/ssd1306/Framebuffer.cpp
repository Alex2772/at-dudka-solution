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



#include <cstring>
#include <vector>
#include <numeric>
#include "Framebuffer.h"

Framebuffer::Framebuffer() {
    clear();
}


void Framebuffer::clear() {
    std::memset(mData.data(), 0, sizeof(mData));
}
static char toKOI8(char c) {
    const char алфавитБожественногоЯзыка[] = {
            1, // a
            2, // b
            23, // v
            7, // g
            4, // d
            5, // e
            22, // zh
            26, // z
            9, // i
            10, // y
            11, // k
            12, // l
            13, // m
            14, // n
            15, // o
            16, // p
            18, // r
            19, // s
            20, // t
            21, // y
            6, // f
            8, // h
            3, // c
            30, // ch
            27, // sh
            29, // sh'
            31, // ]
            25, // y
            24, // m
            28, //
            0, // u
            17, // ya
    };
    char v = c-140-4;
    char ret = алфавитБожественногоЯзыка[v%32] + 192;
    if (v < 32) {
        ret += 32;
    }
    return ret;
}

static std::vector<const font_char_desc_t*> compileString(const font_info_t* info, std::string_view view, bool isKoi8) {
    std::vector<const font_char_desc_t*> output;
    output.reserve(view.size());

    bool utf8 = false;

    for (auto c : view) {
        if (isKoi8) {
            if (utf8) {
                utf8 = false;
                c = toKOI8(c);
            } else if (c == 0b11010000 || c == 0b11010001) {
                // utf8 character; process russian symbols
                utf8 = true;
                continue;
            }
        }
        if (auto desc = font_get_char_desc(info, c)) {
            output.push_back(desc);
        }
    }
    return output;
}

void Framebuffer::string(glm::ivec2 position, Color color, std::string_view string, font_face_t font, TextAlign align) {
    const auto info = font_builtin_fonts[font];
    const auto compiledString = compileString(info, string, font == FONT_FACE_TERMINUS_6X12_KOI8_R);

    if (align != TextAlign::LEFT) {
        const auto stringWidth = std::accumulate(compiledString.begin(), compiledString.end(), 0, [](std::size_t lhs, const font_char_desc_t* rhs) {
            return lhs + rhs->width;
        });

        switch (align) {
            case TextAlign::MIDDLE:
                position.x -= stringWidth / 2;
                break;
            case TextAlign::RIGHT:
                position.x -= stringWidth;
                break;
        }
    }

    for (auto compiledChar : compiledString) {
        const auto charBitmap = info->bitmap + std::uint32_t(compiledChar->offset);

        const std::size_t charWidthInBytes = (compiledChar->width + 7) / 8;
        for (auto charY = 0; charY < info->height; ++charY) {
            for (std::size_t byteIndex = 0; byteIndex < charWidthInBytes; ++byteIndex) {
                unsigned charX = 0;
                for (auto line = charBitmap[charY * charWidthInBytes + byteIndex]; line != 0; line <<= 1, ++charX) {
                    if (line & 0b1000'0000) {
                        pixel(position + glm::ivec2(charX + byteIndex * 8, charY), color);
                    }
                }
            }
        }
        position.x += compiledChar->width;
    }
}
