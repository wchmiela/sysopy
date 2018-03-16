CC = gcc
CFLAGS = -std=c11 -Wall -pedantic -w
LINKERFLAGS=-Wl,-rpath=.
OUT = results3a.txt
ARGS = -n 999 -m 999 --create --add 5 --add 3 --delete 5 --find 300 -s

SOURCE_FILES = my_lib.c app.c
OBJECT_FILES = my_lib.o app.o

all: sharedlib staticlib

my_lib.o: my_lib.c my_lib.h
		@$(CC) $(CFLAGS) -fPIC -c my_lib.c -o my_lib.o

app.o: app.c app.h
		@$(CC) $(CFLAGS) -fPIC -c app.c -o app.o

sharedlib: $(OBJECT_FILES)
		$(CC) $(CFLAGS) -fPIC -shared -shared -o libshared.so $(OBJECT_FILES)

staticlib: $(OBJECT_FILES)
		ar rcs libstatic.a $(OBJECT_FILES)

static: staticlib
		@$(CC) $(LINKERFLAGS) app.o libstatic.a -L ./ -o static

shared: sharedlib
		@$(CC) $(LINKERFLAGS) app.c -o shared $(CFLAGS) -L. -lshared

dll: sharedlib
		@$(CC) app_dll.c $(CFLAGS) -o dll -ldl -DDLL

run_shared: shared
		./shared $(ARGS)

run_static: static
		./static $(ARGS)

run_dll: dll
		./dll $(ARGS)

run_tests: shared static dll
		@echo "Biblioteka statyczna:" >> $(OUT)
		./static $(ARGS) >> $(OUT)
		
		@echo "Biblioteka dzielona:" >> $(OUT)
		./shared $(ARGS) >> $(OUT)
		
		@echo "Biblioteka ladowana dynamicznie:" >> $(OUT)
		./dll $(ARGS) >> $(OUT)

clean:
		@rm -f *.o
		@rm -f *.a
		@rm -f *.so
		@rm -f shared
		@rm -f static
		@rm -f dll
		@rm -f results.txt

.PHONY: all shared static compile_o1 compile_o2 compile_o3 clean
