import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# データをリストとして定義
csv_file = "./result/packet_info_for_each_node/20241208010502_alpha=1000_M=1_sourceSinkNum=10000_num_nodes=200_bandWidth=100Kbps-0-60s.csv"
columns = ["alpha", "flow_num", "index", "BS","col3", "col4", "col5", "col6", "packetloss_rate"]
data = pd.read_csv(csv_file, header=None, names=columns)
# 4列目と9列目の値を抽出
col4 = data["BS"]
col9 = data["packetloss_rate"]

# 散布図をプロット
plt.figure(figsize=(8, 6))
plt.scatter(col4, col9, color='blue', alpha=0.7)
plt.title('Scatter Plot of Column 4 (BS) vs Column 9 (Packet Loss Rate)')
plt.xlabel('Column 4 (BS)')
plt.ylabel('Column 9 (Packet Loss Rate)')
plt.grid(True)

# 画像を保存
plt.savefig('scatter_plot.png')
plt.show()