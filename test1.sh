#!/bin/sh
cd ./ft_tester
clang++ statusCodeTester.cpp -o statusCodeTester
./statusCodeTester 127.0.0.1:8000
if [ "$?" -ne "0" ]; then
  echo "statusCodeTester"
  exit 1
fi
