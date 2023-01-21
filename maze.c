#include "stdio.h"
#include "graphics.h"
#include "tools.h"
#include "time.h"
#include "stdlib.h"

//--- Generate random path ---//
int check_empty(int grid_size, int map[grid_size][grid_size], int x, int y)
{
    int space = true;
    int limit = 3;
    for (int b = 0; b < limit; b++)
    {
        for (int a = 0; a < limit; a++)
        {
            if (map[y + b][x + a] == obst)
            {
                space = false;
            }
        }
    }
    return space;
}

void fill_some_blocks(int grid_size, int map[grid_size][grid_size])
{
    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            if (check_empty(grid_size, map, x, y))
            {
                map[y + 1][x + 1] = obst;
            }
        }
    }
}

int weighted_random_direction(int *add_x, int *add_y, int rand_index, int acc_weight[3])
{
    int rand = random_int(0, rand_index);
    if (rand < acc_weight[0])
    {
        *add_y = -1;
    }
    else if (rand < acc_weight[1])
    {
        *add_x = 1;
    }
    else if (rand < acc_weight[2])
    {
        *add_y = 1;
    }
    else
    {
        *add_x = -1;
    }
}

void random_path(int grid_size, int map[grid_size][grid_size], int start_x, int start_y,
int end_x, int end_y, int rand_index, int acc_weight[3])
{
    int x = start_x, y = start_y, add_x = 0, add_y = 0;
    while (!(x == end_x && y == end_y))
    {
        if (x + add_x > 0 && x + add_x < grid_size - 1)
        {
            if (y + add_y > 0 && y + add_y < grid_size - 1)
            {
                x += add_x;
                y += add_y;
                map[y][x] = path;
            }
        }
        add_x = 0, add_y = 0;
     weighted_random_direction(&add_x, &add_y, rand_index, acc_weight);
    }
    fill_some_blocks(grid_size, map);
}

//---Generate random maze ---//
void plan_maze(int grid_size, int map[grid_size][grid_size], int start_x,
int start_y, int end_x, int end_y, int difficulty)
{
    int acc_weight1[3] = {1, 4, 7};
    int acc_weight2[3] = {4, 7, 8};
    int acc_weight3[3] = {3, 7, 8};
    srand(time(NULL));
    for (int i = 0; i < difficulty; i++)
    {
        random_path(grid_size, map, start_x, start_y, end_x, end_y, 7, acc_weight1);
    
        if (random_int(0, 1) == 0){
            random_path(grid_size, map, start_x + 2, end_y - 1, end_x - 2, start_y + 1, 8, acc_weight2);
        }else{
            random_path(grid_size, map, start_x + 1, end_y - 2, end_x - 1, start_y + 2, 8, acc_weight3);
        }
    }
}

//--- Draw maze ---//
void draw_grid(int big_length, int x_border, int y_border, int small_length)
{
    background();
    double x = x_border, y = y_border;
    while (x <= big_length + x_border)
    {
        drawLine(x, y, x, y + big_length);
        x += small_length;
    }
    x = x_border;
    while (y <= big_length + y_border)
    {
        drawLine(x, y, x + big_length, y);
        y += small_length;
    }
}

void start_end_point(int small_length, int x_border, int y_border, int start_x,
int start_y, int end_x, int end_y)
{
    int x, y;
    setColour(green);
    x = x_border + small_length * start_x;
    y = y_border + small_length * start_y;
    fillRect(x, y, small_length, small_length);
    setColour(red);
    x = x_border + small_length * end_x;
    y = y_border + small_length * end_y;
    fillRect(x, y, small_length, small_length);
}

void draw_maze(int grid_size, int small_length, int x_border, int y_border, int map[grid_size][grid_size],
int start_x, int start_y, int end_x, int end_y)
{
    int x, y;
    for (int row = 0; row < grid_size; row++)
    {
        y = y_border + small_length * row;
        for (int column = 0; column < grid_size; column++)
        {
            if (map[row][column] == obst)
            {
                x = x_border + small_length * column;
                setColour(black);
                fillRect(x, y, small_length, small_length);
            }
        }
    }
    start_end_point(small_length, x_border, y_border, start_x, start_y, end_x, end_y);
}