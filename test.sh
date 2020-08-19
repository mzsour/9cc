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

assert 2 "main(){1+1;}"
assert 0 "main(){0;}"
assert 42 "main(){42;}"
assert 6 "main(){1+2+3;}"
assert 5 "main(){6+3-4;}"
assert 7 "main(){ 2+  2 +  3;  }"
assert 4 "main(){2- 1+ 3;}"
assert 51 "main(){10 - 9 + 50;}"
assert 6 "main(){1+(2+3);}"
assert 4 "main(){((4+2)-((1)+(2-1)));}"
assert 5 "main(){6+(3-4);}"
assert 26 "main(){2* 3* 4 + 2;}"
assert 14 "main(){ 2 * (3 +4);}"
assert 3 "main(){(3+9) / 4;}"
assert 3 "main(){ 11 / 3;}"
assert 70 "main(){(17-3)*(+5);}"
assert 2 "main(){-3*+5+17;}"
assert 1 "main(){5>3;}"
assert 1 "main(){4 < 16;}"
assert 0 "main(){5>30;}"
assert 0 "main(){4 < -16;}"
assert 1 "main(){ 1 + 1 == 2;}"
assert 0 "main(){ (2*4) + 1 == 0;}"
assert 1 "main(){4 >= 4;}"
assert 1 "main(){ 4 >= 1;}"
assert 1 "main(){ 9 <= 9;}"
assert 1 "main(){ 3 <= 10;}"
assert 0 "main(){ 50 >= 51;}"
assert 1 "main(){ 50 != 51;}"
assert 0 "main(){ 50 != 50;}"
assert 1 "main(){(4>3) == (9>=0);}"
assert 9 "main(){(4>3) == (9>=0); 1+ 2 ; 3 * 3;}"
assert 5 "main(){(4>3) == (9>=0); 4<2; 5;}"
assert 5 "main(){a=5; }"
assert 5 "main(){a=5;a; }"
assert 10 "main(){hello = 4;hello+6;}"
assert 8 "main(){a=5; q=3; a + q; }"
assert 86 "main(){a=5 ; 9*9+a; }"
assert 12 "main(){hello=4 ; world=3; world*hello; }"
assert 7 "main(){o=4 ; d=3; return o + d; }"
assert 8 "main(){a = 4; if(a > 2) a*2;}"
assert 8 "main(){a = 4; if(a > 2) a = a*2;a;}"
assert 4 "main(){a = 4; if(a < 2) a = a*2;a;}"
assert 4 "main(){a = 4; if(a < 2) a*2; else a;}"
assert 44 "main(){i = 4; j = 3; if(i > j) 44; else 33;}"
assert 12 "main(){var = 0; while(var < 10) var = var+3; var;}"
assert 14 "main(){s=2; t=3; u=4; if(t>s) while(u < 10) u = u+5; u;}"
assert 6 "main(){sum = 0; for(i=1; i<=3; i=i+1 ) sum = sum +i; sum;}"
assert 4 "main(){i=1; for(; i<=3;) i = i+1; i;}"
assert 6 "main(){{m=2; n=3; m*n;}}"
assert 8 "main(){num = 3; if(num >2){num = num*2; num= num+2;}}"
assert 4 "main(){incr(3);} incr(x){return x + 1;}"
assert 120 "main(){factorial(5);}factorial(n){if(n <= 1)return 1; else return n * factorial(n-1);}"
assert 24 "main(){foo(3, 4, 5);} foo(x, y, z){a = (x+y+z) * 2; return a;}"
echo OK
