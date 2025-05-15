import matplotlib.pyplot as plt
from collections import defaultdict
import re

def parse_md_table(filepath):
    """从 Markdown 文件中解析表格数据"""
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    table_lines = [line.strip() for line in lines if line.strip().startswith('|') and not line.strip().startswith('|---')]
    parsed_data = []

    for line in table_lines[1:]:  # 跳过表头
        parts = [p.strip() for p in line.strip('|').split('|')]
        if len(parts) < 5:
            continue  # 防止格式错误
        steps = int(parts[0])
        env = parts[1]
        try:
            cols = int(env.split('_')[0])
            speedup = float(parts[4].replace('x', '').strip())
            parsed_data.append((steps, cols, speedup))
        except ValueError:
            continue  # 忽略无法解析的行

    return parsed_data

def plot_speedup(data):
    """根据数据绘制折线图"""
    plot_data = defaultdict(lambda: ([], []))
    for steps, cols, speedup in data:
        plot_data[steps][0].append(cols)
        plot_data[steps][1].append(speedup)

    plt.figure(figsize=(10, 6))
    for steps, (cols, speedups) in sorted(plot_data.items()):
        sorted_pairs = sorted(zip(cols, speedups))
        sorted_cols, sorted_speedups = zip(*sorted_pairs)
        plt.plot(sorted_cols, sorted_speedups, marker='o', label=f"Steps = {steps}")

    plt.xlabel("Columns")
    plt.ylabel("Speedup")
    plt.title("Speedup vs Columns for Different Steps")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    # plt.show()
    plt.savefig("Speedup-cols.png", dpi=300)


if __name__ == "__main__":
    filepath = "execution_results_average.md"  # 将此处替换为你的 md 文件名
    data = parse_md_table(filepath)
    plot_speedup(data)
