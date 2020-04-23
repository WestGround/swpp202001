#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "simple.sh build <clang dir>"
  echo "simple.sh run <clang dir>"
  echo "simple.sh all <clang dir>"
  echo "ex)  ./simple.sh all ~/llvm-10.0-releaseassert/bin"
  exit 1
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
  ISYSROOT="-isysroot `xcrun --show-sdk-path`"
else
  ISYSROOT=
fi

if [[ "$1" == "build" || "$1" == "all" ]]; then
  echo "----- build -----"
  LLVMCONFIG=$2/llvm-config
  CXXFLAGS=`$LLVMCONFIG --cxxflags`
  LDFLAGS=`$LLVMCONFIG --ldflags`
  LDFLAGS="$LDFLAGS -Wl,-rpath,`$LLVMCONFIG --libdir`"
  LIBS=`$LLVMCONFIG --libs core irreader bitreader support --system-libs`
  SRCROOT=`$LLVMCONFIG --src-root`

  CXX=$2/clang++
  CXXFLAGS="$CXXFLAGS -std=c++17 -I${SRCROOT}/include"
  CXXFLAGS="$CXXFLAGS -I${SRCROOT}/utils/unittest/googletest/include"
  CXXFLAGS="$CXXFLAGS -I${SRCROOT}/utils/unittest/googletest/"
  set -e

  $CXX $ISYSROOT $CXXFLAGS $LDFLAGS $LIBS simpleTest.cpp \
       $SRCROOT/utils/unittest/googletest/src/gtest-all.cc \
       -o simpleTest
fi

if [[ "$1" == "run" || "$1" == "all" ]]; then
  echo "----- run -----"
  set +e
  ./simpleTest
fi
