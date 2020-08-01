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
assert 6 "1+(2+3)"
assert 4 "((4+2)-((1)+(2-1)))"
assert 5 "6+(3-4)"
assert 26 "2* 3* 4 + 2"
assert 14 " 2 * (3 +4)"
assert 3 "(3+9) / 4"
assert 3 " 11 / 3"
assert 70 "(17-3)*(+5)"
assert 2 "-3*+5+17"
assert 1 "5>3"
assert 1 "4 < 16"
assert 0 "5>30"
assert 0 "4 < -16"
echo OK
