#!/bin/bash

SERIAL=`usbtenkiget -l  | grep "Serial" | cut -d ':' -f 3 | cut -d "'" -f 2`

echo "Using serial: $SERIAL"

CUR=0
INC=100

while true; do
	if [ $INC -eq 0 ]
	then
		echo "Calibration completed";
		exit 0	
	fi

	./usbtenkisetup -s $SERIAL set_rtd_cal $CUR
	VAL=`./usbtenkiget -i a -s $SERIAL`

	echo Calibration set to $CUR gives us $VAL

	POSITIVE=`echo "$VAL > 0" | bc`
	if [ $POSITIVE -ne 1 ]
	then
		let "INC = $INC / 2"
		let "CUR = $CUR - $INC"
		continue;		
	fi
	
	let "CUR = $CUR + $INC"
		
	
done
