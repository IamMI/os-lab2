import subprocess
import time
import os
import filecmp

def verify_outputs(serial_output_path, parallel_output_path):
    try:
        return filecmp.cmp(serial_output_path, parallel_output_path, shallow=False)
    except Exception as e:
        print(f"验证输出时发生错误: {e}")
        return False


# 定义要测试的程序
serial_program = "./life-arrange"
parallel_program = "./life-arrange"

# 定义要测试的步数
steps_list = [10, 50, 100, 500, 1000]
# steps_list = [10, 50]

# 定义要测试的初始环境
# environments = ["input/23334m", "input/make-a", "input/o0075", "input/o0045-gun", "input/puf-qb-c3"]
environments = ["input/100_40", "input/500_40", "input/1000_40", "input/1500_40", "input/2000_40", "input/3000_40", "input/4000_40", \
                "input/5000_40", "input/6000_40", "input/7000_40", "input/8000_40", "input/9000_40", "input/10000_40"]


# 定义重复执行的次数
num_repetitions = 3

# 用于存储结果的字典
results = {}

# 确保输入文件夹存在
if not os.path.exists("input"):
    print("错误：找不到 'input' 文件夹。请确保输入文件位于 'input' 文件夹下。")
    exit(1)

print(f"开始执行并收集数据，每个测试用例重复 {num_repetitions} 次...")

# 遍历不同的步数
for steps in steps_list:
    results[steps] = {}
    # 遍历不同的初始环境
    for env in environments:
        print(f"\n正在测试：steps = {steps}, environment = {env}")
        results[steps][env] = {}

        serial_times = []
        parallel_times = []

        # 重复执行 serial 程序并记录时间
        print(f"  执行 {serial_program} {num_repetitions} 次...")
        for i in range(num_repetitions):
            serial_command = [serial_program, str(steps), env, '1', 'output/serial.txt']
            start_time = time.time()
            try:
                subprocess.run(serial_command, check=True, capture_output=True, timeout=20)
                serial_times.append(time.time() - start_time)
                print(f"    第 {i+1} 次执行时间：{serial_times[-1]:.4f} 秒")
            except subprocess.CalledProcessError as e:
                print(f"    第 {i+1} 次 {serial_program} 执行出错：{e}")
                serial_times.append(float('inf'))  # 使用无穷大标记错误

        # 计算 serial 程序的平均执行时间
        valid_serial_times = [t for t in serial_times if t != float('inf')]
        if valid_serial_times:
            avg_serial_time = sum(valid_serial_times) / len(valid_serial_times)
            results[steps][env]["serial_time"] = f"{avg_serial_time:.4f} s"
        else:
            results[steps][env]["serial_time"] = "Error"
            results[steps][env]["parallel_time"] = "Error"
            results[steps][env]["speedup"] = "Error"
            continue

        # 重复执行 parallel 程序并记录时间
        print(f"  执行 {parallel_program} {num_repetitions} 次...")
        for i in range(num_repetitions):
            parallel_command = [parallel_program, str(steps), env, '4', 'output/parallel.txt']
            start_time = time.time()
            try:
                subprocess.run(parallel_command, check=True, capture_output=True, timeout=20)
                parallel_times.append(time.time() - start_time)
                print(f"    第 {i+1} 次执行时间：{parallel_times[-1]:.4f} 秒")
            except subprocess.CalledProcessError as e:
                print(f"    第 {i+1} 次 {parallel_program} 执行出错：{e}")
                parallel_times.append(float('inf')) # 使用无穷大标记错误

        # 计算 parallel 程序的平均执行时间
        valid_parallel_times = [t for t in parallel_times if t != float('inf')]
        if valid_parallel_times:
            avg_parallel_time = sum(valid_parallel_times) / len(valid_parallel_times)
            results[steps][env]["parallel_time"] = f"{avg_parallel_time:.4f} s"
        else:
            results[steps][env]["parallel_time"] = "Error"
            results[steps][env]["speedup"] = "Error"
            continue

        # 计算加速比
        if isinstance(avg_serial_time, float) and isinstance(avg_parallel_time, float) and avg_parallel_time > 0:
            speedup = avg_serial_time / avg_parallel_time
            results[steps][env]["speedup"] = f"{speedup:.2f}x"
            print(f"  平均加速比：{speedup:.2f}x")
            # 验证串行和并行输出是否一致
            match = verify_outputs("output/serial.txt", "output/parallel.txt")
            results[steps][env]["match"] = "✅" if match else "❌"
            if not match:
                print("  ❌ 输出不一致！")
                exit(0)
            else:
                print("  ✅ 输出一致")
            
        else:
            results[steps][env]["speedup"] = "N/A"

print("\n数据收集完成，正在生成 Markdown 表格...")

# 生成 Markdown 表格
markdown_table = "## 程序执行时间与加速比 (平均值)\n\n"
markdown_table += "| Steps | Environment | Serial Time (Avg) | Parallel Time (Avg) | Speedup |\n"
markdown_table += "|---|---|---|---|---|\n"

for steps, env_data in results.items():
    for env, time_data in env_data.items():
        markdown_table += f"| {steps} | {env.replace('input/', '')} | {time_data.get('serial_time', 'N/A')} | {time_data.get('parallel_time', 'N/A')} | {time_data.get('speedup', 'N/A')} |\n"

print("\n生成的 Markdown 表格如下：")
print(markdown_table)

# 可以选择将表格保存到文件
with open("execution_results_average.md", "w") as f:
    f.write(markdown_table)

print("\n结果已保存到文件：execution_results_average.md")