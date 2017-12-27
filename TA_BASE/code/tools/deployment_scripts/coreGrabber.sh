#!/bin/bash

COUNT=1;

while ["" eq ""]
do
	if [ -e core ]
	then
		eval mv core core.$COUNT
		COUNT=`expr $COUNT + 1`
		echo $COUNT
	else
		sleep 1
	fi
done

