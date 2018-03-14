#include "app_dll.h"

double real_start;
double real_last;

double system_start;
double system_last;

double user_start;
double user_last;

void print_test(double real_time, double user_time, double system_time)
{
    printf("real:\t %f [s]\t\t delta_start: %f [s]\t\t delta_last %f [s]\n",
           real_time, real_time - real_start, real_time - real_last);


    printf("user:\t %f [s]\t\t delta_start: %f [s]\t\t delta_last %f [s]\n",
           user_time, user_time - user_start, user_time - user_last);

    printf("system:\t %f [s]\t\t delta_start: %f [s]\t\t delta_last %f [s]\n",
           system_time, system_time - system_start, system_time - system_last);
}

void set_times()
{
    printf("%s\n", "---Mierzenie czasów---");

    struct tms buffer;
    times(&buffer);

    double real = clock() / (double) CLOCKS_PER_SEC;
    double user = buffer.tms_utime / (double) CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / (double) CLOCKS_PER_SEC;

    real_start = real;
    real_last = real;

    user_start = user;
    user_last = user;

    system_start = sys;
    system_last = sys;
}

void update_times()
{
    struct tms buffer;
    times(&buffer);

    double real = clock() / (double) CLOCKS_PER_SEC;
    double user = buffer.tms_utime / (double) CLOCKS_PER_SEC;
    double sys = buffer.tms_stime / (double) CLOCKS_PER_SEC;

    print_test(real, user, sys);
    real_last = real;
    system_last = sys;
    user_last = user;
}

