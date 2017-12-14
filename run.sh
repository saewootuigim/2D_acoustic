#!/bin/bash
sim="FW TR TRFN"
for str in $sim
do
	echo "./main -s $str -p 15"
	./main -s $str -p 15
done
