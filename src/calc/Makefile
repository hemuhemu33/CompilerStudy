CFLAGS=-std=c11 -g -static

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o) 		#SRCの中の.cを.oに置換する
9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h			#全ての.oファイルは9cc.hに依存することを示す。9cc.hが変更されたら依存ファイルはコンパイルし直す。

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.0 *~ tmp*
.PHONY: test clean
