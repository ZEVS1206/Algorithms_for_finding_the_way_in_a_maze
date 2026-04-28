from generate_mazes import main_generate
from visualize_all import visualize_all
from get_statistics import main_statistics
import subprocess
from pathlib import Path


def run_jps4_implementation(file_with_maze):
    cmd = ["make", "run", f"file={file_with_maze}"]
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=30, check=False)
        if (proc.returncode != 0):
            raise RuntimeError(f"Make вернул код {proc.returncode}, stderr: {proc.stderr}")
    except subprocess.TimeoutExpired:
        raise RuntimeError(f"Таймаут при выполнении {file_with_maze}")


def main():
    output_folder   = input("Введите директорию для тестов:")
    is_visual_required = input("Производить ли визуализацию?(y/n):")
    main_generate(output_folder)
    if (is_visual_required == 'y'):
        visualize_all(output_folder)
    mazes_dir = Path(output_folder)
    maze_files = [f for f in mazes_dir.glob("*.json")]
    if not maze_files:
        print(f"Не найдено JSON-файлов в {mazes_dir}")
        return
    success = 0
    for maze in maze_files:
        run_jps4_implementation(maze)
        success += 1
    print(f"\nГотово. Успешно обработаны: {success} из {len(maze_files)}")
    results_dir = "test_results"
    if (is_visual_required == 'y'):
        visualize_all(results_dir)
    main_statistics(results_dir)

if __name__ == "__main__":
    main()

