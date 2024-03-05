#!/bin/bash
repo="examples_article"
scene="rest_angle.sc"
conf="rest_angle.conf"
xml="packing.xml"
arguments=${@:2}

if [ $# -ne 0 ]; then
    if [ "$1" = 1 ]; then
	scene="rest_angle_pear.sc"
    elif [ "$1" = 2 ]; then
	scene="rest_angle_dragonfly.sc"
    elif [ "$1" = 3 ]; then
	scene="rest_angle_rock.sc"
    else
	arguments=${@:1}
    fi
fi

./dem2d -c $repo/$conf -s $repo/$scene -v $repo/$xml -run $arguments
