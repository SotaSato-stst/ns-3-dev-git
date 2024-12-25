import csv
from collections import defaultdict

# 入力ファイル名と出力ファイル名を指定
input_file = "result/merged_csv/merged.csv"
output_file = "output/heatmap/output.csv"

# データをグループ化して平均値を計算
def calculate_averages(input_file, output_file):
    data = defaultdict(list)

    # 入力CSVを読み込む
    with open(input_file, "r") as infile:
        reader = csv.reader(infile)
        for row in reader:
            key = (float(row[0]), float(row[1]))  # 1列目と2列目をキーとする
            value = float(row[2])  # 3列目の値
            data[key].append(value)

    # 平均値を計算
    averaged_data = [
        (key[0], key[1], sum(values) / len(values)) for key, values in data.items()
    ]

    # 結果を出力CSVに書き込む
    with open(output_file, "w", newline="") as outfile:
        writer = csv.writer(outfile)
        writer.writerows(averaged_data)

# 関数を実行
calculate_averages(input_file, output_file)

print(f"処理が完了しました。結果は'{output_file}'に出力されました。")