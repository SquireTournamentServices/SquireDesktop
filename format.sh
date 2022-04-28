#!/bin/bash

find ./src ./tests | grep ".*\\.[c|h]\$" | xargs astyle --style=kr
find ./src ./tests | grep ".*\\.cpp\$" | xargs astyle --style=kr

