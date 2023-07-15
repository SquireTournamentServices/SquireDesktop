import os
import re
import subprocess

is_rust_src = re.compile("(.*)\\.rs$")
re_type = re.compile(".*pub (struct|enum) ([a-zA-Z0-9]+).*")
re_impl = re.compile(".*impl ([a-zA-Z0-9_]+).*")
re_func = re.compile(".*pub extern fn ([a-zA-Z0-9_]+).*")

dirs = os.listdir("./src/")
types = [
    "AdminId",
    "RoundId",
    "PlayerId",
    "TournamentId",
    "JudgeId",
    "TournamentPreset",
    "TournamentStatus",
    "RoundStatus",
    "PlayerStatus",
    "StandardScore",
]
funcs = []

for dir in dirs:
    mtch = is_rust_src.match(dir)
    if mtch is not None:
        f = open("./src/" + dir, "r")
        data = f.read()
        f.close()

        (mod_name,) = mtch.groups()
        print(f"Prcoessing {mod_name}")
        data = (
            data.replace("(\n", "(")
            .replace(",\n", ",")
            .replace(",)", ")")
            .replace("    ", "")
        )

        # Types
        for line in data.split("\n"):
            t_mtch = re_type.match(line)
            if t_mtch is not None:
                (
                    _,
                    type_name,
                ) = t_mtch.groups()
                print(f" -- Found type {type_name}")
                types.append(type_name)

        # Functions
        impl = "__ERR__NO__IMPL__"
        for line in data.split("\n"):
            i_mtch = re_impl.match(line)
            if i_mtch is not None:
                (impl,) = i_mtch.groups()
                print(f" -- Found interface for {impl}")
                continue

            f_mtch = re_func.match(line)
            if f_mtch is not None:
                (func,) = f_mtch.groups()

                print(f" -- Found function {func}")
                funcs.append(func)
                continue

out = """# AUTO GENERATED WITH ./bindgen.py AT BUILD TIME!
# DO NOT MODIFY BY HAND
language = "C++"
header = "/* AUTO GENERATED SquireFFI BINDINGS DO NOT MODIFY BY HAND. */"
trailer = "/* AUTO GENERATED SquireFFI BINDINGS DO NOT MODIFY BY HAND */"
pragma_once = true
autogen_warning = "/* Warning: this file is autogenerated by cbindgen. Don't modify this manually. */"
include_version = true
namespace = "squire_ffi"
no_includes = false
cpp_compat = true
braces = "NextLine"
documentation = true
documentation_style = "doxy"
style = "both"
sort_by = "None"
usize_is_size_t = true
after_includes = "#define SQUIRE_FFI"
\n"""
out += """[export]
prefix = "sc_"
\n"""
out += "include = ["
for i in types:
    out += f'"{i}",'

for f in funcs:
    out += f'"{f}",'

out = out[:-1]
out += "]\n"
out += """
item_types = ["globals", "enums", "structs", "unions", "typedefs", "opaque", "functions", "constants"]

[export.mangle]
rename_types = "CamelCase"
remove_underscores = false

[fn]
rename_args = "None"
sort_by = "Name"
args = "horizontal"
must_use = "MUST_USE_FUNC"
no_return = "NO_RETURN"

[struct]
associated_constants_in_body = true
rename_fields = "None"
#derive_constructor = true
#derive_eq = true
#derive_neq = true
#derive_lt = true
#derive_lte = true
#derive_gt = true
#derive_gte = true

[enum]
rename_variants = "None"
enum_class = true
derive_helper_methods = true
derive_const_casts = true
derive_mut_casts = true
derive_tagged_enum_destructor = true
derive_tagged_enum_copy_constructor = true
derive_tagged_enum_copy_assignment = true
private_default_tagged_enum_constructor = true

[const]
allow_static_const = true
sort_by = "Name"

[macro_expansion]
bitflags = true

[parse]
parse_deps = true
clean = true
include = ["uuid", "squire_lib", "squire_sdk"]
extra_bindings = ["uuid", "squire_lib", "squire_sdk"]

[parse.expand]
crates = ["uuid"]
all_features = false
default_features = true
"""

f = open("./cbindgen.toml", "w")
f.write(out)
f.close()
