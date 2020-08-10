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

assert 2 "1+1;"
assert 0 "0;"
assert 42 "42;"
assert 6 "1+2+3;"
assert 5 "6+3-4;"
assert 7 " 2+  2 +  3;  "
assert 4 "2- 1+ 3;"
assert 51 "10 - 9 + 50;"
assert 6 "1+(2+3);"
assert 4 "((4+2)-((1)+(2-1)));"
assert 5 "6+(3-4);"
assert 26 "2* 3* 4 + 2;"
assert 14 " 2 * (3 +4);"
assert 3 "(3+9) / 4;"
assert 3 " 11 / 3;"
assert 70 "(17-3)*(+5);"
assert 2 "-3*+5+17;"
assert 1 "5>3;"
assert 1 "4 < 16;"
assert 0 "5>30;"
assert 0 "4 < -16;"
assert 1 " 1 + 1 == 2;"
assert 0 " (2*4) + 1 == 0;"
assert 1 "4 >= 4;"
assert 1 " 4 >= 1;"
assert 1 " 9 <= 9;"
assert 1 " 3 <= 10;"
assert 0 " 50 >= 51;"
assert 1 " 50 != 51;"
assert 0 " 50 != 50;"
assert 1 "(4>3) == (9>=0);"
assert 9 "(4>3) == (9>=0); 1+ 2 ; 3 * 3;"
assert 5 "(4>3) == (9>=0); 4<2; 5;"
assert 5 "a=5; "
assert 5 "a=5;a; "
assert 10 "hello = 4;hello+6;"
assert 8 "a=5; q=3; a + q; "
assert 86 "a=5 ; 9*9+a; "
assert 12 "hello=4 ; world=3; world*hello; "
assert 7 "o=4 ; d=3; return o + d; "
assert 8 "a = 4; if(a > 2) a*2;"
assert 8 "a = 4; if(a > 2) a = a*2;a;"
assert 4 "a = 4; if(a < 2) a = a*2;a;"
assert 4 "a = 4; if(a < 2) a*2 else a;"
assert 44 "i = 4; j = 3; if(i > j) 44 else 33;"
assert 12 "var = 0; while(var < 10) var = var+3; var;"
assert 14 "s=2; t=3; u=4; if(t>s) while(u < 10) u = u+5; u;"
echo OK
