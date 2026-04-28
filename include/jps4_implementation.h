#ifndef JSP4_IMPLEMENTATION_H
#define JSP4_IMPLEMENTATION_H

const int MAGIC_CONST = 131071;
using Coord_type = std::pair<int, int>; //Type for coords
//Functional object for hash coords
struct Pair_hash
{
    size_t operator()(const Coord_type& coords) const
    {
        return coords.first * MAGIC_CONST + coords.second;
    }
};

//Structure for priority queue for A*
struct Node
{
    Coord_type position;
    int cost_from_start;
    int total_cost;//cost_from_start + heuristic

    bool operator > (const Node &other) const
    {
        return total_cost > other.total_cost;
    }
};
//Main structure for getting result
struct Result
{
    std::vector<Coord_type> path;
    int count_of_visited_nodes = 0;
    double total_time = 0.0;
};

//Main algorithm function
Result jps4_work_process(const std::vector<std::vector<int>> &maze, const Coord_type &start_point, const Coord_type &goal_point);


#endif
