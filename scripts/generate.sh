#!/usr/bin/bash

FLAGS=""
CP_COMPILE_COMMANDS=false

if [ -n "$1" ]; then
    GENERATOR=$1
else
    GENERATOR="Ninja"
    FLAGS="${FLAGS} -DCMAKE_EXPORT_COMPILE_COMMANDS=1" # -DCMAKE_CXX_FLAGS=\"-stdlib=libc++\" -DCMAKE_EXE_LINKER_FLAGS=\"-stdlib=libc++\"
    CP_COMPILE_COMMANDS=true
fi

if [ -n "$2" ]; then
    C_COMPILER=$2
else
    C_COMPILER="clang-cl"
fi

if [ -n "$3" ]; then
    CXX_COMPILER=$3
else
    CXX_COMPILER="clang-cl"
fi

FLAGS="${FLAGS} -DCMAKE_C_COMPILER=${C_COMPILER} -DCMAKE_CXX_COMPILER=${CXX_COMPILER}"

cmake -G $GENERATOR $FLAGS ..

if [ $CP_COMPILE_COMMANDS ]; then
    cp compile_commands.json ..
fi
