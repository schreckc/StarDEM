#!/bin/bash
repo="examples_article"
scene="bridges.sc"
conf="bridges.conf"
xml="bridges.xml"

if [ $# -ne 0 ]; then
    if [ "$1" = 1 ]; then
	scene="bridges_flower.sc"
    elif [ "$1" = 2 ]; then
	scene="bridges_blob.sc"
    elif [ "$1" = 3 ]; then
	scene="bridges_blob.sc"
	conf="bridges_network.conf"
    elif [ "$1" = 4 ]; then
	scene="bridges_other.sc"
    fi
fi

./dem2d -c $repo/$conf -s $repo/$scene -v $repo/$xml -run
