#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "sample.sh build <clang dir>"
  echo "sample.sh run <clang dir>"
  echo "sample.sh all <clang dir>"
  echo "ex)  ./sample.sh all ~/llvm-10.0-releaseassert/bin"
  exit 1
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
  EXT=".dylib"
  ISYSROOT="-isysroot `xcrun --show-sdk-path`"
else
  EXT=".so"
  ISYSROOT=
fi

if [[ "$1" != "run" ]]; then
  echo "----- build -----"
  LLVMCONFIG=$2/llvm-config
  CXXFLAGS=`$LLVMCONFIG --cxxflags`
  LDFLAGS=`$LLVMCONFIG --ldflags`
  LIBS=`$LLVMCONFIG --libs core irreader bitreader support --system-libs`
  SRCROOT=`$LLVMCONFIG --src-root`

  CXX=$2/clang++
  CXXFLAGS="$CXXFLAGS -std=c++17 -I\"${SRCROOT}/include\""
  set -e

  $CXX $ISYSROOT $CXXFLAGS $LDFLAGS $LIBS printdom.cpp -o ./libPrintDom$EXT -shared -fPIC -g
  $CXX $ISYSROOT $CXXFLAGS $LDFLAGS $LIBS fillundef.cpp -o ./libFillUndef$EXT -shared -fPIC -g
  $CXX $ISYSROOT $CXXFLAGS $LDFLAGS $LIBS instmatch.cpp -o ./libInstMatch$EXT -shared -fPIC -g
fi

if [[ "$1" != "build" ]]; then
  set +e
  echo "----- run printdom with simpleinputs/printinput.ll -----"
  $2/opt -load-pass-plugin=./libPrintDom$EXT -disable-output \
         -passes="print-dom" sampleinputs/printdom.ll
  echo "----- run fillundef with simpleinputs/fillundef.ll -----"
  $2/opt -load-pass-plugin=./libFillUndef$EXT \
         -passes="fill-undef" sampleinputs/fillundef.ll -S -o -
  echo "----- run instmatch with simpleinputs/instmatch.ll -----"
  $2/opt -load-pass-plugin=./libInstMatch$EXT -disable-output \
         -passes="inst-match" sampleinputs/instmatch.ll
fi
