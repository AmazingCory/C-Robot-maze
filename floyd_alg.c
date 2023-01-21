#include "stdio.h"
#include "tools.h"

struct Node
{
    int x;
    int y;
    int north;
    int east;
    int south;
    int west;
};

void check_all_neighbour_v2(int grid_size, int map[grid_size][grid_size], int x, int y, cndt *north,
cndt *east, cndt *south, cndt *west)
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

void create_node(struct Node *node, int x, int y,
int north, int east, int south, int west)
{
    node->x = x;
    node->y = y;
    node->north = north;
    node->east = east;
    node->south = south;
    node->west = west;
}

void get_nodes_from_map(int grid_size, int map[grid_size][grid_size],
int num_of_blocks, struct Node node_list[num_of_blocks])
{
    int ind = 0;
    cndt north, east, south, west;
    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            if (map[y][x] == path)
            {
                struct Node node;
                north = path, east = path, south = path, west = path;
                check_all_neighbour_v2(grid_size, map, x, y, &north, &east, &south, &west);
                create_node(&node, x, y, north, east, south, west);
                node_list[ind] = node;
                ind += 1;
            }
        }
    }
}

//--- Floyd's algorithm ---//
void change_value(int num_of_blocks, int dist_mtx[num_of_blocks][num_of_blocks],
int rout_mtx[num_of_blocks][num_of_blocks], int ind, int row, int column)
{
    int org_value = dist_mtx[row][column];
    if (-1 != dist_mtx[ind][column] && -1 != dist_mtx[row][ind])
    {
        int new_value = dist_mtx[ind][column] + dist_mtx[row][ind];
        if (org_value == -1 || org_value > new_value)
        {
            dist_mtx[row][column] = new_value;
            rout_mtx[row][column] = rout_mtx[row][ind];
        }
    }
}

void setup_matrices(int num_of_blocks, int dist_mtx[num_of_blocks][num_of_blocks],
int rout_mtx[num_of_blocks][num_of_blocks])
{
    for (int y = 0; y < num_of_blocks; y++)
    {
        for (int x = 0; x < num_of_blocks; x++)
        {
            dist_mtx[y][x] = -1;
            rout_mtx[y][x] = x;
        }
    }
}

void set_dist_mtx_values(struct Node node, int main_node_ind, int grid_size,
int num_of_blocks, int dist_mtx[num_of_blocks][num_of_blocks])
{
    int neigh_node_ind;
    if (node.north == path)
    {
        neigh_node_ind = node.x + (node.y - 1) * grid_size;
        dist_mtx[main_node_ind][neigh_node_ind] = 1;
        dist_mtx[neigh_node_ind][main_node_ind] = 1;
    }
    if (node.east == path)
    {
        neigh_node_ind = (node.x + 1) + node.y * grid_size;
        dist_mtx[main_node_ind][neigh_node_ind] = 1;
        dist_mtx[neigh_node_ind][main_node_ind] = 1;
    }
    if (node.south == path)
    {
        neigh_node_ind = node.x + (node.y + 1) * grid_size;
        dist_mtx[main_node_ind][neigh_node_ind] = 1;
        dist_mtx[neigh_node_ind][main_node_ind] = 1;
    }
    if (node.west == path)
    {
        neigh_node_ind = (node.x - 1) + node.y * grid_size;
        dist_mtx[main_node_ind][neigh_node_ind] = 1;
        dist_mtx[neigh_node_ind][main_node_ind] = 1;
    }
}

void setup_dist_mtx(int num_of_blocks, int dist_mtx[num_of_blocks][num_of_blocks],
int grid_size, struct Node node_list[num_of_blocks])
{
    struct Node node;
    int main_node_ind;
    for (int ind = 0; ind < num_of_blocks; ind++)
    {
        node = node_list[ind];
        main_node_ind = node.x + node.y * grid_size;
        if (main_node_ind < grid_size * grid_size)
        {
            set_dist_mtx_values(node, main_node_ind, grid_size, num_of_blocks, dist_mtx);
        }
        else
        {
            break;
        }
    }
}

void compute_matrices(int num_of_blocks, int dist_mtx[num_of_blocks][num_of_blocks],
int rout_mtx[num_of_blocks][num_of_blocks])
{
    int ind = 0, row, column;
    while (ind < num_of_blocks)
    {
        row = 0;
        while (row < num_of_blocks)
        {
            if (row == ind && row + 1 < num_of_blocks)
            {
                row += 1;
            }
            column = 0;
            while (column < num_of_blocks)
            {
                if (column == ind && column + 1 < num_of_blocks)
                {
                    column += 1;
                }
                change_value(num_of_blocks, dist_mtx, rout_mtx, ind, row, column);
                column += 1;
            }
            row += 1;
        }
        ind += 1;
    }
}

void compute_path(int num_of_blocks, int rout_mtx[num_of_blocks][num_of_blocks], int start_x,
int start_y, int end_x, int end_y, int path_size, int path[path_size], int grid_size)
{
    int x = start_x, y = start_y;
    int ind = 0, node_ind = x + y * grid_size, target_ind = end_x + end_y * grid_size;
    while (node_ind != target_ind)
    {
        path[ind] = node_ind;
        node_ind = rout_mtx[node_ind][target_ind];
        ind += 1;
    }
    path[ind] = node_ind;
}

void find_floyd_path(int grid_size, int map[grid_size][grid_size], int path_size, int path[path_size],
int start_x, int start_y, int end_x, int end_y)
{
    int num_of_blocks = grid_size * grid_size;
    struct Node node_list[num_of_blocks];
    get_nodes_from_map(grid_size, map, num_of_blocks, node_list);

    int dist_mtx[num_of_blocks][num_of_blocks], rout_mtx[num_of_blocks][num_of_blocks];
    setup_matrices(num_of_blocks, dist_mtx, rout_mtx);
    setup_dist_mtx(num_of_blocks, dist_mtx, grid_size, node_list);

    compute_matrices(num_of_blocks, dist_mtx, rout_mtx);

    compute_path(num_of_blocks, rout_mtx, start_x, start_y, end_x, end_y, path_size, path, grid_size);
}