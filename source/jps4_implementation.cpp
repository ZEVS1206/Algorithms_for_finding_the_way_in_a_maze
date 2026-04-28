#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <chrono>
#include "jps4_implementation.h"

static int heuristic(const Coord_type &first, const Coord_type &second)
{
    return abs(first.first - second.first) + abs(first.second - second.second);
}
static bool is_walkable(const std::vector<std::vector<int>>& maze, int x, int y)
{
    return x >= 0 && x < (int)maze.size() &&
           y >= 0 && y < (int)maze[0].size() &&
           maze[x][y] == 0;
}
static bool has_forced_neighbor(const std::vector<std::vector<int>> &maze, int x, int y, int dx, int dy)
{
    if (dx != 0)
    {
        if (!is_walkable(maze, x, y - 1) && is_walkable(maze, x + dx, y - 1)) return true;
        if (!is_walkable(maze, x, y + 1) && is_walkable(maze, x + dx, y + 1)) return true;
    }
    if (dx == 0)
    {
        if (!is_walkable(maze, x - 1, y) && is_walkable(maze, x - 1, y + dy)) return true;
        if (!is_walkable(maze, x + 1, y) && is_walkable(maze, x + 1, y + dy)) return true;
    }
    return false;
}
static bool is_on_goal_line(int x, int y, int dx, int dy, const Coord_type &goal_point)
{
    if (dx != 0) return x == goal_point.first;
    if (dy != 0) return y == goal_point.second;
    return false;
}

//Scan a perpendicular ray from (x, y) in direction (pdx, pdy) iteratively (no recursion).
//Returns true if a jump point exists along that ray — avoids stack overflow on large open areas.
static bool has_perpendicular_jump(const std::vector<std::vector<int>> &maze, int x, int y, int pdx, int pdy, const Coord_type &goal_point)
{
    int px = x;
    int py = y;
    while (true)
    {
        px += pdx;
        py += pdy;
        if (!is_walkable(maze, px, py))
        {
            return false;
        }
        if (px == goal_point.first && py == goal_point.second)
        {
            return true;
        }
        if (is_on_goal_line(px, py, pdx, pdy, goal_point))
        {
            return true;
        }
        if (has_forced_neighbor(maze, px, py, pdx, pdy))
        {
            return true;
        }
    }
}

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
        //Check perpendicular directions iteratively (no recursion — avoids stack overflow on large open areas)
        if (dx != 0) //moving vertically — check horizontal
        {
            if (has_perpendicular_jump(maze, new_x, new_y, 0,  1, goal_point)) return {new_x, new_y};
            if (has_perpendicular_jump(maze, new_x, new_y, 0, -1, goal_point)) return {new_x, new_y};
        }
        if (dy != 0) //moving horizontally — check vertical
        {
            if (has_perpendicular_jump(maze, new_x, new_y,  1, 0, goal_point)) return {new_x, new_y};
            if (has_perpendicular_jump(maze, new_x, new_y, -1, 0, goal_point)) return {new_x, new_y};
        }
        x = new_x;
        y = new_y;
    }
}

Result jps4_work_process(const std::vector<std::vector<int>> &maze, const Coord_type &start_point, const Coord_type &goal_point)
{
    auto start_time = std::chrono::high_resolution_clock::now();

    if (!is_walkable(maze, start_point.first, start_point.second) || !is_walkable(maze, goal_point.first, goal_point.second))
    {
        return {{}, 0, 0.0};
    }
    std::unordered_map<Coord_type, int, Pair_hash> cost_from_start_score;
    std::unordered_map<Coord_type, Coord_type, Pair_hash> parent;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;

    cost_from_start_score[start_point] = 0;
    open_set.push({start_point, 0, heuristic(start_point, goal_point)});

    int total_visited_nodes = 0;

    while (!open_set.empty())
    {
        Node current_node = open_set.top();
        open_set.pop();

        auto it = cost_from_start_score.find(current_node.position);
        if (it != cost_from_start_score.end() && current_node.cost_from_start > it->second)
        {
            continue;
        }

        total_visited_nodes++;

        if (current_node.position == goal_point)
        {
            //Recover full path — expand each jump between consecutive jump points
            std::vector<Coord_type> jump_points;
            Coord_type current = goal_point;
            while (current != start_point)
            {
                jump_points.push_back(current);
                current = parent[current];
            }
            jump_points.push_back(start_point);
            std::reverse(jump_points.begin(), jump_points.end());

            std::vector<Coord_type> path;
            for (int i = 0; i + 1 < (int)jump_points.size(); i++)
            {
                Coord_type from = jump_points[i];
                Coord_type to   = jump_points[i + 1];
                int step_x = (to.first  == from.first)  ? 0 : (to.first  > from.first  ? 1 : -1);
                int step_y = (to.second == from.second)  ? 0 : (to.second > from.second ? 1 : -1);
                Coord_type cur = from;
                while (cur != to)
                {
                    path.push_back(cur);
                    cur.first  += step_x;
                    cur.second += step_y;
                }
            }
            path.push_back(goal_point);

            auto end_time = std::chrono::high_resolution_clock::now();
            double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();

            return {path, total_visited_nodes, total_time};
        }

        const int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
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
    return {{}, total_visited_nodes, total_time};
}
