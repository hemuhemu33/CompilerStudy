#!/bin/bash

assert() {
    expected="$1"
    input="$2"
    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];
    then
	echo "$input => $actual"
    else
	echo "$input => $expected expected, but got $actual"
	exit 1
    fi
}

assert 0 "return 0;"
assert 42 "return 42;"
assert 21 "return (5+20-4);"
assert 41 " return (12 + 34 - 5) ;"
assert 47 " return(5 + 6 * 7) ;"
assert 15 " return (5*(9-6)) ;"
assert 4 " return ((3+5)/2) ;"
assert 10 "return (-10+20) ;"
assert 0 "return (1==2) ;"
assert 1 "return (1!=2) ;"
assert 0 "return (1>(1+1));"
assert 1 "return (1<(1+1));"
assert 0 "return (1>=(1+1));"
assert 1 "return (1<=(1+1));"
assert 1 "a=1; return a;"
assert 3 "a=1;a=1+2; return a;"
assert 1 "a=1; return a;"
assert 1 "foo=1;return foo;"
assert 4 "foo=1;foo=2;var=foo+2;return var;"
assert 1 "returnx=1;return returnx;"
assert 4 "return 4;"
assert 0 "foo=1;if(foo==1)a=0;return a;"
assert 0 "b=0;foo=1;if(foo==1)a=0;else b=10;return b;"
assert 10 "b=0;foo=0;if(foo==1)a=0;else b=10;return b;"
assert 50 "b=0;while(b<50)b=b+10;return b;"
assert 60 "b=0;while(b<=50)b=b+10;return b;"
assert 55 "b=0;for(a=0;a<=10;a=a+1)b=b+5;return b;"
assert 255 "b=0;for(a=0;a<=254;a=a+1)b=b+1;return b;" # 8bitまでしか出力できない？
assert 255 "b=0;for(a=0;a<=254;a=a+1){b=b+1;}return b;" # 8bitまでしか出力できない？
assert 0 "{return 0;}"
assert 160 "b=0;c=0;for(a=0;a<=15;a=a+1){b=b+1;c=c+10;}return c;" # 8bitまでしか出力できない？


echo OK
