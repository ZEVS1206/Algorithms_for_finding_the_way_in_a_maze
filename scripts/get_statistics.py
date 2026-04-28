import os
import json
from pathlib import Path

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Настройка стиля графиков
sns.set_theme(style="whitegrid")
plt.rcParams['figure.figsize'] = (10, 6)
plt.rcParams['font.size'] = 12

def collect_all_stats(results_dir: str) -> pd.DataFrame:
    """
    Собирает данные для всех алгоритмов (jps4, A*, Dijkstra) из JSON-файлов.
    Возвращает DataFrame с колонками:
        file, algorithm, seed, visited_nodes, time_ms, path_length
    """
    data = []
    results_path = Path(results_dir)
    if not results_path.exists():
        raise FileNotFoundError(f"Директория {results_dir} не найдена")

    for file in results_path.glob("*.json"):
        with open(file, 'r') as f:
            res = json.load(f)

        # Извлечение seed из имени файла (предполагается формат maze_XXX_out.json или подобный)
        stem = file.stem
        parts = stem.split('_')
        if len(parts) >= 3 and parts[2].isdigit():
            seed = int(parts[2])
        else:
            seed = -1

        # Перебираем ключи, соответствующие алгоритмам
        for algo in ["jps4", "A*", "Dijkstra"]:
            if algo in res:
                algo_data = res[algo]
                data.append({
                    "file": file.name,
                    "algorithm": algo,
                    "seed": seed,
                    "visited_nodes": algo_data.get("visited nodes", 0),
                    "time_ms": algo_data.get("time", 0.0),
                    "path_length": algo_data.get("path_len", len(algo_data.get("path", [])))
                })

    df = pd.DataFrame(data)
    if df.empty:
        print("Предупреждение: не найдено данных ни по одному алгоритму.")
        return df

    df = df.sort_values(["seed", "algorithm"]).reset_index(drop=True)
    return df

