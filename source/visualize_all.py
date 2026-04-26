import os
from visualize_one_maze import get_maze

def visualize_all(folder_with_mazes):
    for res_file in os.listdir(folder_with_mazes):
        if res_file.endswith(".json"):
            file = folder_with_mazes + "/" + res_file
            get_maze(file)

if __name__ == "__main__":
    folder_with_mazes = input("Введите директорию с лабиринтами для визуализации:")
    visualize_all(folder_with_mazes)

