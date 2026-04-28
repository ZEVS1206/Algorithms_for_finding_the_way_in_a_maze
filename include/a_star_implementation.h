#ifndef ASTAR_IMPLEMENTATION_H
#define ASTAR_IMPLEMENTATION_H
#include <vector>
#include "jps4_implementation.h"
Result astar_work_process(const std::vector<std::vector<int>> &maze, const Coord_type &start_point, const Coord_type &goal_point);
#endif
