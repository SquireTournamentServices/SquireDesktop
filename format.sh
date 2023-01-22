#!/bin/bash

find ./src ./tests* ./crash_handler | grep ".*\\.[c|h]\$" | xargs astyle --style=kr
find ./src ./tests* ./crash_handler | grep ".*\\.[c|h]pp\$" | xargs astyle --style=kr
astyle --style=kr *.cpp
cmake-format ./CMakeLists.txt > ./CMakeLists.txt.orig && cp ./CMakeLists.txt.orig ./CMakeLists.txt

cd ./squire_ffi
bash ./format.sh
cd ..

