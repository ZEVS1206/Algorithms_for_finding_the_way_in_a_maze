#include <vector>
#include <iostream>

#include "jps4_implementation.h"

int main()
{
    //std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));
    std::vector<std::vector<int>> grid =
    {
        {0,0,0,0,0},
        {0,1,1,1,0},
        {0,0,0,0,0},
        {0,1,1,1,0},
        {0,0,0,0,0}
    };
    Coord_type start = {0,0};
    Coord_type goal = {4,4};
    auto res = jps4_work_process(grid, start, goal);
    std::cout << "Visited nodes: " << res.count_of_visited_nodes << "\n";
    std::cout << "Time: " << res.total_time << " ms\n";
    std::cout << "Path length: " << res.path.size() << "\n";
    return 0;
}
