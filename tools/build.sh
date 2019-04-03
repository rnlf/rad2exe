#!/bin/bash

[[ ! -x "prepare" ]] \
|| [[ "prepare.cpp" -nt "prepare" ]] \
&& g++ -std=c++11 -o prepare prepare.cpp

exit 0
