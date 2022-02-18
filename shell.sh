#!/bin/bash

if [ $1 == "build" ]; then 
    bazel build --config=clang_config //src:shell
fi

if [ $1 == "run" ]; then 
    ./bazel-bin/src/shell
fi

if [ $1 == "clean" ]; then 
    bazel clean
fi
