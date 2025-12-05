#!/bin/bash

g++ -o exec $1 -I/home/qiao/share/liburing-master/src/include -L/home/qiao/share/liburing-master/src -luring
