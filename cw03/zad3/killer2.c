#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{

    char *t = (char *) malloc(sizeof(char) * 1000 * 1000 * 5);
    for( int i = 0; i < 1000 * 1000 * 5; ++i )
    {
        t[i] = i;
    }

}
