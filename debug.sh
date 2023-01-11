#!/bin/sh

if [ $# -eq 0 ]; then
	echo "Missing launcher name"
	exit 1
fi


if [ $# -ne 1 ]; then
	echo "Unexpected parameter: $2"
	exit 1
fi

JID=`jps | grep $1 | cut -d " " -f1`

if [ -z "$JID" ]; then
	echo "No such process"
	exit 1
fi

sudo gdb -p $JID -tui
