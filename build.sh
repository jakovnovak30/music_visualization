#!/bin/sh

set -xe

CC=gcc
CFLAGS="-lraylib -lm -g -Wall -pedantic"
ONAME="visualizer"
SRCS="src/main.c"

$CC $CFLAGS $SRCS -o bin/$ONAME
