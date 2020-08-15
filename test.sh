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

blank=0
comment=1
code=0
((val=$((blank<<5))+$((comment<<3))+$code))
assert $val "./sample/line_comment_top.c"
assert $val "./sample/scope_comment_top.c"

blank=0
comment=7
code=0
((val=$((blank<<5))+$((comment<<3))+$code))
assert $val "./sample/scope_comment_7.c"

blank=2
comment=5
code=0
((val=$((blank<<5))+$((comment<<3))+$code))
assert $val "./sample/scope_comment_5.c"


blank=3
comment=4
code=0
((val=$((blank<<5))+$((comment<<3))+$code))
assert $val "./sample/line_comment_and_blank.c"

blank=6
comment=0
code=1
((val=$((blank<<5))+$((comment<<3))+$code))
assert $val "./sample/blank6.c"

blank=2
comment=0
code=0
((val=$((blank<<5))+$((comment<<3))+$code))
assert $val "./sample/blank1.c"



#assert 48 "./sample/hello.c"
#assert 6 "./sample/comment.c"

echo OK
