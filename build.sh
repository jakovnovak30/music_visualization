#!/bin/sh

set -xe

CC=gcc
CFLAGS="-lraylib -lm -g -Wall -pedantic -Iinclude"
ONAME="visualizer"
SRCS="src/main.c src/process_audio.c"
PLUG_SRC="src/reloadable/draw.c"

$CC $CFLAGS $SRCS -o bin/$ONAME
$CC $CFLAGS $PLUG_SRC -shared -o bin/plug.so
