import os
from PIL import Image
import json
import argparse

FONT_MAP_FILENAME = "font_map.json"

BITMAP_SIZE = 8

arg_parser = argparse.ArgumentParser(prog="Bitmaps generator",
                                     description="Generates C header file with bitmap arrays\
                                                  from images and text")

arg_parser.add_argument("-o", "--bitmaps-header",  default="./firmware/include/bitmaps.h")
arg_parser.add_argument("-d", "--img-dir", default="./bitmap_img")
arg_parser.add_argument("-f", "--fonts-dir", default="./bitmap_img/fonts")

args = arg_parser.parse_args()

def gen_bitmaps_check_dimensions(filename, width, height, bmap_width) -> bool:
    if width <= 0:
        print("[ERROR] " + filename + ": width must be greater than 0 instead of %d" % width)
        return False

    if height != BITMAP_SIZE:
        print("[ERROR] " + filename + ": height must be %d instead of %d" % (BITMAP_SIZE, height))
        return False

    if width < bmap_width:
        print("[ERROR] " + filename + ": width must be at least %d instead of %d" % (bmap_width, width))
        return False

    if width % bmap_width != 0:
        print("[WARN] " + filename + ": width must be a multiple of %d instead of %d" % (bmap_width, width))

    return True

def gen_bitmaps_from_pixels(pixels, width, bmap_width) -> list[list[int]]:
    def is_white(rgb_pixel):
        if sum(rgb_pixel) >= 255 * len(rgb_pixel) / 2:
            return True
        else:
            return False

    def get_pixel(pixels, bitmap_idx, x, y):
        return pixels[y * width + bitmap_idx * BITMAP_SIZE + x]

    bitmaps = []
    for bitmap_idx in range(0, width // bmap_width):
        bitmap = []

        for x in range(bmap_width):
            byte = 0
            for y in range(BITMAP_SIZE):
                byte += (is_white(get_pixel(pixels, bitmap_idx, x, y)) << y)
            bitmap.append(byte)

        bitmaps.append(bitmap)
    return bitmaps

def gen_bitmap_from_file(filename) -> list[int] | None:
    im = Image.open(filename)
    pixels = list(im.getdata())
    width  = im.size[0]
    height = im.size[1]

    if not gen_bitmaps_check_dimensions(filename, width, height, width):
        return None

    return gen_bitmaps_from_pixels(pixels, width, width)[0]

def gen_bitmaps_from_file(filename, bmap_width) -> list[list[int]] | None:
    im = Image.open(filename)
    pixels = list(im.getdata())
    width  = im.size[0]
    height = im.size[1]

    if not gen_bitmaps_check_dimensions(filename, width, height, bmap_width):
        return None

    return gen_bitmaps_from_pixels(pixels, width, bmap_width)


def parse_png(filename: str) -> str:
    try:
        bitmaps = gen_bitmaps_from_file(args.img_dir + "/" + filename, BITMAP_SIZE)
    except FileNotFoundError:
        print("[ERROR] " + filename + ": image not found")
        return ""

    if bitmaps == None:
        return ""

    result = "const byte %s[] PROGMEM = {\n" % (filename[:-4].upper())

    for bitmap in bitmaps:
        result += "    " + ", ".join(["0x%02x" % int(i) for i in bitmap]) + ",\n"

    result += "};\n\n"

    return result

def parse_h(filename: str) -> str:
    result = ""

    with open(args.img_dir + "/" + filename, "r") as file:
        result += file.read()

    return result

def parse_txt(filename: str) -> str:
    file_lines = []
    with open(args.img_dir + "/" + filename, "r") as file:
        file_lines = file.readlines()

    lines = []
    for line in file_lines:
        if len(line) != 0:
            lines.append(line.replace("\n", ""))

    if len(lines) != 2:
        print("[ERROR] " + filename + ": text file must contain 2 lines: font file name and text")
        return ""

    font_name = lines[0]
    text = lines[1]
    font_dir = args.fonts_dir + "/" + font_name

    font = dict()
    try:
        with open(font_dir + "/" + FONT_MAP_FILENAME) as font_file:
            font_json = font_file.read()
            font = json.loads(font_json)
    except FileNotFoundError:
        print("[ERROR] " + font_dir + "/" + FONT_MAP_FILENAME + ": image not found")
        return ""

    result = "const byte %s[] PROGMEM = {\n" % (filename[:-4].upper())

    for symbol in text:
        try:
            bitmap = gen_bitmap_from_file(font_dir + "/" + font[symbol])
            if bitmap == None:
                return ""

            bitmap.append(0) # empty column

            line = "    " + ", ".join(["0x%02x" % i for i in bitmap]) + ","
            result += line + " "*max(0, 70 - len(line)) + "//< '%s'\n" % symbol
        except KeyError:
            print("[ERROR] " + filename + ": symbol '%s' from text is not found in font %s" % (symbol, font_name))
            return ""
        except FileNotFoundError:
            print("[ERROR] " + font_dir + "/" + font[symbol] + ": image not found")
            return ""

    result += "};\n\n"

    return result


out_text = """\
// ===== GENERATED AUTOMATICALLY WITH bitmaps_generate.py SCRIPT =====

#ifndef BITMAPS_H_
#define BITMAPS_H_

#include <Arduino.h>

"""

files = [f for f in os.listdir(args.img_dir) if os.path.isfile(args.img_dir + "/" + f)]
files.sort()
for filename in files:
    res = ""

    if filename[-2:] == ".h":
        res = parse_h(filename)
    elif filename[-4:] == ".png":
        res = parse_png(filename)
    elif filename[-4:] == ".txt":
        res = parse_txt(filename)
    else:
        print("[WARN] " + filename + ": unsoppurted filetype")

    out_text += res

out_text += "#endif //< BITMAPS_H_\n\n"

with open(args.bitmaps_header, "w") as out_file:
    out_file.write(out_text)

