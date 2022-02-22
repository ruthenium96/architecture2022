#!/bin/bash

if [ $1 == "clean" ]; then 
    bazel clean
fi


if [ $1 == "build" ]; then 
    bazel build --config=clang_config //shell:shell
    if [ $? -eq 1 ]; then
        exit 1
    fi
fi


if [ $1 == "run" ]; then 
    ./bazel-bin/src/shell
    if [ $? -eq 1 ]
    then
    exit 1
    fi
fi
