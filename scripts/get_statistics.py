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

def collect_stats(results_dir: str) -> pd.DataFrame:
    data = []
    results_path = Path(results_dir)
    if not results_path.exists():
        raise FileNotFoundError(f"Директория {results_dir} не найдена")

    for file in results_path.glob("*.json"):
        with open(file, 'r') as f:
            res = json.load(f)

        visited = res.get("visited nodes", 0)
        time_ms = res.get("time", 0.0)
        path_len = len(res.get("path", []))

        stem = file.stem
        parts = stem.split('_')
        print(parts)
        if len(parts) >= 3 and parts[2].isdigit():
            seed = int(parts[2])
        else:
            seed = -1

        data.append({
            "file": file.name,
            "seed": seed,
            "visited_nodes": visited,
            "time_ms": time_ms,
            "path_length": path_len
        })

    df = pd.DataFrame(data)
    if df.empty:
        print("Предупреждение: не найдено ни одного файла с результатами.")
        return df

    df = df.sort_values("seed").reset_index(drop=True)
    return df

def plot_benchmark(df: pd.DataFrame, output_prefix: str = "benchmarks/benchmark"):
    if df.empty:
        print("Нет данных для построения графиков.")
        return

    # 1. Гистограммы распределений
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))
    sns.histplot(df["visited_nodes"], bins=15, kde=True, ax=axes[0], color='blue')
    axes[0].set_title("Распределение посещённых узлов")
    axes[0].set_xlabel("Количество узлов")

    sns.histplot(df["time_ms"], bins=15, kde=True, ax=axes[1], color='green')
    axes[1].set_title("Распределение времени (мс)")
    axes[1].set_xlabel("Время, мс")

    sns.histplot(df["path_length"], bins=15, kde=True, ax=axes[2], color='red')
    axes[2].set_title("Распределение длины пути")
    axes[2].set_xlabel("Длина пути (количество шагов)")

    plt.tight_layout()
    plt.savefig(f"{output_prefix}_distributions.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 2. Boxplot’ы для сравнения метрик
    fig, axes = plt.subplots(1, 3, figsize=(15, 5))

    sns.boxplot(y=df["visited_nodes"], ax=axes[0], color='lightblue')
    axes[0].set_title("Посещённые узлы")
    axes[0].set_ylabel("Количество")

    sns.boxplot(y=df["time_ms"], ax=axes[1], color='lightgreen')
    axes[1].set_title("Время (мс)")

    sns.boxplot(y=df["path_length"], ax=axes[2], color='lightcoral')
    axes[2].set_title("Длина пути")

    for ax in axes:
        ax.grid(True, linestyle='--', alpha=0.6)

    plt.tight_layout()
    plt.savefig(f"{output_prefix}_boxplots.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 3. Зависимость времени от номера теста (seed) – тренд
    plt.figure(figsize=(12, 6))
    plt.plot(df["seed"], df["time_ms"], 'o-', markersize=4, linewidth=1, alpha=0.7)
    plt.xlabel("Номер теста (seed)")
    plt.ylabel("Время, мс")
    plt.title("Зависимость времени выполнения от теста")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_time_vs_test.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 4. Сравнение посещённых узлов и длины пути (scatter)
    plt.figure(figsize=(10, 6))
    plt.scatter(df["visited_nodes"], df["path_length"], alpha=0.7, c='purple')
    plt.xlabel("Посещённые узлы")
    plt.ylabel("Длина пути")
    plt.title("Связь между количеством посещённых узлов и длиной пути")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.tight_layout()
    plt.savefig(f"{output_prefix}_nodes_vs_path.png", dpi=150, bbox_inches='tight')
    plt.close()

    # 5. Сводная таблица статистики
    stats = df.describe().round(2)
    print("\n=== Статистика по результатам ===")
    print(stats)

    # Сохраняем статистику в CSV
    stats.to_csv(f"{output_prefix}_summary.csv")
    print(f"\nСводная статистика сохранена в {output_prefix}_summary.csv")

    # 6. Все графики в одном PDF
    from matplotlib.backends.backend_pdf import PdfPages
    with PdfPages(f"{output_prefix}_report.pdf") as pdf:
        # Загружаем созданные картинки (можно перерисовать, но проще сохранить уже созданные)
        figs = [
            f"{output_prefix}_distributions.png",
            f"{output_prefix}_boxplots.png",
            f"{output_prefix}_time_vs_test.png",
            f"{output_prefix}_nodes_vs_path.png"
        ]
        for img in figs:
            if os.path.exists(img):
                im = plt.imread(img)
                plt.figure(figsize=(10, 8))
                plt.imshow(im)
                plt.axis('off')
                pdf.savefig()
                plt.close()

    print(f"Полный отчёт сохранён в {output_prefix}_report.pdf")

def main_statistics(folder_with_results):

    try:
        df = collect_stats(folder_with_results)
        if df.empty:
            print("Нет данных. Убедитесь, что папка содержит файлы *.json")
            return
        plot_benchmark(df)
    except Exception as e:
        print(f"Ошибка: {e}")

if __name__ == "__main__":
    folder_with_results = input("Введите директорию с результатами:")
    main_statistics(folder_with_results)
