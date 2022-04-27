# SquireDesktop
## Build Info
[![Main](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/main.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/main.yml)
[![Memory tests](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/memtests.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/memtests.yml)
[![codecov](https://codecov.io/gh/MonarchDevelopment/SquireDesktop/branch/main/graph/badge.svg?token=JCS3Y40XYR)](https://codecov.io/gh/MonarchDevelopment/SquireDesktop)

## Start Here!
### Installing
#### Installing on Windows
TODO

#### Installing on Mac
TODO

#### Installing on Linux
TODO

### Basic Usage
TODO

### Crash Reporting
TODO

## Building From Sources
```bash
mkdir -p build && cd build
# cmake .. -DCMAKE_BUILD_TYPE=TEST # Use if want to compile tests
cmake ..
cmake --build . -j
# ctest -V # use to run the tests if you build them
```

## Requirements
| Name | Usage |
|---|---|
| `cmake` | Build System |
| `clang++`/`g++`/`mingw` | C++ Compiler |
| `qt5/qt6` | GUI Library |

