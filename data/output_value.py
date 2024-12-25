import numpy as np

# 設定
start_value = 0
step = 0.5
num_divisions = 50

# 等間隔の値を計算
values = [start_value + i * step for i in range(num_divisions)]

# 四捨五入して "- value" の形式で出力
for value in values:
    print(f"- {round(value, 1)}")
