#include "stdio.h"
#include "graphics.h"
#include "tools.h"
#include "time.h"
#include "stdlib.h"
#include "floyd_alg.h"
#include "string.h"

struct Robots
{
    int x;
    int y;
    dir direction;
};

//--- Robot tools ---//
void print_robot(struct Robots robot)
{
    printf("\nRobot details:\nx: %d\n", robot.x);
    printf("y: %d\n", robot.y);
    printf("dir: %d\n\n", robot.direction);
}

int draw_robot(struct Robots robot, int x_border, int y_border, int small_length)
{
    int num_of_sides = 4;
    int x_coords[num_of_sides], y_coords[num_of_sides];
    int dir[4][8] = {{0, 1, 2, 1, 2, 1, 2, 0},
                     {0, 1, 0, 2, 2, 1, 0, 1},
                     {0, 1, 2, 1, 0, 1, 0, 2},
                     {2, 1, 2, 0, 0, 1, 2, 1}};
    int x_box, y_box;
    for (int i = 0; i < num_of_sides; i++)
    {
        x_coords[i] = small_length * (robot.x + dir[robot.direction][i] * 0.5) + x_border;
        y_coords[i] = small_length * (robot.y + dir[robot.direction][i + num_of_sides] * 0.5) + y_border;
    }
    clear();
    fillPolygon(num_of_sides, x_coords, y_coords);
}

void check_all_neighbour(int grid_size, int map[grid_size][grid_size], int x, int y,
                         cndt *north, cndt *east, cndt *south, cndt *west)
{
    if (map[y - 1][x] == obst)
    {
        *north = obst;
    }
    if (map[y][x + 1] == obst)
    {
        *east = obst;
    }
    if (map[y + 1][x] == obst)
    {
        *south = obst;
    }
    if (map[y][x - 1] == obst)
    {
        *west = obst;
    }
}

bool is_clamped(int grid_size, struct Robots *robot, int map[grid_size][grid_size])
{
    int x = robot->x, y = robot->y;
    if (robot->direction % 2 == 0)
    {
        if (map[y][x - 1] == obst && map[y][x + 1] == obst)
        {
            return true;
        }
    }
    else
    {
        if (map[y - 1][x] == obst && map[y + 1][x] == obst)
        {
            return true;
        }
    }
    return false;
}

bool can_forward(int grid_size, struct Robots robot, int map[grid_size][grid_size])
{
    cndt north = path, east = path, south = path, west = path;
    check_all_neighbour(grid_size, map, robot.x, robot.y, &north, &east, &south, &west);

    switch (robot.direction)
    {
    case 0:
        if (north == path)
        {
            return true;
        }
        break;
    case 1:
        if (east == path)
        {
            return true;
        }
        break;
    case 2:
        if (south == path)
        {
            return true;
        }
        break;
    case 3:
        if (west == path)
        {
            return true;
        }
    }
    return false;
}

void move_forward(struct Robots *robot, int pace)
{
    switch (robot->direction)
    {
    case up:
        robot->y -= pace;
        break;
    case right:
        robot->x += pace;
        break;
    case down:
        robot->y += pace;
        break;
    case left:
        robot->x -= pace;
    }
}

bool at_point(int robot_x, int robot_y, int x, int y)
{
    if (robot_x == x && robot_y == y)
    {
        return true;
    }
    return false;
}

//--- The random robot ---//
void make_turn(struct Robots *robot, dir turn)
{
    if (turn == right)
    {
        if (robot->direction == left)
        {
            robot->direction = up;
        }
        else
        {
            robot->direction += 1;
        }
    }
    else
    {
        if (robot->direction == up)
        {
            robot->direction = down;
        }
        else
        {
            robot->direction -= 1;
        }
    }
}

int face_better_dir(dir direction, int mean, int *min, int *max)
{
    if (direction == up || direction == right)
    {
        *min -= 1;
        if (direction == up)
        {
            *min -= 1;
        }
    }
    else
    {
        *max += 1;
        if (direction == left)
        {
            *max += 1;
        }
    }
}

void turn_one_way(int grid_size, struct Robots *robot, int map[grid_size][grid_size])
{
    int mean = 3;
    int min = mean - 1, max = mean;
    face_better_dir(robot->direction, mean, &min, &max);
    if (random_int(min, max) < mean)
    {
        make_turn(robot, right);
    }
    else
    {
        make_turn(robot, left);
    }
}

