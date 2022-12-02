#  /*
#   * Copyright (c) Alex2772, https://github.com/alex2772
#   *
#   * This program is free software: you can redistribute it and/or modify
#   * it under the terms of the GNU General Public License as published by
#   * the Free Software Foundation, either version 3 of the License, or
#   * (at your option) any later version.
#   *
#   * This program is distributed in the hope that it will be useful,
#   * but WITHOUT ANY WARRANTY; without even the implied warranty of
#   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   * GNU General Public License for more details.
#   *
#   * You should have received a copy of the GNU General Public License
#   * along with this program.  If not, see <http://www.gnu.org/licenses/>.
#   */
import importlib
import os.path
import site
import sys

import pip
pip.main(["install", "gil"])
importlib.reload(site)

from PIL import Image


source = sys.argv[1]
destination = sys.argv[2]


img = Image.open(source)

width = img.size[0]
height = img.size[1]
pixels = img.load()

with open(destination, 'w') as fos:
    fos.write("#pragma once\n")
    fos.write("#include <cstdint>\n")

    varName = "image2cpp_" + os.path.basename(source).replace(".", "_")

    fos.write("extern const std::uint8_t ")
    fos.write(varName)

    fos.write("[];\nconst std::uint8_t ")
    fos.write(varName)
    fos.write("[] = {")

    fos.write(hex(width))
    fos.write(",")
    fos.write(hex(height))
    fos.write(",")

    for y in range(0, height, 8):
        for x in range(width):
            byte = 0
            for i in range(8):
                byte >>= 1
                if y + i >= height:
                    break

                if pixels[x, y + i] > 128:
                    byte |= 0b1000_0000
            fos.write(hex(byte))
            fos.write(",")
        fos.write("\n")

    fos.write("};")

print(f'{source} -> {destination}')
