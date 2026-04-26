import matplotlib.pyplot as plt
import matplotlib.patches as patches
import os
from typing import List, Tuple
from pathlib import Path
import json


def visualize_maze(maze: List[List[int]],
                   path: List[Tuple[int, int]],
                   start: Tuple[int, int],
                   goal: Tuple[int, int],
                   output_filename: str = "maze.png",
                   dpi: int = 150):
    height = len(maze)
    width = len(maze[0])

    fig_width = max(8, width / 3)
    fig_height = max(8, height / 3)
    fig, ax = plt.subplots(figsize=(fig_width, fig_height), dpi=dpi)
    #Walls
    for y in range(height):
        for x in range(width):
            if maze[y][x] == 1:
                rect = patches.Rectangle((x, y), 1, 1, facecolor='black', edgecolor='none')
                ax.add_patch(rect)

    #Path
    if path:
        xs = [p[1] for p in path]
        ys = [p[0] for p in path]
        ax.plot(xs, ys, 'r-', linewidth=2, label='Path')

    #Start and goal
    ax.plot(start[1], start[0], 'go', markersize=10, label='Start')
    ax.plot(goal[1], goal[0], 'ro', markersize=10, label='Goal')

    ax.set_xlim(0, width)
    ax.set_ylim(height, 0)
    ax.set_aspect('equal')
    ax.axis('off')

    ax.legend(loc='upper left', bbox_to_anchor=(1.02, 1), framealpha=0.8)

    plt.tight_layout()
    plt.savefig(output_filename, dpi=dpi, bbox_inches='tight')
    plt.close()
    print(f"Saved visualization to {output_filename}")

def get_maze(file_with_maze = "mazes/test_1.json"):
    with open(file_with_maze, "r") as f:
        data = json.load(f)
    maze = data["maze"]
    start = data["start"]
    goal  = data["goal"]
    path = data.get("path", [])
    if path and isinstance(path[0], list):
        path = [tuple(p) for p in path]
    file_dir = Path(file_with_maze)
    dir_for_save = file_dir.parent.name
    output_filename = dir_for_save + "/" + file_dir.name + "_maze.png"
    visualize_maze(maze, path, start, goal, output_filename)

if __name__ == "__main__":
    file_for_visualize = input("Введите путь к файлу с лабиринтом:")
    if not os.path.isfile(file_for_visualize):
        print(f"Файл {file_for_visualize} не найден.")
    else:
        get_maze(file_for_visualize)
