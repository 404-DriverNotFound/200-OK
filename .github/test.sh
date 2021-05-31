#!/bin/sh
make re
./webserv test.config &
echo "\n\n\n\n\n\n\n" | ./ft_tester/tester http://localhost:8000