#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <chrono>
#include "jps4_implementation.h"

Result jps4_work_process(const std::vector<std::vector<int>> &maze,
                                const Coord_type &start_point,
                                const Coord_type &goal_point)
{
    Maze      grid(maze);
    JPS4Solver solver(grid);
    return solver.solve(start_point, goal_point);
}
