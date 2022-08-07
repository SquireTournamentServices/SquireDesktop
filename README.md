# SquireDesktop
**An all in one desktop app for running MTG tournaments**

## Build Info
[![Main](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/main.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/main.yml)
[![Memory tests](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/memtests.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/memtests.yml)
[![Coverage tests](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/coverage.yml/badge.svg)](https://github.com/MonarchDevelopment/SquireDesktop/actions/workflows/coverage.yml)
[![codecov](https://codecov.io/gh/MonarchDevelopment/SquireDesktop/branch/main/graph/badge.svg?token=JCS3Y40XYR)](https://codecov.io/gh/MonarchDevelopment/SquireDesktop)

## Start Here!
Currently this is under development and, not released, please check back later!

[Read the docs in full here](https://github.com/MonarchDevelopment/SquireDesktop/wiki)

### Installing from Binaries
**See releases.**

### Installing from Sources
#### Start Here
##### 0. Clone this repo
You must checkout recursively. 

```bash
git clone --recursive https://github.com/MonarchDevelopment/SquireDesktop
```

##### 1. Install the following
 - `Rust`
 - `CMake`
 - `cbindgen` (execute: `cargo install --force cbindgen`)
 - `python`
 - `Qt` (5 or, 6, Windows users can install qtcreator to make this easier)
   - Linguistics
   - Core
   - Networking
 - `Compiler` (gcc, clang or, msvc)

##### 2. Update Rust to Nightly
```
rustup default nightly
rustup update
```

##### 3. OS Dependant Instructions
###### Installing on Windows
Install:
 - Visual Studio Build Tools (default options are fine)
 - QtCreator (sorts most things out for you)
Build in qtcreator (it is easier I promise)

###### Installing on Mac
Install Deps
```
brew install qt6
brew install llvm
brew install gcc
```

You should be able to make this in XCode.

###### Installing on Linux
```sh
# Ubuntu
sudo apt-get update
sudo apt-get install -y --no-install-recommends build-essential \
cmake gcovr qt6-base-dev qt6-base-dev-tools qt6-declarative-dev \
qt6-tools-dev qt6-tools-dev-tools linguist-qt6 \
valgrind python3 astyle qt6-l10n-tool \
libgl1-mesa-dev
```
```sh
# Arch
sudo pacman -S gcc build-essential g++ cmake 
```

Then run the compiler
```bash
mkdir -p build && cd build # dont build in src please
# cmake .. -DCMAKE_BUILD_TYPE=TEST # Use if want to compile tests
cmake .. -DCMAKE_BUILD_TYPE=RELEASE # or DEBUG if you want debug symbols + debug logging
cmake --build . -j
# ctest -V # use to run the tests if you built them
```

### Copyright, Iconography and, Image Assets
Some icons are from [https://www.flaticon.com](https://www.flaticon.com/), others are
owned by Wizards of The Coast and, are trademarks of them (MTG Icons and, logos).
You can enquire about individual assets in issues. Main icons of note are below. Other
icons and, assets are ours (Monarch's), enquire if you wish to use.

| Icon Name | Owner |
|---|---|
| `warning.png`, `fluid.png`, `swiss.png` | Flat Icon |
| `icon.png`, `banner.png` | Monarch |

### Basic Usage
See the [Wiki](https://github.com/MonarchDevelopment/SquireDesktop/wiki) for a guide (coming
soon).

### Translations
The GUI is translated in English (UK) at the moment and, can be translated with the Qt 
translation tool, at the moment no such translations exist, but you can help by creating them!

### Bug / Crash Reporting
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
