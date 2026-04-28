import random
import json
import os
from typing import List
from pathlib import Path

def generate_maze(width, height, seed: int = None) -> List[List[int]]:
    #0 - free way, 1 - dead end
    if seed is not None:
        random.seed(seed)
    if width % 2 == 0:
        width += 1
    if height % 2 == 0:
        height += 1
    maze = [[1] * width for _ in range(height)]
    start_x, start_y = 1, 1
    maze[start_y][start_x] = 0
    stack = [(start_x, start_y)]
    while stack:
        x, y = stack[-1]
        neighbors = []
        for dx, dy in [(0, -2), (0, 2), (-2, 0), (2, 0)]:
            nx, ny = x + dx, y + dy
            if 0 < nx < width - 1 and 0 < ny < height - 1 and maze[ny][nx] == 1:
                neighbors.append((nx, ny, dx // 2, dy // 2))
        if neighbors:
            nx, ny, mx, my = random.choice(neighbors)
            maze[ny][nx] = 0
            maze[y + my][x + mx] = 0
            stack.append((nx, ny))
        else:
            stack.pop()
    maze[1][1] = 0
    maze[height-2][width-2] = 0
    return maze

def add_rooms(maze, num_rooms=10, min_size=5, max_size=15):
    height = len(maze)
    width = len(maze[0])
    for _ in range(num_rooms):
        rw = random.randrange(min_size, max_size, 2)
        rh = random.randrange(min_size, max_size, 2)
        rx = random.randrange(1, width  - rw - 1)
        ry = random.randrange(1, height - rh - 1)
        for y in range(ry, ry + rh):
            for x in range(rx, rx + rw):
                maze[y][x] = 0
    return maze

def generate_and_save(num_mazes, width, height, output_dir="mazes", seed_offset=0, is_any_rooms = 'n'):
    os.makedirs(output_dir, exist_ok=True)
    for i in range(num_mazes):
        maze = generate_maze(width, height, seed=seed_offset + i)
        if (is_any_rooms == 'y'):
            maze = add_rooms(maze, num_rooms=20, min_size=5, max_size=21)
        filename = os.path.join(output_dir, f"maze_{i:03d}.json")
        with open(filename, "w") as f:
            json.dump({
                "maze": maze,
                "start": [1, 1],
                "goal": [height-2, width-2]
            }, f, indent=2)
        print(f"Сохранён {filename}")
def main_generate(output_folder):
    number_of_mazes = int(input("Введите количество лабиринтов для генерации:"))
    is_any_rooms    = input("Создать лабиринт с комнатами?(y/n):")
    maze_width      = int(input("Ширина(должна быть нечетной):"))
    maze_height     = int(input("Высота(должна быть нечетной):"))
    output_dir = Path(output_folder)
    output_dir.mkdir(exist_ok=True)
    generate_and_save(number_of_mazes, maze_width, maze_height, output_folder, seed_offset=0, is_any_rooms = is_any_rooms)
    return is_any_rooms

if __name__ == "__main__":
    output_folder   = input("Папка с тестами:")
    main_generate(output_folder)
