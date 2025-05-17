# 重新执行上一步骤，因连接中断而未能完成文件写入操作

import random
import os

# 固定列数和行数列表
cols = 2000
row_counts = [100, 500, 1000, 1500, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000]
alive_ratio = 0.2

# 输出目录
output_dir = "input"
os.makedirs(output_dir, exist_ok=True)

file_paths_large = []

for rows in row_counts:
    lines = []
    lines.append(f"{cols} {rows}")
    total_cells = cols * rows
    border_rows = {0, rows - 1}
    alive_cells = int(total_cells * alive_ratio)

    # 不能放置'o'的位置是边界行和边界列
    forbidden_indices = set()
    for r in range(rows):
        for c in range(cols):
            if r == 0 or r == rows - 1 or c == 0 or c == cols - 1:
                idx = r * cols + c
                forbidden_indices.add(idx)

    valid_indices = list(set(range(total_cells)) - forbidden_indices)
    alive_positions = set(random.sample(valid_indices, alive_cells))

    for r in range(rows):
        row = ''
        for c in range(cols):
            idx = r * cols + c
            if r == 0 or r == rows - 1 or c == 0 or c == cols - 1:
                row += '.'
            else:
                row += 'o' if idx in alive_positions else '.'
        lines.append(row)

    filename = f"{cols}_{rows}"
    file_path = os.path.join(output_dir, filename)
    with open(file_path, "w") as f:
        f.write("\n".join(lines))


