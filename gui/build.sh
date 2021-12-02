#!/bin/bash

PROG_NAME=greenhouse_gui

COMPILER=gcc

BUILD_DIR=./build
DEBUG_VAR="debug"

if [[ -d "$BUILD_DIR" ]]; then
	if [[ -f "$BUILD_DIR/$PROG_NAME" ]]; then
		rm $BUILD_DIR/$PROG_NAME
	fi
else
	mkdir $BUILD_DIR
fi

GENERAL_COMPILER_FLAGS="-std=c11 -Iinclude -Ithirdparty -Isrc -Wall -Wextra -Werror -Wno-unused -Wno-unused-parameter -Wno-missing-braces -Wno-sign-compare"

RASPI_FLAGS="-DRPI4 -I./ -I/usr/include -lEGL -lGLESv2 -L/usr/lib -lm -ldl -I/usr/include/EGL -I/usr/include/GLES2 -I/usr/include/libdrm -ldrm -lgbm"

if [ "$1" = "$DEBUG_VAR"  ]; then
	#Debug Flags Enabled
	echo Debug Build
	COMPILER_FLAGS="$GENERAL_COMPILER_FLAGS -ggdb -g -DDEBUG"
else
	#Optimizing Flags Enabled
	echo Optimized Build
	COMPILER_FLAGS="$GENERAL_COMPILER_FLAGS -O2"
fi

C_FILES="src/main.c"
LINKER_FLAGS="-pthread"

echo $COMPILER $COMPILER_FLAGS $C_FILES -o $BUILD_DIR/$PROG_NAME $LINKER_FLAGS $RASPI_FLAGS
$COMPILER $COMPILER_FLAGS $C_FILES -o $BUILD_DIR/$PROG_NAME $LINKER_FLAGS $RASPI_FLAGS
