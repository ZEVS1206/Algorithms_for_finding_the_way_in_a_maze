#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <chrono>
#include "jps4_implementation.h"
#include "a_star_implementation.h"

static int heuristic_astar(const Coord_type &first, const Coord_type &second)
{
    return abs(first.first - second.first) + abs(first.second - second.second);
}

static bool is_walkable_astar(const std::vector<std::vector<int>> &maze, int x, int y)
{
    return x >= 0 && x < (int)maze.size() &&
           y >= 0 && y < (int)maze[0].size() &&
           maze[x][y] == 0;
}

Result astar_work_process(const std::vector<std::vector<int>>& maze, const Coord_type &start_point, const Coord_type &goal_point)
{
    auto start_time = std::chrono::high_resolution_clock::now();

    if (!is_walkable_astar(maze, start_point.first, start_point.second) || !is_walkable_astar(maze, goal_point.first, goal_point.second))
    {
        return {{}, 0, 0.0};
    }

    std::unordered_map<Coord_type, int, Pair_hash> cost_from_start_score;
    std::unordered_map<Coord_type, Coord_type, Pair_hash> parent;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set;

    cost_from_start_score[start_point] = 0;
    open_set.push({start_point, 0, heuristic_astar(start_point, goal_point)});

    int total_visited_nodes = 0;

    const int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

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
            //Recover path
            std::vector<Coord_type> path;
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

        for (auto &offset : directions)
        {
            int nx = current_node.position.first  + offset[0];
            int ny = current_node.position.second + offset[1];

            if (!is_walkable_astar(maze, nx, ny))
            {
                continue;
            }

            Coord_type neighbour = {nx, ny};
            int new_potentian_best_cost = cost_from_start_score[current_node.position] + 1;

            auto jt = cost_from_start_score.find(neighbour);
            if (jt == cost_from_start_score.end() || new_potentian_best_cost < jt->second)
            {
                cost_from_start_score[neighbour] = new_potentian_best_cost;
                parent[neighbour] = current_node.position;
                int total_cost = new_potentian_best_cost + heuristic_astar(neighbour, goal_point);
                open_set.push({neighbour, new_potentian_best_cost, total_cost});
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    //If algorithm has not found path
    return {{}, total_visited_nodes, total_time};
}
