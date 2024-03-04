#!/bin/bash
repo="examples_article"
scene="packing.sc"
conf="packing.conf"
xml="packing.xml"

if [ $# -ne 0 ]; then
    if [ "$1" = 1 ]; then
	scene="packing_sphere.sc"
    elif [ "$1" = 2 ]; then
	scene="packing_cross.sc"
    elif [ "$1" = 3 ]; then
	scene="packing_butterfly.sc"
    elif [ "$1" = 4 ]; then
	scene="packing_bat.sc"
    fi
fi

echo "TODO: same volume!!!"
./dem2d -c $repo/$conf -s $repo/$scene -v $repo/$xml -run