def plot_jps4_benchmark(df_jps4: pd.DataFrame, output_prefix: str = "benchmarks/benchmark"):
    """Строит все графики, которые ранее были только для JPS4 (распределения, связь visited vs path и т.д.)"""
    if df_jps4.empty:
        print("Нет данных для JPS4.")
        return

    # 1. Гистограммы распределений
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    sns.histplot(df_jps4["visited_nodes"], bins=15, kde=True, ax=axes[0], color='blue')
    axes[0].set_title("JPS4: Распределение посещённых узлов")
    axes[0].set_xlabel("Количество узлов")

    sns.histplot(df_jps4["time_ms"], bins=15, kde=True, ax=axes[1], color='green')
    axes[1].set_title("JPS4: Распределение времени (мс)")
    axes[1].set_xlabel("Время, мс")

    sns.histplot(df_jps4["path_length"], bins=15, kde=True, ax=axes[2], color='red')
    axes[2].set_title("JPS4: Распределение длины пути")
    axes[2].set_xlabel("Длина пути (количество шагов)")

    plt.tight_layout()
    plt.savefig(f"{output_prefix}_jps4_distributions.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 2. Boxplot’ы для метрик JPS4
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    sns.boxplot(y=df_jps4["visited_nodes"], ax=axes[0], color='lightblue')
    axes[0].set_title("JPS4: Посещённые узлы")
    sns.boxplot(y=df_jps4["time_ms"], ax=axes[1], color='lightgreen')
    axes[1].set_title("JPS4: Время (мс)")
    sns.boxplot(y=df_jps4["path_length"], ax=axes[2], color='lightcoral')
    axes[2].set_title("JPS4: Длина пути")
    for ax in axes:
        ax.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_jps4_boxplots.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 3. Зависимость времени от номера теста (seed) для JPS4
    plt.figure(figsize=(12, 6))
    plt.plot(df_jps4["seed"], df_jps4["time_ms"], 'o-', markersize=4, linewidth=1, alpha=0.7)
    plt.xlabel("Номер теста (seed)")
    plt.ylabel("Время, мс")
    plt.title("JPS4: Зависимость времени выполнения от теста")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_jps4_time_vs_test.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 4. Связь посещённых узлов и длины пути для JPS4
    plt.figure(figsize=(10, 6))
    plt.scatter(df_jps4["visited_nodes"], df_jps4["path_length"], alpha=0.7, c='purple')
    plt.xlabel("Посещённые узлы")
    plt.ylabel("Длина пути")
    plt.title("JPS4: Связь между количеством посещённых узлов и длиной пути")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_jps4_nodes_vs_path.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 5. Сводная статистика для JPS4
    stats = df_jps4.describe().round(2)
    print("\n=== Статистика JPS4 ===")
    print(stats)
    stats.to_csv(f"{output_prefix}_jps4_summary.csv")
    print(f"Сводная статистика JPS4 сохранена в {output_prefix}_jps4_summary.csv")

def plot_comparison(df_all: pd.DataFrame, output_prefix: str = "benchmarks/comparison"):
    """Строит сравнительные графики для всех трёх алгоритмов (время и посещённые узлы)"""
    if df_all.empty:
        print("Нет данных для сравнения.")
        return

    # 1. Сравнение времени выполнения (boxplot)
    plt.figure(figsize=(8, 6))
    sns.boxplot(data=df_all, x="algorithm", y="time_ms", palette="Set2")
    plt.title("Сравнение времени выполнения (мс)")
    plt.ylabel("Время, мс")
    plt.xlabel("Алгоритм")
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_time_boxplot.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 2. Сравнение посещённых узлов (boxplot)
    plt.figure(figsize=(8, 6))
    sns.boxplot(data=df_all, x="algorithm", y="visited_nodes", palette="Set2")
    plt.title("Сравнение количества посещённых узлов")
    plt.ylabel("Посещённые узлы")
    plt.xlabel("Алгоритм")
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_visited_boxplot.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 3. Сравнение средних значений в виде столбчатой диаграммы
    mean_stats = df_all.groupby("algorithm")[["time_ms", "visited_nodes"]].mean().reset_index()
    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    sns.barplot(data=mean_stats, x="algorithm", y="time_ms", ax=axes[0], palette="Set2")
    axes[0].set_title("Среднее время выполнения (мс)")
    sns.barplot(data=mean_stats, x="algorithm", y="visited_nodes", ax=axes[1], palette="Set2")
    axes[1].set_title("Среднее количество посещённых узлов")
    for ax in axes:
        ax.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_mean_comparison.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 4. Сводная таблица сравнения (средние, медианы и т.д.)
    comp_stats = df_all.groupby("algorithm").agg({
        "time_ms": ["mean", "median", "std"],
        "visited_nodes": ["mean", "median", "std"]
    }).round(2)
    print("\n=== Сравнительная статистика алгоритмов ===")
    print(comp_stats)
    comp_stats.to_csv(f"{output_prefix}_comparison_summary.csv")
    print(f"Сравнительная статистика сохранена в {output_prefix}_comparison_summary.csv")

def main_statistics(folder_with_results, output_dir="benchmarks_with_rooms"):
    # Создаём папку для выходных файлов, если её нет
    Path(output_dir).mkdir(exist_ok=True)
    output_prefix = os.path.join(output_dir, "benchmark")

    try:
        df_all = collect_all_stats(folder_with_results)
        if df_all.empty:
            print("Нет данных. Убедитесь, что папка содержит JSON-файлы с результатами.")
            return

        # Отделяем данные только для JPS4
        df_jps4 = df_all[df_all["algorithm"] == "jps4"].copy()

        # Строим отчёты
        plot_jps4_benchmark(df_jps4, output_prefix)
        plot_comparison(df_all, output_prefix)

        # Дополнительно: объединяем все графики в один PDF (по желанию)
        from matplotlib.backends.backend_pdf import PdfPages
        report_path = f"{output_prefix}_full_report.pdf"
        with PdfPages(report_path) as pdf:
            # Список всех созданных PNG-файлов
            images = [
                f"{output_prefix}_jps4_distributions.png",
                f"{output_prefix}_jps4_boxplots.png",
                f"{output_prefix}_jps4_time_vs_test.png",
                f"{output_prefix}_jps4_nodes_vs_path.png",
                f"{output_prefix}_comparison_time_boxplot.png",
                f"{output_prefix}_comparison_visited_boxplot.png",
                f"{output_prefix}_comparison_mean_comparison.png"
            ]
            for img in images:
                if os.path.exists(img):
                    im = plt.imread(img)
                    plt.figure(figsize=(10, 8))
                    plt.imshow(im)
                    plt.axis('off')
                    pdf.savefig()
                    plt.close()
        print(f"Полный отчёт сохранён в {report_path}")

    except Exception as e:
        print(f"Ошибка: {e}")

if __name__ == "__main__":
    folder = input("Введите директорию с результатами: ").strip()
    main_statistics(folder)
