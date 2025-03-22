
all: bitmaps
	pio -f -c vim run

upload: bitmaps
	pio -f -c vim run --target upload

clean:
	pio -f -c vim run --target clean

uploadfs:
	pio -f -c vim run --target uploadfs

update: bitmaps
	pio -f -c vim update

fuses:
	pio run -t fuses

compile_commands: bitmaps
	pio run --target compiledb

bitmaps:
	python ../bitmaps_generate.py --img-dir=../bitmap_img --fonts-dir=../bitmap_img/fonts --bitmaps-header=./include/bitmaps.h

