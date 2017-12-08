#!/bin/bash
sim="TRFR TRFI TRFN"
for str in $sim
do
	echo "./main -s $str -p 1000"
	./main -s $str -p 1000
done