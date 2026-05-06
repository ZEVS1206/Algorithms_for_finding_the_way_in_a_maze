#ifndef JSP4_IMPLEMENTATION_H
#define JSP4_IMPLEMENTATION_H
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <chrono>

// ─────────────────────────────────────────────
// Required types
// ─────────────────────────────────────────────
const int MAGIC_CONST = 131071;
using Coord_type = std::pair<int, int>;

struct Pair_hash
{
    size_t operator()(const Coord_type &coords) const
    {
        return coords.first * MAGIC_CONST + coords.second;
    }
};

struct Node
{
    Coord_type position;
    int cost_from_start;
    int total_cost;

    bool operator > (const Node &other) const
    {
        return total_cost > other.total_cost;
    }
};

struct Result
{
    std::vector<Coord_type> path;
    int count_of_visited_nodes = 0;
    double total_time = 0.0;
};

// ─────────────────────────────────────────────
// class Maze — incapsulation and checking walkable
// ─────────────────────────────────────────────
class Maze
{
public:
    explicit Maze(const std::vector<std::vector<int>> &grid)
        : grid_(grid),
          rows_(static_cast<int>(grid.size())),
          cols_(static_cast<int>(grid.empty() ? 0 : grid[0].size()))
    {}

    bool is_walkable(int x, int y) const
    {
        return x >= 0 && x < rows_ &&
               y >= 0 && y < cols_ &&
               grid_[x][y] == 0;
    }

    int rows() const { return rows_; }
    int cols() const { return cols_; }

private:
    const std::vector<std::vector<int>> &grid_;
    int rows_;
    int cols_;
};

// ─────────────────────────────────────────────
// class PathReconstructor — recovering full path from jump points
// ─────────────────────────────────────────────
class PathReconstructor
{
public:
    explicit PathReconstructor(const std::unordered_map<Coord_type, Coord_type, Pair_hash> &parent)
        : parent_(parent)
    {}

    std::vector<Coord_type> reconstruct(const Coord_type &start_point, const Coord_type &goal_point) const
    {
        //Collect jump points from goal to start
        std::vector<Coord_type> jump_points;
        Coord_type current = goal_point;
        while (current != start_point)
        {
            jump_points.push_back(current);
            current = parent_.at(current);
        }
        jump_points.push_back(start_point);
        std::reverse(jump_points.begin(), jump_points.end());

        //Expand each segment between consecutive jump points into individual cells
        std::vector<Coord_type> path;
        for (int i = 0; i + 1 < static_cast<int>(jump_points.size()); i++)
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
        return path;
    }

private:
    const std::unordered_map<Coord_type, Coord_type, Pair_hash> &parent_;
};

// ─────────────────────────────────────────────
// class JumpPointScanner — logic of jump points
// ─────────────────────────────────────────────
class JumpPointScanner
{
public:
    explicit JumpPointScanner(const Maze &maze, const Coord_type &goal_point)
        : maze_(maze), goal_point_(goal_point)
    {}

    //Check if cell (x,y) moving in direction (dx,dy) has a forced neighbour
    bool has_forced_neighbour(int x, int y, int dx, int dy) const
    {
        //Moving horizontally (dx != 0) — check sides along columns
        if (dx != 0)
        {
            if (!maze_.is_walkable(x, y - 1) && maze_.is_walkable(x + dx, y - 1)) return true;
            if (!maze_.is_walkable(x, y + 1) && maze_.is_walkable(x + dx, y + 1)) return true;
        }
        //Moving vertically (dx == 0) — check sides along rows
        if (dx == 0)
        {
            if (!maze_.is_walkable(x - 1, y) && maze_.is_walkable(x - 1, y + dy)) return true;
            if (!maze_.is_walkable(x + 1, y) && maze_.is_walkable(x + 1, y + dy)) return true;
        }
        return false;
    }

    //Returns true if cell (x,y) is on the same row/column as the goal (allows turning toward it)
    bool is_on_goal_line(int x, int y, int dx, int dy) const
    {
        if (dx != 0) return x == goal_point_.first;  //horizontal — same row as goal
        if (dy != 0) return y == goal_point_.second; //vertical   — same column as goal
        return false;
    }

    //Iterative perpendicular ray scan — returns true if a jump point exists along (pdx,pdy) from (x,y)
    bool has_perpendicular_jump(int x, int y, int pdx, int pdy) const
    {
        int px = x;
        int py = y;
        while (true)
        {
            px += pdx;
            py += pdy;
            if (!maze_.is_walkable(px, py))
            {
                return false;
            }
            if (px == goal_point_.first && py == goal_point_.second)
            {
                return true;
            }
            if (is_on_goal_line(px, py, pdx, pdy))
            {
                return true;
            }
            if (has_forced_neighbour(px, py, pdx, pdy))
            {
                return true;
            }
        }
    }

