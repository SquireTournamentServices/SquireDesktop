# SquireDesktop
## Build Info
[![Main](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/main.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/main.yml)
[![Memory tests](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/memtests.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/memtests.yml)
[![codecov](https://codecov.io/gh/MonarchDevelopment/SquireDesktop/branch/main/graph/badge.svg?token=JCS3Y40XYR)](https://codecov.io/gh/MonarchDevelopment/SquireDesktop)
[![Mingw](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/mingw.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/mingw.yml)

## Start Here!
Currently this is under development and, not released, please check back later!

### Installing
#### Installing on Windows
TODO

#### Installing on Mac
TODO

#### Installing on Linux
TODO

### Basic Usage
See the [Wiki](https://github.com/MonarchDevelopment/SquireDesktop/wiki) for a guide (coming
soon).

### Translations
The GUI is translated in English (UK) at the moment and, can be translated with the Qt 
translation tool, at the moment no such translations exist, but you can help by creating them!

### Bug Crash Reporting
Please create an [issue on github](https://github.com/MonarchDevelopment/SquireDesktop/issues)
describing the bug/crash and what you did to cause it, if possible please attach logs or, (for
advanced users) ||a stacktrace or core dump (if you do this please don't post it in full as
they contain sensitive information on the heap (squire id user token)).||

By default the application logs to `squire_desktop.log` found next to the configuration file,
this logging is done by the desktop shortcut not the app. Developers will see all logs in 
`stderr` and none in this file.

### Privacy
This app contains no trackers, however when using the Squire online services it follows the
privacy policy for those.

### Configuration
The default configuration is stored in ./config.json, on Windows this is in `CHANGE ME`, 
on Linux and mac this is in `CHANGE ME`. You *can* edit the configuration by hand or, go to
`File > Settings` and use the form.

## Building From Sources
### Windows

### Linux / Mac
```bash
mkdir -p build && cd build
# cmake .. -DCMAKE_BUILD_TYPE=TEST # Use if want to compile tests
cmake .. -DCMAKE_BUILD_TYPE=RELEASE # or DEBUG if you want debug symbols + debug logging
cmake --build . -j
# ctest -V # use to run the tests if you built them
```

## Software Requirements
| Name | Usage |
|---|---|
| `cmake` | Build System |
| `clang++`/`g++`/`mingw` | C++ Compiler |
| `qt5/qt6` | GUI Library |
| `rust` | Squire Core |

## Copyright, Iconography and, Image Assets
Some icons are from [https://www.flaticon.com](https://www.flaticon.com/), others are
owned by Wizards of The Coast and, are trademarks of them (MTG Icons and, logos).
You can enquire about individual assets in issues. Main icons of note are below. Other
icons and, assets are ours (Monarch's), enquire if you wish to use.

| Icon Name | Owner |
|---|---|
| `warning.png` | Flat Icon |
| `icon.png` | Monarch |

