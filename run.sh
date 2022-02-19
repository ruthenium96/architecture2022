#!/bin/bash

# bash ./shell.sh clean
bash ./shell.sh build
bash ./shell.sh run

if [ $? -eq 1 ]
then
   exit 1
fi
