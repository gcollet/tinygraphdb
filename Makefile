all:
	g++ -O3 -c src/tinygraphdb.cpp -o tinygraphdb.o
	@if [ ! -d lib ]; then mkdir lib; fi
	@ar rcs lib/libtinygraphdb.a tinygraphdb.o
	@rm tinygraphdb.o

install:
	@cp lib/libtinygraphdb.a /usr/lib/
	@cp src/tinygraphdb.h /usr/include/