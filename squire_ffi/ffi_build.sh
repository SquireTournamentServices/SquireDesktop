#/bin/bash
python3 bindgen.py
cbindgen --config cbindgen.toml --output squire_ffi.h -v
echo "Exported to ./squire_ffi.h"
cargo build --release
python3 postprocess.py
