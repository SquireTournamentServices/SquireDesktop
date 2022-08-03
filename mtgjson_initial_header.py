import requests
import zipfile
import tempfile

DATA_URL = "https://mtgjson.com/api/v5/AllPrintings.json.zip"
FILE = "AllPrintings.json"
OUTPUT_FILE = "mtgjson_initial.h"


def escape(s: str) -> str:
    ret = ""
    for i in range(len(s)):
        ret += s[i]
        if i % 200 == 0:
            ret += '\n'

    # Escape \ to \\, Escape " to \", Fix new lines
    ret = ret.replace("\\", "\\\\").replace('"', '\\"').replace("\n", '"\n"')
    return ret


print("Downloading atomic json as compressed stream")
resp = requests.get(DATA_URL)

tmp_file = tempfile.TemporaryFile("wb+")
for chunk in resp.iter_content(chunk_size=512 * 1024):
    if chunk:  # filter out keep-alive new chunks
        tmp_file.write(chunk)

tmp_file.seek(0)

print("Decompressing")
file = zipfile.ZipFile(tmp_file, "r")
file.extract(FILE, "./")
file.close()
tmp_file.close()

print("Generating output")
f = open(FILE, "rb")
data = f.read().decode("UTF-8")
f.close()

output = "#pragma once\n"
output += f'const char *initial_json = "{escape(data)}";'

print("Writing output to CMAKE_BINARY_DIR")
f = open(OUTPUT_FILE, "w")
f.write(output)
f.close()
