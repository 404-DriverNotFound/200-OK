#!/bin/sh
# cd ./yunslee_debugfile
# echo 0 | ./ByteGenerator
# curl localhost:8000/yunslee_debugfile/1MB.txt
# if [ "$?" -ne "0" ]; then
#   echo "GET 1MB.txt";
#   exit 1;
# fi

# echo 1 | ./ByteGenerator
# curl localhost:8000/yunslee_debugfile/100MB.txt
# if [ "$?" -ne "0" ]; then
#   echo "GET 100MB.txt"
#   exit 1
# fi

# 너무 시간이 오래걸리는 관계로 주석처리 함
# echo 2 | ./ByteGenerator
# curl localhost:8000/yunslee_debugfile/1GB.txt
# if [ "$?" -ne "0" ]; then
#   echo "GET 1GB.txt"
#   exit 1
# fi

cd ./yunslee_debugfile
echo 3 | ./ByteGenerator
curl localhost:8000/yunslee_debugfile/500KB.txt
if [ "$?" -ne "0" ]; then
  echo "GET 500KB.txt";
  exit 1;
fi