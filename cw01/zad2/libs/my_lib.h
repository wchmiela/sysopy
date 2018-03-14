#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <assert.h>
#include <values.h>

#define LIMIT 1000

typedef struct block
{
    char *signs;
    int m;                              //dlugosc bloku
} block_t;

typedef struct array
{
    struct block **blocks;
    int n;                              //ilosc blokow
} array_t;

typedef struct block_static
{
    char signs[LIMIT];
    int m;                              //dlugosc bloku
} block_static_t;

typedef struct array_static
{
    struct block_static blocks[LIMIT];
    int n;                              //ilosc blokow
} array_static_t;

struct block *make_block(int m);

struct array *make_array(int n, int m);

struct array_static *make_static_array(int n, int m);

void free_array(struct array *A);

bool add_blocks(int start, int end, struct array *A, struct block **B);

bool add_block(int index, struct array *A, struct block *B);

bool add_block_static(int index, struct array_static *A, struct block_static B);

bool remove_blocks(int start, int end, struct array *A);

bool remove_block(int index, struct array *A);

bool remove_blocks_static(int start, int end, struct array_static *A);

bool remove_block_static(int index, struct array_static *A);

struct block *find_block(int number, struct array *A);

struct block_static find_block_static(int number, struct array_static *A);

int sum_chars_in_block(struct block *B);

int sum_chars_in_block_static(struct block_static B);

char random_char();
