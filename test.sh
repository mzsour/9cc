#!/bin/bash

assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

assert 0 0
assert 42 42
assert 6 "1+2+3"
assert 5 "6+3-4"
assert 7 " 2+  2 +  3  "
assert 4 "2- 1+ 3"
assert 51 "10 - 9 + 50"

echo OK
