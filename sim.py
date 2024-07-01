import subprocess
from gen_topology import gen, calc, preferentialAttachment
from datetime import datetime

alphas = [0.5]
sourceSinkNums = [1, 10]
loopCount = 1

def main():
    # 現在の日時を取得
    now = datetime.now()
    # 指定されたフォーマットで日時をフォーマット
    fileName = now.strftime("%Y%m%d%H%M%S") + ".csv"

    for alpha in alphas:
        for sourceSinkNum in sourceSinkNums:
            for _ in range(loopCount):
                gen.execute(alpha=alpha, sourceSinkNum=
                sourceSinkNum, fileName=fileName)
                subprocess.run(['./ns3', 'run', 'sim.cc', '--', fileName, str(alpha), str(sourceSinkNum)])

if __name__ == "__main__":
    main()
