#include "stdio.h"
#include "graphics.h"
#include "maze.h"
#include "robot.h"
#include "tools.h"
#include "string.h"

//--- Read text files ---//
int read_txt_file(int num_of_lines, int line_length, char data[num_of_lines][line_length])
{
    FILE *file;

    file = fopen("config.txt", "r");
    if (file == NULL)
    {
        return 1;
    }

    int line = 0;
    while (!feof(file) && !ferror(file))
    {
        if (fgets(data[line], line_length, file) != NULL)
        {
            line++;
        }
    }
    fclose(file);
    return line;
}

int exceed_value(int value, int min, int max)
{
    if (value < min)
    {
        value = min;
    }
    else if (value > max)
    {
        value = max;
    }
    return value;
}

void check_values(int value, int line, int *big_length, int *grid_size, int *x_border, int *y_border, int *difficulty)
{
    switch (line)
    {
    case 0:
        value = exceed_value(value, 400, 650);
        *big_length = value;
        break;
    case 1:
        value = exceed_value(value, 4, 15);
        *grid_size = value;
        break;
    case 2:
        value = exceed_value(value, 0, 465);
        *x_border = value;
        break;
    case 3:
        value = exceed_value(value, 0, 60);
        *y_border = value;
        break;
    case 4:
        value = exceed_value(value, 1, 3);
        *difficulty = 4 - value;
    }
}

int get_int(char *str_value, int line, int *big_length, int *grid_size, int *x_border,
int *y_border, int *difficulty, int indented)
{
    int value = 0;
    int value_length = strlen(str_value) - 2;
    for (int ind = indented; ind <= value_length; ind++) // skipping the ": "
    {
        value = value * 10 + (int)str_value[ind] - 48; // accii "0" = 48
    }
    check_values(value, line, big_length, grid_size, x_border, y_border, difficulty);
}

int get_data_from_file(char algorithm[1], int *big_length, int *grid_size, int *x_border, int *y_border, int *difficulty)
{
    int num_of_lines = 6, line_length = 70, indented = 2;
    char data[num_of_lines][line_length];
    num_of_lines = read_txt_file(num_of_lines, line_length, data);
    for (int line = 0; line < num_of_lines; line++)
    {
        char *str_ptr = strchr(data[line], 58); // ascii ":" = 58
        if (line == num_of_lines - 1)
        {
            strcpy(algorithm, &str_ptr[indented]);
        }
        else
        {
            get_int(str_ptr, line, big_length, grid_size, x_border, y_border, difficulty, indented);
        }
    }
    return 0;
}

//--- Setup array ---//
int setup_array(int grid_size, int map[grid_size][grid_size])
{
    int value;
    for (int b = 0; b < grid_size; b++)
    {
        for (int a = 0; a < grid_size; a++)
        {
            map[b][a] = obst;
        }
    }
}

//--- Main ---//
int main()
{
    char algorithm[1];
    int big_length, grid_size, x_border, y_border, difficulty;
    get_data_from_file(algorithm, &big_length, &grid_size, &x_border, &y_border, &difficulty);

    int small_length = big_length / grid_size;
    int x_window_size = big_length + x_border * 2;
    int y_window_size = big_length + y_border * 2;
    setWindowSize(x_window_size, y_window_size);

    draw_grid(big_length, x_border, y_border, small_length);

    int map[grid_size][grid_size];
    setup_array(grid_size, map);

    int start_x = 1, start_y = 1;
    int end_x = big_length / small_length - 2;
    int end_y = end_x;
    plan_maze(grid_size, map, start_x, start_y, end_x, end_y, difficulty);
    draw_maze(grid_size, small_length, x_border, y_border, map, start_x, start_y, end_x, end_y);

    run_robot(algorithm, grid_size, x_border, y_border, small_length, big_length, map, start_x, start_y, end_x, end_y);
}