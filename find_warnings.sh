#!/bin/bash
gcc -Wall $1 -o test 2> warning.txt
grep "warning" warning.txt > result.txt
sed -i 's/warning: //g' result.txt
a=1
if [$? -eq 0]
then
while [$a -le $2]
do
$a | ./test >> result.txt
done
fi
pwd
