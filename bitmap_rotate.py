result = []

while True:
    bitmap = input().strip()

    if len(bitmap) == 0 or not bitmap[0].isnumeric():
        break

    if bitmap.endswith(","):
        bitmap = bitmap[:-1]

    bitmap = bitmap.split(",")

    if len(bitmap) != 8:
        print("[FAIL] - one line must contain 8 values")
        exit(1)

    bitmap_arr = [int(hex_val, 16) for hex_val in bitmap]

    rotated_bitmap_arr = []

    for i in range(8):
        byte = 0

        for j in range(8):
            byte += ((bitmap_arr[j] >> i) & 1) << j

        rotated_bitmap_arr.append(byte)

    result.append("    " + ", ".join(["0x%02x" % int(i) for i in rotated_bitmap_arr]) + ",")

for i in result:
    print(i)
