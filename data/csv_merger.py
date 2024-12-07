import os

def merge_csv_files(directory, output_file):
    # ディレクトリ内の全ファイルを取得
    files = [f for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
    print(len(files))
    with open(output_file, 'w') as outfile:
        for filename in files:
            with open(os.path.join(directory, filename), 'r') as infile:
                # 各ファイルの内容を読み込み、改行で区切って書き込む
                outfile.write(infile.read().strip() + '\n')

# 使用例
directory_path = './result/merged_csv'
output_file = './result/merged_csv/merged.csv'
merge_csv_files(directory_path, output_file)
