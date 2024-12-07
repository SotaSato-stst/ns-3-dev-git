import pandas as pd
import matplotlib.pyplot as plt
import os

# データの読み込み
csv_file = "./result/merged_csv/merged.csv"  # CSVファイル名を指定
columns = ["alpha", "flow_num", "throughput"]
data = pd.read_csv(csv_file, header=None, names=columns)

# 平均値を計算
grouped = data.groupby(["alpha", "flow_num"])["throughput"].mean().reset_index()

# 出力ディレクトリの作成
output_dir = "./output/average_plot/"
os.makedirs(output_dir, exist_ok=True)

# 折れ線グラフを描画
fig, ax = plt.subplots(figsize=(10, 6))

# alphaごとにグラフを描画
lables = ["clustered", "less clustered"]
i=0
for alpha_value, group in grouped.groupby("alpha"):
    ax.plot(group["flow_num"], group["throughput"] * 100, label=lables[i])
    i+=1

# グラフの装飾
ax.set_xscale("log")
ax.set_xlabel("flow_num")
ax.set_ylabel("average throughput(bps)")
ax.set_title("correlation of throughput and alpha")
ax.legend()
ax.grid(True)

# 画像を保存
output_file = os.path.join(output_dir, "average_plot.png")
plt.savefig(output_file)
plt.close()

print(f"グラフを {output_file} に保存しました。")
