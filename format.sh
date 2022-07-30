#!/bin/bash

find ./src ./tests* | grep ".*\\.[c|h]\$" | xargs astyle --style=kr
find ./src ./tests* | grep ".*\\.cpp\$" | xargs astyle --style=kr
cmake-format ./CMakeLists.txt > ./CMakeLists.txt.orig && cp ./CMakeLists.txt.orig ./CMakeLists.txt