    //Main ray scan: walks from (x,y) in direction (dx,dy) and returns the first jump point
    Coord_type get_jump_point(int x, int y, int dx, int dy) const
    {
        while (true)
        {
            int new_x = x + dx;
            int new_y = y + dy;
            //Wall or border
            if (!maze_.is_walkable(new_x, new_y))
            {
                return {-1, -1};
            }
            //Found goal point
            if (new_x == goal_point_.first && new_y == goal_point_.second)
            {
                return {new_x, new_y};
            }
            //Has forced neighbour — this cell is a jump point
            if (has_forced_neighbour(new_x, new_y, dx, dy))
            {
                return {new_x, new_y};
            }
            //Aligned with goal line — stop to allow turning toward the goal
            if (is_on_goal_line(new_x, new_y, dx, dy))
            {
                return {new_x, new_y};
            }
            //Check perpendicular directions iteratively (no recursion — avoids stack overflow)
            if (dx != 0) //moving vertically — check horizontal perpendiculars
            {
                if (has_perpendicular_jump(new_x, new_y, 0,  1)) return {new_x, new_y};
                if (has_perpendicular_jump(new_x, new_y, 0, -1)) return {new_x, new_y};
            }
            if (dy != 0) //moving horizontally — check vertical perpendiculars
            {
                if (has_perpendicular_jump(new_x, new_y,  1, 0)) return {new_x, new_y};
                if (has_perpendicular_jump(new_x, new_y, -1, 0)) return {new_x, new_y};
            }
            x = new_x;
            y = new_y;
        }
    }

private:
    const Maze       &maze_;
    const Coord_type &goal_point_;
};

// ─────────────────────────────────────────────
// class JPS4Solver — main solver
// ─────────────────────────────────────────────
class JPS4Solver
{
private:
    //Manhattan distance heuristic
    static int heuristic(const Coord_type &first, const Coord_type &second)
    {
        return abs(first.first - second.first) + abs(first.second - second.second);
    }

    //Clear state before each solve() call
    void reset()
    {
        cost_from_start_score_.clear();
        parent_.clear();
        open_set_ = {};
    }

    const Maze &maze_;

    std::unordered_map<Coord_type, int,        Pair_hash> cost_from_start_score_;
    std::unordered_map<Coord_type, Coord_type, Pair_hash> parent_;
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> open_set_;

public:
    explicit JPS4Solver(const Maze &maze)
        : maze_(maze)
    {}

    Result solve(const Coord_type &start_point, const Coord_type &goal_point)
    {
        auto start_time = std::chrono::high_resolution_clock::now();

        if (!maze_.is_walkable(start_point.first, start_point.second) ||
            !maze_.is_walkable(goal_point.first,  goal_point.second))
        {
            return {{}, 0, 0.0};
        }

        reset();

        JumpPointScanner scanner(maze_, goal_point);

        cost_from_start_score_[start_point] = 0;
        open_set_.push({start_point, 0, heuristic(start_point, goal_point)});

        int total_visited_nodes = 0;

        const int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

        while (!open_set_.empty())
        {
            Node current_node = open_set_.top();
            open_set_.pop();

            //Skip stale entries — a better path to this node was already found
            auto it = cost_from_start_score_.find(current_node.position);
            if (it != cost_from_start_score_.end() && current_node.cost_from_start > it->second)
            {
                continue;
            }

            total_visited_nodes++;

            if (current_node.position == goal_point)
            {
                PathReconstructor reconstructor(parent_);
                std::vector<Coord_type> path = reconstructor.reconstruct(start_point, goal_point);

                auto end_time = std::chrono::high_resolution_clock::now();
                double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();

                return {path, total_visited_nodes, total_time};
            }

            for (auto &offset : directions)
            {
                Coord_type jump_point = scanner.get_jump_point(
                    current_node.position.first,
                    current_node.position.second,
                    offset[0], offset[1]
                );

                if (jump_point.first == -1)
                {
                    continue;
                }

                int new_potentian_best_cost = cost_from_start_score_[current_node.position] +
                                              abs(current_node.position.first  - jump_point.first) +
                                              abs(current_node.position.second - jump_point.second);

                auto jt = cost_from_start_score_.find(jump_point);
                if (jt == cost_from_start_score_.end() || new_potentian_best_cost < jt->second)
                {
                    cost_from_start_score_[jump_point] = new_potentian_best_cost;
                    parent_[jump_point] = current_node.position;
                    int total_cost = new_potentian_best_cost + heuristic(jump_point, goal_point);
                    open_set_.push({jump_point, new_potentian_best_cost, total_cost});
                }
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double total_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        //If algorithm has not found path
        return {{}, total_visited_nodes, total_time};
    }
};

//Main algorithm function
Result jps4_work_process(const std::vector<std::vector<int>> &maze, const Coord_type &start_point, const Coord_type &goal_point);


#endif
