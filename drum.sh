#!/bin/bash
repo="examples_article"
scene="drum.sc"
conf="drum.conf"
xml="drum.xml"
arguments=${@:2}

if [ $# -ne 0 ]; then
    if [ "$1" = 1 ]; then
	scene="drum_flower.sc"
    elif [ "$1" = 2 ]; then
	scene="drum_peanut.sc"
    elif [ "$1" = 3 ]; then
	scene="drum_rock.sc"
    else
	arguments=${@:1}
    fi
fi

./dem2d -c $repo/$conf -s $repo/$scene -v $repo/$xml -run $arguments
