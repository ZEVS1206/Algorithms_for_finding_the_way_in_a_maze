#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <chrono>

#include "jps4_implementation.h"


//Heuristic for algorithm A* (Manhattan distance)
static int heuristic(const Coord_type &first, const Coord_type &second)
{
    return abs(first.first - second.first) + abs(first.second - second.second);
}
//Can we go to the cell
static bool is_walkable(const std::vector<std::vector<int>>& maze, int x, int y)
{
    return x >= 0 && x < (int)maze.size() &&
           y >= 0 && y < (int)maze[0].size() &&
           maze[x][y] == 0;
}
//Check if significant cell has "forced" neighbor, direction -> (dx, dy)
static bool has_forced_neighbor(const std::vector<std::vector<int>> &maze, int x, int y, int dx, int dy)
{
    //on the right or left
    if (dx != 0)
    {
        if (!is_walkable(maze, x, y - 1) && is_walkable(maze, x + dx, y - 1))
        {
            return true;
        }
        if (!is_walkable(maze, x, y + 1) && is_walkable(maze, x + dx, y + 1))
        {
            return true;
        }
    }
    //up or down
    if (dx == 0)
    {
        if (!is_walkable(maze, x - 1, y) && is_walkable(maze, x - 1, y + dy))
        {
            return true;
        }
        if (!is_walkable(maze, x + 1, y) && is_walkable(maze, x + 1, y + dy))
        {
            return true;
        }
    }
    return false;
}

//If moving horizontally (dx != 0): jump point when aligned with goal's row
//If moving vertically  (dy != 0): jump point when aligned with goal's column
//This lets the algorithm "turn" toward the goal even with no walls nearby
static bool is_on_goal_line(int x, int y, int dx, int dy, const Coord_type &goal_point)
{
    if (dx != 0) return x == goal_point.first;  //horizontal — same row as goal
    if (dy != 0) return y == goal_point.second; //vertical   — same column as goal
    return false;
}

//Check and iterative get jump points
static Coord_type get_jump_point(const std::vector<std::vector<int>> &maze, int x, int y, int dx, int dy, const Coord_type &goal_point)
{
    while (true)
    {
        int new_x = x + dx;
        int new_y = y + dy;
        //Wall or border
        if (!is_walkable(maze, new_x, new_y))
        {
            return {-1, -1};
        }
        //Found goal point
        if (new_x == goal_point.first && new_y == goal_point.second)
        {
            return {new_x, new_y};
        }
        //Is it jump point
        if (has_forced_neighbor(maze, new_x, new_y, dx, dy))
        {
            return {new_x, new_y};
        }
        //Aligned with goal line — must stop here to allow turning toward the goal
        if (is_on_goal_line(new_x, new_y, dx, dy, goal_point))
        {
            return {new_x, new_y};
        }
        x = new_x;
        y = new_y;
    }
}

//Main function of algorithm

Result jps4_work_process(const std::vector<std::vector<int>> maze, const Coord_type &start_point, const Coord_type &goal_point)
{
    auto start_time = std::chrono::high_resolution_clock::now();

    if (!is_walkable(maze, start_point.first, start_point.second) || !is_walkable(maze, goal_point.first, goal_point.second))
    {
        return {{}, 0, 0.0};
    }
    //dictionary of best costs from start to points
    std::unordered_map<Coord_type, int, Pair_hash> cost_from_start_score;
    //dictionary of parents of points
    std::unordered_map<Coord_type, Coord_type, Pair_hash> parent;
    //priority queue for A*, prioritize by minimum total_cost
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;

    cost_from_start_score[start_point] = 0;
    open_set.push({start_point, 0, heuristic(start_point, goal_point)});

    int total_visited_nodes = 0;

    while (!open_set.empty())
    {
        Node current_node = open_set.top();
        open_set.pop();

        //Skip stale entries — a better path to this node was already found
        auto it = cost_from_start_score.find(current_node.position);
        if (it != cost_from_start_score.end() && current_node.cost_from_start > it->second)
        {
            continue;
        }

        total_visited_nodes++;

        if (current_node.position == goal_point)
        {
            //Recover path
            std::vector <Coord_type> path;
            Coord_type current = goal_point;
            while (current != start_point)
            {
                path.push_back(current);
                current = parent[current];
            }
            path.push_back(start_point);
            std::reverse(path.begin(), path.end());

            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();

            return {path, total_visited_nodes, total_time};
        }

        const int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};//Directions for movement
        for (auto &offset: directions)
        {
            Coord_type jump_point = get_jump_point(maze, current_node.position.first, current_node.position.second, offset[0], offset[1], goal_point);

            if (jump_point.first == -1)
            {
                continue;
            }

            int new_potentian_best_cost = cost_from_start_score[current_node.position] +
                                          abs(current_node.position.first - jump_point.first) +
                                          abs(current_node.position.second - jump_point.second);
            auto it = cost_from_start_score.find(jump_point);
            if (it == cost_from_start_score.end() || new_potentian_best_cost < cost_from_start_score[jump_point])
            {
                cost_from_start_score[jump_point] = new_potentian_best_cost;
                parent[jump_point] = current_node.position;
                int total_cost = new_potentian_best_cost + heuristic(jump_point, goal_point);
                open_set.push({jump_point, new_potentian_best_cost, total_cost});
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    //If algorithm has not found path
    return {{}, total_visited_nodes, total_time};
}
