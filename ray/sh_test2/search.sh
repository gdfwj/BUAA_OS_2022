#!bin/bash
#First you can use grep (-n) to find the number of lines of string.
#Then you can use awk to separate the answer.
touch $3
cat $1|grep ${2}>$3
