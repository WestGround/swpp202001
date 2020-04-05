#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Please give the directory where clang resides in"
  echo "ex) ./run-ubsan.sh ~/my-llvm-release/bin"
  echo "ex2) ./run-ubsan.sh ~/clang+llvm-9.0.0-x86_64-darwin-apple/bin"
  exit 1
fi

CC=$1/clang
CXXFLAGS="-fsanitize=undefined"
set -e

if [[ "$OSTYPE" == "darwin"* ]]; then
  ISYSROOT="-isysroot `xcrun --show-sdk-path`"
else
  ISYSROOT=
fi

$CC $ISYSROOT $CXXFLAGS ubsan.c -o ubsan
./ubsan
