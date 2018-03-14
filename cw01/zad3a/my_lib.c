#include "my_lib.h"



struct block *make_block(int m)
{
    struct block *B = malloc(sizeof(struct block));
    B->m = m;
    B->signs = calloc((size_t) m, sizeof(char));
    for( int i = 0; i < B->m; ++i )
    {
        B->signs[i] = random_char();
    }

    return B;
}

struct array *make_array(int n, int m)
{
    struct array *A = malloc(sizeof(struct array));
    A->n = n;
    A->blocks = malloc(n * sizeof(struct block));
    for( int i = 0; i < n; ++i )
    {
        A->blocks[i] = make_block(m);
    }

    return A;
}

struct array_static *make_static_array(int n, int m)
{
    struct array_static *A = malloc(sizeof(struct array_static));
    A->n = n;
    for( int i = 0; i < n; ++i )
    {
        for( int j = 0; j < m; ++j )
        {
            A->blocks[i].signs[j] = random_char();
            A->blocks[i].m = m;
        }
    }

    return A;
}

void free_array(struct array *A)
{
    for( int i = 0; i < A->n; ++i )
    {
        free(A->blocks[i]->signs);
        A->blocks[i]->signs = NULL;
        free(A->blocks[i]);
        A->blocks[i] = NULL;
    }

    free(A);
    A = NULL;
}

bool add_blocks(int start, int end, struct array *A, struct block **B)
{
    if( A == NULL || B == NULL || start < 0 || end >= A->n || start > end )
    {
        return false;
    }

    int j = 0;
    for( int i = start; i <= end; ++i )
    {
        A->blocks[i] = B[j++];
    }

    return true;
}

bool add_block(int index, struct array *A, struct block *B)
{
    if( A == NULL || B == NULL || index < 0 || index >= A->n )
    {
        return false;
    }

    A->blocks[index] = B;

    return true;
}

bool add_block_static(int index, struct array_static *A, struct block_static B)
{
    if( A == NULL || index < 0 || index >= A->n )
    {
        return false;
    }

    A->blocks[index] = B;

    return true;
}

bool remove_blocks(int start, int end, struct array *A)
{
    if( A == NULL || start < 0 || end >= A->n || start > end )
    {
        return false;
    }

    for( int i = start; i <= end; ++i )
    {
        A->blocks[i] = NULL;
    }

    return true;
}

bool remove_block(int index, struct array *A)
{
    if( A == NULL || index < 0 || index >= A->n )
    {
        return false;
    }

    A->blocks[index] = NULL;

    return true;
}

bool remove_blocks_static(int start, int end, struct array_static *A)
{
    if( A == NULL || start < 0 || end >= A->n || start > end )
    {
        return false;
    }

    for( int i = start; i <= end; ++i )
    {
        for( int j = 0; j < A->blocks[i].m; ++j )
        {
            A->blocks[i].signs[j] = 0;
        }

    }

    return true;
}

bool remove_block_static(int index, struct array_static *A)
{
    if( A == NULL || index < 0 || index >= A->n )
    {
        return false;
    }

    for( int j = 0; j < A->blocks[index].m; ++j )
    {
        A->blocks[index].signs[j] = 0;
    }

    return true;
}

struct block *find_block(int number, struct array *A)
{
    int diff = INT_MAX;
    int diff_idx = 0;
    for( int i = 0; i < A->n; ++i )
    {
        int sum = sum_chars_in_block(A->blocks[i]);
        int d = abs(sum - number);

        if( d <= diff )
        {
            diff = d;
            diff_idx = i;
        }
    }

    return A->blocks[diff_idx];
}

struct block_static find_block_static(int number, struct array_static *A)
{
    int diff = INT_MAX;
    int diff_idx = 0;
    for( int i = 0; i < A->n; ++i )
    {
        int sum = sum_chars_in_block_static(A->blocks[i]);
        int d = abs(sum - number);

        if( d <= diff )
        {
            diff = d;
            diff_idx = i;
        }
    }

    return A->blocks[diff_idx];
}

int sum_chars_in_block(struct block *B)
{
    int sum = 0;
    for( int i = 0; i < B->m; ++i )
    {
        sum += B->signs[i];
    }

    return sum;
}

int sum_chars_in_block_static(struct block_static B)
{
    int sum = 0;
    for( int i = 0; i < B.m; ++i )
    {
        sum += B.signs[i];
    }

    return sum;
}

char random_char()
{
    return (char) (' ' + (rand() % 2));
}