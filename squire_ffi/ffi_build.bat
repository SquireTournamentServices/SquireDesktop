python bindgen.py
cbindgen --config cbindgen.toml --output squire_ffi.h -v
echo "Exported to ./squire_ffi.h"
cargo build --release

echo "Removing polymorphism"
python postprocess.py
