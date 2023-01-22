import re
import sys

f = open("Cargo.lock", "r")
data = f.read()
f.close()

matches = re.search(
    r'source\s*=\s*"git\+https:\/\/github\.com\/MonarchDevelopment\/SquireCore#([a-f0-9]+)"',
    data,
    re.I | re.M,
)
if matches is None:
    print("Cannot find version information")
    sys.exit(1)

print(matches.groups()[0])
