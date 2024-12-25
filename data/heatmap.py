import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

# CSVファイルの読み込み
data_file = "./output/heatmap/output.csv"  # CSVファイル名を指定してください
df = pd.read_csv(data_file, header=None, names=["縦", "横", "値"])

# データをピボットして、Heatmap用の形式に整形
heatmap_data = df.pivot(index="縦", columns="横", values="値")

# Heatmapのプロット
plt.figure(figsize=(12, 8))
# sns.heatmap(heatmap_data, annot=False, fmt=".1f", cmap="coolwarm")
sns.heatmap(heatmap_data, annot=False, fmt=".1f", cmap="gist_stern")
plt.title("Heatmap: Color represents average throughput(bps)")
plt.xlabel("flownum")
plt.ylabel("alpha")

# 画像として保存
plt.savefig("./output/heatmap/heatmap.png")
plt.close()

print("Heatmap saved as 'heatmap.png'")
