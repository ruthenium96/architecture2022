#!/bin/bash

bash ./shell.sh clean
bash ./shell.sh build
if [ $? -eq 1 ]; then
   echo "Build failed!"
   exit 1
fi

bash ./shell.sh run
if [ $? -eq 1 ]; then
   echo "Some of tests failed!"
   exit 1
fi
