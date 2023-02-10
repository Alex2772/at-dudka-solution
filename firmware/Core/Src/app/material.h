// /*
//  * Copyright (c) Alex2772, https://github.com/alex2772
//  *
//  * This program is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU General Public License as published by
//  * the Free Software Foundation, either version 3 of the License, or
//  * (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License
//  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  *

#pragma once
#include "enum_traits.h"

enum class Material {
    SS316L,
    SS304,
    SS317L,
    NICR80,
    NI200,
    KANTHAL,
    TITANIUMG1,
};

template<>
struct enum_traits<Material> {
    static constexpr Material values[] = {
            Material::KANTHAL     ,
            Material::NICR80      ,
            Material::NI200       ,
            Material::TITANIUMG1  ,
            Material::SS304       ,
            Material::SS316L      ,
            Material::SS317L      ,
            };
    static const char* name(Material value) {
        switch (value) {
            case Material::SS304: return "SS304";
            case Material::SS316L: return "SS316L";
            case Material::SS317L: return "SS317L";
            case Material::NICR80: return "NiCr80";
            case Material::KANTHAL: return "KANTHAL";
            case Material::NI200: return "NI200";
            case Material::TITANIUMG1: return "TiG1";
        }
    }
    static float tcr(Material value) {
        switch (value) {
            case Material::SS304: return 0.00105f;

            case Material::SS316L: return 0.000915f;
            case Material::SS317L: return 0.00095f;
            case Material::NICR80: return 0.0001f;
            case Material::KANTHAL: return 0.00005f;
            case Material::NI200: return 0.0053f;
            case Material::TITANIUMG1: return 0.0035f;
        }
    }
};