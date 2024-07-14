import pandas as pd
import matplotlib.pyplot as plt
import os

# ディレクトリのパス
directory = './average_throughput'

def main():
    filename = latestFileName()
    # CSVファイルからデータを読み込む
    df = pd.read_csv(directory + '/' + filename, header=None, names=['alpha', 'flowNum', 'throughput'])
    output_dir = './output/boxplots/' + filename[:-4]
    os.makedirs(output_dir, exist_ok=True)
    # Y軸の値でグループ化して、それぞれのグループについてヒゲ図を描画して保存
    for y_value, grp in df.groupby(['flowNum']):
        # プロットを作成
        fig, ax = plt.subplots()

        # グループごとに箱ひげ図を描画
        for key, subgrp in grp.groupby(['alpha']):
            ax.boxplot(subgrp['throughput'], positions=[key], widths=0.3)

        # グラフのタイトルと軸ラベルを設定
        ax.set_xlabel('alpha')
        ax.set_ylim(0, 5000)
        ax.set_xlim(0, 50)
        ax.set_ylabel('throughput(kbps)')
        ax.set_title(f'flowNum={y_value}')

        # 画像として保存
        filename = f'flowNum_{y_value}.png'
        filepath = os.path.join(output_dir, filename)
        plt.savefig(filepath)
        plt.close()

        print(f'saved: {filepath}')

    for x_value, grp in df.groupby(['alpha']):
        # プロットを作成
        fig, ax = plt.subplots()

        # グループごとに箱ひげ図を描画
        for key, subgrp in grp.groupby(['flowNum']):
            ax.boxplot(subgrp['throughput'], positions=[key], widths=0.3)

        # グラフのタイトルと軸ラベルを設定
        ax.set_xlabel('flowNum')
        ax.set_ylim(0, 5000)
        ax.set_xlim(0, 50)
        ax.set_ylabel('throughput(kbps)')
        ax.set_title(f'alpha={x_value}')

        # 画像として保存
        filename = f'alpha_{x_value}.png'
        filepath = os.path.join(output_dir, filename)
        plt.savefig(filepath)
        plt.close()

        print(f'saved: {filepath}')

def latestFileName():
# ディレクトリ内のファイルをリストアップ
    files = os.listdir(directory)

    # 数値部分を抽出して比較するための変数を初期化
    max_number = float('-inf')
    max_filename = None

    # 各ファイルについて処理
    for filename in files:
        # 拡張子が.csvでない場合はスキップ
        if not filename.endswith('.csv'):
            continue

        # ファイル名から数値部分を抽出（.csvを取り除いた後の部分）
        try:
            number_part = int(filename[:-4])  # .csvの分を取り除く
        except ValueError:
            continue  # 数値に変換できない場合はスキップ

        # 最大数値の更新
        if number_part > max_number:
            max_number = number_part
            max_filename = filename

    return max_filename

if __name__ == "__main__":
    main()



