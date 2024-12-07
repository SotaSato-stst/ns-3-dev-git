import pandas as pd
import matplotlib.pyplot as plt
import os

# データの読み込み
# csv_file = "./result/merged_csv/merged.csv"  # CSVファイル名を指定
csv_file = "output/average_plot/20241120_packetloss_and_flownum_for_alpha_bindwidth100kbps_M1.csv"
columns = ["alpha", "flow_num", "col3", "col4", "col5", "col6", "packetloss_rate"]
data = pd.read_csv(csv_file, header=None, names=columns)

# 平均値を計算
grouped = data.groupby(["alpha", "flow_num"])["packetloss_rate"].mean().reset_index()

# 出力ディレクトリの作成
output_dir = "./output/average_plot/"
os.makedirs(output_dir, exist_ok=True)

# 折れ線グラフを描画
fig, ax = plt.subplots(figsize=(10, 6))

# alphaごとにグラフを描画
lables = ["clustered", "less clustered"]
i=0
for alpha_value, group in grouped.groupby("alpha"):
    ax.plot(group["flow_num"], group["packetloss_rate"] * 100, label=lables[i])
    i += 1

# グラフの装飾
ax.set_xscale("log")
ax.set_xlabel("flow_num")
ax.set_ylabel("average packetloss_rate(%)")
ax.set_title("correlation of packet_ross and alpha")
ax.legend()
ax.grid(True)

# 画像を保存
output_file = os.path.join(output_dir, "average_plot.png")
plt.savefig(output_file)
plt.close()

print(f"グラフを {output_file} に保存しました。")
