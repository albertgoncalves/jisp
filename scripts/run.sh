#!/usr/bin/env bash

set -eu

if [ ! -d "$WD/bin" ]; then
    mkdir "$WD/bin"
fi

flags=(
    -D_GNU_SOURCE
    "-ferror-limit=1"
    "-march=native"
    -O3
    "-std=c99"
    -Werror
    -Weverything
    -Wno-c11-extensions
    -Wno-covered-switch-default
    -Wno-declaration-after-statement
    -Wno-disabled-macro-expansion
    -Wno-padded
    -Wno-reserved-macro-identifier
)

clang-format -i -verbose "$WD/src/"*
clang "${flags[@]}" -o "$WD/bin/main" "$WD/src/main.c"
"$WD/bin/main" "$WD/ex/main.asm"
