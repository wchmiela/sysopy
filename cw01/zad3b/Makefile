CC = gcc
CFLAGS = -std=c11 -Wall -pedantic -w
LINKERFLAGS=-Wl,-rpath=.
OUT = results3b.txt
ARGS = -n 999 -m 999 --create --add 5 --add 3 --delete 5 --find 300
OPT_1=-O1
OPT_2=-O2
OPT_3=-O3

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

static_o1: staticlib
		@$(CC) $(LINKERFLAGS) $(CFLAGS) $(OPT_1) app.c -o static_o1 -static -L. -lstatic

shared_o1: sharedlib
		@$(CC) $(LINKERFLAGS) $(CFLAGS) $(OPT_1) app.c -o shared_o1 -L. -lshared

dll_o1: sharedlib
		@$(CC) app_dll.c $(CFLAGS) $(OPT_1) -o dll_o1 -ldl -DDLL

run_shared_o1: shared_o1
		./shared_o1 $(ARGS)

run_static_o1: static_o1
		./static_o1 $(ARGS)

run_dll_o1: dll_o1
		./dll_o1 $(ARGS)

static_o2: staticlib
		@$(CC) $(LINKERFLAGS) $(CFLAGS) $(OPT_2) app.c -o static_o2 -static -L. -lstatic

shared_o2: sharedlib
		@$(CC) $(LINKERFLAGS) $(CFLAGS) $(OPT_2) app.c -o shared_o2 -L. -lshared

dll_o2: sharedlib
		@$(CC) app_dll.c $(CFLAGS) $(OPT_2) -o dll_o2 -ldl -DDLL

run_shared_o2: shared_o2
		./shared_o2 $(ARGS)

run_static_o2: static_o2
		./static_o2 $(ARGS)

run_dll_o2: dll_o2
		./dll_o2 $(ARGS)

static_o3: staticlib
		@$(CC) $(LINKERFLAGS) $(CFLAGS) $(OPT_3) app.c -o static_o3 -static -L. -lstatic

shared_o3: sharedlib
		@$(CC) $(LINKERFLAGS) $(CFLAGS) $(OPT_3) app.c -o shared_o3 -L. -lshared

dll_o3: sharedlib
		@$(CC) app_dll.c $(CFLAGS) $(OPT_3) -o dll_o3 -ldl -DDLL

run_shared_o3: shared_o3
		./shared_o3 $(ARGS)

run_static_o3: static_o3
		./static_o3 $(ARGS)

run_dll_o3: dll_o3
		./dll_o3 $(ARGS)

run_tests: shared_o1 static_o1 dll_o1 shared_o2 static_o2 dll_o2 shared_o3 static_o3 dll_o3
		@echo "Flaga optymalizacji: " $(OPT_1) >> $(OUT)
		@echo "Biblioteka statyczna:" >> $(OUT)
		./static_o1 $(ARGS) >> $(OUT)
		
		@echo "Biblioteka dzielona:" >> $(OUT)
		./shared_o1 $(ARGS) >> $(OUT)
		
		@echo "Biblioteka ladowana dynamicznie:" >> $(OUT)
		./dll_o1 $(ARGS) >> $(OUT)

		@echo "Flaga optymalizacji: " $(OPT_2) >> $(OUT)
		@echo "Biblioteka statyczna:" >> $(OUT)
		./static_o2 $(ARGS) >> $(OUT)
		
		@echo "Biblioteka dzielona:" >> $(OUT)
		./shared_o2 $(ARGS) >> $(OUT)
		
		@echo "Biblioteka ladowana dynamicznie:" >> $(OUT)
		./dll_o2 $(ARGS) >> $(OUT)

		@echo "Flaga optymalizacji: " $(OPT_3) >> $(OUT)
		@echo "Biblioteka statyczna:" >> $(OUT)
		./static_o3 $(ARGS) >> $(OUT)
		
		@echo "Biblioteka dzielona:" >> $(OUT)
		./shared_o3 $(ARGS) >> $(OUT)
		
		@echo "Biblioteka ladowana dynamicznie:" >> $(OUT)
		./dll_o3 $(ARGS) >> $(OUT)

clean:
		@rm -f *.o
		@rm -f *.a
		@rm -f *.so
		@rm -f shared*
		@rm -f static**
		@rm -f dll*
		@rm -f results3b.txt

.PHONY: all shared static compile_o1 compile_o2 compile_o3 clean
