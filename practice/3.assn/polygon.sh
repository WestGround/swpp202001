#!/bin/bash

if [[ "$#" -ne 2 ]] && ! [[ "$#" -eq 1 && "$1" == "run" ]] ; then
  echo "run-polygon.sh build <clang dir>"
  echo "run-polygon.sh run"
  echo "run-polygon.sh all <clang dir>"
  echo "ex)  ./run-polygon.sh build ~/my-llvm-release/bin"
  echo "ex2) ./run-polygon.sh all   ~/clang+llvm-9.0.0-x86_64-darwin-apple/bin"
  exit 1
fi

if [ "$1" != "run" ]; then
  CXX=$2/clang++
  CXXFLAGS="-fsanitize=undefined -std=c++17"
  set -e

  if [[ "$OSTYPE" == "darwin"* ]]; then
    ISYSROOT="-isysroot `xcrun --show-sdk-path`"
  else
    ISYSROOT=
  fi

  $CXX $ISYSROOT $CXXFLAGS polygon.cpp polygon_main.cpp -o polygon
fi

if [ "$1" != "build" ]; then
  set +e
  SCORE=0
  TOTAL=0

  for i in {1..9} ; do
    echo "Testing data-poly/input${i}.txt.."
    ./polygon data-poly/input${i}.txt 1>tmp 2>&1
    diff tmp ./data-poly/output${i}.txt
    if [ "$?" -eq 0 ]; then
      SCORE=$((SCORE+10))
    fi
    TOTAL=$((TOTAL+10))
  done

  echo "Score: $SCORE / $TOTAL"
fi
