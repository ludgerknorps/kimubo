#!/bin/bash


echo "Takes X.ogg files in dir and renames them by adding prefix label \"RIP\""
echo "   e.g. 1.ogg will be moved to RIP1.ogg "


for i in *.ogg
do 
	mv -v $i RIP$i
done
