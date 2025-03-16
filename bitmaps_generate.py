import os
from PIL import Image
import argparse

BITMAP_SIZE = 8

arg_parser = argparse.ArgumentParser(prog="Bitmaps generator",
                                     description="Generates C header file with bitmap arrays\
                                                  from images and text")

arg_parser.add_argument("-o", "--bitmaps-header",  default="./firmware/include/bitmaps.h")
arg_parser.add_argument("-d", "--img-dir", default="./bitmap_img")

args = arg_parser.parse_args()

def parse_png(filename: str) -> str:
    im = Image.open(args.img_dir + "/" + filename)
    pixels = list(im.getdata())
    width  = im.size[0]
    height = im.size[1]


    if height != BITMAP_SIZE:
        print("[ERROR] " + filename + ": height must be %d instead of %d" % (BITMAP_SIZE, height))
        return ""

    if width < BITMAP_SIZE:
        print("[ERROR] " + filename + ": width must be at least %d instead of %d" % (BITMAP_SIZE, width))
        return ""

    if width % BITMAP_SIZE != 0:
        print("[WARN] " + filename + ": width must be a multiple of %d instead of %d" % (BITMAP_SIZE, width))


    def is_white(rgb_pixel):
        if sum(rgb_pixel) >= 255 * len(rgb_pixel) / 2:
            return True
        else:
            return False

    def get_pixel(pixels, bitmap_idx, x, y):
        return pixels[y * width + bitmap_idx * BITMAP_SIZE + x]

    bitmaps = []
    for bitmap_idx in range(0, width // BITMAP_SIZE):
        bitmap = []

        for x in range(BITMAP_SIZE):
            byte = 0
            for y in range(BITMAP_SIZE):
                byte += (is_white(get_pixel(pixels, bitmap_idx, x, y)) << y)
            bitmap.append(byte)

        bitmaps.append(bitmap)

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
    print("[WARN] " + filename + ": .txt parsing is not implemented yet") # TODO
    return ""


out_text = """\
#ifndef BITMAPS_H_
#define BITMAPS_H_

#include <Arduino.h>

"""


files = [f for f in os.listdir(args.img_dir) if os.path.isfile(args.img_dir + "/" + f)]
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

