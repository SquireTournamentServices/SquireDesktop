#!/usr/bin/env python3

import os

ASSETS_FOLDER = "assets"
HEADER_FILE = "assets.h"


def escape(s):
    ret = ""
    for i in range(len(s) // 2):
        ret += "\\x"
        base = i * 2
        ret += s[base]
        ret += s[base + 1]
        if i % 100 == 0:
            ret += '"\n"'
    return ret


assets_proc = []
assets = os.listdir(ASSETS_FOLDER)
for asset in assets:
    name = "_".join(asset.split(".")).strip().replace("-", "_").upper()
    f = open(f"{ASSETS_FOLDER}/{asset}", "rb")
    data = f.read()
    f.close()

    assets_proc.append((name, data))

output = ""
for asset_proc in assets_proc:
    (
        name,
        str,
    ) = asset_proc
    str = str.hex()

    print(f"Found {name}")
    output += f'const unsigned char {name}[] = "{escape(str)}";'
    output += os.linesep

f = open(HEADER_FILE, "w")
f.write(output)
f.close()
