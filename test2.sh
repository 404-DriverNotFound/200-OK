#!/bin/sh
cd ./yunslee_debugfile
echo 0 | ./ByteGenerator
curl localhost:8000/yunslee_debugfile/1MB.bla
if [ "$?" -ne "0" ]; then
  echo "GET 1MB.bla"
  exit 1
fi

echo 1 | ./ByteGenerator
curl localhost:8000/yunslee_debugfile/100MB.bla
if [ "$?" -ne "0" ]; then
  echo "GET 100MB.bla"
  exit 1
fi

echo 2 | ./ByteGenerator
curl localhost:8000/yunslee_debugfile/1GB.bla
if [ "$?" -ne "0" ]; then
  echo "GET 1GB.bla"
  exit 1
fi
