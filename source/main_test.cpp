#include <vector>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

#include "jps4_implementation.h"

using json = nlohmann::json;

int main(int argc, char *argv[])
{
    //std::vector<std::vector<int>> grid(5, std::vector<int>(5, 0));
    // std::vector<std::vector<int>> grid =
    // {
    //     {0,0,0,0,0},
    //     {0,1,1,1,0},
    //     {0,0,0,0,0},
    //     {0,1,1,1,0},
    //     {0,0,0,0,0}
    // };
    //Coord_type start = {0,0};
    //Coord_type goal = {4,4};
    std::string input_file = "mazes/test_1.json";//default file
    if (argc == 2)
    {
        input_file = argv[1];
    }
    std::ifstream file(input_file);
    if (!file.is_open())
    {
        std::cerr << "Не удалось открыть файл: " << input_file << "\n";
        return 1;
    }
    json data;
    try
    {
        file >> data;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Ошибка обработки файла с лабиринтом:" << e.what() << "\n";
        return 1;
    }
    std::vector<std::vector<int>> grid = data["maze"].get<std::vector<std::vector<int>>>();
    Coord_type start                   = {data["start"][0], data["start"][1]};
    Coord_type goal                    = {data["goal"][0], data["goal"][1]};
    auto res = jps4_work_process(grid, start, goal);
    std::cout << "Analyzed file: " << input_file << "\n";
    std::cout << "Visited nodes: " << res.count_of_visited_nodes << "\n";
    std::cout << "Time: " << res.total_time << " ms\n";
    std::cout << "Path length: " << res.path.size() << "\n";
    for (auto &coords: res.path)
    {
        std::cout << "[" << coords.first << "," << coords.second << "] ->";
    }
    std::cout << "end\n";
    json result;
    result["maze"]          = grid;
    result["start"]         = start;
    result["goal"]          = goal;
    result["path"]          = res.path;
    result["path_len"]      = res.path.size();
    result["visited nodes"] = res.count_of_visited_nodes;
    result["time"]          = res.total_time;
    std::filesystem::path input_path(input_file);
    std::string base_name   = input_path.stem().string();
    std::string output_file = "test_results/result_" + base_name + ".json";
    std::ofstream out_file(output_file);
    if (!out_file.is_open())
    {
        std::cerr << "Ошибка, невозможно открыть файл результата\n";
        return 1;
    }
    out_file << result;
    return 0;
}
