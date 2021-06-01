#!/bin/sh
./ft_tester clang++ ./ft_tester/byte_generator.cpp
echo 0 | ./ft_tester/a.out
curl localhost:8000/yunslee_debugfile/1MB.bla
if [ "$?" -ne "0" ]; then
  echo "GET 1MB.bla"
  exit(1);
fi

echo 1 | ./ft_tester/a.out
curl localhost:8000/yunslee_debugfile/100MB.bla
if [ "$?" -ne "0" ]; then
  echo "GET 100MB.bla"
  exit(1);
fi

echo 2 | ./ft_tester/a.out
curl localhost:8000/yunslee_debugfile/1GB.bla
if [ "$?" -ne "0" ]; then
  echo "GET 1GB.bla"
  exit(1);
fi
