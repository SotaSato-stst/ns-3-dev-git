import numpy as np

# 設定
max_value = 20000
num_divisions = 50
min_value = 10  # 最小値

# 対数スケールで等間隔の値を計算
log_min = np.log10(min_value)
log_max = np.log10(max_value)
log_values = np.linspace(log_min, log_max, num_divisions)

# 10^xを取ることで元のスケールに変換
values = 10**log_values

# 四捨五入して "- value" の形式で出力
for value in values:
    print(f"- {round(value)}")