//-n 5 -m 100 5 -s 7 --create --add 5 --add 3 --delete 5 --find 3
int main(int argc, char *argv[])
{
    handler = dlopen("./libshared.so", RTLD_NOW);

    make_block = dlsym(handler, "make_block");

    make_array = dlsym(handler, "make_array");

    make_static_array = dlsym(handler, "make_static_array");

    free_array = dlsym(handler, "free_array");

    add_blocks = dlsym(handler, "add_blocks");

    add_block = dlsym(handler, "add_block");

    add_block_static = dlsym(handler, "add_block_static");

    remove_blocks = dlsym(handler, "remove_blocks");

    remove_block = dlsym(handler, "remove_block");

    remove_blocks_static = dlsym(handler, "remove_blocks_static");

    remove_block_static = dlsym(handler, "remove_block_static");

    find_block = dlsym(handler, "find_block");

    find_block_static = dlsym(handler, "find_block_static");

    sum_chars_in_block = dlsym(handler, "sum_chars_in_block");

    sum_chars_in_block_static = dlsym(handler, "sum_chars_in_block_static");

    random_char = dlsym(handler, "random_char");

    const int OPERATION_LIMIT = 10;
    int next_option = 0;
    int n = -1;
    int m = -1;
    bool dynamic = true;

    bool create_operation = false;

    bool add_operation = false;
    int blocks_to_add[OPERATION_LIMIT];
    memset(blocks_to_add, -1, sizeof(blocks_to_add));
    int blocks_to_add_idx = 0;                                  //index used for adding blocks from command line

    bool delete_operation = false;
    int blocks_to_delete[OPERATION_LIMIT];
    memset(blocks_to_delete, -1, sizeof(blocks_to_delete));
    int blocks_to_delete_idx = 0;                               //index used for adding blocks from command line

    bool find_operation = false;
    int find_sum = -1;

    const char *short_opt = "n:m:so:";
    struct option long_opt[] =
            {
                    { "create", no_argument,       NULL, 'c' },
                    { "add",    required_argument, NULL, 'a' },
                    { "delete", required_argument, NULL, 'd' },
                    { "find",   required_argument, NULL, 'f' },
                    { NULL, 0,                     NULL, 0 }
            };

    do
    {
        next_option = getopt_long(argc, argv, short_opt, long_opt, NULL);
        if( next_option == 'n' )
        {
            n = atoi(optarg);
        } else if( next_option == 'm' )
        {
            m = atoi(optarg);
        } else if( next_option == 's' )
        {
            dynamic = false;
        } else if( next_option == 'c' )
        {
            create_operation = true;
        } else if( next_option == 'a' )
        {
            add_operation = true;
            int elem = atoi(optarg);
            blocks_to_add[blocks_to_add_idx++] = elem;
        } else if( next_option == 'd' )
        {
            delete_operation = true;
            int elem = atoi(optarg);
            blocks_to_delete[blocks_to_delete_idx++] = elem;
        } else if( next_option == 'f' )
        {
            find_operation = true;
            find_sum = atoi(optarg);
        }


    } while( next_option != -1 );

    if( ( n <= 0 || m <= 0 ) && !create_operation )
    {
        printf("%s\n", "Podano niepoprawne argumenty");
        exit(1);
    }

    
    if ( (n >= LIMIT || m >= LIMIT) && !dynamic){
        printf("%s\n", "Podano niepoprawne argumenty - LIMIT na 1000 w przypadku alokacji statycznej");
        exit(1);
    }

    printf("%s\n", "---Argumenty---");
    printf("Operacje: %s%s%s%s\n", create_operation ? "create " : "",
           add_operation ? "add " : "",
           delete_operation ? "delete " : "",
           find_operation ? "find " : "");
    printf("Rozmiar tablicy: %d. Rozmiar bloków: %d\n", n, m);
    if( add_operation )
    {
        printf("Indeksy blokow do dodania: ");
        for( int i = 0; i < blocks_to_add_idx; ++i )
        {
            printf("%d ", blocks_to_add[i]);
        }
        printf("\n");
    }
    if( delete_operation )
    {
        printf("Indeksy blokow do usuniecia: ");
        for( int i = 0; i < blocks_to_delete_idx; ++i )
        {
            printf("%d ", blocks_to_delete[i]);
        }
        printf("\n");
    }
    if( find_operation )
    {
        printf("Szukanie bloku o podobnej sumie jak: %d\n", find_sum);
    }
    printf("Alokacja: %s\n", dynamic ? "dynamiczna" : "statyczna");

    set_times();

    struct array *array_dynamic;
    struct array_static *array_static;

    if( create_operation )
    {
        if( dynamic )
        {
            printf("%s\n", "Dynamiczna alokacja");
            array_dynamic = make_array(n, m);
        } else
        {
            printf("%s\n", "Statyczna alokacja");
            array_static = make_static_array(n, m);
        }
        update_times();
    }

    if( find_operation )
    {
        if( dynamic )
        {
            printf("%s\n", "Wyszukiwanie - dynamiczna alokacja");
            find_block(find_sum, array_dynamic);        
        } else
        {
            printf("%s\n", "Wyszukiwanie - statyczna alokacja");
            find_block_static(find_sum, array_static);
        }
        update_times();
    }

    if( add_operation )
    {
        if( dynamic )
        {
            printf("%s\n", "Dodawanie - dynamiczna alokacja");
            for( int i = 0; i < blocks_to_add_idx; ++i )
            {
                add_block(blocks_to_add[i], array_dynamic, make_block(m));
            }
        } else
        {
            printf("%s\n", "Dodawanie - statyczna alokacja");
            for( int i = 0; i < blocks_to_add_idx; ++i )
            {
                struct block_static block;
                block.m = m;
                for( int j = 0; j < m; ++j )
                {
                    block.signs[j] = random_char();
                }
                add_block_static(blocks_to_add[i], array_static, block);
            }
        }
        update_times();
    }

    if( delete_operation )
    {
        if( dynamic )
        {
            printf("%s\n", "Usuwanie - dynamiczna alokacja");
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                remove_block(blocks_to_delete[i], array_dynamic);

            }
        } else
        {
            printf("%s\n", "Usuwanie - statyczna alokacja");
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                remove_block_static(blocks_to_delete[i], array_static);
            }
        }
        update_times();
    }


    if( delete_operation )
    {
        if( dynamic )
        {
            printf("%s\n", "Usuwanie - wariant - usuniecie i dodanie w to samo miejsce - dynamiczna alokacja");
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                remove_block(blocks_to_delete[i], array_dynamic);
                add_block(blocks_to_delete[i], array_dynamic, make_block(m));
            }
        } else
        {
            printf("%s\n", "Usuwanie - wariant - usuniecie i dodanie w to samo miejsce - statyczna alokacja");
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                remove_block_static(blocks_to_delete[i], array_static);
                struct block_static block;
                block.m = m;
                for( int j = 0; j < m; ++j )
                {
                    block.signs[j] = random_char();
                }
                add_block_static(blocks_to_delete[i], array_static, block);
            }
        }
        update_times();
    }

    if( delete_operation )
    {
        if( dynamic )
        {
            printf("%s\n", "Usuwanie - wariant - usuniecie blokow, nastepnie dodanie blokow - dynamiczna alokacja");
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                remove_block(blocks_to_delete[i], array_dynamic);
            }
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                add_block(blocks_to_delete[i], array_dynamic, make_block(m));
            }
        } else
        {
            printf("%s\n", "Usuwanie - wariant - usuniecie blokow, nastepnie dodanie blokow - statyczna alokacja");
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                remove_block_static(blocks_to_delete[i], array_static);
            }
            for( int i = 0; i < blocks_to_delete_idx; ++i )
            {
                struct block_static block;
                block.m = m;
                for( int j = 0; j < m; ++j )
                {
                    block.signs[j] = random_char();
                }
                add_block_static(blocks_to_delete[i], array_static, block);
            }
        }
        update_times();
    }

    if( dynamic )
    {   
        free_array(array_dynamic);
    } else
    {
        free(array_static);
    }

    dlclose(handler);
    
    return 0;
}