void move_random_robot(int grid_size, struct Robots *robot, int map[grid_size][grid_size])
{
    int rand_index = 15;
    if ((robot->direction == up || robot->direction == left) && is_clamped(grid_size, robot, map) == false)
    {
        rand_index = 6;
    }
    if (can_forward(grid_size, *robot, map) == true && random_int(0, rand_index) > 0)
    {
        move_forward(robot, 1);
    }
    else
    {
        if (is_clamped(grid_size, robot, map) == true)
        {
            make_turn(robot, right);
            sleep(100);
            make_turn(robot, right);
        }
        else
        {
            turn_one_way(grid_size, robot, map);
        }
        sleep(150);
    }
}

void setup_random_robot(int grid_size, struct Robots *robot, int x_border, int y_border, int small_length, int map[grid_size][grid_size])
{
    move_random_robot(grid_size, robot, map);
    draw_robot(*robot, x_border, y_border, small_length);
}

//--- Floyd robot ---//
bool wrong_dir(int dir, int diff, int grid_size)
{
    bool pass = false;
    if (dir == up && diff == -1 * grid_size)
    {
        pass = true;
    }
    else if (dir == right && diff == 1)
    {
        pass = true;
    }
    else if (dir == down && diff == grid_size)
    {
        pass = true;
    }
    else if (dir == left && diff == -1)
    {
        pass = true;
    }
    return pass;
}

void face_right_dir(struct Robots *robot, int diff, int grid_size, int x_border, int y_border, int small_length)
{
    if (wrong_dir(robot->direction - 1, diff, grid_size) == true)
    {
        make_turn(robot, left);
    }
    else if (wrong_dir(robot->direction + 1, diff, grid_size) == true)
    {
        make_turn(robot, right);
        draw_robot(*robot, x_border, y_border, small_length);
        sleep(200);
    }
}

void animate_floyd_robot(struct Robots *robot, int x_border, int y_border, int small_length)
{
    int pace = 2;
    int old_x = robot->x, old_y = robot->y;
    double current_x = old_x, current_y = old_y;
    move_forward(robot, 1);
    int target_x = robot->x, target_y = robot->y;
    double add_x = (target_x - old_x) / pace, add_y = (target_y - old_y) / pace;
    move_forward(robot, -1);
    while (abs(current_x - target_x) > 0.1 || abs(current_y - target_y) > 0.1)
    {
        move_forward(robot, 1/pace);
        draw_robot(*robot, x_border, y_border, small_length);
        sleep(100);
        current_x += add_x;
        current_y += add_y;
    }
}

void move_floyd_robot(struct Robots *robot, int length, int path[length + 1], int start_ind,
                      int grid_size, int x_border, int y_border, int small_length, int end_x, int end_y)
{
    int x, y, value = start_ind;
    for (int ind = 0; ind <= length; ind++)
    {
        if (!at_point(robot->x, robot->y, end_x, end_y))
        {
            face_right_dir(robot, path[ind + 1] - value, grid_size, x_border, y_border, small_length);
            draw_robot(*robot, x_border, y_border, small_length);
            sleep(200);
            value = path[ind + 1];
            move_forward(robot, 1);
            draw_robot(*robot, x_border, y_border, small_length);
            sleep(150);
        }
    }
}

void setup_floyd_robot(struct Robots *robot, int grid_size, int map[grid_size][grid_size], int start_x, int start_y, int end_x, int end_y,
                       int path_size, int path[path_size], int x_border, int y_border, int small_length)
{
    find_floyd_path(grid_size, map, path_size, path, start_x, start_y, end_x, end_y);

    int target_ind = end_x + end_y * grid_size, length;
    for (length = 0; path[length] != target_ind; length++)
    {
        continue;
    }
    move_floyd_robot(robot, length, path, path[0], grid_size, x_border, y_border, small_length, end_x, end_y);
}

//--- Robot setup ---//
void run_robot(char algorithm[1], int grid_size, int x_border, int y_border, int small_length, int big_length,
               int map[grid_size][grid_size], int start_x, int start_y, int end_x, int end_y)
{
    struct Robots robot;
    robot.x = start_x;
    robot.y = start_y;
    robot.direction = right;

    foreground();
    setColour(blue);
    draw_robot(robot, x_border, y_border, small_length);
    sleep(600);

    // The random algorithm
    int ascii_alg = (int)algorithm[0];
    if (ascii_alg == 114 || ascii_alg == 82) // ascii "r", "R" = 114, 82
    {
        while (!at_point(robot.x, robot.y, end_x, end_y))
        {
            setup_random_robot(grid_size, &robot, x_border, y_border, small_length, map);
            sleep(150);
        }
    }
    // Floyd's algorithm
    else if (ascii_alg == 102 || ascii_alg == 70) // ascii "f", "F" = 102, 70
    {
        int path_size = 100, path[path_size];
        setup_floyd_robot(&robot, grid_size, map, start_x, start_y, end_x, end_y, path_size, path, x_border, y_border, small_length);
    }
}