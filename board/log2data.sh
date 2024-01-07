#!/bin/sh

if [ -z "${1}" ]
then
	echo "Usage: $0 filename"
	exit 1
fi

#cat log.txt | sed -r -e 's#time=([0-9.]+)s, gps_speed=([0-9]+)mm/s, speed=([0-9]+)mm/s, a=([0-9\-]+)mm/s\^2, af=([0-9\-]+)mm/s\^2#\1;\2;\3;\4;\5#g'
sed -r -e 's#time=([0-9.]+), speedFiltered=([0-9]+), speed=([0-9]+), speedraw=([0-9]+)#\1;\2;\3;\4#g' $1
