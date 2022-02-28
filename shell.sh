#!/bin/bash

if [ $1 == "clean" ]; then 
    bazel clean
fi


if [ $1 == "build" ]; then 
  bazel build --repo_env=CC=clang //shell:main
    if [ $? -eq 1 ]; then
        exit 1
    fi
fi


if [ $1 == "run" ]; then 
    ./bazel-bin/shell/main
    if [ $? -eq 1 ]
    then
    exit 1
    fi
fi
