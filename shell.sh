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

if [ $1 == "build_tests" ]; then
  bazel build --repo_env=CC=clang //shell/tests:unit_tests
    if [ $? -eq 1 ]; then
      exit 1
    fi
    bazel build --repo_env=CC=clang //shell/tests:integration_tests
    if [ $? -eq 1 ]; then
      exit 1
    fi
fi


if [ $1 == "run_tests" ]; then
    ./bazel-bin/shell/tests/unit_tests
    if [ $? -eq 1 ]
    then
      exit 1
    fi
    ./bazel-bin/shell/tests/integration_tests
    if [ $? -eq 1 ]
    then
      exit 1
    fi
fi