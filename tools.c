#include "stdio.h"
#include "time.h"
#include "stdlib.h"

int random_int(int from, int to)
{
    int rand_num = rand() % (to - from + 1) + from;
    return rand_num;
}

int max(int a, int b)
{
    if (a < b)
    {
        return b;
    }
    else
    {
        return a;
    }
}

int abs(int a)
{
    if (a < 0)
    {
        return a * -1;
    }
    return a;
}

void print_array(int size, int array[size][size])
{
    int value;
    printf("\n");
    for (int b = 0; b < size; b++)
    {
        for (int a = 0; a < size; a++)
        {
            value = array[b][a];
            printf("%i ", value);
        }
        printf("\n");
    }
}