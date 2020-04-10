#!/bin/bash

if [ "$#" -ne 2 ]; then
  echo "unreachable.sh build <clang dir>"
  echo "unreachable.sh run <clang dir>"
  echo "unreachable.sh all <clang dir>"
  echo "ex)  ./unreachable.sh all ~/my-llvm-release/bin"
  echo "ex2) ./unreachable.sh all ~/clang+llvm-9.0.0-x86_64-darwin-apple/bin"
  exit 1
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
  EXT=".dylib"
  ISYSROOT="-isysroot `xcrun --show-sdk-path`"
else
  EXT=".so"
  ISYSROOT=
fi

if [ "$1" != "run" ]; then
  LLVMCONFIG=$2/llvm-config
  CXXFLAGS=`$LLVMCONFIG --cxxflags`
  LDFLAGS=`$LLVMCONFIG --ldflags`
  LIBS=`$LLVMCONFIG --libs core irreader bitreader support --system-libs`
  SRCROOT=`$LLVMCONFIG --src-root`

  CXX=$2/clang++
  CXXFLAGS="$CXXFLAGS -std=c++17 -I\"${SRCROOT}/include\""
  #CXXFLAGS="$CXXFLAGS -std=c++17"
  set -e

  $CXX $ISYSROOT $CXXFLAGS $LDFLAGS $LIBS unreachable.cpp -o ./libUnreachable$EXT -shared

fi

if [ "$1" != "build" ]; then
  set +e
  SCORE=0
  TOTAL=0

  for i in {1..7} ; do
    echo "Testing data-unreachable/input${i}.ll.."

    $2/opt -disable-output \
        -load-pass-plugin=./libUnreachable$EXT \
        -passes="my-unreachable" data-unreachable/input${i}.ll >tmp 2>&1

    diff tmp ./data-unreachable/output${i}.txt
    if [ "$?" -eq 0 ]; then
      SCORE=$((SCORE+10))
    fi
    TOTAL=$((TOTAL+10))
  done

  echo "Score: $SCORE / $TOTAL"
fi
