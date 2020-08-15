#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    ./loc $input
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected ecpected, but got $actual"
        exit 1
    fi
}

assert 47 "./sample/hello.c"

echo OK
