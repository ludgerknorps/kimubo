#!/bin/bash


echo "Takes RIPx.wav files in dir and renames them by adding PARAM1 to x"
echo "   e.g. RIP1.wav will be moved to RIP14.wav if the command \"rename 13\" was used"


offset=$1
for i in RIP*.wav 
do 
	oldNumber=${i:3:-4}
	newNumber=$((oldNumber + offset))
	mv -v $i RIP$newNumber.wav
done
