CC=emcc
AR=emar

CFLAGS=#-fpass-plugin=/src/passes/build/lib/libVoidToIntReturn.so -fno-legacy-pass-manager -fPIC
LDFLAGS=-s SIDE_MODULE=1 -Wl,--fatal-warnings


final.so : library.a src/main.o src/other.o
	$(CC) library.a src/main.o src/other.o -o final.so $(LDFLAGS)

library.a: src/library.c
	$(CC) -c src/library.c -o src/library.o $(CFLAGS)
	$(AR) rcs library.a src/library.o

src/main.o : src/main.c
	$(CC) -c src/main.c -o src/main.o $(CFLAGS)

src/other.o : src/other.c
	$(CC) -c src/other.c -o src/other.o $(CFLAGS)

clean:
	rm -f src/library.o library.a src/main.o src/other.o final.so 
