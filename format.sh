#!/bin/bash

find ./src ./tests | grep ".*\\.[c|h]\$" | xargs astyle --style=kr

